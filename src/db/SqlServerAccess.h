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

#import "c:\\Program Files\\Common Files\\System\\ADO\\msado15.dll" no_namespace rename("EOF", "EndOfFile")
/// 本类的功能:  SqlServer数据库操作类
/// 数据库的增删改查 

#ifdef   SQLSERVERACCESS_EXPORTS 
#define   JO_SQLSERVER_API           __declspec(dllexport) 
#else 
#define   JO_SQLSERVER_API           __declspec(dllimport) 
#endif 

class JO_SQLSERVER_API CSqlServerAccess : public J_DbAccess
{
public:
	CSqlServerAccess();
	~CSqlServerAccess();

public:
	///J_DbAccess
	virtual j_result_t Connect(const j_char_t *pAddr, j_int16_t nPort, const j_char_t *pUa, const j_char_t *pPwd);
	virtual j_result_t Release();

private:  
	_ConnectionPtr m_pConn;		/// 连接对象  
	_RecordsetPtr m_pRec;				/// 记录集对象  
	_CommandPtr m_pCmd;			/// 命令对象
};
#endif // ~__SQLSERVERACCESS_H_
