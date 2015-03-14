///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx科技有限公司(版权声明) 
/// All rights reserved. 
/// 
/// @file      Client.cpp 
/// @brief     客户端处理模块实现
///
/// @version   1.0 (版本声明)
/// @author    Jorhy (joorhy@gmail.com) 
/// @date      2013/09/24 15:13 
///
///
/// 修订说明：最初版本
///////////////////////////////////////////////////////////////////////////  
#include "XlClient.h"
#include "XlDType.h"
#include "XlCType.h"
#include "DeviceManager.h"
#include "ClientManager.h"
#include "XlHelper.h"
#include "DataBus.h"

#include <iostream>
#include <strstream>
#include <boost/property_tree/xml_parser.hpp>

#define XL_CMD_BUFFER_SIZE (1024 * 1024)
#define XL_DATA_BUFFER_SIZE (1024 * 1024)
#define XL_ALARM_BUFFER_SIZE (2 * 1024)
#define XL_RCD_BUFFER_SIZE (64)

CXlClient::CXlClient(j_socket_t nSock)
 : m_socket(nSock)
 , m_pUploadDev(NULL)
{
	memset(m_userName, 0, sizeof(m_userName));
	strcpy(m_userName, "");
	m_readBuff = new char[XL_CMD_BUFFER_SIZE];			
	m_writeBuff = new char[XL_CMD_BUFFER_SIZE];		
	m_dataBuff = new char[XL_DATA_BUFFER_SIZE];	
	m_alarmBuff = new char[XL_ALARM_BUFFER_SIZE];	
	m_rcdBuff = new char[XL_RCD_BUFFER_SIZE];	
	m_ioCmdState = xl_init_state;
	m_ioDataState = xl_init_state;
	m_ioAlarmState = xl_init_state;
	m_ioRcdState = xl_init_state;
	m_nRefCnt = 0;
	m_nAlarmRefCnt = 0;
	m_lastBreatTime = time(0);

	J_OS::LOGINFO("CXlClient::CXlClient() %d", this);
}

CXlClient::~CXlClient()
{
	StopAll();

	delete m_readBuff;
	delete m_writeBuff;
	delete m_dataBuff;
	delete m_alarmBuff;
	delete m_rcdBuff;
	JoClientManager->Logout(m_userName, this);
	J_OS::LOGINFO("CXlClient::~CXlClient() %d", this);
}

j_result_t CXlClient::SendMsgInfo(j_string_t strHostId, j_int32_t nType, j_int32_t nNo, j_int32_t nChannel)
{
	if (nType == xlc_msg_host)
	{
		J_OS::LOGINFO("CXlClient::SendMsgInfo xlc_msg_host sub = %d", nNo);
		if (nNo == xlc_host_broken)
		{
			TLock(m_vecLocker);
			VideoInfoVec::iterator it = m_videoInfoVec.begin();
			for (; it!=m_videoInfoVec.end(); ++it)
			{
				if (it->strHost == strHostId)
				{
					it->bConnected = false;
				}
			}
			TUnlock(m_vecLocker);
		}
		else if (nNo == xlc_host_connected)
		{
			TLock(m_vecLocker);
			VideoInfoVec::iterator it = m_videoInfoVec.begin();
			for (; it!=m_videoInfoVec.end(); ++it)
			{
				if (!it->bConnected)
				{
					CliRealPlay realPlay = {0};
					strcpy(realPlay.hostId, it->strHost.c_str());
					realPlay.channel = it->nChanId;
					if (StartView(realPlay, NULL) == J_OK)
					{
						it->bConnected = true;
					}
				}
			}
			TUnlock(m_vecLocker);
		}
	}
	else
	{
		struct MessageBody
		{
			CmdHeader header;
			CliMessage data;
			CmdTail tial;
		} messageBody;
		memset (&messageBody, 0, sizeof(MessageBody));
		messageBody.data.uiType = nType;
		messageBody.data.uiNO = nNo;
		messageBody.data.nChannelID = nChannel;
		strcpy(messageBody.data.hostID, strHostId.c_str());
		CXlHelper::MakeResponse(xlc_message, (j_char_t *)&messageBody.data, sizeof(CliMessage), (j_char_t *)&messageBody);
		m_socket.Write_n((j_char_t *)&messageBody, sizeof(CmdHeader) + sizeof(CliMessage) + sizeof(CmdTail));
		J_OS::LOGINFO("CXlClient::SendMessage %d-%d", nType, nNo);
	}

	return J_OK;
}

j_result_t CXlClient::ParserRequest(J_AsioDataBase *pAsioData)
{
	j_result_t nResult = J_OK;
	switch (pAsioData->ioType)
	{
	case J_AsioDataBase::j_command_e:
		nResult = ProcessRequest(pAsioData);
		break;
	case J_AsioDataBase::j_data_e:
		nResult = ProcessData(pAsioData);
		break;
	case J_AsioDataBase::j_alarm_e:
		nResult = ProcessAlarm(pAsioData);
		break;
	case J_AsioDataBase::j_rcd_e: case J_AsioDataBase::j_rcd_keep_e:
		nResult = ProcessRcd(pAsioData);
		break;
	}

	return nResult;
}

