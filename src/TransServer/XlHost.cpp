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
#include "XlType.h"
#include "XlChannel.h"
#include "MsSqlServer.h"

#pragma comment (lib, "Debug\\core.lib")

CXlHost::CXlHost(j_socket_t nSock)
: m_cmdSocket(nSock)
{
	m_bReady = false;
	m_lastBreatTime = time(0);

	m_readBuff = new char[BUFFER_SIZE];			
	m_writeBuff = new char[BUFFER_SIZE];			
	m_ioState = xl_init_state;
}

CXlHost::~CXlHost()
{
	delete m_readBuff;
	delete m_writeBuff;
}
j_result_t CXlHost::MakeChannel(const j_int32_t nChannelNum, J_Channel *&pObj)
{
	J_Channel *pChannel = NULL;
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
	if (time(0) - m_lastBreatTime > 5)
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
		SetTimeReq data;
		CmdTail tail;
	} sysTimeBody;
	sysTimeBody.data.systime = sysTime;
	MakeRequest(xl_set_time, (char *)&sysTimeBody.data, sizeof(SetTimeReq), (char *)&sysTimeBody);
	m_cmdSocket.Write_n((char *)&sysTimeBody, sizeof(SysTimeBody));

	return J_OK;
}

j_result_t CXlHost::GetDeviceInfo()
{
	struct DevInfoBody {
		CmdHeader head;
		CmdTail tail;
	} devInfoBody;
	MakeRequest(xl_get_devinfo, NULL, 0, (char *)&devInfoBody);
	m_cmdSocket.Write_n((char *)&devInfoBody, sizeof(DevInfoBody));

	struct LogInfoBody
	{
		CmdHeader head;
		GetLogInfoReq data;
		CmdTail tail;
	} logInfoBody;
	memset(&logInfoBody, 0, sizeof(LogInfoBody));
	time_t logLastTime = JoDataBaseObj->GetDevLogLastTime("CD508893472E");
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
	MakeRequest(xl_get_loginfo, (char *)&logInfoBody.data, sizeof(GetLogInfoReq), (char *)&logInfoBody);
	m_cmdSocket.Write_n((char *)&logInfoBody, sizeof(LogInfoBody));

	return J_OK;
}

