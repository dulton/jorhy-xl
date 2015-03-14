///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx科技有限公司(版权声明) 
/// All rights reserved. 
/// 
/// @file      XlHost.cpp 
/// @brief     XL_DVR设备模块实现
///
/// @version   1.0 (版本声明)
/// @author    Jorhy (joorhy@gmail.com) 
/// @date      2013/09/22 17:41 
///
///
/// 修订说明：最初版本
/////////////////////////////////////////////////////////////////////////// 
#include "XlHost.h"
#include "XlDType.h"
#include "XlChannel.h"
#include "MsSqlServer.h"
#include "XlHelper.h"
#include "ClientManager.h"
#include "TcpServer4Device.h"
#include "DataBus.h"

#include <iostream>
#include <strstream>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/lexical_cast.hpp>  

extern CTcpServer4Device g_deviceServer;

CXlHost::CXlHost(j_socket_t nSock)
: m_cmdSocket(nSock)
, m_uploadRingBuffer(0, 1024 * 1024 * 30)
{
	m_bReady = false;
	m_lastBreatTime = time(0);

	m_readBuff = new char[BUFFER_SIZE];			
	m_writeBuff = new char[BUFFER_SIZE];	
	m_rcdBuffer = new char[1024];
	m_fileBuffer = new char[1024 * 1024];
	m_ioState = xl_init_state;
}

CXlHost::~CXlHost()
{
	if (m_readBuff != NULL)
	{
		delete m_readBuff;
		m_readBuff = NULL;
	}
	if (m_writeBuff != NULL)
	{
		delete m_writeBuff;
		m_writeBuff = NULL;
	}
	if (m_rcdBuffer != NULL)
	{
		delete m_rcdBuffer;
		m_rcdBuffer = NULL;
	}
	if (m_fileBuffer != NULL)
	{
		delete m_fileBuffer;
		m_fileBuffer = NULL;
	}
	DevHostInfo devInfo = {0};
	strcpy(devInfo.hostId, m_hostId.c_str());
	//devInfo.bOnline = false;
	JoDataBaseObj->UpdateDevInfo(devInfo, false);
	J_OS::LOGINFO("CXlHost::~CXlHost()");
}

j_result_t CXlHost::MakeChannel(const j_int32_t nChannelNum, J_Obj *&pObj)
{
	J_Obj *pChannel = NULL;
	ChannelMap::iterator it = m_channelMap.find(nChannelNum);
	if (it != m_channelMap.end())
	{
		pChannel = it->second.pChannel;
	}
	else
	{
		pChannel = new CXlChannel(this, nChannelNum);

		TLock(m_channelLocker);
		ChannelInfo info = {0};
		info.pChannel = pChannel;
		info.nRef = 0;
		m_channelMap[nChannelNum] = info;
		TUnlock(m_channelLocker);
	}
	pObj = pChannel;
	return J_OK;
}

j_boolean_t CXlHost::IsReady()
{
	if (time(0) - m_lastBreatTime > 120)
	{
		m_bReady = false;
		J_OS::LOGINFO("CXlHost::IsReady() TIME_OUT %d - %d", time(0) - m_lastBreatTime);
	}

	return m_bReady;
}

j_result_t CXlHost::OnBroken()
{
	J_OS::LOGINFO("CXlHost::OnBroken() %s", m_hostId.c_str());
	TLock(m_fileUploadLocker);
	try 
	{
		VecFileUploadInfo::iterator it = m_vecFileUploadInfo.begin();
		for (; it!=m_vecFileUploadInfo.end(); ++it)
		{
			JoDataBaseObj->UpdateFileInfo(it->nFileId,  2, false);
			j_string_t userName;
			if (JoDataBaseObj->GetUserNameById(it->nUserId, userName) == J_OK)
			{
				J_Client *pClient = dynamic_cast<J_Client *>(JoClientManager->GetClientObj(userName));
				if (pClient)
				{
					pClient->SendMsgInfo(m_hostId, xlc_msg_upload, xlc_upload_failed);
				}
			}
		}
		m_vecFileUploadInfo.clear();
	}
	catch(...)
	{
		J_OS::LOGINFO("CXlHost::OnBroken() Exception");
	}
	TUnlock(m_fileUploadLocker);
	JoDataBus->OnMessage(m_hostId, xlc_msg_host, xlc_host_broken);
	JoDataBus->RegisterDevice(m_hostId, NULL);
	//TLock(m_vecClientLocker);
	//std::vector<J_Client *>::iterator it2 = m_vecClient.begin();
	//for (; it2!=m_vecClient.end(); ++it2)
	//{
	//	J_Client *pClient = dynamic_cast<J_Client *>(*it2);
	//	if (pClient != NULL)
	//	{
	//		pClient->SendMsgInfo(m_hostId, xlc_msg_host, xlc_host_broken);
	//	}
	//}
	//TUnlock(m_vecClientLocker);
	J_OS::LOGINFO("CXlHost::OnBroken() end");

	return J_OK;
}

j_result_t CXlHost::GetHostId(j_string_t &strDevId)
{
	strDevId = m_hostId;
	return J_OK;
}

j_result_t CXlHost::SetTime(j_time_t sysTime)
{
	struct SysTimeBody {
		CmdHeader head;
		DevSetTime data;
		CmdTail tail;
	} sysTimeBody;
	sysTimeBody.data.systime = sysTime;
	CXlHelper::MakeRequest(xld_set_time, (char *)&sysTimeBody.data, sizeof(DevSetTime), (char *)&sysTimeBody);
	m_cmdSocket.Write_n((char *)&sysTimeBody, sizeof(SysTimeBody));

	return J_OK;
}