j_result_t CXlClient::SendRespInfo(void *pData)
{
	return J_OK;
}

j_result_t CXlClient::Broken()
{
	StopAll();
	JoDataBus->SubscribeMsg("all", xlc_msg_host, this, false);
	return J_OK;
}

j_int32_t CXlClient::GetState()
{
	if (time(0) - m_lastBreatTime > 30)
		return -1;

	return 0;
}

j_result_t CXlClient::SendContentInfo(j_char_t *pContent, j_int32_t nLen)
{
	char *pDataBuff = new char[sizeof(CmdHeader) + nLen + sizeof(CmdTail)];
	CXlHelper::MakeResponse(xlc_send_msg, (j_char_t *)pContent, nLen, (j_char_t *)pDataBuff);
	m_socket.Write_n((j_char_t *)pDataBuff, sizeof(CmdHeader) + nLen + sizeof(CmdTail));
	delete pDataBuff;

	return J_OK;
}

j_result_t CXlClient::MakeTransData(J_AsioDataBase *pAsioData)
{
	memset(&m_streamHeader, 0, sizeof(J_StreamHeader));
	m_ringBuffer.PopBuffer(m_dataBuff, m_streamHeader);
	pAsioData->ioWrite.buf = m_dataBuff;
	pAsioData->ioWrite.bufLen = m_streamHeader.dataLen;
	pAsioData->ioWrite.finishedLen = 0;
	pAsioData->ioWrite.whole = true;
	pAsioData->ioWrite.shared = true;

	if (m_streamHeader.dataLen > 0)
	{
		CmdHeader *pHeader = (CmdHeader *)m_dataBuff;
		//J_OS::LOGINFO("%d", pHeader->cmd);
		if (pHeader->beginCode == 0xFF && pHeader->cmd == xlc_playvod_complete_inner)
		{
			DevStopVod *pResp = (DevStopVod *)(m_dataBuff + sizeof(CmdHeader));

			pAsioData->ioWrite.bufLen = 0;
			SendMsgInfo(pResp->hostId, xlc_msg_dev, xlc_playvod_complete, pResp->channel);
		}
	}

	return J_OK;
}

j_result_t CXlClient::MakeAlarmData(J_AsioDataBase *pAsioData)
{
	memset(&m_alarmHeader, 0, sizeof(J_StreamHeader));
	m_alarmBuffer.PopBuffer(m_alarmBuff, m_alarmHeader);
	pAsioData->ioWrite.buf = m_alarmBuff;
	pAsioData->ioWrite.bufLen = m_alarmHeader.dataLen;
	pAsioData->ioWrite.finishedLen = 0;
	pAsioData->ioWrite.whole = true;
	pAsioData->ioWrite.shared = true;

	return J_OK;
}

j_result_t CXlClient::MakeRcdData(J_AsioDataBase *pAsioData)
{
	memset(&m_rcdHeader, 0, sizeof(J_StreamHeader));
	m_rcdBuffer.PopBuffer(m_rcdBuff, m_rcdHeader);
	pAsioData->ioWrite.buf = m_rcdBuff;
	pAsioData->ioWrite.bufLen = m_rcdHeader.dataLen;
	pAsioData->ioWrite.finishedLen = 0;
	pAsioData->ioWrite.whole = true;
	pAsioData->ioWrite.shared = true;

	if (pAsioData->ioWrite.bufLen > 0)
	{
		//J_OS::LOGINFO("CXlClient::MakeRcdData");
		CmdHeader *pHeader = (CmdHeader *)m_rcdBuff;
		if (pHeader->cmd == xlc_rcdinfo_complete_inner)
		{
			CliRcdInfo *pResp = (CliRcdInfo *)(m_rcdBuff + sizeof(CmdHeader));
			J_Host *pHost = JoDeviceManager->GetDeviceObj(pResp->hostId);
			if (pHost != NULL)
			{
				pHost->EnableRcdInfo(&m_rcdBuffer, false);
				pAsioData->ioType = J_AsioDataBase::j_rcd_keep_e;
			}
			pAsioData->ioWrite.bufLen = 0;
			//J_OS::LOGINFO("CXlClient::SendMsgInfo %d-%d", xlc_msg_dev, xlc_rcd_info_complete);
			SendMsgInfo(pResp->hostId, xlc_msg_dev, xlc_rcd_info_complete);
		}
	}

	return J_OK;
}

