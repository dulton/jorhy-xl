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
#include "XlType.h"
#include "DeviceManager.h"
#include "MsSqlServer.h"

#pragma comment (lib, "Debug\\core.lib")

#define XL_CMD_BUFFER_SIZE (2 * 1024)
#define XL_DATA_BUFFER_SIZE (1024 * 1024)
#define XL_ALARM_BUFFER_SIZE (2 * 1024)
#define XL_LOG_BUFFER_SIZE (1024)
#define XL_DVR_BUFFER_SIZE (1024)
#define XL_USER_BUFFER_SIZE (1024)
#define XL_DEPARTMENT_BUFFER_SIZE (1024)
CXlClient::CXlClient(j_socket_t nSock)
// : m_socket(nSock)
{
	m_readBuff = new char[XL_CMD_BUFFER_SIZE];			
	m_writeBuff = new char[XL_CMD_BUFFER_SIZE];		
	m_dataBuff = new char[XL_DATA_BUFFER_SIZE];	
	m_alarmBuff = new char[XL_ALARM_BUFFER_SIZE];	
	m_logBuff = new char[XL_LOG_BUFFER_SIZE];	
	m_dvrBuff = new char[XL_DVR_BUFFER_SIZE];
	m_userBuff = new char[XL_USER_BUFFER_SIZE];
	m_departmentBuff = new char[XL_DEPARTMENT_BUFFER_SIZE];
	m_ioCmdState = xl_init_state;
	m_ioDataState = xl_init_state;
	m_ioAlarmState = xl_init_state;
	m_ioLogState = xl_init_state;
	m_ioDvrListState = xl_init_state;
	m_ioUserListState = xl_init_state;
	m_ioDepartmentListState = xl_init_state;
	m_nRefCnt = 0;
	m_nAlarmRefCnt = 0;
}

CXlClient::~CXlClient()
{
	delete m_readBuff;
	delete m_writeBuff;
	delete m_dataBuff;
	delete m_alarmBuff;
	delete m_logBuff;
	delete m_dvrBuff;
	delete m_userBuff;
	delete m_departmentBuff;
}

j_result_t CXlClient::ParserRequest(J_AsioDataBase *pAsioData)
{
	j_result_t nResult = J_OK;
	if (pAsioData->ioType == J_AsioDataBase::j_command_e)
	{
		nResult = ProcessRequest(pAsioData);
	}
	else if (pAsioData->ioType == J_AsioDataBase::j_data_e)
	{
		nResult = ProcessData(pAsioData);
	}
	else if (pAsioData->ioType == J_AsioDataBase::j_log_e)
	{
		nResult = ProcessLog(pAsioData);
	}
	else if (pAsioData->ioType == J_AsioDataBase::j_alarm_e)
	{
		nResult = ProcessAlarm(pAsioData);
	}
	else if (pAsioData->ioType == J_AsioDataBase::j_dvr_list_e)
	{
		nResult = ProcessDvrList(pAsioData);
	}
	else if (pAsioData->ioType == J_AsioDataBase::j_user_list_e)
	{
		nResult = ProcessUserList(pAsioData);
	}
		else if (pAsioData->ioType == J_AsioDataBase::j_department_list_e)
	{
		nResult = ProcessDepartmentList(pAsioData);
	}

	return nResult;
}

j_result_t CXlClient::Broken()
{
	StopAll();
	return J_OK;
}

j_result_t CXlClient::MakeRequest(j_char_t bCmd, j_char_t *pData, j_int32_t nLen, j_char_t *pBody)
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