j_result_t CXlHost::GetDeviceInfo()
{
	struct DevInfoBody {
		CmdHeader head;
		CmdTail tail;
	} devInfoBody;
	CXlHelper::MakeRequest(xld_get_devinfo, NULL, 0, (char *)&devInfoBody);
	m_cmdSocket.Write_n((char *)&devInfoBody, sizeof(DevInfoBody));

	struct LogInfoBody
	{
		CmdHeader head;
		DevGetLogInfo data;
		CmdTail tail;
	} logInfoBody;
	memset(&logInfoBody, 0, sizeof(LogInfoBody));
	time_t logLastTime = JoDataBaseObj->GetDevLogLastTime(m_hostId.c_str());
	if (logLastTime == 0)
	{
		logInfoBody.data.tmStart = -1;
		logInfoBody.data.tmEnd = -1;
	}
	else
	{
		logInfoBody.data.tmStart = logLastTime;
		logInfoBody.data.tmEnd = time(0);
	}
	CXlHelper::MakeRequest(xld_get_loginfo, (char *)&logInfoBody.data, sizeof(DevGetLogInfo), (char *)&logInfoBody);
	m_cmdSocket.Write_n((char *)&logInfoBody, sizeof(LogInfoBody));

	return J_OK;
}

j_result_t CXlHost::EnableRcdInfo(CRingBuffer *pRingBuffer, j_boolean_t bEnable)
{
	if (bEnable)
	{
		///清除所有记录
		TLock(m_vecRcdLocker);
		m_vecRcdRingBuffer.clear();
		//J_OS::LOGINFO("m_vecRcdRingBuffer.clear = %d", m_vecRcdRingBuffer.size());
		TUnlock(m_vecRcdLocker);
		///添加新记录
		AddRingBuffer(m_vecRcdLocker, m_vecRcdRingBuffer, pRingBuffer);
		//J_OS::LOGINFO("m_vecRcdRingBuffer.size = %d", m_vecRcdRingBuffer.size());
	}
	else
	{
		///删除记录
		DelRingBuffer(m_vecRcdLocker, m_vecRcdRingBuffer, pRingBuffer);
		//J_OS::LOGINFO("m_vecRcdRingBuffer.DelRingBuffer = %d", m_vecRcdRingBuffer.size());
	}
	return J_OK;
}

j_result_t CXlHost::EnableAlarm(CRingBuffer *pRingBuffer, j_boolean_t bEnable)
{
	if (bEnable)
	{
		AddRingBuffer(m_vecLocker, m_vecRingBuffer, pRingBuffer);
	}
	else
	{
		DelRingBuffer(m_vecLocker, m_vecRingBuffer, pRingBuffer);
	}
	return J_OK;
}

j_result_t CXlHost::ParserRequest(J_AsioDataBase *pAsioData)
{
	if (pAsioData->ioType == J_AsioDataBase::j_data_e)
	{
		ProcessDeviceCmd(pAsioData);
	}
	else if (pAsioData->ioType == J_AsioDataBase::j_upload_e)
	{
		ProcessUploadData(pAsioData);
	}
	else
	{
		ProcessClientCmd(pAsioData);
	}

	return J_OK;
}

/// 向车载设备发送消息
 j_result_t CXlHost::SendMessage(j_char_t *pData, j_int32_t nLen)
 {
	 j_int32_t nBodyLen = sizeof(CmdHeader) + nLen + sizeof(CmdTail);
	 j_char_t *pBuffer = new j_char_t[nBodyLen];
	 CXlHelper::MakeRequest(xld_send_message, pData, nLen, pBuffer);
	 m_cmdSocket.Write_n(pBuffer, nBodyLen);
	 delete pBuffer;

	 return J_OK;
 }

j_result_t CXlHost::ProcessClientCmd(J_AsioDataBase *pAsioData)
{
	j_char_t *pDataBuff = pAsioData->ioRead.buf;
	CmdHeader *pHeader = (CmdHeader *)pDataBuff;
	switch (pHeader->cmd)
	{
	case xlc_start_real_view:
		{
			CliRealPlay *pRealPlay = (CliRealPlay *)(pDataBuff + sizeof(CmdHeader));
			ReqRealPlay(pRealPlay->channel);
		}
		break;
	case xlc_stop_real_view:
		{
			CliRealPlay *pRealPlay = (CliRealPlay *)(pDataBuff + sizeof(CmdHeader));
			J_OS::LOGINFO("CXlHost::ProcessClientCmd client invoke ReqRealStop");
			ReqRealStop(pRealPlay->channel);
		}
		break;
	case xlc_start_vod_view:
		{
			CliStartVod*pStartVod = (CliStartVod *)(pDataBuff + sizeof(CmdHeader));
			J_OS::LOGINFO("CXlHost::ProcessClientCmd client invoke ReqVodPlay");
			ReqVodPlay(pStartVod->sessionId, pStartVod->channel, pStartVod->tmStartTime, pStartVod->tmEndTime);
		}
		break;
	case xlc_stop_vod_view:
		{
			CliStopVod*pStopVod = (CliStopVod *)(pDataBuff + sizeof(CmdHeader));
			ReqVodStop(pStopVod->sessionId, pStopVod->channel);
		}
		break;
	case xlc_get_rcd_info:
		{
			DevEquipmentId *pHostId = (DevEquipmentId *)(pDataBuff + sizeof(CmdHeader));
			ReqGetRcdInfo(pHostId->hostId);
		}
		break;
	default:
		break;
	}
	return J_OK;
}