j_result_t CXlClient::ProcessRequest(J_AsioDataBase *pAsioData)	
{
	if (m_ioCmdState == xl_init_state)
	{
		pAsioData->ioCall = J_AsioDataBase::j_read_e;
		CXlHelper::MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));

		m_ioCmdState = xl_read_head_state;
	}
	else if (m_ioCmdState == xl_read_head_state)
	{
		CXlHelper::MakeNetData(pAsioData, m_readBuff + sizeof(CmdHeader), *((j_int32_t *)(m_readBuff + 4)) + sizeof(CmdTail));
		pAsioData->ioCall = J_AsioDataBase::j_read_e;

		m_ioCmdState = xl_read_data_state;
	}
	else if (m_ioCmdState == xl_read_data_state)
	{
		CmdHeader *pHeader = (CmdHeader *)m_readBuff;
		switch (pHeader->cmd)
		{
		case xlc_login:
			OnLogin(pAsioData);
			m_ioCmdState = xl_write_body_state;
			break;
		case xlc_logout:
			OnLogout(pAsioData);
			m_ioCmdState = xl_write_body_state;
			break;
		case xlc_heart_beat:
			OnHeartBeat(pAsioData);
			m_ioCmdState = xl_read_head_state;
			break;
		case xlc_start_real_view:
			OnRealPlay(pAsioData);
			m_ioCmdState = xl_read_head_state;
			break;
		case xlc_stop_real_view:
			OnRealStop(pAsioData);
			m_ioCmdState = xl_write_body_state;
			break;
		case xlc_start_vod_view:
			OnVodPlay(pAsioData);
			m_ioCmdState = xl_read_head_state;
			break;
		case xlc_stop_vod_view:
			OnVodStop(pAsioData);
			m_ioCmdState = xl_write_body_state;
			break;
		case xlc_start_real_alarm_info:
			OnStartAlarm(pAsioData);
			m_ioCmdState = xl_read_head_state;
			break;
		case xlc_stop_real_alarm_info:
			OnStopAlarm(pAsioData);
			m_ioCmdState = xl_write_body_state;
			break;
		case xlc_send_msg:
			OnSendMsg(pAsioData);
			m_ioCmdState = xl_write_body_state;
			break;
		case xlc_get_rcd_info:
			OnGetRctInfo(pAsioData);
			m_ioCmdState = xl_read_head_state;
			break;
		case xlc_start_upload:
			OnUploadStart(pAsioData);
			m_ioCmdState = xl_write_body_state;
			break;
		case xlc_uploading:
			OnUploadFile(pAsioData);
			m_ioCmdState = xl_read_head_state;
			break;
		case xlc_stop_upload:
			OnUploadStop(pAsioData);
			m_ioCmdState = xl_write_body_state;
			break;
		default:
			J_OS::LOGINFO("%d %X", pHeader->cmd, pHeader->flag & 0xFF);
			break;
		}
		//J_OS::LOGINFO("%d %X", pHeader->cmd, pHeader->flag & 0xFF);
	}
	else
	{	
		pAsioData->ioCall = J_AsioDataBase::j_read_e;
		CXlHelper::MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));

		m_ioCmdState = xl_read_head_state;
	}

	return J_OK;
}

j_result_t CXlClient::ProcessData(J_AsioDataBase *pAsioData)
{
	if (m_nRefCnt > 0)
	{
		if (m_ioDataState == xl_init_state)
		{
			pAsioData->ioType = J_AsioDataBase::j_data_e;
			//pAsioData->hEvent = WSACreateEvent();

			pAsioData->ioCall = J_AsioDataBase::j_write_e;
			MakeTransData(pAsioData);
			m_ioDataState = xl_write_body_state;
		}
		else
		{
			pAsioData->ioCall = J_AsioDataBase::j_write_e;
			MakeTransData(pAsioData);
			m_ioDataState = xl_write_body_state;
		}
	}
	else
	{
		pAsioData->ioCall = J_AsioDataBase::j_keep_e;
	}
	return J_OK;
}

j_result_t CXlClient::ProcessAlarm(J_AsioDataBase *pAsioData)
{
	j_result_t nResult = J_OK;
	if (m_nAlarmRefCnt > 0)
	{
		if (m_ioAlarmState == xl_init_state)
		{
			pAsioData->ioType = J_AsioDataBase::j_alarm_e;
			//pAsioData->hEvent = WSACreateEvent();

			pAsioData->ioCall = J_AsioDataBase::j_write_e;
			m_ioAlarmState = xl_write_body_state;
			nResult = MakeAlarmData(pAsioData);
		}
		else
		{
			pAsioData->ioCall = J_AsioDataBase::j_write_e;
			m_ioAlarmState = xl_write_body_state;
			nResult = MakeAlarmData(pAsioData);
		}
	}
	else
	{
		pAsioData->ioCall = J_AsioDataBase::j_keep_e;
	}

	return nResult;
}

j_result_t CXlClient::ProcessRcd(J_AsioDataBase *pAsioData)
{
	j_result_t nResult = J_OK;
	if (m_ioRcdState == xl_init_state)
	{
		pAsioData->ioType = J_AsioDataBase::j_rcd_e;
		//pAsioData->hEvent = WSACreateEvent();

		pAsioData->ioCall = J_AsioDataBase::j_write_e;
		m_ioRcdState = xl_write_body_state;
		nResult = MakeRcdData(pAsioData);
	}
	else
	{
		if (pAsioData->ioType == J_AsioDataBase::j_rcd_keep_e)
		{
			m_ioRcdState = xl_init_state;
			pAsioData->ioCall = J_AsioDataBase::j_keep_e;
		}
		else
		{
			pAsioData->ioCall = J_AsioDataBase::j_write_e;
			m_ioRcdState = xl_write_body_state;
			nResult = MakeRcdData(pAsioData);
		}
	}

	return nResult;
}

