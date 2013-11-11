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
#include "MsSqlServer.h"
#include "XlHelper.h"

#pragma comment (lib, "Debug\\core.lib")

#define XL_CMD_BUFFER_SIZE (2 * 1024)
#define XL_DATA_BUFFER_SIZE (1024 * 1024)
#define XL_ALARM_BUFFER_SIZE (2 * 1024)

CXlClient::CXlClient(j_socket_t nSock)
// : m_socket(nSock)
{
	m_readBuff = new char[XL_CMD_BUFFER_SIZE];			
	m_writeBuff = new char[XL_CMD_BUFFER_SIZE];		
	m_dataBuff = new char[XL_DATA_BUFFER_SIZE];	
	m_alarmBuff = new char[XL_ALARM_BUFFER_SIZE];	
	m_ioCmdState = xl_init_state;
	m_ioDataState = xl_init_state;
	m_ioAlarmState = xl_init_state;
	m_nRefCnt = 0;
	m_nAlarmRefCnt = 0;
	m_lastBreatTime = time(0);

	m_config.Init(m_readBuff, m_writeBuff);
}

CXlClient::~CXlClient()
{
	delete m_readBuff;
	delete m_writeBuff;
	delete m_dataBuff;
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
	case J_AsioDataBase::j_log_e:
	case J_AsioDataBase::j_alarm_history_e:
	case J_AsioDataBase::j_dvr_list_e:
	case J_AsioDataBase::j_user_list_e:
	case J_AsioDataBase::j_department_list_e:
		nResult = m_config.ProcessConfigData(pAsioData->ioType, pAsioData);
		break;
	}

	return nResult;
}

j_result_t CXlClient::Broken()
{
	StopAll();
	return J_OK;
}

j_int32_t CXlClient::GetState()
{
	if (time(0) - m_lastBreatTime > 120)
		return -1;

	return 0;
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
		default:
			ProcessConfig(pAsioData);
			break;
		}
		J_OS::LOGINFO("%d %X", pHeader->cmd, pHeader->flag & 0xFF);
	}
	else
	{	
		pAsioData->ioCall = J_AsioDataBase::j_read_e;
		CXlHelper::MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));

		m_ioCmdState = xl_read_head_state;
	}

	return J_OK;
}

j_result_t CXlClient::ProcessConfig(J_AsioDataBase *pAsioData)
{
	CmdHeader *pHeader = (CmdHeader *)m_readBuff;
	switch (pHeader->cmd)
	{
		//case xl_set_time:
		//	OnSetTime(pAsioData);
		//	m_ioCmdState = xl_write_body_state;
		//	break;
		//case xl_get_devinfo:
		//	OnGetDevInfo(pAsioData);
		//	m_ioCmdState = xl_write_body_state;
		//	break;
		//case xl_get_loginfo:
		//	OnGetLogInfo(pAsioData);
		//	m_ioCmdState = xl_read_head_state;
		//	break;
		case xlc_get_alarm_info:
		case xlc_get_dvr_list:
		case xlc_get_user_list:
		case xlc_get_department_list:
			m_config.ProcessConfigRequest(pHeader->cmd, pAsioData);
			m_ioCmdState = xl_read_head_state;
			break;
		case xlc_get_dvr_summary:
		case xlc_get_user_summary:
		case xlc_get_dvr_info:
		case xlc_get_user_info:
		case xlc_get_department_info:
		case xlc_add_dvr_info:
		case xlc_add_user_info:
		case xlc_add_department_info:
		case xlc_mod_dvr_info:
		case xlc_mod_user_info:
		case xlc_mod_department_info:
		case xlc_del_dvr_info:
		case xlc_del_user_info:
		case xlc_del_department_info:
			m_config.ProcessConfigRequest(pHeader->cmd, pAsioData);
			m_ioCmdState = xl_write_body_state;
			break;
		default: 
			break;
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
			nResult =MakeAlarmData(pAsioData);
		}
	}
	else
	{
		pAsioData->ioCall = J_AsioDataBase::j_keep_e;
	}

	return nResult;
}

j_result_t CXlClient::StartView(const CliRealPlay &realPlay, J_AsioDataBase *pAsioData)
{
	J_Host *pHost = JoDeviceManager->GetDeviceObj(realPlay.hostId);
	if (pHost != NULL)
	{
		J_Channel *pChannel = NULL;
		pHost->MakeChannel(realPlay.channel, pChannel);
		if (pChannel != NULL)
			pChannel->OpenStream(&m_ringBuffer);

		pAsioData->ioRead.buf = m_readBuff;
		pAsioData->ioRead.bufLen = pAsioData->ioRead.finishedLen + sizeof(CmdHeader);
		pHost->ParserRequest(pAsioData);

		TLock(m_vecLocker);
		VideoInfo info;
		info.strHost = realPlay.hostId;
		info.nChanId = realPlay.channel;
		m_videoInfoVec.push_back(info);
		TUnlock(m_vecLocker);
	}
	return J_OK;
}