j_result_t CXlHost::ProcessDeviceCmd(J_AsioDataBase *pAsioData)
{
	// 收到数据表示连接正常
	m_lastBreatTime = time(0);
	//J_OS::LOGINFO("%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X", m_readBuff[0] & 0xFF, m_readBuff[1] & 0xFF
	//	, m_readBuff[2] & 0xFF, m_readBuff[3] & 0xFF, m_readBuff[4] & 0xFF, m_readBuff[5] & 0xFF, m_readBuff[6] & 0xFF, m_readBuff[7] & 0xFF);
	if (m_ioState == xl_init_state)
	{
		CXlHelper::MakeRequest(xld_register, NULL, 0, (j_char_t *)m_writeBuff);
		pAsioData->ioCall = J_AsioDataBase::j_write_e;
		CXlHelper::MakeNetData(pAsioData, m_writeBuff, sizeof(CmdHeader) + sizeof(CmdTail));

		m_ioState = xl_write_body_state;
	}
	else if (m_ioState == xl_read_head_state)
	{
		pAsioData->ioCall = J_AsioDataBase::j_read_e;
		CmdHeader *pHeader = (CmdHeader *)m_readBuff;
		J_OS::LOGDEBUG("CXlHost::ProcessDeviceCmd dataLen = %d type = %d", pHeader->length, pHeader->cmd);
		CXlHelper::MakeNetData(pAsioData, m_readBuff + sizeof(CmdHeader), pHeader->length + sizeof(CmdTail));

		m_ioState = xl_read_data_state;
	}
	else if (m_ioState == xl_read_data_state)
	{
		CmdHeader *pHeader = (CmdHeader *)m_readBuff;
		switch (pHeader->cmd)
		{
		case xld_register:
			OnRegister(pAsioData);
			m_ioState = xl_read_head_state;
			break;
		case xld_send_message:
			OnSendMessage(pAsioData);
			m_ioState = xl_read_head_state;
			break;
		case xld_heartbeat:
			OnHeartBeat(pAsioData);
			m_ioState = xl_read_head_state;
			break;
		case xld_get_alraminfo:
			OnAlarmInfo(pAsioData);
			m_ioState = xl_read_head_state;
			break;
		case xld_real_play:
			OnRealPlayData(pAsioData, pHeader->length + sizeof(CmdHeader) + sizeof(CmdTail));
			m_ioState = xl_read_head_state;
			break;
		case xld_real_stop:
			OnRealStopData(pAsioData);
			m_ioState = xl_read_head_state;
			break;
		case xld_vod_play:
			OnVodPlayData(pAsioData, pHeader->length + sizeof(CmdHeader) + sizeof(CmdTail));
			m_ioState = xl_read_head_state;
			break;
		case xld_vod_stop:
			OnVodStopData(pAsioData);
			m_ioState = xl_read_head_state;
			break;
		case xld_msg_info:
			if (OnMsgInfo(pAsioData) == J_OK)
			{
				m_ioState = xl_read_head_state;
			}
			else
			{
				m_ioState = xl_error_state;
			}
			break;
		case xld_get_rcd_info:
			OnRcdInfoData(pAsioData);
			m_ioState = xl_read_head_state;
			break;
		case xld_set_time:
			OnSetSysTime(pAsioData);   
			m_ioState = xl_read_head_state;
			break;
		case xld_get_devinfo:
			OnGetDevInfo(pAsioData);   
			m_ioState = xl_read_head_state;
			break;
		case xld_get_loginfo:
			OnGetLogInfo(pAsioData);   
			m_ioState = xl_read_head_state;
			break;
		default:
			J_OS::LOGINFO("CXlHost::ProcessDeviceCmd cmd = %d flag = %X", pHeader->cmd, pHeader->flag & 0xFF);
			OnDefault(pAsioData);
			m_ioState = xl_read_head_state;
			break;
		}
	}
	else
	{
		pAsioData->ioCall = J_AsioDataBase::j_read_e;
		CXlHelper::MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
		pAsioData->ioCall = J_AsioDataBase::j_read_e;

		m_ioState = xl_read_head_state;
	}
	return J_OK;
}

j_result_t CXlHost::ProcessUploadData(J_AsioDataBase *pAsioData)
{
	J_StreamHeader streamHeader = {0};
	m_uploadRingBuffer.PopBuffer(m_writeBuff, streamHeader);
	pAsioData->ioWrite.buf = m_writeBuff;
	pAsioData->ioWrite.bufLen = streamHeader.dataLen;
	pAsioData->ioWrite.finishedLen = 0;
	pAsioData->ioWrite.whole = true;
	pAsioData->ioWrite.shared = true;

	if (streamHeader.frameType == 99 || streamHeader.frameType == 0)
	{
		pAsioData->ioCall = J_AsioDataBase::j_write_e;
		if (streamHeader.dataLen > 0)
		{
			//static int s_totlelen = 0;
			//s_totlelen += streamHeader.dataLen;
			//printf("%d \n", s_totlelen);
		}
	}
	else
	{
		pAsioData->ioCall = J_AsioDataBase::j_write_end_e;
		//printf("end \n");
	}

	return J_OK;
}