j_result_t CXlClient::StartView(const CliRealPlay &realPlay, J_AsioDataBase *pAsioData)
{
	J_Host *pHost = JoDeviceManager->GetDeviceObj(realPlay.hostId);
	if (pHost != NULL)
	{
		J_Obj *pObj = NULL;
		pHost->MakeChannel(realPlay.channel, pObj);
		if (pObj != NULL)
		{
			J_Channel *pChannel = dynamic_cast<J_Channel *>(pObj);
			if (pChannel != NULL)
				pChannel->OpenStream(&m_ringBuffer);
		}
		else
		{
			J_OS::LOGINFO("CXlClient::StartView transmission");
		}

		if (pAsioData != NULL)
		{
			pAsioData->ioRead.buf = m_readBuff;
			pAsioData->ioRead.bufLen = pAsioData->ioRead.finishedLen + sizeof(CmdHeader);
			if (pHost->ParserRequest(pAsioData) == J_OK)
			{
				TLock(m_vecLocker);
				VideoInfo info = {0};
				info.strHost = realPlay.hostId;
				info.nChanId = realPlay.channel;
				info.bConnected = true;
				m_videoInfoVec.push_back(info);
				TUnlock(m_vecLocker);
			}
			return J_OK;
		}
		else
		{
			J_OS::LOGINFO(" pHost->ReConnect(%d)", realPlay.channel);
			if (pHost->ReConnect(realPlay.channel) == J_OK)
			{
				return J_OK;
			}
		}
	}
	return J_UNKNOW;
}

j_result_t CXlClient::StopView(const CliRealPlay &realPlay, J_AsioDataBase *pAsioData)
{
	J_Host *pHost = JoDeviceManager->GetDeviceObj(realPlay.hostId);
	if (pHost != NULL)
	{
		pAsioData->ioRead.buf = m_readBuff;
		pAsioData->ioRead.bufLen = pAsioData->ioRead.finishedLen + sizeof(CmdHeader);
		pHost->ParserRequest(pAsioData);

		J_Obj *pObj = NULL;
		pHost->MakeChannel(realPlay.channel, pObj);
		if (pObj != NULL)
		{
			J_Channel *pChannel = dynamic_cast<J_Channel *>(pObj);
			if (pChannel != NULL)
				pChannel->CloseStream(&m_ringBuffer);
		}
	}

	TLock(m_vecLocker);
	VideoInfoVec::iterator it = m_videoInfoVec.begin();
	J_OS::LOGINFO("CXlClient::StopView begin");
	for (; it!=m_videoInfoVec.end(); ++it)
	{
		J_OS::LOGINFO("CXlClient::StopView mid1");
		if (it->strHost == realPlay.hostId && it->nChanId == realPlay.channel)
		{
			J_OS::LOGINFO("CXlClient::StopView mid2");
			m_videoInfoVec.erase(it);
			break;
		}
	}
	J_OS::LOGINFO("CXlClient::StopView end");
	TUnlock(m_vecLocker);

	return J_OK;
}

j_result_t CXlClient::StartVod(const CliStartVod &startVod, J_AsioDataBase *pAsioData)
{
	J_Host *pHost = JoDeviceManager->GetDeviceObj(startVod.hostId);
	if (pHost != NULL)
	{
		J_Obj *pObj = NULL;
		pHost->MakeChannel(startVod.channel, pObj);
		if (pObj != NULL)
		{
			J_Vod *pVod = dynamic_cast<J_Vod *>(pObj);
			if (pVod != NULL)
				pVod->OpenVod(startVod.sessionId, &m_ringBuffer);
		}

		pAsioData->ioRead.buf = m_readBuff;
		pAsioData->ioRead.bufLen = pAsioData->ioRead.finishedLen + sizeof(CmdHeader);
		pHost->ParserRequest(pAsioData);

		TLock(m_vodLocker);
		VodInfo info = {0};
		info.sessionId = startVod.sessionId;
		info.nChanId = startVod.channel;
		info.strHost = startVod.hostId;
		m_vodInfoVec.push_back(info);
		TUnlock(m_vodLocker);
	}
	return J_OK;
}

j_result_t CXlClient::StopVod(const CliStopVod &stopVod, J_AsioDataBase *pAsioData)
{
	J_Host *pHost = JoDeviceManager->GetDeviceObj(stopVod.hostId);
	if (pHost != NULL)
	{
		pAsioData->ioRead.buf = m_readBuff;
		pAsioData->ioRead.bufLen = pAsioData->ioRead.finishedLen + sizeof(CmdHeader);
		pHost->ParserRequest(pAsioData);

		J_Obj *pObj = NULL;
		pHost->MakeChannel(stopVod.channel, pObj);
		if (pObj != NULL)
		{
			J_Vod *pVod = dynamic_cast<J_Vod *>(pObj);
			if (pVod != NULL)
				pVod->CloseVod(stopVod.sessionId, &m_ringBuffer);
		}

		TLock(m_vodLocker);
		VodInfoVec::iterator it = m_vodInfoVec.begin();
		for (; it!=m_vodInfoVec.end(); ++it)
		{
			if (it->sessionId == stopVod.sessionId)
			{
				m_vodInfoVec.erase(it);
				break;
			}
		}
		TUnlock(m_vodLocker);
	}
	return J_OK;
}

