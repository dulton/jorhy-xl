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

#pragma comment (lib, "Debug\\core.lib")

CXlHost::CXlHost(j_socket_t nSock)
: m_cmdSocket(nSock)
{
	m_bReady = false;
	m_lastBreatTime = time(0);

	m_readBuff = new char[BUFFER_SIZE];			
	m_writeBuff = new char[BUFFER_SIZE];	
	m_rcdBuffer = new char[1024];
	m_ioState = xl_init_state;
}

CXlHost::~CXlHost()
{
	delete m_readBuff;
	delete m_writeBuff;
	delete m_rcdBuffer;
	DevHostInfo devInfo = {0};
	strcpy(devInfo.hostId, m_hostId.c_str());
	//devInfo.bOnline = false;
	JoDataBaseObj->UpdateDevInfo(devInfo, false);
}

j_result_t CXlHost::MakeChannel(const j_int32_t nChannelNum, J_Obj *&pObj)
{
	//J_OS::LOGINFO("MakeChannel ch = %d", nChannelNum);
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
	if (time(0) - m_lastBreatTime > 15)
		m_bReady = false;

	return m_bReady;
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
	else
	{
		ProcessClientCmd(pAsioData);
	}

	return J_OK;
}

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
			OnRealPlay(pRealPlay->channel);
		}
		break;
	case xlc_stop_real_view:
		{
			CliRealPlay *pRealPlay = (CliRealPlay *)(pDataBuff + sizeof(CmdHeader));
			OnRealStop(pRealPlay->channel);
		}
		break;
	case xlc_start_vod_view:
		{
			CliStartVod*pStartVod = (CliStartVod *)(pDataBuff + sizeof(CmdHeader));
			OnVodPlay(pStartVod->sessionId, pStartVod->channel, pStartVod->tmStartTime, pStartVod->tmEndTime);
		}
		break;
	case xlc_stop_vod_view:
		{
			CliStopVod*pStopVod = (CliStopVod *)(pDataBuff + sizeof(CmdHeader));
			OnVodStop(pStopVod->sessionId, pStopVod->channel);
		}
		break;
	case xlc_get_rcd_info:
		{
			DevEquipmentId *pHostId = (DevEquipmentId *)(pDataBuff + sizeof(CmdHeader));
			OnGetRcdInfo(pHostId->hostId);
		}
		break;
	default:
		break;
	}
	return J_OK;
}

