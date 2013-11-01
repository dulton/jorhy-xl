///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx科技有限公司(版权声明) 
/// All rights reserved. 
/// 
/// @file      XlConfig.cpp 
/// @brief     数据库相关中间接口实现
///
/// @version   1.0 (版本声明)
/// @author    Jorhy (joorhy@gmail.com) 
/// @date      2013/11/01 09:03 
/// 修订说明：最初版本
///////////////////////////////////////////////////////////////////////////  
#include "XlConfig.h"
#include "XlHelper.h"
#include "MsSqlServer.h"

#define XL_DVR_BUFFER_SIZE (1024)
#define XL_USER_BUFFER_SIZE (1024)
#define XL_DEPARTMENT_BUFFER_SIZE (1024)
#define XL_ALARM_BUFFER_SIZE (2 * 1024)
#define XL_LOG_BUFFER_SIZE (1024)

CXlConfig::CXlConfig()
{
	m_ioDvrListState = xl_init_state;
	m_ioUserListState = xl_init_state;
	m_ioDepartmentListState = xl_init_state;
	m_ioAlarmState = xl_init_state;
	m_ioLogState = xl_init_state;

	m_dvrBuff = new char[XL_DVR_BUFFER_SIZE];
	m_userBuff = new char[XL_USER_BUFFER_SIZE];
	m_departmentBuff = new char[XL_DEPARTMENT_BUFFER_SIZE];
	m_alarmBuff = new char[XL_ALARM_BUFFER_SIZE];	
	m_logBuff = new char[XL_LOG_BUFFER_SIZE];	
}

CXlConfig::~CXlConfig()
{
	delete m_dvrBuff;
	delete m_userBuff;
	delete m_departmentBuff;
	delete m_alarmBuff;
	delete m_logBuff;
}

j_result_t CXlConfig::ProcessConfigRequest(j_int32_t nType, J_AsioDataBase *pAsioData)
{
	j_result_t nResult = J_OK;
	switch (nType)
	{
	case xlc_get_alarm_info:
		nResult = OnGetAlarmInfo(pAsioData);
		break;
	case xlc_get_dvr_summary:
		nResult = OnGetTotleDvrInfo(pAsioData);
		break;
	case xlc_get_user_summary:
		nResult = OnGetTotleUserInfo(pAsioData);
		break;
	case xlc_get_dvr_list:
		nResult = OnGetDvrList(pAsioData);
		break;
	case xlc_get_user_list:
		nResult = OnGetUserList(pAsioData);
		break;
	case xlc_get_department_list:
		nResult = OnGetDepartmentList(pAsioData);
		break;
	case xlc_get_dvr_info:
		nResult = OnGetDvrInfo(pAsioData);
		break;
	case xlc_get_user_info:
		nResult = OnGetUserInfo(pAsioData);
		break;
	case xlc_get_department_info:
		nResult = OnGetDepartmentInfo(pAsioData);
		break;
	case xlc_add_dvr_info:
		nResult = OnAddDvrInfo(pAsioData);
		break;
	case xlc_add_user_info:
		nResult = OnAddUserInfo(pAsioData);
		break;
	case xlc_add_department_info:
		nResult = OnAddDepartmentInfo(pAsioData);
		break;
	case xlc_mod_dvr_info:
		nResult = OnModDvrInfo(pAsioData);
		break;
	case xlc_mod_user_info:
		nResult = OnModUserInfo(pAsioData);
		break;
	case xlc_mod_department_info:
		nResult = OnModDepartmentInfo(pAsioData);
		break;
	case xlc_del_dvr_info:
		nResult = OnDelDvrInfo(pAsioData);
		break;
	case xlc_del_user_info:
		nResult = OnDelUserInfo(pAsioData);
		break;
	case xlc_del_department_info:
		nResult = OnDelDepartmentInfo(pAsioData);
		break;
	default:
		break;
	}
	return nResult;
}