j_result_t CXlHost::EnableAlarm(CRingBuffer *pRingBuffer, j_boolean_t bEnable)
{
	if (bEnable)
	{
		AddRingBuffer(pRingBuffer);
	}
	else
	{
		DelRingBuffer(pRingBuffer);
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

j_result_t CXlHost::MakeRequest(j_char_t bCmd, j_char_t *pData, j_int32_t nLen, j_char_t *pBody)
{
	//填充头信息
	CmdHeader *pHeader = (CmdHeader *)pBody;
	pHeader->beginCode = 0xFF;
	pHeader->version = 0x0A;
	pHeader->flag = 0x00;
	pHeader->cmd = bCmd;
	pHeader->length = nLen;
	//填充数据区
	if (nLen > 0)
		memcpy(pBody + sizeof(CmdHeader), pData, nLen);
	//填充尾信息
	CmdTail *pTail = (CmdTail *)(pBody + sizeof(CmdHeader) + nLen);
	pTail->verify = CheckNum(pBody, sizeof(CmdHeader) + nLen);
	pTail->endCode = 0xFE;

	return J_OK;
}

j_uint32_t CXlHost::CheckNum(j_char_t *pData, j_int32_t nLen)
{
	j_uint32_t nCheckNum = 0xFE;
	for (int i=0; i<nLen; ++i)
		nCheckNum += pData[i];

	return (nCheckNum % 256);
}

j_result_t CXlHost::MakeNetData(J_AsioDataBase *pAsioData, j_char_t *pDataBuff, j_int32_t nLen)
{
	if (pAsioData->ioCall == J_AsioDataBase::j_read_e)
	{
		pAsioData->ioRead.buf = pDataBuff;
		pAsioData->ioRead.bufLen = nLen;
		pAsioData->ioRead.finishedLen = 0;
		pAsioData->ioRead.whole = true;
		pAsioData->ioRead.shared = true;
	}
	else if (pAsioData->ioCall == J_AsioDataBase::j_write_e)
	{
		pAsioData->ioWrite.buf = pDataBuff;
		pAsioData->ioWrite.bufLen = nLen;
		pAsioData->ioWrite.finishedLen = 0;
		pAsioData->ioWrite.whole = true;
		pAsioData->ioWrite.shared = true;
	}
	return J_OK;
}

j_result_t CXlHost::ProcessClientCmd(J_AsioDataBase *pAsioData)
{
	j_char_t *pDataBuff = pAsioData->ioRead.buf;
	CmdHeader *pHeader = (CmdHeader *)pDataBuff;
	switch (pHeader->cmd)
	{
	case xl_real_play:
		OnRealPlay(pDataBuff, pAsioData->ioRead.bufLen);
		break;
	case xl_real_stop:
		OnRealStop(pDataBuff, pAsioData->ioRead.bufLen);
		break;
	case xl_vod_play:
		OnVodPlay(pDataBuff, pAsioData->ioRead.bufLen);
		break;
	case xl_vod_stop:
		OnVodStop(pDataBuff, pAsioData->ioRead.bufLen);
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
		MakeRequest(xl_register, NULL, 0, (j_char_t *)m_writeBuff);
		pAsioData->ioCall = J_AsioDataBase::j_write_e;
		MakeNetData(pAsioData, m_writeBuff, sizeof(CmdHeader) + sizeof(CmdTail));

		m_ioState = xl_write_body_state;
	}
	else if (m_ioState == xl_read_head_state)
	{
		pAsioData->ioCall = J_AsioDataBase::j_read_e;
		MakeNetData(pAsioData, m_readBuff + sizeof(CmdHeader), *((j_int32_t *)(m_readBuff + 4)) + sizeof(CmdTail));

		m_ioState = xl_read_data_state;
	}
	else if (m_ioState == xl_read_data_state)
	{
		CmdHeader *pHeader = (CmdHeader *)m_readBuff;
		switch (pHeader->cmd)
		{
		case xl_register:
			OnRegister(pAsioData);
			m_ioState = xl_read_head_state;
			break;
		case xl_heartbeat:
			OnHeartBeat(pAsioData);
			m_ioState = xl_read_head_state;
			break;
		case xl_get_alraminfo:
			OnAlarmInfo(pAsioData);
			m_ioState = xl_read_head_state;
			break;
		case xl_real_play:
			OnRealPlayData(pAsioData, pHeader->length + sizeof(CmdHeader) + sizeof(CmdTail));
			m_ioState = xl_read_head_state;
			break;
		case xl_real_stop:
			OnRealStopData(pAsioData);
			m_ioState = xl_read_head_state;
			break;
		case xl_vod_play:
			OnVodPlayData(pAsioData, pHeader->length + sizeof(CmdHeader) + sizeof(CmdTail));
			m_ioState = xl_read_head_state;
			break;
		case xl_vod_stop:
			OnVodStopData(pAsioData);
			m_ioState = xl_read_head_state;
			break;
		case xl_set_time:
			OnSetSysTime(pAsioData);   
			m_ioState = xl_read_head_state;
			break;
		case xl_get_devinfo:
			OnGetDevInfo(pAsioData);   
			m_ioState = xl_read_head_state;
			break;
		case xl_get_loginfo:
			OnGetLogInfo(pAsioData);   
			m_ioState = xl_read_head_state;
			break;
		default:
			break;
		}
		//J_OS::LOGINFO("XlHost %d %X", pHeader->cmd, pHeader->flag & 0xFF);
	}
	else
	{
		pAsioData->ioCall = J_AsioDataBase::j_read_e;
		MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
		pAsioData->ioCall = J_AsioDataBase::j_read_e;

		m_ioState = xl_read_head_state;
	}
	return J_OK;
}

j_result_t CXlHost::OnRegister(J_AsioDataBase *pAsioData)
{
	RegisterResp *pReps = (RegisterResp *)(m_readBuff + sizeof(CmdHeader));
	m_hostId = pReps->hostId;
	m_bReady = true;
	J_OS::LOGINFO("XlHost hostId =%s", pReps->hostId);
	MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	pAsioData->ioCall = J_AsioDataBase::j_read_e;
	return J_OK;
}

j_result_t CXlHost::OnHeartBeat(J_AsioDataBase *pAsioData)
{
	m_lastBreatTime = time(0);
	MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	pAsioData->ioCall = J_AsioDataBase::j_read_e;
	return J_OK;
}

j_result_t CXlHost::OnAlarmInfo(J_AsioDataBase *pAsioData)
{
	AlarmInfoResp *pAlarmInfo = (AlarmInfoResp *)(m_readBuff + sizeof(CmdHeader));
	J_OS::LOGINFO("%I64d GPS(%f %f %f) %f %d", pAlarmInfo->bAlarm & 0xFF, pAlarmInfo->gps.dLatitude, pAlarmInfo->gps.dLongitude,
		pAlarmInfo->gps.dGPSSpeed, pAlarmInfo->speed, pAlarmInfo->tmTimeStamp);
	if (m_bReady)
	{
		JoDataBaseObj->InsertAlarmInfo(m_hostId.c_str(), *pAlarmInfo);
		J_StreamHeader streamHeader = {0};
		streamHeader.dataLen = sizeof(CmdHeader) + sizeof(AlarmInfoResp) + sizeof(CmdTail);
		TLock(m_vecLocker);
		std::vector<CRingBuffer *>::iterator it = m_vecRingBuffer.begin();
		for (; it != m_vecRingBuffer.end(); it++)
		{
			(*it)->PushBuffer(m_readBuff, streamHeader);
		}
		TUnlock(m_vecLocker);
	}
	MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	pAsioData->ioCall = J_AsioDataBase::j_read_e;
	return J_OK;
}

j_result_t CXlHost::OnRealPlayData(J_AsioDataBase *pAsioData, j_int32_t nDadaLen)
{
	RealPlayReq *pReps = (RealPlayReq *)(m_readBuff + sizeof(CmdHeader));
	//J_OS::LOGINFO("host hostId = %s, channel = %d", pReps->hostId, pReps->channel & 0xFF);
	TLock(m_channelLocker);
	ChannelMap::iterator it = m_channelMap.find(pReps->channel & 0xFF);
	if (it != m_channelMap.end())
	{
		CXlChannel *pXlChannel = dynamic_cast<CXlChannel *>(it->second.pChannel);
		if (pXlChannel != NULL)
			pXlChannel->InputData(m_readBuff, nDadaLen);
	}
	TUnlock(m_channelLocker);
	///视频数据
	pAsioData->ioCall = J_AsioDataBase::j_read_e;
	MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	return J_OK;
}

j_result_t CXlHost::OnRealStopData(J_AsioDataBase *pAsioData)
{
	RealStopReq *pReps = (RealStopReq *)(m_readBuff + sizeof(CmdHeader));
	J_OS::LOGINFO("XlHost hostId = %s, channel = %d", pReps->hostId, pReps->channel & 0xFF);
	///返回数据
	MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	pAsioData->ioCall = J_AsioDataBase::j_read_e;
	return J_OK;
}

j_result_t CXlHost::OnVodPlayData(J_AsioDataBase *pAsioData,  j_int32_t nDadaLen)
{
	VodPlayReq *pReps = (VodPlayReq *)(m_readBuff + sizeof(CmdHeader));
	//J_OS::LOGINFO("host hostId = %s, channel = %d", pReps->hostId, pReps->channel & 0xFF);
	TLock(m_channelLocker);
	ChannelMap::iterator it = m_channelMap.find(pReps->channel & 0xFF);
	if (it != m_channelMap.end())
	{
		CXlChannel *pXlChannel = dynamic_cast<CXlChannel *>(it->second.pChannel);
		if (pXlChannel != NULL)
			pXlChannel->InputData(m_readBuff, nDadaLen);
	}
	TUnlock(m_channelLocker);
	///视频数据
	pAsioData->ioCall = J_AsioDataBase::j_read_e;
	MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	return J_OK;
}

j_result_t CXlHost::OnVodStopData(J_AsioDataBase *pAsioData)
{
	VodStopReq *pReps = (VodStopReq *)(m_readBuff + sizeof(CmdHeader));
	J_OS::LOGINFO("XlHost hostId = %s, channel = %d", pReps->hostId, pReps->channel & 0xFF);
	///返回数据
	MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	pAsioData->ioCall = J_AsioDataBase::j_read_e;
	return J_OK;
}

j_result_t CXlHost::OnSetSysTime(J_AsioDataBase *pAsioData)
{
	SetTimeResp *pReps = (SetTimeResp *)(m_readBuff + sizeof(CmdHeader));
	J_OS::LOGINFO("XlHost setTime ret = %d", pReps->ret);

	MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	pAsioData->ioCall = J_AsioDataBase::j_read_e;

	return J_OK;
}

j_result_t CXlHost::OnGetDevInfo(J_AsioDataBase *pAsioData)
{
	HostInfoResp *pReps = (HostInfoResp *)(m_readBuff + sizeof(CmdHeader));
	J_OS::LOGINFO("XlHost GetInfo hostId = %s, vehicleNum = %s, chnnelNum=%d, phoneNum = %s", 
		pReps->hostId, pReps->vehicleNum, pReps->totalChannels & 0xFF, pReps->phoneNum);
	JoDataBaseObj->UpdateDevInfo(*pReps);

	MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	pAsioData->ioCall = J_AsioDataBase::j_read_e;

	return J_OK;
}

j_result_t CXlHost::OnGetLogInfo(J_AsioDataBase *pAsioData)
{
	GetLogInfoResp *pReps = (GetLogInfoResp *)(m_readBuff + sizeof(CmdHeader));
	J_OS::LOGINFO("XlHost GetLogInfo state=%d, time=%d", pReps->bStatus & 0xFF, pReps->tmTime);
	JoDataBaseObj->InsertLogInfo(m_hostId.c_str(), pReps->bStatus & 0xFF, pReps->tmTime);

	MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	pAsioData->ioCall = J_AsioDataBase::j_read_e;
	return J_OK;
}

j_result_t CXlHost::OnRealPlay(j_char_t *pData, j_int32_t nLen)
{
	RealPlayReq *pReps = (RealPlayReq *)(pData + sizeof(CmdHeader));
	J_OS::LOGINFO("XlHost host OnRealPlay hostId = %s, channel = %d", pReps->hostId, pReps->channel & 0xFF);

	TLock(m_channelLocker);
	ChannelMap::iterator it = m_channelMap.find(pReps->channel & 0xFF);
	if (it != m_channelMap.end())
	{
		if (it->second.nRef == 0)
			m_cmdSocket.Write_n(pData, nLen);
		++(it->second.nRef);
	}
	TUnlock(m_channelLocker);

	return J_OK;
}

j_result_t CXlHost::OnRealStop(j_char_t *pData, j_int32_t nLen)
{
	RealStopReq *pReps = (RealStopReq *)(pData + sizeof(CmdHeader));
	J_OS::LOGINFO("XlHost OnRealStop hostId = %s, channel = %d", pReps->hostId, pReps->channel & 0xFF);
	
	TLock(m_channelLocker);
	ChannelMap::iterator it = m_channelMap.find(pReps->channel & 0xFF);
	if (it != m_channelMap.end())
	{
		--(it->second.nRef);
		if (it->second.nRef == 0)
			m_cmdSocket.Write_n(pData, nLen);
	}
	TUnlock(m_channelLocker);

	return J_OK;
}

j_result_t CXlHost::OnVodPlay(j_char_t *pData, j_int32_t nLen)
{
	RealPlayReq *pReps = (RealPlayReq *)(pData + sizeof(CmdHeader));
	J_OS::LOGINFO("XlHost host hostId = %s, channel = %d", pReps->hostId, pReps->channel & 0xFF);
	m_cmdSocket.Write_n(pData, nLen);
	return J_OK;
}

j_result_t CXlHost::OnVodStop(j_char_t *pData, j_int32_t nLen)
{
	RealStopReq *pReps = (RealStopReq *)(pData + sizeof(CmdHeader));
	J_OS::LOGINFO("XlHost OnVodStop hostId = %s, channel = %d", pReps->hostId, pReps->channel & 0xFF);
	char vodStopBuff[128] = {0};
	MakeRequest(xl_vod_stop, NULL, 0, vodStopBuff);
	m_cmdSocket.Write_n(vodStopBuff, 10);
	return J_OK;
}