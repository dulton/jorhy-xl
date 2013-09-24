///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx科技有限公司(版权声明) 
/// All rights reserved. 
/// 
/// @file      SqlServerAccess.cpp 
/// @brief     SqlServer数据库模块实现
///
/// @version   1.0 (版本声明)
/// @author    Jorhy (joorhy@gmail.com) 
/// @date      2013/09/22 11:11 
///
///
/// 修订说明：最初版本
///////////////////////////////////////////////////////////////////////////  
#include "SqlServerAccess.h"

//#import "c:\\Program Files\\Common Files\\System\\ADO\\msado15.dll" no_namespace rename("EOF", "EndOfFile")

CSqlServerAccess::CSqlServerAccess()
{
	::CoInitialize(NULL); // 初始化OLE/COM库环境 ，为访问ADO接口做准备
}

CSqlServerAccess::~CSqlServerAccess()
{
	::CoUninitialize();
}

j_result_t CSqlServerAccess::Connect(const j_char_t *pAddr, j_int16_t nPort, const j_char_t *pUa, const j_char_t *pPwd)
{
	m_pConn.CreateInstance("ADODB.Connection");
	m_pRec.CreateInstance("ADODB.Recordset");
	m_pCmd.CreateInstance("ADODB.Command");

	j_char_t connString[256] = {0};
	sprintf(connString, "Provider=SQLOLEDB;Server=%s,%d;Database=xl; uid=%s; pwd=%s;", pAddr, nPort, pUa, pPwd);
	m_pConn->ConnectionString = connString;
	m_pConn->ConnectionTimeout = 5;				//连接5秒超时
	if (m_pConn->Open("", "", "", adConnectUnspecified) != S_OK)
		return J_DB_ERROR;

	return J_OK;
}

j_result_t CSqlServerAccess::Release()
{
	m_pRec->Close();
	m_pConn->Close();
	m_pCmd.Release();
	m_pRec.Release();
	m_pConn.Release();

	return J_OK;
}