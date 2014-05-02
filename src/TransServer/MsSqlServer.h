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
#include "XlDType.h"
#include "XlCType.h"

//#import "c:\\Program Files\\Common Files\\System\\ADO\\msado15.dll" no_namespace rename("EOF", "EndOfFile")
#import "msado15.dll" no_namespace rename("EOF", "EndOfFile")
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
	virtual j_result_t Init();
	virtual j_result_t Release();

public:
	/// �û���¼
	j_result_t Login(const char *pUserName, const char *pPasswd, int nForce, int &nRet);
	/// ��ȡ�û���
	j_result_t GetUserNameById(j_int32_t userId, j_string_t &userName);
	/// �û��˳�
	j_result_t Logout(const char *pUserName);
	/// �޸��ļ���Ϣ
	j_result_t UpdateFileInfo(j_int32_t nFileId, j_int32_t nState, bool bFlag = true);
	/// �޸��豸��Ϣ
	j_result_t UpdateDevInfo(const DevHostInfo &devInfo, bool bOnline);
	/// ��ȡ�豸��Ϣ
	j_result_t GetDevInfo(DevHostInfo &devInfo);
	/// ���ӱ�����Ϣ
	j_result_t InsertAlarmInfo(const char *pHostId, const DevAlarmInfo& alarmInfo);
	/// ����豸��־�����ʱ��
	time_t GetDevLogLastTime(const char *pHostId);
	/// ������־��Ϣ
	j_result_t InsertLogInfo(const char *pHostId, int nState, time_t tmTimeStamp);
	/// ��ȡ��־�б�
	j_result_t GetLogInfoList(const char *pHostId, time_t tmStart, time_t tmEnd, DevLogInfoQueue &logInfoQueue);
	/// ��ȡ�����б�
	j_result_t GetAlarmInfoList(const char *pHostId, time_t tmStart, time_t tmEnd, CliAlarmInfoQueue &alarmInfoQueue);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////      ���½ӿ���δʹ��     /////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	j_result_t GetDvrTotleInfo(CliHostTotleInfo &dvrTotleInfo);
	j_result_t GetUserTotleInfo(CliUserTotleInfo &userTotleInfo);
	j_result_t GetDvrList(int nType, long lDepartmentId, CliHostInfoQueue &dvrInfoQueue);
	j_result_t GetUserList(int nType, CliUserInfoQueue &userInfoQueue);
	j_result_t GetDepartmentList(CliDepartmentInfoQueue &dvrInfoQueue);
	j_result_t GetDvrInfo(const CliHostId &dvrId, CliHostInfo &info);
	j_result_t GetUserInfo(const CliUserId &userId, CliUserInfo &info);
	j_result_t GetDepartmentInfo(const CliDepartmentId &departmengId, CliDepartmentInfo &info);
	j_result_t AddDvrInfo(const CliHostInfo &info);
	j_result_t AddUserInfo(const CliUserInfo &info);
	j_result_t AddDepartmentInfo(const CliDepartmentInfo &info);
	j_result_t ModDvrInfo(const CliHostInfo &info);
	j_result_t ModUserInfo(const CliUserInfo &info);
	j_result_t ModDepartmentInfo(const CliDepartmentInfo &info);
	j_result_t DelDvrInfo(const CliHostId &dvrId);
	j_result_t DelUserInfo(const CliUserId &userId);
	j_result_t DelDepartmentInfo(const CliDepartmentId &departmengId);

private:  
	_ConnectionPtr m_pConn;		/// ���Ӷ���  
	_RecordsetPtr m_pRec;				/// ��¼������  
	_CommandPtr m_pCmd;			/// �������
};

JO_DECLARE_SINGLETON(SqlServerAccess)

#endif // ~__SQLSERVERACCESS_H_