j_result_t CXlClient::MakeResponse(j_char_t bCmd, j_char_t *pData, j_int32_t nLen, j_char_t *pBody)
{
	//填充头信息
	CmdHeader *pHeader = (CmdHeader *)pBody;
	pHeader->beginCode = 0xFF;
	pHeader->version = 0x0A;
	pHeader->flag = 0xFF;
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

j_uint32_t CXlClient::CheckNum(j_char_t *pData, j_int32_t nLen)
{
	j_uint32_t nCheckNum = 0xFE;
	for (int i=0; i<nLen; ++i)
		nCheckNum += pData[i];

	return (nCheckNum % 256);
}

j_result_t CXlClient::MakeNetData(J_AsioDataBase *pAsioData, j_char_t *pDataBuff, j_int32_t nLen)
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

j_result_t CXlClient::MakeLogData(J_AsioDataBase *pAsioData)
{
	if (m_logInfoQueue.size() > 0)
	{
		GetLogInfoResp logInfo = m_logInfoQueue.front();
		m_logInfoQueue.pop();
		MakeResponse(xl_get_loginfo, (char *)&logInfo, sizeof(GetLogInfoResp), m_logBuff);
		pAsioData->ioWrite.buf = m_logBuff;
		pAsioData->ioWrite.bufLen = sizeof(CmdHeader) + sizeof(GetLogInfoResp) + sizeof(CmdTail);
		pAsioData->ioWrite.finishedLen = 0;
		pAsioData->ioWrite.whole = true;
		pAsioData->ioWrite.shared = true;
	}
	else
	{
		m_ioLogState = xl_init_state;
		pAsioData->ioCall = J_AsioDataBase::j_keep_e;
	}

	return  J_OK;
}

j_result_t CXlClient::MakeAlarmData(J_AsioDataBase *pAsioData)
{
	if (m_alarmInfoQueue.size() > 0)
	{
		AlarmInfo alarmInfo = m_alarmInfoQueue.front();
		m_alarmInfoQueue.pop();
		MakeResponse(xl_get_alraminfo, (char *)&alarmInfo, sizeof(AlarmInfo), m_alarmBuff);
		pAsioData->ioWrite.buf = m_alarmBuff;
		pAsioData->ioWrite.bufLen = sizeof(CmdHeader) + sizeof(AlarmInfo) + sizeof(CmdTail);
		pAsioData->ioWrite.finishedLen = 0;
		pAsioData->ioWrite.whole = true;
		pAsioData->ioWrite.shared = true;
	}
	else
	{
		memset(&m_alarmHeader, 0, sizeof(J_StreamHeader));
		pAsioData->ioWrite.buf = m_alarmBuff;
		m_alarmBuffer.PopBuffer(m_alarmBuff, m_alarmHeader);
		pAsioData->ioWrite.bufLen = m_alarmHeader.dataLen;
		pAsioData->ioWrite.finishedLen = 0;
		pAsioData->ioWrite.whole = true;
		pAsioData->ioWrite.shared = true;
	}
	return J_OK;
}

j_result_t CXlClient::MakeDvrListData(J_AsioDataBase *pAsioData)
{
	if (m_dvrInfoQueue.size() > 0)
	{
		DVRInfo dvrInfo = m_dvrInfoQueue.front();
		m_dvrInfoQueue.pop();
		MakeResponse(xl_get_dvr_list, (char *)&dvrInfo, sizeof(DVRInfo), m_dvrBuff);
		pAsioData->ioWrite.buf = m_dvrBuff;
		pAsioData->ioWrite.bufLen = sizeof(CmdHeader) + sizeof(DVRInfo) + sizeof(CmdTail);
		pAsioData->ioWrite.finishedLen = 0;
		pAsioData->ioWrite.whole = true;
		pAsioData->ioWrite.shared = true;
	}
	else
	{
		m_ioDvrListState = xl_init_state;
		pAsioData->ioCall = J_AsioDataBase::j_keep_e;
	}

	return  J_OK;
}

j_result_t CXlClient::MakeUserListData(J_AsioDataBase *pAsioData)
{
	if (m_userInfoQueue.size() > 0)
	{
		UserInfo userInfo = m_userInfoQueue.front();
		m_userInfoQueue.pop();
		MakeResponse(xl_get_user_list, (char *)&userInfo, sizeof(UserInfo), m_userBuff);
		pAsioData->ioWrite.buf = m_userBuff;
		pAsioData->ioWrite.bufLen = sizeof(CmdHeader) + sizeof(UserInfo) + sizeof(CmdTail);
		pAsioData->ioWrite.finishedLen = 0;
		pAsioData->ioWrite.whole = true;
		pAsioData->ioWrite.shared = true;
	}
	else
	{
		m_ioUserListState = xl_init_state;
		pAsioData->ioCall = J_AsioDataBase::j_keep_e;
	}

	return  J_OK;
}

j_result_t CXlClient::MakeDepartmentListData(J_AsioDataBase *pAsioData)
{
	if (m_departmentInfoQueue.size() > 0)
	{
		DepartmentInfo departmentInfo = m_departmentInfoQueue.front();
		m_departmentInfoQueue.pop();
		MakeResponse(xl_get_department_list, (char *)&departmentInfo, sizeof(DepartmentInfo), m_departmentBuff);
		pAsioData->ioWrite.buf = m_departmentBuff;
		pAsioData->ioWrite.bufLen = sizeof(CmdHeader) + sizeof(DepartmentInfo) + sizeof(CmdTail);
		pAsioData->ioWrite.finishedLen = 0;
		pAsioData->ioWrite.whole = true;
		pAsioData->ioWrite.shared = true;
	}
	else
	{
		m_ioDepartmentListState = xl_init_state;
		pAsioData->ioCall = J_AsioDataBase::j_keep_e;
	}

	return  J_OK;
}

j_result_t CXlClient::ProcessRequest(J_AsioDataBase *pAsioData)	
{
	if (m_ioCmdState == xl_init_state)
	{
		pAsioData->ioCall = J_AsioDataBase::j_read_e;
		MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));

		m_ioCmdState = xl_read_head_state;
	}
	else if (m_ioCmdState == xl_read_head_state)
	{
		MakeNetData(pAsioData, m_readBuff + sizeof(CmdHeader), *((j_int32_t *)(m_readBuff + 4)) + sizeof(CmdTail));
		pAsioData->ioCall = J_AsioDataBase::j_read_e;

		m_ioCmdState = xl_read_data_state;
	}
	else if (m_ioCmdState == xl_read_data_state)
	{
		CmdHeader *pHeader = (CmdHeader *)m_readBuff;
		//assert((pHeader->flag & 0xFF)== 0x00);
		switch (pHeader->cmd)
		{
		case xl_login:
			OnLogin(pAsioData);
			m_ioCmdState = xl_write_body_state;
			break;
		case xl_logout:
			OnLogout(pAsioData);
			m_ioCmdState = xl_write_body_state;
			break;
		case xl_real_play:
			OnRealPlay(pAsioData);
			m_ioCmdState = xl_read_head_state;
			break;
		case xl_real_stop:
			OnRealStop(pAsioData);
			m_ioCmdState = xl_write_body_state;
			break;
		case xl_vod_play:
			OnVodPlay(pAsioData);
			m_ioCmdState = xl_read_head_state;
			break;
		case xl_vod_stop:
			OnVodStop(pAsioData);
			m_ioCmdState = xl_write_body_state;
			break;
		case xl_set_time:
			OnSetTime(pAsioData);
			m_ioCmdState = xl_write_body_state;
			break;
		case xl_get_devinfo:
			OnGetDevInfo(pAsioData);
			m_ioCmdState = xl_write_body_state;
			break;
		case xl_get_loginfo:
			OnGetLogInfo(pAsioData);
			m_ioCmdState = xl_read_head_state;
			break;
		case xl_start_real_alarm:
			OnStartAlarm(pAsioData);
			m_ioCmdState = xl_read_head_state;
			break;
		case xl_stop_real_alarm:
			OnStopAlarm(pAsioData);
			m_ioCmdState = xl_write_body_state;
			break;
		case xl_get_totle_dvr_info:
			OnGetTotleDvrInfo(pAsioData);
			m_ioCmdState = xl_write_body_state;
			break;
		case xl_get_totle_user_info:
			OnGetTotleUserInfo(pAsioData);
			m_ioCmdState = xl_write_body_state;
			break;
		case xl_get_dvr_list:
			OnGetDvrList(pAsioData);
			m_ioCmdState = xl_read_head_state;
			break;
		case xl_get_user_list:
			OnGetUserList(pAsioData);
			m_ioCmdState = xl_read_head_state;
			break;
		case xl_get_department_list:
			OnGetDepartmentList(pAsioData);
			m_ioCmdState = xl_read_head_state;
			break;
		default:
			break;
		}
		J_OS::LOGINFO("%d %X", pHeader->cmd, pHeader->flag & 0xFF);
	}
	else
	{	
		pAsioData->ioCall = J_AsioDataBase::j_read_e;
		MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));

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