j_result_t CXlClient::StartAlarm(j_string_t strHostId)
{
	TLock(m_vecAlarmLocker);
	AlarmInfoVec::iterator it = m_alarmInfoVec.begin();
	for (; it!=m_alarmInfoVec.end(); ++it)
	{
		if (*it == strHostId)
		{
			TUnlock(m_vecAlarmLocker);
			return J_OK;
		}
	}
	J_Host *pHost = JoDeviceManager->GetDeviceObj(strHostId.c_str());
	if (pHost != NULL)
	{
		pHost->EnableAlarm(&m_alarmBuffer);
		m_alarmInfoVec.push_back(strHostId);
		atomic_inc(&m_nAlarmRefCnt);
	}

	TUnlock(m_vecAlarmLocker);

	return J_OK;
}

j_result_t CXlClient::StopAlarm(j_string_t strHostId)
{
	TLock(m_vecAlarmLocker);
	AlarmInfoVec::iterator it = m_alarmInfoVec.begin();
	for (; it!=m_alarmInfoVec.end(); ++it)
	{
		if (*it == strHostId)
		{
			J_Host *pHost = JoDeviceManager->GetDeviceObj(strHostId.c_str());
			if (pHost != NULL)
				pHost->EnableAlarm(&m_alarmBuffer, false);
			m_alarmInfoVec.erase(it);
			if (m_nAlarmRefCnt > 0)
				atomic_dec(&m_nAlarmRefCnt);

			TUnlock(m_vecAlarmLocker);
			return J_OK;
		}
	}
	TUnlock(m_vecAlarmLocker);

	return J_OK;
}

j_result_t CXlClient::StopAll()
{
	StopAllReal();
	StopAllVod();
	StopAllAlarm();
	m_nAlarmRefCnt = 0;
	m_nRefCnt = 0;

	return J_OK;
}

j_result_t CXlClient::StopAllReal()
{
	J_Host *pHost = NULL;
	J_AsioDataBase asioData;
	struct RealPlayBody {
		CmdHeader head;
		DevRealPlay data;
		CmdTail tail;
	} realPlayBody;

	TLock(m_vecLocker);
	J_OS::LOGINFO("CXlClient::StopAllReal There is %d real video channel", m_videoInfoVec.size());

	VideoInfoVec::iterator it = m_videoInfoVec.begin();
	for (; it!=m_videoInfoVec.end(); ++it)
	{
		pHost = JoDeviceManager->GetDeviceObj(it->strHost.c_str());
		if (pHost != NULL)
		{
			memset(&realPlayBody, 0, sizeof(RealPlayBody));
			memcpy(realPlayBody.data.hostId, it->strHost.c_str(), strlen(it->strHost.c_str()));
			realPlayBody.data.channel = it->nChanId;
			CXlHelper::MakeRequest(xlc_stop_real_view, (char *)&realPlayBody.data, sizeof(DevRealPlay), (char *)&realPlayBody);

			asioData.ioType = J_AsioDataBase::j_command_e;
			asioData.ioRead.buf = (j_char_t *)&realPlayBody;
			asioData.ioRead.bufLen = sizeof(CmdHeader) + sizeof(DevRealPlay) + sizeof(CmdTail);
			pHost->ParserRequest(&asioData);

			J_Obj *pObj = NULL;
			pHost->MakeChannel(it->nChanId, pObj);
			if (pObj != NULL)
			{
				J_Channel *pChannel = dynamic_cast<J_Channel *>(pObj);
				if (pChannel != NULL)
				{
					pChannel->CloseStream(&m_ringBuffer);
				}
				else
				{
					J_OS::LOGINFO("CXlClient::StopAllReal channel obj issue");
				}
			}
			else
			{
				J_OS::LOGINFO("CXlClient::StopAllReal channel not found, channel_num = %d", it->nChanId);
			}
		}
		else
		{
			J_OS::LOGINFO("CXlClient::StopAllReal host not found, host_id = %s", it->strHost.c_str());
		}
	}
	m_videoInfoVec.clear();
	TUnlock(m_vecLocker);

	return J_OK;
}

j_result_t CXlClient::StopAllVod()
{
	J_Host *pHost = NULL;
	J_AsioDataBase asioData;

	struct VodStopBody {
		CmdHeader head;
		DevStopVod data;
		CmdTail tail;
	} vodStopBody;
	TLock(m_vodLocker);
	VodInfoVec::iterator it = m_vodInfoVec.begin();
	for (; it!=m_vodInfoVec.end(); ++it)
	{
		pHost = JoDeviceManager->GetDeviceObj(it->strHost.c_str());
		if (pHost != NULL)
		{
			memset(&vodStopBody, 0, sizeof(VodStopBody));
			memcpy(vodStopBody.data.hostId, it->strHost.c_str(), strlen(it->strHost.c_str()));
			vodStopBody.data.sessionId = it->sessionId;
			vodStopBody.data.channel = it->nChanId;
			CXlHelper::MakeRequest(xlc_stop_vod_view, (char *)&vodStopBody.data, sizeof(DevStopVod), (char *)&vodStopBody);

			asioData.ioType = J_AsioDataBase::j_command_e;
			asioData.ioRead.buf = (j_char_t *)&vodStopBody;
			asioData.ioRead.bufLen = sizeof(CmdHeader) + sizeof(DevStopVod) + sizeof(CmdTail);
			pHost->ParserRequest(&asioData);

			J_Obj * pObj = NULL;
			pHost->MakeChannel(it->nChanId, pObj);
			if (pObj != NULL)
			{
				J_Vod *pVod = dynamic_cast<J_Vod *>(pObj);
				if (pVod != NULL)
					pVod->CloseVod(it->sessionId, &m_ringBuffer);
			}
		}
	}
	m_videoInfoVec.clear();
	TUnlock(m_vecLocker);

	return J_OK;
}