j_result_t CXlHost::OnRegister(J_AsioDataBase *pAsioData)
{
	DevEquipmentId *pResp = (DevEquipmentId *)(m_readBuff + sizeof(CmdHeader));
	m_hostId = pResp->hostId;
	m_bReady = true;

	J_OS::LOGINFO("CXlHost::OnRegister hostId =%s", pResp->hostId);
	CXlHelper::MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	pAsioData->ioCall = J_AsioDataBase::j_read_e;
	return J_OK;
}

j_result_t CXlHost::OnSendMessage(J_AsioDataBase *pAsioData)
{
	CmdHeader *pHeader = (CmdHeader *)m_readBuff;
	char *pContent = (char *)(m_readBuff + sizeof(CmdHeader));
	boost::property_tree::ptree pt;														//定义一个存放xml的容器指针
	std::istrstream iss(pContent, pHeader->length);	
	boost::property_tree::read_xml(iss, pt);											//读取内存中的数据 入口在pt这个指针
	j_string_t userId = pt.get<j_string_t>("message.userid");			//获取message下一层id的值
	j_string_t strType = pt.get<j_string_t>("message.type");			//获取message下一层id的值

	int nUserId = boost::lexical_cast<int>(userId.c_str());
	if (strType == "FILE_COPY")
	{
		j_string_t fileId = pt.get<j_string_t>("message.breserver");				//获取message下一层id的值
		int nFileId = boost::lexical_cast<int>(fileId.c_str());
		J_OS::LOGINFO("CXlHost::OnSendMessage file id = %d", nFileId);
		JoDataBaseObj->UpdateFileInfo(nFileId,  3);
	}
	
	j_string_t userName;
	J_OS::LOGINFO("CXlHost::OnSendMessage user id = %d", nUserId);
	if (JoDataBaseObj->GetUserNameById(nUserId, userName) == J_OK)
	{
		J_Client *pClient = JoClientManager->GetClientObj(userName);
		if (pClient)
		{
			pClient->SendContentInfo(pContent, pHeader->length);
		}
	}
	CXlHelper::MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	pAsioData->ioCall = J_AsioDataBase::j_read_e;

	return J_OK;
}

j_result_t CXlHost::OnHeartBeat(J_AsioDataBase *pAsioData)
{
	m_lastBreatTime = time(0);
	CXlHelper::MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	pAsioData->ioCall = J_AsioDataBase::j_read_e;
	J_OS::LOGDEBUG("CXlHost::OnHeartBeat %s %d %d", m_hostId.c_str(), m_cmdSocket.GetHandle().sock, m_lastBreatTime);
	
	return J_OK;
}

j_result_t CXlHost::OnAlarmInfo(J_AsioDataBase *pAsioData)
{
	DevAlarmInfo *pAlarmInfo = (DevAlarmInfo *)(m_readBuff + sizeof(CmdHeader));
	//J_OS::LOGDEBUG("%I64d GPS(%f %f %f) %f %d", pAlarmInfo->bAlarm & 0xFF, pAlarmInfo->gps.dLatitude, pAlarmInfo->gps.dLongitude,
	//	pAlarmInfo->gps.dGPSSpeed, pAlarmInfo->speed, pAlarmInfo->tmTimeStamp);
	if (m_bReady)
	{
		JoDataBaseObj->InsertAlarmInfo(m_hostId.c_str(), *pAlarmInfo);
		CliAlarmInfo cliAlarmInfo = {0};
		strcpy((char *)cliAlarmInfo.pHostId, m_hostId.c_str());
		//memcpy((((char *)&cliAlarmInfo)+sizeof(cliAlarmInfo.pHostId)), pAlarmInfo, sizeof(CliAlarmInfo));
		cliAlarmInfo.bAlarm = pAlarmInfo->bAlarm & 0xFF;
		//J_OS::LOGINFO("%d %s", cliAlarmInfo.bAlarm, cliAlarmInfo.pHostId);
		cliAlarmInfo.gps.dLatitude = pAlarmInfo->dLatitude;
		cliAlarmInfo.gps.dLongitude = pAlarmInfo->dLongitude;
		cliAlarmInfo.gps.dGPSSpeed = pAlarmInfo->dGPSSpeed;
		cliAlarmInfo.speed = pAlarmInfo->dGPSSpeed;
		cliAlarmInfo.tmTimeStamp = pAlarmInfo->tmTimeStamp;
		CXlHelper::MakeResponse2(xlc_start_real_alarm_info, (char *)&cliAlarmInfo, sizeof(CliAlarmInfo), m_readBuff);
		J_StreamHeader streamHeader = {0};
		streamHeader.dataLen = sizeof(CmdHeader) + sizeof(CliAlarmInfo) + sizeof(CmdTail);
		TLock(m_vecLocker);
		std::vector<CRingBuffer *>::iterator it = m_vecRingBuffer.begin();
		for (; it != m_vecRingBuffer.end(); it++)
		{
			(*it)->PushBuffer(m_readBuff, streamHeader);
		}
		TUnlock(m_vecLocker);
	}
	CXlHelper::MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	pAsioData->ioCall = J_AsioDataBase::j_read_e;
	return J_OK;
}