j_result_t CXlClient::ProcessLog(J_AsioDataBase *pAsioData)
{
	j_result_t nResult = J_OK;
	if (m_ioLogState == xl_init_state)
	{
		pAsioData->ioType = J_AsioDataBase::j_log_e;
		//pAsioData->hEvent = WSACreateEvent();

		pAsioData->ioCall = J_AsioDataBase::j_write_e;
		m_ioLogState = xl_write_body_state;
		nResult = MakeLogData(pAsioData);
	}
	else
	{
		pAsioData->ioCall = J_AsioDataBase::j_write_e;
		m_ioLogState = xl_write_body_state;
		nResult =MakeLogData(pAsioData);
	}

	return nResult;
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

j_result_t CXlClient::ProcessDvrList(J_AsioDataBase *pAsioData)
{
	j_result_t nResult = J_OK;
	if (m_ioDvrListState == xl_init_state)
	{
		pAsioData->ioType = J_AsioDataBase::j_dvr_list_e;
		//pAsioData->hEvent = WSACreateEvent();

		pAsioData->ioCall = J_AsioDataBase::j_write_e;
		m_ioDvrListState = xl_write_body_state;
		nResult = MakeDvrListData(pAsioData);
	}
	else
	{
		pAsioData->ioCall = J_AsioDataBase::j_write_e;
		m_ioDvrListState = xl_write_body_state;
		nResult =MakeDvrListData(pAsioData);
	}

	return nResult;
}

j_result_t CXlClient::ProcessUserList(J_AsioDataBase *pAsioData)
{
	j_result_t nResult = J_OK;
	if (m_ioUserListState == xl_init_state)
	{
		pAsioData->ioType = J_AsioDataBase::j_user_list_e;
		//pAsioData->hEvent = WSACreateEvent();

		pAsioData->ioCall = J_AsioDataBase::j_write_e;
		m_ioUserListState = xl_write_body_state;
		nResult = MakeUserListData(pAsioData);
	}
	else
	{
		pAsioData->ioCall = J_AsioDataBase::j_write_e;
		m_ioUserListState = xl_write_body_state;
		nResult =MakeUserListData(pAsioData);
	}

	return nResult;
}

j_result_t CXlClient::ProcessDepartmentList(J_AsioDataBase *pAsioData)
{
	j_result_t nResult = J_OK;
	if (m_ioDepartmentListState == xl_init_state)
	{
		pAsioData->ioType = J_AsioDataBase::j_department_list_e;
		//pAsioData->hEvent = WSACreateEvent();

		pAsioData->ioCall = J_AsioDataBase::j_write_e;
		m_ioDepartmentListState = xl_write_body_state;
		nResult = MakeDepartmentListData(pAsioData);
	}
	else
	{
		pAsioData->ioCall = J_AsioDataBase::j_write_e;
		m_ioDepartmentListState = xl_write_body_state;
		nResult =MakeDepartmentListData(pAsioData);
	}

	return nResult;
}

j_result_t CXlClient::StartView(j_string_t strHostId, j_int32_t nChanId, J_AsioDataBase *pAsioData)
{
	J_Host *pHost = JoDeviceManager->GetDeviceObj(strHostId.c_str());
	if (pHost != NULL)
	{
		J_Channel *pChannel = NULL;
		pHost->MakeChannel(nChanId, pChannel);
		if (pChannel != NULL)
			pChannel->OpenStream(&m_ringBuffer);

		pAsioData->ioRead.buf = m_readBuff;
		pAsioData->ioRead.bufLen = pAsioData->ioRead.finishedLen + sizeof(CmdHeader);
		pHost->ParserRequest(pAsioData);

		TLock(m_vecLocker);
		VideoInfo info;
		info.strHost = strHostId;
		info.nChanId = nChanId;
		m_videoInfoVec.push_back(info);
		TUnlock(m_vecLocker);
	}
	return J_OK;
}

j_result_t CXlClient::StopView(j_string_t strHostId, j_int32_t nChanId, J_AsioDataBase *pAsioData)
{
	J_Host *pHost = JoDeviceManager->GetDeviceObj(strHostId.c_str());
	if (pHost != NULL)
	{
		pAsioData->ioRead.buf = m_readBuff;
		pAsioData->ioRead.bufLen = pAsioData->ioRead.finishedLen + sizeof(CmdHeader);
		pHost->ParserRequest(pAsioData);

		J_Channel *pChannel = NULL;
		pHost->MakeChannel(nChanId, pChannel);
		if (pChannel != NULL)
			pChannel->CloseStream(&m_ringBuffer);

		TLock(m_vecLocker);
		VideoInfoVec::iterator it = m_videoInfoVec.begin();
		for (; it!=m_videoInfoVec.end(); ++it)
		{
			if (it->strHost == strHostId && it->nChanId == nChanId)
			{
				m_videoInfoVec.erase(it);
				break;
			}
		}
		TUnlock(m_vecLocker);
	}
	return J_OK;
}

j_result_t CXlClient::StartAlarm(j_string_t strHostId, j_time_t tmStart, j_time_t tmEnd)
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
	JoDataBaseObj->GetAlarmInfoList(strHostId.c_str(), tmStart, tmEnd, m_alarmInfoQueue);
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
	J_Host *pHost = NULL;
	J_AsioDataBase asioData;
	struct RealPlayBody {
		CmdHeader head;
		RealPlayReq data;
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
			MakeRequest(xl_real_stop, (char *)&realPlayBody.data, sizeof(RealPlayReq), (char *)&realPlayBody);

			asioData.ioType = J_AsioDataBase::j_command_e;
			asioData.ioRead.buf = (j_char_t *)&realPlayBody;
			asioData.ioRead.bufLen = sizeof(CmdHeader) + sizeof(RealPlayReq) + sizeof(CmdTail);
			pHost->ParserRequest(&asioData);

			J_Channel *pChannel = NULL;
			pHost->MakeChannel(it->nChanId, pChannel);
			if (pChannel != NULL)
				pChannel->CloseStream(&m_ringBuffer);
		}
	}
	m_videoInfoVec.clear();
	TUnlock(m_vecLocker);

	TLock(m_vecAlarmLocker);
	AlarmInfoVec::iterator itAlarm = m_alarmInfoVec.begin();
	for (; itAlarm!=m_alarmInfoVec.end(); ++itAlarm)
	{
		pHost = JoDeviceManager->GetDeviceObj(itAlarm->c_str());
		if (pHost != NULL)
		{
			pHost->EnableAlarm(&m_alarmBuffer, false);
		}
	}
	m_alarmInfoVec.clear();
	TUnlock(m_vecAlarmLocker);

	m_nAlarmRefCnt = 0;
	m_nRefCnt = 0;

	return J_OK;
}