j_result_t CXlClient::StopAllAlarm()
{
	J_Host *pHost = NULL;
	J_AsioDataBase asioData;
	TLock(m_vecAlarmLocker);
	AlarmInfoVec::iterator it = m_alarmInfoVec.begin();
	for (; it!=m_alarmInfoVec.end(); ++it)
	{
		pHost = JoDeviceManager->GetDeviceObj(it->c_str());
		if (pHost != NULL)
		{
			pHost->EnableAlarm(&m_alarmBuffer, false);
		}
	}
	m_alarmInfoVec.clear();
	TUnlock(m_vecAlarmLocker);

	return J_OK;
}

j_result_t CXlClient::OnLogin(J_AsioDataBase *pAsioData)
{
	CliUserLogin *pLoginInfo = (CliUserLogin *)(m_readBuff + sizeof(CmdHeader));
	int nBodyLen = sizeof(CmdHeader) + sizeof(CliRetValue2) + sizeof(CmdTail);
	CliRetValue2 data = {0};
	JoClientManager->Login(pLoginInfo->userName, pLoginInfo->passWord, pLoginInfo->nForced, data.nRetVal, this);
	memset(m_userName, 0, sizeof(m_userName));
	strcpy(m_userName, pLoginInfo->userName);
	if (data.nRetVal == J_OK)
	{
		JoDataBus->SubscribeMsg("all", xlc_msg_host, this);
	}
	else
	{
		// 登陆失败处理
	}
	J_OS::LOGINFO("CXlClient::OnLogin Return = %d", data.nRetVal);
	CXlHelper::MakeResponse(xlc_login, (j_char_t *)&data, sizeof(CliRetValue2), m_writeBuff);
	pAsioData->ioCall = J_AsioDataBase::j_write_e;
	CXlHelper::MakeNetData(pAsioData, m_writeBuff, nBodyLen);

	return J_OK;
}

j_result_t CXlClient::OnLogout(J_AsioDataBase *pAsioData)
{
	JoClientManager->Logout(m_userName, this);
	int nBodyLen = sizeof(CmdHeader) + sizeof(CliRetValue2) + sizeof(CmdTail);
	CliRetValue2 data = {0};
	data.nRetVal = 0x00;
	CXlHelper::MakeResponse(xlc_logout, (j_char_t *)&data, sizeof(CliRetValue2), m_writeBuff);
	pAsioData->ioCall = J_AsioDataBase::j_write_e;
	CXlHelper::MakeNetData(pAsioData, m_writeBuff, nBodyLen);

	return J_OK;
}

j_result_t CXlClient::OnHeartBeat(J_AsioDataBase *pAsioData)
{
	m_lastBreatTime = time(0);
	CXlHelper::MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	pAsioData->ioCall = J_AsioDataBase::j_read_e;
	//J_OS::LOGINFO("CXlClient::OnHeartBeat");

	return J_OK;
}

j_result_t CXlClient::OnRealPlay(J_AsioDataBase *pAsioData)
{
	CliRealPlay *pReps = (CliRealPlay *)(m_readBuff + sizeof(CmdHeader));
	J_OS::LOGINFO("CXlClient::OnRealPlay hostId = %s, channel = %d", pReps->hostId, pReps->channel & 0xFF);

	if (StartView(*pReps, pAsioData) != J_OK)
	{
		J_OS::LOGINFO("CXlClient::OnRealPlay StartView failed");
	}

	pAsioData->ioCall = J_AsioDataBase::j_read_e;
	CXlHelper::MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	if (m_nRefCnt == 0)
		pAsioData->ioCall = J_AsioDataBase::j_read_write_e;
	atomic_inc(&m_nRefCnt);

	return J_OK;
}

j_result_t CXlClient::OnRealStop(J_AsioDataBase *pAsioData)
{
	CliRealPlay *pReps = (CliRealPlay *)(m_readBuff + sizeof(CmdHeader));
	J_OS::LOGINFO("CXlClient::OnRealStop hostId = %s, channel = %d", pReps->hostId, pReps->channel & 0xFF);

	StopView(*pReps, pAsioData);

	int nBodyLen = sizeof(CmdHeader) + sizeof(CliRealViewRetValue) + sizeof(CmdTail);
	CliRealViewRetValue data = {0};
	strcpy(data.pHostId, pReps->hostId);
	data.nChannelId = pReps->channel;
	data.nRetVal = 0x00;
	CXlHelper::MakeResponse(xlc_stop_real_view, (j_char_t *)&data, sizeof(CliRealViewRetValue), m_writeBuff);
	pAsioData->ioCall = J_AsioDataBase::j_write_e;
	CXlHelper::MakeNetData(pAsioData, m_writeBuff, nBodyLen);
	if (m_nRefCnt > 0)
		atomic_dec(&m_nRefCnt);

	return J_OK;
}