j_result_t CXlHost::OnRealPlayData(J_AsioDataBase *pAsioData, j_int32_t nDadaLen)
{
	CmdHeader *pHeader = (CmdHeader *)m_readBuff;
	/// 临时调整结构
	DevRealPlay *pResp = (DevRealPlay *)(m_readBuff + sizeof(CmdHeader) );
	J_OS::LOGDEBUG("host hostId = %s, channel = %d", pResp->hostId, pResp->channel & 0xFF);

	TLock(m_channelLocker);
	ChannelMap::iterator it = m_channelMap.find(pResp->channel & 0xFF);
	if (it != m_channelMap.end())
	{
		CXlChannel *pXlChannel = dynamic_cast<CXlChannel *>(it->second.pChannel);
		if (pXlChannel != NULL)
		{
			CXlHelper::MakeResponse2(xlc_start_real_view, (j_char_t *)(m_readBuff + sizeof(CmdHeader)), pHeader->length, m_readBuff);
			pXlChannel->InputData(0, m_readBuff, nDadaLen);
		}
	}
	TUnlock(m_channelLocker);
	///视频数据
	pAsioData->ioCall = J_AsioDataBase::j_read_e;
	CXlHelper::MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	return J_OK;
}

j_result_t CXlHost::OnRealStopData(J_AsioDataBase *pAsioData)
{
	DevRetVal *pResp = (DevRetVal *)(m_readBuff + sizeof(CmdHeader));
	J_OS::LOGINFO("CXlHost OnRealStopData %d", pResp->nRetVal);
	///返回数据
	CXlHelper::MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	pAsioData->ioCall = J_AsioDataBase::j_read_e;
	return J_OK;
}

j_result_t CXlHost::OnVodPlayData(J_AsioDataBase *pAsioData,  j_int32_t nDadaLen)
{
	CmdHeader *pHeader = (CmdHeader *)m_readBuff;
	DevStartVod *pResp = (DevStartVod *)(m_readBuff + sizeof(CmdHeader));
	J_OS::LOGDEBUG("CXlHost::OnVodPlayData host hostId = %s, channel = %d", pResp->hostId, pResp->channel & 0xFF);
	
	TLock(m_channelLocker);
	ChannelMap::iterator it = m_channelMap.find(pResp->channel & 0xFF);
	if (it != m_channelMap.end())
	{
		CXlChannel *pXlChannel = dynamic_cast<CXlChannel *>(it->second.pChannel);
		if (pXlChannel != NULL)
		{
			CXlHelper::MakeResponse2(xlc_start_vod_view, (j_char_t *)(m_readBuff + sizeof(CmdHeader)), pHeader->length, m_readBuff);
			pXlChannel->InputData(1, m_readBuff, nDadaLen);
		}
	}
	TUnlock(m_channelLocker);
	///视频数据
	pAsioData->ioCall = J_AsioDataBase::j_read_e;
	CXlHelper::MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	return J_OK;
}

j_result_t CXlHost::OnVodStopData(J_AsioDataBase *pAsioData)
{
	DevRetVal *pResp = (DevRetVal *)(m_readBuff + sizeof(CmdHeader));
	J_OS::LOGINFO("CXlHost OnVodStopData %d", pResp->nRetVal);
	///返回数据
	CXlHelper::MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	pAsioData->ioCall = J_AsioDataBase::j_read_e;
	return J_OK;
}

j_result_t CXlHost::OnRcdInfoData(J_AsioDataBase *pAsioData)
{
	DevRcdInfo *pResp = (DevRcdInfo *)(m_readBuff + sizeof(CmdHeader));
	//J_OS::LOGDEBUG("CXlHost::OnRcdInfoData hostId = %s %I64d %I64d BEGIN", pResp->szID,
	//	pResp->tmRecIntervalStartPt, pResp->tmRecIntervalEndPt);
	if (m_bReady)
	{
		CliRcdInfo cliRcdInfo = {0};
		strcpy((char *)cliRcdInfo.hostId, m_hostId.c_str());
		cliRcdInfo.tmRecIntervalStartPt = pResp->tmRecIntervalStartPt;
		cliRcdInfo.tmRecIntervalEndPt = pResp->tmRecIntervalEndPt;

		int nBodyLen = sizeof(CmdHeader) + sizeof(CliRcdInfo) + sizeof(CmdTail);
		CXlHelper::MakeResponse2(xlc_get_rcd_info, (char *)&cliRcdInfo, sizeof(CliRcdInfo), m_rcdBuffer);
		J_StreamHeader streamHeader = {0};
		streamHeader.dataLen = nBodyLen;
		TLock(m_vecRcdLocker);
		//J_OS::LOGDEBUG("m_vecRcdRingBuffer.size = %d", m_vecRcdRingBuffer.size());
		std::vector<CRingBuffer *>::iterator it = m_vecRcdRingBuffer.begin();
		for (; it != m_vecRcdRingBuffer.end(); ++it)
		{
			//J_OS::LOGDEBUG("CXlHost::OnRcdInfoData hostId = %s %I64d %I64d END", pResp->szID,
			//	pResp->tmRecIntervalStartPt, pResp->tmRecIntervalEndPt);
			(*it)->PushBuffer(m_rcdBuffer, streamHeader);
		}
		TUnlock(m_vecRcdLocker);
	}
	///返回数据
	CXlHelper::MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	pAsioData->ioCall = J_AsioDataBase::j_read_e;

	return J_OK;
}

j_result_t CXlHost::OnSetSysTime(J_AsioDataBase *pAsioData)
{
	DevRetVal *pResp = (DevRetVal *)(m_readBuff + sizeof(CmdHeader));
	J_OS::LOGINFO("CXlHost OnSetSysTime %d", pResp->nRetVal);

	CXlHelper::MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	pAsioData->ioCall = J_AsioDataBase::j_read_e;

	JoDataBus->RegisterDevice(m_hostId, this);
	JoDataBus->OnMessage(m_hostId, xlc_msg_host, xlc_host_connected);

	return J_OK;
}