j_result_t CXlClient::OnLogin(J_AsioDataBase *pAsioData)
{
	int nBodyLen = sizeof(CmdHeader) + sizeof(LoginResp) + sizeof(CmdTail);
	LoginResp data = {0};
	data.ret = 0x00;
	MakeResponse(xl_login, (j_char_t *)&data, sizeof(LoginResp), m_writeBuff);
	pAsioData->ioCall = J_AsioDataBase::j_write_e;
	MakeNetData(pAsioData, m_writeBuff, nBodyLen);

	return J_OK;
}

j_result_t CXlClient::OnLogout(J_AsioDataBase *pAsioData)
{
	int nBodyLen = sizeof(CmdHeader) + sizeof(LoginResp) + sizeof(CmdTail);
	LoginResp data = {0};
	data.ret = 0x00;
	MakeResponse(xl_logout, (j_char_t *)&data, sizeof(LoginResp), m_writeBuff);
	pAsioData->ioCall = J_AsioDataBase::j_write_e;
	MakeNetData(pAsioData, m_writeBuff, nBodyLen);

	return J_OK;
}

j_result_t CXlClient::OnRealPlay(J_AsioDataBase *pAsioData)
{
	RealPlayReq *pReps = (RealPlayReq *)(m_readBuff + sizeof(CmdHeader));
	J_OS::LOGINFO("hostId = %s, channel = %d", pReps->hostId, pReps->channel & 0xFF);
	StartView(pReps->hostId, pReps->channel, pAsioData);
	pAsioData->ioCall = J_AsioDataBase::j_read_e;
	MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	if (m_nRefCnt == 0)
		pAsioData->ioCall = J_AsioDataBase::j_read_write_e;
	atomic_inc(&m_nRefCnt);

	return J_OK;
}