j_result_t CXlClient::OnVodPlay(J_AsioDataBase *pAsioData)
{
	CliStartVod *pReps = (CliStartVod *)(m_readBuff + sizeof(CmdHeader));
	J_OS::LOGINFO("CXlClient::OnVodPlay hostId = %s, channel = %d %I64d %I64d", pReps->hostId, pReps->channel & 0xFF,
		pReps->tmStartTime, pReps->tmEndTime);
	StartVod(*pReps, pAsioData);
	pAsioData->ioCall = J_AsioDataBase::j_read_e;
	CXlHelper::MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	if (m_nRefCnt == 0)
		pAsioData->ioCall = J_AsioDataBase::j_read_write_e;
	atomic_inc(&m_nRefCnt);

	return J_OK;
}

j_result_t CXlClient::OnVodStop(J_AsioDataBase *pAsioData)
{
	CliStopVod *pReps = (CliStopVod *)(m_readBuff + sizeof(CmdHeader));
	J_OS::LOGINFO("CXlClient::OnVodStop hostId = %s, channel = %d", pReps->hostId, pReps->channel & 0xFF);
	StopVod(*pReps, pAsioData);

	int nBodyLen = sizeof(CmdHeader) + sizeof(CliVodStopRetValue) + sizeof(CmdTail);
	CliVodStopRetValue data = {0};
	data.sessionId = pReps->sessionId;
	strcpy(data.pHostId, pReps->hostId);
	data.nChannelId = pReps->channel;
	data.nRetVal = 0x00;
	CXlHelper::MakeResponse(xlc_stop_vod_view, (j_char_t *)&data, sizeof(CliVodStopRetValue), m_writeBuff);
	pAsioData->ioCall = J_AsioDataBase::j_write_e;
	CXlHelper::MakeNetData(pAsioData, m_writeBuff, nBodyLen);
	if (m_nRefCnt > 0)
		atomic_dec(&m_nRefCnt);

	return J_OK;
}

j_result_t CXlClient::OnStartAlarm(J_AsioDataBase *pAsioData)
{
	CliRealAlarmInfo *pReps = (CliRealAlarmInfo *)(m_readBuff + sizeof(CmdHeader));
	J_OS::LOGINFO("CXlClient::OnStartAlarm hostId = %s", pReps->hostId);
	pAsioData->ioCall = J_AsioDataBase::j_read_e;
	CXlHelper::MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	if (m_ioAlarmState == xl_init_state)
	{
		StartAlarm(pReps->hostId);
		pAsioData->ioCall = J_AsioDataBase::j_read_write_alarm_e;
		m_ioAlarmState = xl_write_body_state;
	}

	return J_OK;
}

j_result_t CXlClient::OnStopAlarm(J_AsioDataBase *pAsioData)
{
	CliRealAlarmInfo *pReps = (CliRealAlarmInfo *)(m_readBuff + sizeof(CmdHeader));
	J_OS::LOGINFO("CXlClient::OnStopAlarm hostId = %s", pReps->hostId);
	int nBodyLen = sizeof(CmdHeader) + sizeof(CliRetValue) + sizeof(CmdTail);
	CliRetValue data = {0};
	strcpy(data.pHostId, pReps->hostId);
	data.nRetVal = 0x00;
	CXlHelper::MakeResponse(xlc_stop_real_alarm_info, (j_char_t *)&data, sizeof(CliRetValue), m_writeBuff);
	pAsioData->ioCall = J_AsioDataBase::j_write_e;
	CXlHelper::MakeNetData(pAsioData, m_writeBuff, nBodyLen);

	StopAlarm(pReps->hostId);
	m_ioAlarmState = xl_init_state;

	return J_OK;
}

/// 客户端向车载设备发送文本消息
j_result_t CXlClient::OnSendMsg(J_AsioDataBase *pAsioData)
{
	CmdHeader *pHeader = (CmdHeader *)m_readBuff;
	char *pContent = (char *)(m_readBuff + sizeof(CmdHeader));
	boost::property_tree::ptree pt;												//定义一个存放xml的容器指针
	std::istrstream iss(pContent, pHeader->length);	
	boost::property_tree::read_xml(iss, pt);									//读取内存中的数据 入口在pt这个指针
	j_string_t hostId = pt.get<j_string_t>("message.vehicleid");			//获取message下一层id的值
	
	//J_OS::LOGINFO("CXlClient::OnSendMsg = %s", pContent);
	J_Host *pHost = JoDeviceManager->GetDeviceObj(hostId);
	if (pHost != NULL)
		pHost->SendMessage(pContent, pHeader->length);

	int nBodyLen = sizeof(CmdHeader) + sizeof(CliRetValue) + sizeof(CmdTail);
	CliRetValue data = {0};
	strcpy(data.pHostId, hostId.c_str());
	data.nRetVal = 0x00;
	CXlHelper::MakeResponse(xlc_send_msg, (j_char_t *)&data, sizeof(CliRetValue), m_writeBuff);
	pAsioData->ioCall = J_AsioDataBase::j_write_e;
	CXlHelper::MakeNetData(pAsioData, m_writeBuff, nBodyLen);

	return J_OK;
}