j_result_t CXlConfig::ProcessConfigData(j_int32_t nType, J_AsioDataBase *pAsioData)
{
	j_result_t nResult = J_OK;
	switch (nType)
	{
	case J_AsioDataBase::j_log_e:
		nResult = ProcessLog(pAsioData);
		break;
	case J_AsioDataBase::j_alarm_history_e:
		nResult = ProcessAlarm(pAsioData);
		break;
	case J_AsioDataBase::j_dvr_list_e:
		nResult = ProcessDvrList(pAsioData);
		break;
	case J_AsioDataBase::j_user_list_e:
		nResult = ProcessUserList(pAsioData);
		break;
	case J_AsioDataBase::j_department_list_e:
		nResult = ProcessDepartmentList(pAsioData);
		break;
	default:
		break;
	}
	return nResult;
}

j_result_t CXlConfig::ProcessDvrList(J_AsioDataBase *pAsioData)
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

j_result_t CXlConfig::ProcessUserList(J_AsioDataBase *pAsioData)
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

j_result_t CXlConfig::ProcessDepartmentList(J_AsioDataBase *pAsioData)
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

j_result_t CXlConfig::ProcessLog(J_AsioDataBase *pAsioData)
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

j_result_t CXlConfig::ProcessAlarm(J_AsioDataBase *pAsioData)
{
	j_result_t nResult = J_OK;
	if (m_ioAlarmState == xl_init_state)
	{
		pAsioData->ioType = J_AsioDataBase::j_alarm_history_e;
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

	return nResult;
}

j_result_t CXlConfig::MakeLogData(J_AsioDataBase *pAsioData)
{
	//if (m_logInfoQueue.size() > 0)
	//{
	//	GetLogInfoResp logInfo = m_logInfoQueue.front();
	//	m_logInfoQueue.pop();
	//	MakeResponse(xl_get_loginfo, (char *)&logInfo, sizeof(GetLogInfoResp), m_logBuff);
	//	pAsioData->ioWrite.buf = m_logBuff;
	//	pAsioData->ioWrite.bufLen = sizeof(CmdHeader) + sizeof(GetLogInfoResp) + sizeof(CmdTail);
	//	pAsioData->ioWrite.finishedLen = 0;
	//	pAsioData->ioWrite.whole = true;
	//	pAsioData->ioWrite.shared = true;
	//}
	//else
	//{
	//	m_ioLogState = xl_init_state;
	//	pAsioData->ioCall = J_AsioDataBase::j_keep_e;
	//}

	return  J_OK;
}

j_result_t CXlConfig::MakeAlarmData(J_AsioDataBase *pAsioData)
{
	if (m_alarmInfoQueue.size() > 0)
	{
		CliAlarmInfo alarmInfo = m_alarmInfoQueue.front();
		m_alarmInfoQueue.pop();
		CXlHelper::MakeResponse(xlc_get_alarm_info, (char *)&alarmInfo, sizeof(CliAlarmInfo), m_alarmBuff);
		pAsioData->ioWrite.buf = m_alarmBuff;
		pAsioData->ioWrite.bufLen = sizeof(CmdHeader) + sizeof(CliAlarmInfo) + sizeof(CmdTail);
		pAsioData->ioWrite.finishedLen = 0;
		pAsioData->ioWrite.whole = true;
		pAsioData->ioWrite.shared = true;
	}
	else
	{
		m_ioAlarmState = xl_init_state;
		pAsioData->ioCall = J_AsioDataBase::j_keep_e;
	}
	return J_OK;
}
j_result_t CXlConfig::MakeDvrListData(J_AsioDataBase *pAsioData)
{
	if (m_dvrInfoQueue.size() > 0)
	{
		CliHostInfo dvrInfo = m_dvrInfoQueue.front();
		m_dvrInfoQueue.pop();
		CXlHelper::MakeResponse(xlc_get_dvr_list, (char *)&dvrInfo, sizeof(CliHostInfo), m_dvrBuff);
		pAsioData->ioWrite.buf = m_dvrBuff;
		pAsioData->ioWrite.bufLen = sizeof(CmdHeader) + sizeof(CliHostInfo) + sizeof(CmdTail);
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

j_result_t CXlConfig::MakeUserListData(J_AsioDataBase *pAsioData)
{
	if (m_userInfoQueue.size() > 0)
	{
		CliUserInfo userInfo = m_userInfoQueue.front();
		m_userInfoQueue.pop();
		CXlHelper::MakeResponse(xlc_get_user_list, (char *)&userInfo, sizeof(CliUserInfo), m_userBuff);
		pAsioData->ioWrite.buf = m_userBuff;
		pAsioData->ioWrite.bufLen = sizeof(CmdHeader) + sizeof(CliUserInfo) + sizeof(CmdTail);
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

j_result_t CXlConfig::MakeDepartmentListData(J_AsioDataBase *pAsioData)
{
	if (m_departmentInfoQueue.size() > 0)
	{
		CliDepartmentInfo departmentInfo = m_departmentInfoQueue.front();
		m_departmentInfoQueue.pop();
		CXlHelper::MakeResponse(xlc_get_department_list, (char *)&departmentInfo, sizeof(CliDepartmentInfo), m_departmentBuff);
		pAsioData->ioWrite.buf = m_departmentBuff;
		pAsioData->ioWrite.bufLen = sizeof(CmdHeader) + sizeof(CliDepartmentInfo) + sizeof(CmdTail);
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

j_result_t CXlConfig::OnGetAlarmInfo(J_AsioDataBase *pAsioData)
{
	//DevGetAlarmInfo *pReps = (AlarmInfo *)(m_readBuff + sizeof(CmdHeader));
	//J_OS::LOGINFO("hostId = %s, start = %I64d, end = %I64d", pReps->hostId, pReps->tmStart, pReps->tmEnd);
	//pAsioData->ioCall = J_AsioDataBase::j_read_e;
	//CXlHelper::MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	//if (m_ioAlarmState == xl_init_state)
	//{
	//	JoDataBaseObj->GetAlarmInfoList(pReps->hostId, pReps->tmStart, pReps->tmEnd, m_alarmInfoQueue);
	//	pAsioData->ioCall = J_AsioDataBase::j_read_write_alarm_e;
	//	m_ioAlarmState = xl_write_body_state;
	//}
	return J_OK;
}

j_result_t CXlConfig::OnGetTotleDvrInfo(J_AsioDataBase *pAsioData)
{
	CliHostTotleInfo dvrTotleInfoResp = {0};
	JoDataBaseObj->GetDvrTotleInfo(dvrTotleInfoResp);

	int nBodyLen = sizeof(CmdHeader) + sizeof(CliHostTotleInfo) + sizeof(CmdTail);
	CXlHelper::MakeResponse(xlc_get_dvr_summary, (j_char_t *)&dvrTotleInfoResp, sizeof(CliHostTotleInfo), m_writeBuff);
	pAsioData->ioCall = J_AsioDataBase::j_write_e;
	CXlHelper::MakeNetData(pAsioData, m_writeBuff, nBodyLen);

	return J_OK;
}

j_result_t CXlConfig::OnGetTotleUserInfo(J_AsioDataBase *pAsioData)
{
	CliUserTotleInfo userTotleInfoResp = {0};
	JoDataBaseObj->GetUserTotleInfo(userTotleInfoResp);

	int nBodyLen = sizeof(CmdHeader) + sizeof(CliUserTotleInfo) + sizeof(CmdTail);
	CXlHelper::MakeResponse(xlc_get_user_summary, (j_char_t *)&userTotleInfoResp, sizeof(CliUserTotleInfo), m_writeBuff);
	pAsioData->ioCall = J_AsioDataBase::j_write_e;
	CXlHelper::MakeNetData(pAsioData, m_writeBuff, nBodyLen);

	return J_OK;
}

j_result_t CXlConfig::OnGetDvrList(J_AsioDataBase *pAsioData)
{
	//CliHostListReq *pResp = (DvrListReq *)(m_readBuff + sizeof(CmdHeader));
	//J_OS::LOGINFO("nType = %d, lDepartMentId = %d", pResp->nType, pResp->lDepartmentID);
	//pAsioData->ioCall = J_AsioDataBase::j_read_e;
	//CXlHelper::MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	//if (m_ioDvrListState == xl_init_state)
	//{
	//	JoDataBaseObj->GetDvrList(pResp->nType, pResp->lDepartmentID, m_dvrInfoQueue);
	//	pAsioData->ioCall = J_AsioDataBase::j_read_write_dvrlist_e;
	//}

	return J_OK;
}

j_result_t CXlConfig::OnGetUserList(J_AsioDataBase *pAsioData)
{
	//UserType *pResp = (UserType *)(m_readBuff + sizeof(CmdHeader));
	//J_OS::LOGINFO("nType = %d", pResp->nType);
	//pAsioData->ioCall = J_AsioDataBase::j_read_e;
	//CXlHelper::MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	//if (m_ioUserListState == xl_init_state)
	//{
	//	JoDataBaseObj->GetUserList(pResp->nType, m_userInfoQueue);
	//	pAsioData->ioCall = J_AsioDataBase::j_read_write_userlist_e;
	//}

	return J_OK;
}

j_result_t CXlConfig::OnGetDepartmentList(J_AsioDataBase *pAsioData)
{
	pAsioData->ioCall = J_AsioDataBase::j_read_e;
	CXlHelper::MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	if (m_ioDepartmentListState == xl_init_state)
	{
		JoDataBaseObj->GetDepartmentList(m_departmentInfoQueue);
		pAsioData->ioCall = J_AsioDataBase::j_read_write_departmentlist_e;
	}

	return J_OK;
}

j_result_t CXlConfig::OnGetDvrInfo(J_AsioDataBase *pAsioData)
{
	CliHostId *pResp = (CliHostId *)(m_readBuff + sizeof(CmdHeader));

	CliHostInfo info = {0};
	JoDataBaseObj->GetDvrInfo(*pResp, info);

	int nBodyLen = sizeof(CmdHeader) + sizeof(CliHostInfo) + sizeof(CmdTail);
	CXlHelper::MakeResponse(xlc_get_dvr_info, (j_char_t *)&info, sizeof(CliHostInfo), m_writeBuff);
	pAsioData->ioCall = J_AsioDataBase::j_write_e;
	CXlHelper::MakeNetData(pAsioData, m_writeBuff, nBodyLen);

	return J_OK;
}

j_result_t CXlConfig::OnGetUserInfo(J_AsioDataBase *pAsioData)
{
	CliUserId *pResp = (CliUserId *)(m_readBuff + sizeof(CmdHeader));

	CliUserInfo info = {0};
	JoDataBaseObj->GetUserInfo(*pResp, info);

	int nBodyLen = sizeof(CmdHeader) + sizeof(CliUserInfo) + sizeof(CmdTail);
	CXlHelper::MakeResponse(xlc_get_user_info, (j_char_t *)&info, sizeof(CliUserInfo), m_writeBuff);
	pAsioData->ioCall = J_AsioDataBase::j_write_e;
	CXlHelper::MakeNetData(pAsioData, m_writeBuff, nBodyLen);

	return J_OK;
}

j_result_t CXlConfig::OnGetDepartmentInfo(J_AsioDataBase *pAsioData)
{
	CliDepartmentId *pResp = (CliDepartmentId *)(m_readBuff + sizeof(CmdHeader));

	CliDepartmentInfo info = {0};
	JoDataBaseObj->GetDepartmentInfo(*pResp, info);

	int nBodyLen = sizeof(CmdHeader) + sizeof(CliDepartmentInfo) + sizeof(CmdTail);
	CXlHelper::MakeResponse(xlc_get_department_info, (j_char_t *)&info, sizeof(CliDepartmentInfo), m_writeBuff);
	pAsioData->ioCall = J_AsioDataBase::j_write_e;
	CXlHelper::MakeNetData(pAsioData, m_writeBuff, nBodyLen);

	return J_OK;
}

j_result_t CXlConfig::OnAddDvrInfo(J_AsioDataBase *pAsioData)
{
	CliHostInfo *pResp = (CliHostInfo *)(m_readBuff + sizeof(CmdHeader));
	JoDataBaseObj->AddDvrInfo(*pResp);

	CliRetValue2 data = {0};
	data.nRetVal = 0;
	int nBodyLen = sizeof(CmdHeader) + sizeof(CliRetValue2) + sizeof(CmdTail);
	CXlHelper::MakeResponse(xlc_add_dvr_info, (j_char_t *)&data, sizeof(CliRetValue2), m_writeBuff);
	pAsioData->ioCall = J_AsioDataBase::j_write_e;
	CXlHelper::MakeNetData(pAsioData, m_writeBuff, nBodyLen);

	return J_OK;
}

j_result_t CXlConfig::OnAddUserInfo(J_AsioDataBase *pAsioData)
{
	CliUserInfo *pResp = (CliUserInfo *)(m_readBuff + sizeof(CmdHeader));
	JoDataBaseObj->AddUserInfo(*pResp);

	CliRetValue2 data = {0};
	data.nRetVal = 0;
	int nBodyLen = sizeof(CmdHeader) + sizeof(CliRetValue2) + sizeof(CmdTail);
	CXlHelper::MakeResponse(xlc_add_user_info, (j_char_t *)&data, sizeof(CliRetValue2), m_writeBuff);
	pAsioData->ioCall = J_AsioDataBase::j_write_e;
	CXlHelper::MakeNetData(pAsioData, m_writeBuff, nBodyLen);

	return J_OK;
}

j_result_t CXlConfig::OnAddDepartmentInfo(J_AsioDataBase *pAsioData)
{
	CliDepartmentInfo *pResp = (CliDepartmentInfo *)(m_readBuff + sizeof(CmdHeader));
	JoDataBaseObj->AddDepartmentInfo(*pResp);

	CliRetValue2 data = {0};
	data.nRetVal = 0;
	int nBodyLen = sizeof(CmdHeader) + sizeof(CliRetValue2) + sizeof(CmdTail);
	CXlHelper::MakeResponse(xlc_add_department_info, (j_char_t *)&data, sizeof(CliRetValue2), m_writeBuff);
	pAsioData->ioCall = J_AsioDataBase::j_write_e;
	CXlHelper::MakeNetData(pAsioData, m_writeBuff, nBodyLen);

	return J_OK;
}

j_result_t CXlConfig::OnModDvrInfo(J_AsioDataBase *pAsioData)
{
	CliHostInfo *pResp = (CliHostInfo *)(m_readBuff + sizeof(CmdHeader));
	JoDataBaseObj->ModDvrInfo(*pResp);

	CliRetValue2 data = {0};
	data.nRetVal = 0;
	int nBodyLen = sizeof(CmdHeader) + sizeof(CliRetValue2) + sizeof(CmdTail);
	CXlHelper::MakeResponse(xlc_mod_dvr_info, (j_char_t *)&data, sizeof(CliRetValue2), m_writeBuff);
	pAsioData->ioCall = J_AsioDataBase::j_write_e;
	CXlHelper::MakeNetData(pAsioData, m_writeBuff, nBodyLen);

	return J_OK;
}
j_result_t CXlConfig::OnModUserInfo(J_AsioDataBase *pAsioData)
{
	CliUserInfo *pResp = (CliUserInfo *)(m_readBuff + sizeof(CmdHeader));
	JoDataBaseObj->ModUserInfo(*pResp);

	CliRetValue2 data = {0};
	data.nRetVal = 0;
	int nBodyLen = sizeof(CmdHeader) + sizeof(CliRetValue2) + sizeof(CmdTail);
	CXlHelper::MakeResponse(xlc_mod_user_info, (j_char_t *)&data, sizeof(CliRetValue2), m_writeBuff);
	pAsioData->ioCall = J_AsioDataBase::j_write_e;
	CXlHelper::MakeNetData(pAsioData, m_writeBuff, nBodyLen);

	return J_OK;
}
j_result_t CXlConfig::OnModDepartmentInfo(J_AsioDataBase *pAsioData)
{
	CliDepartmentInfo *pResp = (CliDepartmentInfo *)(m_readBuff + sizeof(CmdHeader));
	JoDataBaseObj->ModDepartmentInfo(*pResp);

	CliRetValue2 data = {0};
	data.nRetVal = 0;
	int nBodyLen = sizeof(CmdHeader) + sizeof(CliRetValue2) + sizeof(CmdTail);
	CXlHelper::MakeResponse(xlc_mod_department_info, (j_char_t *)&data, sizeof(CliRetValue2), m_writeBuff);
	pAsioData->ioCall = J_AsioDataBase::j_write_e;
	CXlHelper::MakeNetData(pAsioData, m_writeBuff, nBodyLen);

	return J_OK;
}
j_result_t CXlConfig::OnDelDvrInfo(J_AsioDataBase *pAsioData)
{
	CliHostId *pResp = (CliHostId *)(m_readBuff + sizeof(CmdHeader));
	JoDataBaseObj->DelDvrInfo(*pResp);

	CliRetValue2 data = {0};
	data.nRetVal = 0;
	int nBodyLen = sizeof(CmdHeader) + sizeof(CliRetValue2) + sizeof(CmdTail);
	CXlHelper::MakeResponse(xlc_del_dvr_info, (j_char_t *)&data, sizeof(CliRetValue2), m_writeBuff);
	pAsioData->ioCall = J_AsioDataBase::j_write_e;
	CXlHelper::MakeNetData(pAsioData, m_writeBuff, nBodyLen);

	return J_OK;
}
j_result_t CXlConfig::OnDelUserInfo(J_AsioDataBase *pAsioData)
{
	CliUserId *pResp = (CliUserId *)(m_readBuff + sizeof(CmdHeader));
	JoDataBaseObj->DelUserInfo(*pResp);

	CliRetValue2 data = {0};
	data.nRetVal = 0;
	int nBodyLen = sizeof(CmdHeader) + sizeof(CliRetValue2) + sizeof(CmdTail);
	CXlHelper::MakeResponse(xlc_del_user_info, (j_char_t *)&data, sizeof(CliRetValue2), m_writeBuff);
	pAsioData->ioCall = J_AsioDataBase::j_write_e;
	CXlHelper::MakeNetData(pAsioData, m_writeBuff, nBodyLen);

	return J_OK;
}
j_result_t CXlConfig::OnDelDepartmentInfo(J_AsioDataBase *pAsioData)
{
	CliDepartmentId *pResp = (CliDepartmentId *)(m_readBuff + sizeof(CmdHeader));
	JoDataBaseObj->DelDepartmentInfo(*pResp);

	CliRetValue2 data = {0};
	data.nRetVal = 0;
	int nBodyLen = sizeof(CmdHeader) + sizeof(CliRetValue2) + sizeof(CmdTail);
	CXlHelper::MakeResponse(xlc_del_department_info, (j_char_t *)&data, sizeof(CliRetValue2), m_writeBuff);
	pAsioData->ioCall = J_AsioDataBase::j_write_e;
	CXlHelper::MakeNetData(pAsioData, m_writeBuff, nBodyLen);

	return J_OK;
}

j_result_t CXlConfig::OnGetDevInfo(J_AsioDataBase *pAsioData)
{
	//typedef struct _hostInfo
	//{
	//	char hostId[32];
	//}HostInfo, *LPHostInfo;
	//HostInfo *pReps = (HostInfo *)(m_readBuff + sizeof(CmdHeader));

	//HostInfoResp hostResp = {0};
	//strcpy(hostResp.hostId, pReps->hostId);
	//JoDataBaseObj->GetDevInfo(hostResp);

	//int nBodyLen = sizeof(CmdHeader) + sizeof(HostInfoResp) + sizeof(CmdTail);
	//CXlHelper::MakeResponse(xl_get_devinfo, (j_char_t *)&hostResp, sizeof(HostInfoResp), m_writeBuff);
	//pAsioData->ioCall = J_AsioDataBase::j_write_e;
	//MakeNetData(pAsioData, m_writeBuff, nBodyLen);

	return J_OK;
}

j_result_t CXlConfig::OnGetLogInfo(J_AsioDataBase *pAsioData)
{
	//typedef struct _logInfo
	//{
	//	char hostId[32];
	//	time_t tmStart;
	//	time_t tmEnd;
	//}LogInfo, *LPLogInfo;

	//LogInfo *pReps = (LogInfo *)(m_readBuff + sizeof(CmdHeader));
	//J_OS::LOGINFO("hostId = %s, start = %I64d, end = %I64d", pReps->hostId, pReps->tmStart, pReps->tmEnd);
	//pAsioData->ioCall = J_AsioDataBase::j_read_e;
	//CXlHelper::MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
	//if (m_ioLogState == xl_init_state)
	//{
	//	JoDataBaseObj->GetLogInfoList(pReps->hostId, pReps->tmStart, pReps->tmEnd, m_logInfoQueue);
	//	pAsioData->ioCall = J_AsioDataBase::j_read_write_log_e;
	//}

	return J_OK;
}