j_result_t CXlClient::OnRealStop(J_AsioDataBase *pAsioData)
{
	RealStopReq *pReps = (RealStopReq *)(m_readBuff + sizeof(CmdHeader));
	J_OS::LOGINFO("hostId = %s, channel = %d", pReps->hostId, pReps->channel & 0xFF);
	StopView(pReps->hostId, pReps->channel, pAsioData);

	int nBodyLen = sizeof(CmdHeader) + sizeof(RealPlayResp) + sizeof(CmdTail);
	RealPlayResp data = {0};
	data.ret = 0x00;
	MakeResponse(xl_real_stop, (j_char_t *)&data, sizeof(RealPlayResp), m_writeBuff);
	pAsioData->ioCall = J_AsioDataBase::j_write_e;
	MakeNetData(pAsioData, m_writeBuff, nBodyLen);
	if (m_nRefCnt > 0)
		atomic_dec(&m_nRefCnt);

	return J_OK;
}

j_result_t CXlClient::OnVodPlay(J_AsioDataBase *pAsioData)
{
	VodPlayReq *pReps = (VodPlayReq *)(m_readBuff + sizeof(CmdHeader));
	J_OS::LOGINFO("hostId = %s, channel = %d", pReps->hostId, pReps->channel & 0xFF);
	StartView(pReps->hostId, pReps->channel, pAsioData);
	pAsioData->ioCall = J_AsioDataBase::j_read_e;
	MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	if (m_nRefCnt == 0)
		pAsioData->ioCall = J_AsioDataBase::j_read_write_e;
	atomic_inc(&m_nRefCnt);

	return J_OK;
}