j_result_t CXlHost::OnGetDevInfo(J_AsioDataBase *pAsioData)
{
	DevHostInfo *pReps = (DevHostInfo *)(m_readBuff + sizeof(CmdHeader));
	J_OS::LOGINFO("CXlHost::OnGetDevInfo hostId = %s, vehicleNum = %s, chnnelNum=%d, phoneNum = %s", 
		pReps->hostId, pReps->vehicleNum, pReps->totalChannels & 0xFF, pReps->phoneNum);

	for (int i=0; i<pReps->totalChannels; i++)
	{
		J_OS::LOGDEBUG("type = %d, name=%s", pReps->mediaType[i] & 0xFF, pReps->chName[i]);
	}
	//pReps->bOnline = true;
	JoDataBaseObj->UpdateDevInfo(*pReps, true);

	CXlHelper::MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	pAsioData->ioCall = J_AsioDataBase::j_read_e;

	return J_OK;
}

j_result_t CXlHost::OnGetLogInfo(J_AsioDataBase *pAsioData)
{
	DevLogInfo *pReps = (DevLogInfo *)(m_readBuff + sizeof(CmdHeader));
	J_OS::LOGINFO("CXlHost GetLogInfo state=%d, time=%d", pReps->bStatus & 0xFF, pReps->tmTime);
	JoDataBaseObj->InsertLogInfo(m_hostId.c_str(), pReps->bStatus & 0xFF, pReps->tmTime);

	CXlHelper::MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	pAsioData->ioCall = J_AsioDataBase::j_read_e;
	return J_OK;
}

j_result_t CXlHost::OnMsgInfo(J_AsioDataBase *pAsioData)
{
	CmdHeader *pHeader = (CmdHeader *)m_readBuff;
	DevMsgInfo *pReps = (DevMsgInfo *)(m_readBuff + sizeof(CmdHeader));
	J_OS::LOGINFO("CXlHost OnMsgInfo msg_code=%d, msg_ch=%d, %d", pReps->nMsgCode, pReps->channel & 0xFF, pHeader->length);
	//if (pReps->nMsgCode == xld_alarm_complete)
	//{
	//	pAsioData->ioCall = J_AsioDataBase::j_disconnect_e;
	//	return J_OK;
	//}

	if (m_bReady)
	{
		if (pReps->nMsgCode == xld_rcd_info_complete)
		{
			CliRcdInfo cliRcdInfo = {0};
			strcpy((char *)cliRcdInfo.hostId, m_hostId.c_str());
			cliRcdInfo.tmRecIntervalStartPt = -1;
			cliRcdInfo.tmRecIntervalEndPt = -1;

			int nBodyLen = sizeof(CmdHeader) + sizeof(CliRcdInfo) + sizeof(CmdTail);
			CXlHelper::MakeResponse2(xlc_rcdinfo_complete_inner, (char *)&cliRcdInfo, sizeof(CliRcdInfo), m_rcdBuffer);
			J_StreamHeader streamHeader = {0};
			streamHeader.dataLen = nBodyLen;
			TLock(m_vecRcdLocker);
			std::vector<CRingBuffer *>::iterator it = m_vecRcdRingBuffer.begin();
			for (; it != m_vecRcdRingBuffer.end(); it++)
			{
				(*it)->PushBuffer(m_rcdBuffer, streamHeader);
			}
			TUnlock(m_vecRcdLocker);
		}
		else if (pReps->nMsgCode == xld_playvod_complete)
		{
			TLock(m_channelLocker);
			ChannelMap::iterator it = m_channelMap.find(pReps->channel & 0xFF);
			if (it != m_channelMap.end())
			{
				CXlChannel *pXlChannel = dynamic_cast<CXlChannel *>(it->second.pChannel);
				if (pXlChannel != NULL)
				{
					DevStopVod data = {0};
					int nDataLen = sizeof(CmdHeader) + sizeof(DevStopVod) + sizeof(CmdTail);
					memcpy(&data.sessionId, &pReps->bReserve, sizeof(GUID));
					//strcpy(data.hostId, m_hostId.c_str());
					//data.channel = pReps->channel & 0xFF;
					CXlHelper::MakeResponse2(xlc_playvod_complete_inner, (j_char_t *)&data, sizeof(DevStopVod), m_readBuff);
					pXlChannel->InputData(1, m_readBuff, nDataLen);
				}
			}
			TUnlock(m_channelLocker);
		}
		else if (pReps->nMsgCode == xld_upload_success || pReps->nMsgCode == xld_upload_failed)
		{
			j_int32_t *pUserId = (j_int32_t *)pReps->bReserve;
			j_int32_t *pFileId = (j_int32_t *)(pReps->bReserve + sizeof(j_int32_t));
			j_string_t userName;
			JoDataBaseObj->UpdateFileInfo(*pFileId,  pReps->nMsgCode == xld_upload_success ? 1 : 2, false);
			if (JoDataBaseObj->GetUserNameById(*pUserId, userName) == J_OK)
			{
				J_Client *pClient = dynamic_cast<J_Client *>(JoClientManager->GetClientObj(userName));
				if (pClient)
				{
					pClient->SendMsgInfo(m_hostId, xlc_msg_upload, pReps->nMsgCode == xld_upload_success ? xlc_upload_success : xlc_upload_failed);
				}
			}
			///删除已经上传完成的会话
			TLock(m_fileUploadLocker);
			VecFileUploadInfo::iterator it = m_vecFileUploadInfo.begin();
			for (; it!=m_vecFileUploadInfo.end(); ++it)
			{
				if (it->nFileId == *pFileId)
				{
					m_vecFileUploadInfo.erase(it);
					break;
				}
			}
			TUnlock(m_fileUploadLocker);
		}
		CXlHelper::MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
		pAsioData->ioCall = J_AsioDataBase::j_read_e;
	}

	return J_OK;
}

