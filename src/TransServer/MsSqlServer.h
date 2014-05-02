///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx科技有限公司(版权声明) 
/// All rights reserved. 
/// 
/// @file      SqlServerAccess.h 
/// @brief     SqlServer数据库模块
///
/// @version   1.0 (版本声明)
/// @author    Jorhy (joorhy@gmail.com) 
/// @date      2013/09/22 11:08 
///
///
/// 修订说明：最初版本
///////////////////////////////////////////////////////////////////////////  
#ifndef __SQLSERVERACCESS_H_
#define __SQLSERVERACCESS_H_
#include "j_includes.h"
#include "XlDType.h"
#include "XlCType.h"

//#import "c:\\Program Files\\Common Files\\System\\ADO\\msado15.dll" no_namespace rename("EOF", "EndOfFile")
#import "msado15.dll" no_namespace rename("EOF", "EndOfFile")
/// 本类的功能:  SqlServer数据库操作类
/// 数据库的增删改查 
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
	/// 用户登录
	j_result_t Login(const char *pUserName, const char *pPasswd, int nForce, int &nRet);
	/// 获取用户名
	j_result_t GetUserNameById(j_int32_t userId, j_string_t &userName);
	/// 用户退出
	j_result_t Logout(const char *pUserName);
	/// 修改文件信息
	j_result_t UpdateFileInfo(j_int32_t nFileId, j_int32_t nState, bool bFlag = true);
	/// 修改设备信息
	j_result_t UpdateDevInfo(const DevHostInfo &devInfo, bool bOnline);
	/// 获取设备信息
	j_result_t GetDevInfo(DevHostInfo &devInfo);
	/// 增加报警信息
	j_result_t InsertAlarmInfo(const char *pHostId, const DevAlarmInfo& alarmInfo);
	/// 获得设备日志的最近时间
	time_t GetDevLogLastTime(const char *pHostId);
	/// 增加日志信息
	j_result_t InsertLogInfo(const char *pHostId, int nState, time_t tmTimeStamp);
	/// 获取日志列表
	j_result_t GetLogInfoList(const char *pHostId, time_t tmStart, time_t tmEnd, DevLogInfoQueue &logInfoQueue);
	/// 获取报警列表
	j_result_t GetAlarmInfoList(const char *pHostId, time_t tmStart, time_t tmEnd, CliAlarmInfoQueue &alarmInfoQueue);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////      以下接口暂未使用     /////////////////////////////////////////////////////////////////////////////////////////////
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
	_ConnectionPtr m_pConn;		/// 连接对象  
	_RecordsetPtr m_pRec;				/// 记录集对象  
	_CommandPtr m_pCmd;			/// 命令对象
};

JO_DECLARE_SINGLETON(SqlServerAccess)

#endif // ~__SQLSERVERACCESS_H_