j_result_t CXlClient::OnVodStop(J_AsioDataBase *pAsioData)
{
	VodStopReq *pReps = (VodStopReq *)(m_readBuff + sizeof(CmdHeader));
	J_OS::LOGINFO("hostId = %s, channel = %d", pReps->hostId, pReps->channel & 0xFF);
	StopView(pReps->hostId, pReps->channel, pAsioData);

	int nBodyLen = sizeof(CmdHeader) + sizeof(VodPlayResp) + sizeof(CmdTail);
	VodStopResp data = {0};
	data.ret = 0x00;
	MakeResponse(xl_vod_stop, (j_char_t *)&data, sizeof(VodStopResp), m_writeBuff);
	pAsioData->ioCall = J_AsioDataBase::j_write_e;
	MakeNetData(pAsioData, m_writeBuff, nBodyLen);
	if (m_nRefCnt > 0)
		atomic_dec(&m_nRefCnt);

	return J_OK;
}

j_result_t CXlClient::OnSetTime(J_AsioDataBase *pAsioData)
{
	SetTimeReq *pReps = (SetTimeReq *)(m_readBuff + sizeof(CmdHeader));
	J_OS::LOGINFO("XlClient SysTime = %d", pReps->systime);
	JoDeviceManager->SetDeviceTime(pReps->systime);

	int nBodyLen = sizeof(CmdHeader) + sizeof(SetTimeReq) + sizeof(CmdTail);
	SetTimeResp data = {0};
	data.ret = 0x00;
	MakeResponse(xl_set_time, (j_char_t *)&data, sizeof(SetTimeReq), m_writeBuff);
	pAsioData->ioCall = J_AsioDataBase::j_write_e;
	MakeNetData(pAsioData, m_writeBuff, nBodyLen);

	return J_OK;
}