j_result_t CXlHost::OnDefault(J_AsioDataBase *pAsioData)
{
	CXlHelper::MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	pAsioData->ioCall = J_AsioDataBase::j_read_e;

	return J_OK;
}

j_result_t CXlHost::ReqRealPlay(j_int32_t nChannel)
{
	j_result_t nResult = J_UNKNOW;
	TLock(m_channelLocker);
	ChannelMap::iterator it = m_channelMap.find(nChannel);
	if (it != m_channelMap.end())
	{
		if (it->second.nRef == 0)
		{
			struct RealPlayInfoBody
			{
				CmdHeader head;
				DevRealPlay data;
				CmdTail tail;
			} realPlayBody;
			memset (&realPlayBody, 0, sizeof(RealPlayInfoBody));
			strcpy(realPlayBody.data.hostId, m_hostId.c_str());
			realPlayBody.data.channel = nChannel;
			CXlHelper::MakeRequest(xld_real_play, (char *)&realPlayBody.data, sizeof(DevRealPlay), (char *)&realPlayBody);
			if (m_cmdSocket.Write_n((const char *)&realPlayBody, sizeof(RealPlayInfoBody)) > 0)
			{
				nResult = J_OK;
			}
		}
		++(it->second.nRef);
	}
	TUnlock(m_channelLocker);

	return nResult;
}

j_result_t CXlHost::ReqRealStop(j_int32_t nChannel)
{
	TLock(m_channelLocker);
	ChannelMap::iterator it = m_channelMap.find(nChannel);
	if (it != m_channelMap.end())
	{
		--(it->second.nRef);
		if (it->second.nRef == 0)
		{
			struct RealPlayInfoBody
			{
				CmdHeader head;
				DevRealPlay data;
				CmdTail tail;
			} realPlayBody;
			memset (&realPlayBody, 0, sizeof(RealPlayInfoBody));
			strcpy(realPlayBody.data.hostId, m_hostId.c_str());
			realPlayBody.data.channel = nChannel;
			CXlHelper::MakeRequest(xld_real_stop, (char *)&realPlayBody.data, sizeof(DevRealPlay), (char *)&realPlayBody);
			m_cmdSocket.Write_n((const char *)&realPlayBody, sizeof(RealPlayInfoBody));
		}
		else 
		{
			if (it->second.nRef < 0)
			{
				it->second.nRef = 0;
			}
			J_OS::LOGINFO("CXlHost::ReqRealStop real channel ref = %d", it->second.nRef);
		}
	}
	else
	{
		J_OS::LOGINFO("CXlHost::ReqRealStop real channel not found, channel_id = %d", nChannel);
	}
	TUnlock(m_channelLocker);

	return J_OK;
}

j_result_t CXlHost::ReqVodPlay(GUID sessionId, j_int32_t nChannel, j_time_t startTime, j_time_t endTime)
{
	//J_OS::LOGINFO("OnVodPlay ch=%d", nChannel);
	struct VodPlayInfoBody
	{
		CmdHeader head;
		DevStartVod data;
		CmdTail tail;
	} vodPlayBody;
	memset (&vodPlayBody, 0, sizeof(VodPlayInfoBody));
	strcpy(vodPlayBody.data.hostId, m_hostId.c_str());
	vodPlayBody.data.channel = nChannel;
	vodPlayBody.data.sessionId = sessionId;
	vodPlayBody.data.tmStartTime = startTime;
	vodPlayBody.data.tmEndTime = endTime;
	CXlHelper::MakeRequest(xld_vod_play, (char *)&vodPlayBody.data, sizeof(DevStartVod), (char *)&vodPlayBody);
	m_cmdSocket.Write_n((const char *)&vodPlayBody, sizeof(DevStartVod) + 10);

	return J_OK;
}

j_result_t CXlHost::ReqVodStop(GUID sessionId, j_int32_t nChannel)
{
	struct VodStopInfoBody
	{
		CmdHeader head;
		DevStopVod data;
		CmdTail tail;
	} vodStopBody;
	memset (&vodStopBody, 0, sizeof(VodStopInfoBody));
	//strcpy(vodStopBody.data.hostId, m_hostId.c_str());
	//vodStopBody.data.channel = nChannel;
	vodStopBody.data.sessionId = sessionId;
	CXlHelper::MakeRequest(xld_vod_stop, (char *)&vodStopBody.data, sizeof(DevStopVod), (char *)&vodStopBody);
	m_cmdSocket.Write_n((const char *)&vodStopBody, sizeof(VodStopInfoBody));

	return J_OK;
}

j_result_t CXlHost::ReqGetRcdInfo(j_string_t hostId)
{
	struct GetRcdBody
	{
		CmdHeader head;
		DevEquipmentId data;
		CmdTail tail;
	} getRcdBody;
	memset (&getRcdBody, 0, sizeof(GetRcdBody));
	strcpy(getRcdBody.data.hostId, m_hostId.c_str());
	CXlHelper::MakeRequest(xld_get_rcd_info, (char *)&getRcdBody.data, sizeof(DevEquipmentId), (char *)&getRcdBody);
	m_cmdSocket.Write_n((const char *)&getRcdBody, sizeof(GetRcdBody));

	return J_OK;
}

