///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx�Ƽ����޹�˾(��Ȩ����) 
/// All rights reserved. 
/// 
/// @file      SqlServerAccess.h 
/// @brief     SqlServer���ݿ�ģ��
///
/// @version   1.0 (�汾����)
/// @author    Jorhy (joorhy@gmail.com) 
/// @date      2013/09/22 11:08 
///
///
/// �޶�˵��������汾
///////////////////////////////////////////////////////////////////////////  
#ifndef __SQLSERVERACCESS_H_
#define __SQLSERVERACCESS_H_
#include "j_includes.h"
#include "XlType.h"

#import "c:\\Program Files\\Common Files\\System\\ADO\\msado15.dll" no_namespace rename("EOF", "EndOfFile")
/// ����Ĺ���:  SqlServer���ݿ������
/// ���ݿ����ɾ�Ĳ� 
class CSqlServerAccess : public J_DbAccess
{
public:
	CSqlServerAccess();
	~CSqlServerAccess();

public:
	///J_DbAccess
	virtual j_result_t Connect(const j_char_t *pAddr, j_int16_t nPort, const j_char_t *pUa, const j_char_t *pPwd);
	virtual j_result_t Release();

public:
	j_result_t CheckUser(const char *pUserName, const char *pPasswd);
	j_result_t UpdateDevInfo(const DevInfo &devInfo);
	j_result_t GetDevInfo(DevInfo &devInfo);
	j_result_t InsertAlarmInfo(const char *pHostId, const AlarmInfo& alarmInfo);
	time_t GetDevLogLastTime(const char *pHostId);
	j_result_t InsertLogInfo(const char *pHostId, int nState, time_t tmTimeStamp);
	j_result_t GetLogInfoList(const char *pHostId, time_t tmStart, time_t tmEnd, LogInfoQueue &logInfoQueue);
	j_result_t GetAlarmInfoList(const char *pHostId, time_t tmStart, time_t tmEnd, AlarmInfoQueue &alarmInfoQueue);
	j_result_t GetDvrTotleInfo(DVRTotleInfo &dvrTotleInfo);
	j_result_t GetUserTotleInfo(UserTotleInfo &userTotleInfo);
	j_result_t GetDvrList(int nType, long lDepartmentId, DvrInfoQueue &dvrInfoQueue);
	j_result_t GetUserList(int nType, UserInfoQueue &userInfoQueue);
	j_result_t GetDepartmentList(DepartmentInfoQueue &dvrInfoQueue);

private:  
	_ConnectionPtr m_pConn;		/// ���Ӷ���  
	_RecordsetPtr m_pRec;				/// ��¼������  
	_CommandPtr m_pCmd;			/// �������
};

JO_DECLARE_SINGLETON(SqlServerAccess)

#endif // ~__SQLSERVERACCESS_H_