j_result_t CXlClient::OnGetDevInfo(J_AsioDataBase *pAsioData)
{
	typedef struct _hostInfo
	{
		char hostId[32];
	}HostInfo, *LPHostInfo;
	HostInfo *pReps = (HostInfo *)(m_readBuff + sizeof(CmdHeader));

	HostInfoResp hostResp = {0};
	strcpy(hostResp.hostId, pReps->hostId);
	JoDataBaseObj->GetDevInfo(hostResp);

	int nBodyLen = sizeof(CmdHeader) + sizeof(HostInfoResp) + sizeof(CmdTail);
	MakeResponse(xl_get_devinfo, (j_char_t *)&hostResp, sizeof(HostInfoResp), m_writeBuff);
	pAsioData->ioCall = J_AsioDataBase::j_write_e;
	MakeNetData(pAsioData, m_writeBuff, nBodyLen);

	return J_OK;
}

j_result_t CXlClient::OnGetLogInfo(J_AsioDataBase *pAsioData)
{
	typedef struct _logInfo
	{
		char hostId[32];
		time_t tmStart;
		time_t tmEnd;
	}LogInfo, *LPLogInfo;

	LogInfo *pReps = (LogInfo *)(m_readBuff + sizeof(CmdHeader));
	J_OS::LOGINFO("hostId = %s, start = %I64d, end = %I64d", pReps->hostId, pReps->tmStart, pReps->tmEnd);
	pAsioData->ioCall = J_AsioDataBase::j_read_e;
	MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	if (m_ioLogState == xl_init_state)
	{
		JoDataBaseObj->GetLogInfoList(pReps->hostId, pReps->tmStart, pReps->tmEnd, m_logInfoQueue);
		pAsioData->ioCall = J_AsioDataBase::j_read_write_log_e;
	}

	return J_OK;
}

j_result_t CXlClient::OnStartAlarm(J_AsioDataBase *pAsioData)
{
	typedef struct _alarmInfo
	{
		char hostId[32];
		time_t tmStart;
		time_t tmEnd;
	}AlarmInfo, *LPAlarmInfo;

	AlarmInfo *pReps = (AlarmInfo *)(m_readBuff + sizeof(CmdHeader));
	J_OS::LOGINFO("hostId = %s, start = %I64d, end = %I64d", pReps->hostId, pReps->tmStart, pReps->tmEnd);
	pAsioData->ioCall = J_AsioDataBase::j_read_e;
	MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	if (m_ioAlarmState == xl_init_state)
	{
		StartAlarm(pReps->hostId, pReps->tmStart, pReps->tmEnd);
		pAsioData->ioCall = J_AsioDataBase::j_read_write_alarm_e;
		m_ioAlarmState = xl_write_body_state;
	}

	return J_OK;
}