j_result_t CXlClient::StopView(const CliRealPlay &realPlay, J_AsioDataBase *pAsioData)
{
	J_Host *pHost = JoDeviceManager->GetDeviceObj(realPlay.hostId);
	if (pHost != NULL)
	{
		pAsioData->ioRead.buf = m_readBuff;
		pAsioData->ioRead.bufLen = pAsioData->ioRead.finishedLen + sizeof(CmdHeader);
		pHost->ParserRequest(pAsioData);

		J_Channel *pChannel = NULL;
		pHost->MakeChannel(realPlay.channel, pChannel);
		if (pChannel != NULL)
			pChannel->CloseStream(&m_ringBuffer);

		TLock(m_vecLocker);
		VideoInfoVec::iterator it = m_videoInfoVec.begin();
		for (; it!=m_videoInfoVec.end(); ++it)
		{
			if (it->strHost == realPlay.hostId && it->nChanId == realPlay.channel)
			{
				m_videoInfoVec.erase(it);
				break;
			}
		}
		TUnlock(m_vecLocker);
	}
	return J_OK;
}

j_result_t CXlClient::StartVod(const CliStartVod &startVod, J_AsioDataBase *pAsioData)
{
	J_Host *pHost = JoDeviceManager->GetDeviceObj(startVod.hostId);
	if (pHost != NULL)
	{
		J_Channel *pChannel = NULL;
		pHost->MakeChannel(startVod.channel, pChannel);
		if (pChannel != NULL)
			pChannel->OpenStream(&m_ringBuffer);

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

		J_Channel *pChannel = NULL;
		pHost->MakeChannel(stopVod.channel, pChannel);
		if (pChannel != NULL)
			pChannel->CloseStream(&m_ringBuffer);

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
		pHost->EnableAlarm(&m_alarmBuffer);
	m_alarmInfoVec.push_back(strHostId);
	atomic_inc(&m_nAlarmRefCnt);

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

			J_Channel *pChannel = NULL;
			pHost->MakeChannel(it->nChanId, pChannel);
			if (pChannel != NULL)
				pChannel->CloseStream(&m_ringBuffer);
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

			J_Channel *pChannel = NULL;
			pHost->MakeChannel(it->nChanId, pChannel);
			if (pChannel != NULL)
				pChannel->CloseStream(&m_ringBuffer);
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
	int nBodyLen = sizeof(CmdHeader) + sizeof(CliRetValue2) + sizeof(CmdTail);
	CliRetValue2 data = {0};
	data.nRetVal = 0x01;
	CXlHelper::MakeResponse(xlc_login, (j_char_t *)&data, sizeof(CliRetValue2), m_writeBuff);
	pAsioData->ioCall = J_AsioDataBase::j_write_e;
	CXlHelper::MakeNetData(pAsioData, m_writeBuff, nBodyLen);

	return J_OK;
}

j_result_t CXlClient::OnLogout(J_AsioDataBase *pAsioData)
{
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

	return J_OK;
}

j_result_t CXlClient::OnRealPlay(J_AsioDataBase *pAsioData)
{
	CliRealPlay *pReps = (CliRealPlay *)(m_readBuff + sizeof(CmdHeader));
	J_OS::LOGINFO("hostId = %s, channel = %d", pReps->hostId, pReps->channel & 0xFF);
	StartView(*pReps, pAsioData);
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
	J_OS::LOGINFO("hostId = %s, channel = %d", pReps->hostId, pReps->channel & 0xFF);
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
	J_OS::LOGINFO("hostId = %s, channel = %d %I64d %I64d", pReps->hostId, pReps->channel & 0xFF,
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
	J_OS::LOGINFO("hostId = %s, channel = %d", pReps->hostId, pReps->channel & 0xFF);
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

j_result_t CXlClient::OnSetTime(J_AsioDataBase *pAsioData)
{
	//SetTimeReq *pReps = (SetTimeReq *)(m_readBuff + sizeof(CmdHeader));
	//J_OS::LOGINFO("XlClient SysTime = %d", pReps->systime);
	//JoDeviceManager->SetDeviceTime(pReps->systime);

	//int nBodyLen = sizeof(CmdHeader) + sizeof(SetTimeReq) + sizeof(CmdTail);
	//SetTimeResp data = {0};
	//data.ret = 0x00;
	//CXlHlper::MakeResponse(xl_set_time, (j_char_t *)&data, sizeof(SetTimeReq), m_writeBuff);
	//pAsioData->ioCall = J_AsioDataBase::j_write_e;
	//MakeNetData(pAsioData, m_writeBuff, nBodyLen);

	return J_OK;
}

j_result_t CXlClient::OnStartAlarm(J_AsioDataBase *pAsioData)
{
	CliRealAlarmInfo *pReps = (CliRealAlarmInfo *)(m_readBuff + sizeof(CmdHeader));
	J_OS::LOGINFO("hostId = %s", pReps->hostId);
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
	J_OS::LOGINFO("hostId = %s", pReps->hostId);
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