j_result_t CXlHost::OnStartUpload(j_int32_t nUserId, j_int32_t nFileId, j_string_t pFileName)
{
	/// 添加上传文件会话信息
	TLock(m_fileUploadLocker);
	FileUploadInfo info = {0};
	info.nUserId = nUserId;
	info.nFileId = nFileId;
	m_vecFileUploadInfo.push_back(info);
	TUnlock(m_fileUploadLocker);

	/// 转发开始信息
	struct StartUploadBody
	{
		CmdHeader head;
		DevUploadStart data;
		CmdTail tail;
	} startUploadBody;

	memset (&startUploadBody, 0, sizeof(StartUploadBody));
	startUploadBody.data.nUserId = nUserId;
	startUploadBody.data.nFileId = nFileId;
	strcpy(startUploadBody.data.szID, m_hostId.c_str());
	strcpy(startUploadBody.data.szFileName, pFileName.c_str());

	//u_long flag = 0;
	//ioctlsocket(m_cmdSocket.GetHandle().sock, FIONBIO, &flag);

	CXlHelper::MakeRequest(xld_start_upload, (char *)&startUploadBody.data, sizeof(DevUploadStart), (char *)&startUploadBody);
	//m_cmdSocket.Write_n((const char *)&startUploadBody, sizeof(StartUploadBody));
	J_StreamHeader streamHeader = {0};
	streamHeader.dataLen = sizeof(StartUploadBody);
	streamHeader.frameType = 99;
	m_uploadRingBuffer.PushBuffer((const char *)&startUploadBody, streamHeader);

	g_deviceServer.SentData(m_cmdSocket.GetHandle());

	return J_OK;
}

j_result_t CXlHost::OnUploading(j_int32_t nFileId, j_char_t *pData, j_int32_t nLen)
{
	CXlHelper::MakeRequest(xld_uploading, (char *)pData, nLen, m_fileBuffer);
	//m_cmdSocket.Write_n((const char *)m_fileBuffer, sizeof(CmdHeader) + nLen + sizeof(CmdTail));

	//J_StreamHeader streamHeader = {0};
	//streamHeader.dataLen = sizeof(CmdHeader) + nLen + sizeof(CmdTail);
	//streamHeader.frameType = 99;
	//m_uploadRingBuffer.PushBuffer(m_fileBuffer, streamHeader);

	j_int32_t totleLen = sizeof(CmdHeader) + nLen + sizeof(CmdTail);
	//printf("totle len = %d\n", totleLen);
	j_int32_t offset = 0;
	while (totleLen > 0)
	{
		J_StreamHeader streamHeader = {0};
		if (totleLen > 1024)
		{
			streamHeader.dataLen = 1024;
			streamHeader.frameType = 99;
			m_uploadRingBuffer.PushBuffer(m_fileBuffer + offset, streamHeader);
			//m_cmdSocket.Write_n((const char *)(m_fileBuffer + offset), 1024);
			offset += 1024;
			totleLen -= 1024;
		}
		else
		{
			streamHeader.dataLen = totleLen;
			streamHeader.frameType = 99;
			m_uploadRingBuffer.PushBuffer(m_fileBuffer + offset, streamHeader);
			//m_cmdSocket.Write_n((const char *)(m_fileBuffer + offset), totleLen);
			totleLen = 0;
		}
	}

	//u_long ret = 0;
	//ioctlsocket(m_cmdSocket.GetHandle().sock, FIONBIO, &ret);
	//if ((ret = send(m_cmdSocket.GetHandle().sock, m_fileBuffer, sizeof(CmdHeader) + nLen + sizeof(CmdTail), 0)) <= 0)
	//	J_OS::LOGINFO("send len = %d", GetLastError());
	//J_OS::LOGINFO("send len2222 = %d", ret);
	//ret  = 1;
	//ioctlsocket(m_cmdSocket.GetHandle().sock, FIONBIO, &ret);

	return J_OK;
}

j_result_t CXlHost::OnStopUpload(j_int32_t nFileId, j_char_t *pMD5)
{
	struct StopUploadBody
	{
		CmdHeader head;
		DevUploadStop data;
		CmdTail tail;
	} stopUploadBody;
	memset (&stopUploadBody, 0, sizeof(StopUploadBody));
	stopUploadBody.data.nFileId = nFileId;
	memcpy(stopUploadBody.data.szCheck, pMD5, 16);
	CXlHelper::MakeRequest(xld_stop_upload, (char *)&stopUploadBody.data, sizeof(DevUploadStop), (char *)&stopUploadBody);
	//m_cmdSocket.Write_n((const char *)&stopUploadBody, sizeof(StopUploadBody));

	J_StreamHeader streamHeader = {0};
	streamHeader.dataLen = sizeof(StopUploadBody);
	streamHeader.frameType = 99;
	m_uploadRingBuffer.PushBuffer((const char *)&stopUploadBody, streamHeader);

	streamHeader.dataLen = 0;
	streamHeader.frameType = 100;
	m_uploadRingBuffer.PushBuffer((const char *)&stopUploadBody, streamHeader);

	//u_long flag = 1;
	//ioctlsocket(m_cmdSocket.GetHandle().sock, FIONBIO, &flag);

	return J_OK;
}