j_result_t CXlClient::OnStopAlarm(J_AsioDataBase *pAsioData)
{
	typedef struct _alarmStop
	{
		char hostId[32];
	}AlarmStop, *LPAlarmStop;

	AlarmStop *pReps = (AlarmStop *)(m_readBuff + sizeof(CmdHeader));
	J_OS::LOGINFO("hostId = %s", pReps->hostId);
	int nBodyLen = sizeof(CmdHeader) + sizeof(StopAlarmInfoResp) + sizeof(CmdTail);
	StopAlarmInfoResp data = {0};
	data.ret = 0x00;
	MakeResponse(xl_stop_real_alarm, (j_char_t *)&data, sizeof(StopAlarmInfoResp), m_writeBuff);
	pAsioData->ioCall = J_AsioDataBase::j_write_e;
	MakeNetData(pAsioData, m_writeBuff, nBodyLen);

	StopAlarm(pReps->hostId);
	m_ioAlarmState = xl_init_state;

	return J_OK;
}

j_result_t CXlClient::OnGetTotleDvrInfo(J_AsioDataBase *pAsioData)
{
	DVRTotleInfo dvrTotleInfoResp = {0};
	JoDataBaseObj->GetDvrTotleInfo(dvrTotleInfoResp);

	int nBodyLen = sizeof(CmdHeader) + sizeof(DVRTotleInfo) + sizeof(CmdTail);
	MakeResponse(xl_get_totle_dvr_info, (j_char_t *)&dvrTotleInfoResp, sizeof(DVRTotleInfo), m_writeBuff);
	pAsioData->ioCall = J_AsioDataBase::j_write_e;
	MakeNetData(pAsioData, m_writeBuff, nBodyLen);

	return J_OK;
}

j_result_t CXlClient::OnGetTotleUserInfo(J_AsioDataBase *pAsioData)
{
	UserTotleInfo userTotleInfoResp = {0};
	JoDataBaseObj->GetUserTotleInfo(userTotleInfoResp);

	int nBodyLen = sizeof(CmdHeader) + sizeof(UserTotleInfo) + sizeof(CmdTail);
	MakeResponse(xl_get_totle_user_info, (j_char_t *)&userTotleInfoResp, sizeof(UserTotleInfo), m_writeBuff);
	pAsioData->ioCall = J_AsioDataBase::j_write_e;
	MakeNetData(pAsioData, m_writeBuff, nBodyLen);

	return J_OK;
}

j_result_t CXlClient::OnGetDvrList(J_AsioDataBase *pAsioData)
{
	DvrListReq *pResp = (DvrListReq *)(m_readBuff + sizeof(CmdHeader));
	J_OS::LOGINFO("nType = %d, lDepartMentId = %d", pResp->nType, pResp->lDepartmentID);
	pAsioData->ioCall = J_AsioDataBase::j_read_e;
	MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	if (m_ioDvrListState == xl_init_state)
	{
		JoDataBaseObj->GetDvrList(pResp->nType, pResp->lDepartmentID, m_dvrInfoQueue);
		pAsioData->ioCall = J_AsioDataBase::j_read_write_dvrlist_e;
	}

	return J_OK;
}

j_result_t CXlClient::OnGetUserList(J_AsioDataBase *pAsioData)
{
	UserType *pResp = (UserType *)(m_readBuff + sizeof(CmdHeader));
	J_OS::LOGINFO("nType = %d", pResp->nType);
	pAsioData->ioCall = J_AsioDataBase::j_read_e;
	MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	if (m_ioUserListState == xl_init_state)
	{
		JoDataBaseObj->GetUserList(pResp->nType, m_userInfoQueue);
		pAsioData->ioCall = J_AsioDataBase::j_read_write_userlist_e;
	}

	return J_OK;
}

j_result_t CXlClient::OnGetDepartmentList(J_AsioDataBase *pAsioData)
{
	pAsioData->ioCall = J_AsioDataBase::j_read_e;
	MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	if (m_ioDepartmentListState == xl_init_state)
	{
		JoDataBaseObj->GetDepartmentList(m_departmentInfoQueue);
		pAsioData->ioCall = J_AsioDataBase::j_read_write_departmentlist_e;
	}

	return J_OK;
}