j_result_t CXlHost::ProcessDeviceCmd(J_AsioDataBase *pAsioData)
{
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
		//J_OS::LOGINFO("dataLen = %d type = %d", pHeader->length, pHeader->cmd);
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
			OnMsgInfo(pAsioData);
			m_ioState = xl_read_head_state;
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
			J_OS::LOGINFO("XlHost %d %X", pHeader->cmd, pHeader->flag & 0xFF);
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

j_result_t CXlHost::OnRegister(J_AsioDataBase *pAsioData)
{
	DevEquipmentId *pResp = (DevEquipmentId *)(m_readBuff + sizeof(CmdHeader));
	m_hostId = pResp->hostId;
	m_bReady = true;
	J_OS::LOGINFO("XlHost hostId =%s", pResp->hostId);
	CXlHelper::MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	pAsioData->ioCall = J_AsioDataBase::j_read_e;
	return J_OK;
}

j_result_t CXlHost::OnHeartBeat(J_AsioDataBase *pAsioData)
{
	m_lastBreatTime = time(0);
	CXlHelper::MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	pAsioData->ioCall = J_AsioDataBase::j_read_e;
	return J_OK;
}

j_result_t CXlHost::OnAlarmInfo(J_AsioDataBase *pAsioData)
{
	DevAlarmInfo *pAlarmInfo = (DevAlarmInfo *)(m_readBuff + sizeof(CmdHeader));
	//J_OS::LOGINFO("%I64d GPS(%f %f %f) %f %d", pAlarmInfo->bAlarm & 0xFF, pAlarmInfo->gps.dLatitude, pAlarmInfo->gps.dLongitude,
	//	pAlarmInfo->gps.dGPSSpeed, pAlarmInfo->speed, pAlarmInfo->tmTimeStamp);
	if (m_bReady)
	{
		JoDataBaseObj->InsertAlarmInfo(m_hostId.c_str(), *pAlarmInfo);
		CliAlarmInfo cliAlarmInfo = {0};
		strcpy((char *)cliAlarmInfo.pHostId, m_hostId.c_str());
		//memcpy((((char *)&cliAlarmInfo)+sizeof(cliAlarmInfo.pHostId)), pAlarmInfo, sizeof(CliAlarmInfo));
		cliAlarmInfo.bAlarm = pAlarmInfo->bAlarm & 0xFF;
		//J_OS::LOGINFO("%d %s", cliAlarmInfo.bAlarm, cliAlarmInfo.pHostId);
		/*cliAlarmInfo.gps.dLatitude = pAlarmInfo->gps.dLatitude;
		cliAlarmInfo.gps.dLongitude = pAlarmInfo->gps.dLongitude;
		cliAlarmInfo.gps.dGPSSpeed = pAlarmInfo->gps.dGPSSpeed;
		cliAlarmInfo.speed = pAlarmInfo->speed;*/
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
	DevRealPlay *pResp = (DevRealPlay *)(m_readBuff + sizeof(CmdHeader));
	//J_OS::LOGINFO("host hostId = %s, channel = %d", pResp->hostId, pResp->channel & 0xFF);
	TLock(m_channelLocker);
	ChannelMap::iterator it = m_channelMap.find(pResp->channel & 0xFF);
	if (it != m_channelMap.end())
	{
		CXlChannel *pXlChannel = dynamic_cast<CXlChannel *>(it->second.pChannel);
		if (pXlChannel != NULL)
		{
			CXlHelper::MakeResponse2(xlc_start_real_view, (j_char_t *)pResp, pHeader->length, m_readBuff);
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
	J_OS::LOGINFO("XlHost OnRealStopData %d", pResp->nRetVal);
	///返回数据
	CXlHelper::MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	pAsioData->ioCall = J_AsioDataBase::j_read_e;
	return J_OK;
}

j_result_t CXlHost::OnVodPlayData(J_AsioDataBase *pAsioData,  j_int32_t nDadaLen)
{
	CmdHeader *pHeader = (CmdHeader *)m_readBuff;
	DevStartVod *pResp = (DevStartVod *)(m_readBuff + sizeof(CmdHeader));
	//J_OS::LOGINFO("host hostId = %s, channel = %d", pResp->hostId, pResp->channel & 0xFF);
	TLock(m_channelLocker);
	ChannelMap::iterator it = m_channelMap.find(pResp->channel & 0xFF);
	if (it != m_channelMap.end())
	{
		CXlChannel *pXlChannel = dynamic_cast<CXlChannel *>(it->second.pChannel);
		if (pXlChannel != NULL)
		{
			CXlHelper::MakeResponse2(xlc_start_vod_view, (j_char_t *)pResp, pHeader->length, m_readBuff);
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
	J_OS::LOGINFO("XlHost OnVodStopData %d", pResp->nRetVal);
	///返回数据
	CXlHelper::MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	pAsioData->ioCall = J_AsioDataBase::j_read_e;
	return J_OK;
}

j_result_t CXlHost::OnRcdInfoData(J_AsioDataBase *pAsioData)
{
	DevRcdInfo *pResp = (DevRcdInfo *)(m_readBuff + sizeof(CmdHeader));
	//J_OS::LOGINFO("1 -- XlHost RcdInfoData hostId = %s %I64d %I64d", pResp->szID,
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
		//J_OS::LOGINFO("m_vecRcdRingBuffer.size = %d", m_vecRcdRingBuffer.size());
		std::vector<CRingBuffer *>::iterator it = m_vecRcdRingBuffer.begin();
		for (; it != m_vecRcdRingBuffer.end(); ++it)
		{
			//J_OS::LOGINFO("2 -- XlHost RcdInfoData hostId = %s %I64d %I64d", pResp->szID,
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
	J_OS::LOGINFO("XlHost OnSetSysTime %d", pResp->nRetVal);

	CXlHelper::MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	pAsioData->ioCall = J_AsioDataBase::j_read_e;

	return J_OK;
}

j_result_t CXlHost::OnGetDevInfo(J_AsioDataBase *pAsioData)
{
	DevHostInfo *pReps = (DevHostInfo *)(m_readBuff + sizeof(CmdHeader));
	J_OS::LOGINFO("XlHost GetInfo hostId = %s, vehicleNum = %s, chnnelNum=%d, phoneNum = %s", 
		pReps->hostId, pReps->vehicleNum, pReps->totalChannels & 0xFF, pReps->phoneNum);
	//pReps->bOnline = true;
	JoDataBaseObj->UpdateDevInfo(*pReps, true);

	CXlHelper::MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	pAsioData->ioCall = J_AsioDataBase::j_read_e;

	return J_OK;
}

j_result_t CXlHost::OnGetLogInfo(J_AsioDataBase *pAsioData)
{
	DevLogInfo *pReps = (DevLogInfo *)(m_readBuff + sizeof(CmdHeader));
	J_OS::LOGINFO("XlHost GetLogInfo state=%d, time=%d", pReps->bStatus & 0xFF, pReps->tmTime);
	JoDataBaseObj->InsertLogInfo(m_hostId.c_str(), pReps->bStatus & 0xFF, pReps->tmTime);

	CXlHelper::MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	pAsioData->ioCall = J_AsioDataBase::j_read_e;
	return J_OK;
}

j_result_t CXlHost::OnMsgInfo(J_AsioDataBase *pAsioData)
{
	CmdHeader *pHeader = (CmdHeader *)m_readBuff;
	DevMsgInfo *pReps = (DevMsgInfo *)(m_readBuff + sizeof(CmdHeader));
	J_OS::LOGINFO("XlHost OnMsgInfo msg_code=%d, msg_ch=%d, %d", pReps->nMsgCode, pReps->channel & 0xFF, pHeader->length);
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
	}
	CXlHelper::MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	pAsioData->ioCall = J_AsioDataBase::j_read_e;
	return J_OK;
}

j_result_t CXlHost::OnDefault(J_AsioDataBase *pAsioData)
{
	CXlHelper::MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	pAsioData->ioCall = J_AsioDataBase::j_read_e;

	return J_OK;
}

j_result_t CXlHost::OnRealPlay(j_int32_t nChannel)
{
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
			m_cmdSocket.Write_n((const char *)&realPlayBody, sizeof(RealPlayInfoBody));
		}
		++(it->second.nRef);
	}
	TUnlock(m_channelLocker);

	return J_OK;
}

j_result_t CXlHost::OnRealStop(j_int32_t nChannel)
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
	}
	TUnlock(m_channelLocker);

	return J_OK;
}

j_result_t CXlHost::OnVodPlay(GUID sessionId, j_int32_t nChannel, j_time_t startTime, j_time_t endTime)
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

j_result_t CXlHost::OnVodStop(GUID sessionId, j_int32_t nChannel)
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

j_result_t CXlHost::OnGetRcdInfo(j_string_t hostId)
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