j_result_t CXlClient::OnGetRctInfo(J_AsioDataBase *pAsioData)
{
	CliRcdInfo *pReps = (CliRcdInfo *)(m_readBuff + sizeof(CmdHeader));
	J_OS::LOGINFO("CXlClient::OnGetRctInfo hostId = %s", pReps->hostId);
	pAsioData->ioCall = J_AsioDataBase::j_read_e;
	CXlHelper::MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	if (m_ioRcdState == xl_init_state)
	{
		J_Host *pHost = JoDeviceManager->GetDeviceObj(pReps->hostId);
		if (pHost != NULL)
		{
			pHost->EnableRcdInfo(&m_rcdBuffer);
			pAsioData->ioRead.buf = m_readBuff;
			pAsioData->ioRead.bufLen = pAsioData->ioRead.finishedLen + sizeof(CmdHeader);
			pHost->ParserRequest(pAsioData);
		}

		pAsioData->ioCall = J_AsioDataBase::j_read_write_rcd_e;
		m_ioAlarmState = xl_write_body_state;
	}

	return J_OK;
}

j_result_t CXlClient::OnUploadStart(J_AsioDataBase *pAsioData)
{
	//sNum = 0;
	CliUploadStart *pReq= (CliUploadStart *)(m_readBuff + sizeof(CmdHeader));
	char hostId[32] = {0};
	char fileName[512] = {0};
	WideCharToMultiByte(CP_OEMCP, NULL, pReq->szID, -1, hostId, 32, NULL, FALSE);
	WideCharToMultiByte(CP_OEMCP, NULL, pReq->szFileName, -1, fileName, 512, NULL, FALSE);
	J_Host *pHost = JoDeviceManager->GetDeviceObj(hostId);
	//J_OS::LOGINFO("CXlClient::OnUploadStart %s %d", hostId, ++sNum);
	if (pHost != NULL)
	{
		//J_OS::LOGINFO("CXlClient::OnUploadStart %s", pReq->szID);
		pHost->OnStartUpload(pReq->nUserId, pReq->nFileId, fileName);
		m_uploadFile = fileName;
		m_pUploadDev = pHost;
	}

	int nBodyLen = sizeof(CmdHeader) + sizeof(CliRetValue2) + sizeof(CmdTail);
	CliRetValue2 data = {0};
	data.nRetVal = 0x00;
	CXlHelper::MakeResponse(xlc_start_upload, (j_char_t *)&data, sizeof(CliRetValue2), m_writeBuff);
	pAsioData->ioCall = J_AsioDataBase::j_write_e;
	CXlHelper::MakeNetData(pAsioData, m_writeBuff, nBodyLen);

	return J_OK;
}

j_result_t CXlClient::OnUploadFile(J_AsioDataBase *pAsioData)
{
	CmdHeader *pReq= (CmdHeader *)m_readBuff;
	DevEquipmentId *pHostId = (DevEquipmentId *)(m_readBuff + sizeof(CmdHeader));
	J_Host *pHost = JoDeviceManager->GetDeviceObj(pHostId->hostId);
	//J_OS::LOGINFO("%s %d", pHostId->hostId, *(int*)(m_readBuff + sizeof(CmdHeader) + sizeof(DevEquipmentId)));
	if (pHost != NULL)
	{
		//J_OS::LOGINFO("%d", pReq->length - sizeof(DevEquipmentId));
		pHost->OnUploading(0, m_readBuff + sizeof(CmdHeader) + sizeof(DevEquipmentId), pReq->length - sizeof(DevEquipmentId));
	}
	memset(m_readBuff, 0, XL_CMD_BUFFER_SIZE);
	CXlHelper::MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	pAsioData->ioCall = J_AsioDataBase::j_read_e; 

	return J_OK;
}

j_result_t CXlClient::OnUploadStop(J_AsioDataBase *pAsioData)
{
	CliUploadStop *pReq= (CliUploadStop *)(m_readBuff + sizeof(CmdHeader));
	J_Host *pHost = JoDeviceManager->GetDeviceObj(pReq->szID);
	if (pHost != NULL)
	{
		J_OS::LOGINFO("CXlClient::OnUploadStop %s", pReq->szID);
		pHost->OnStopUpload(pReq->nFileId, pReq->szCheck);
		m_pUploadDev = NULL;
		m_uploadFile.clear();
	}

	int nBodyLen = sizeof(CmdHeader) + sizeof(CliRetValue2) + sizeof(CmdTail);
	CliRetValue2 data = {0};
	data.nRetVal = 0x00;
	CXlHelper::MakeResponse(xlc_stop_upload, (j_char_t *)&data, sizeof(CliRetValue2), m_writeBuff);
	pAsioData->ioCall = J_AsioDataBase::j_write_e;
	CXlHelper::MakeNetData(pAsioData, m_writeBuff, nBodyLen);

	return J_OK;
}