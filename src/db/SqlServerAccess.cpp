///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx�Ƽ����޹�˾(��Ȩ����) 
/// All rights reserved. 
/// 
/// @file      SqlServerAccess.cpp 
/// @brief     SqlServer���ݿ�ģ��ʵ��
///
/// @version   1.0 (�汾����)
/// @author    Jorhy (joorhy@gmail.com) 
/// @date      2013/09/22 11:11 
///
///
/// �޶�˵��������汾
///////////////////////////////////////////////////////////////////////////  
#include "SqlServerAccess.h"

//#import "c:\\Program Files\\Common Files\\System\\ADO\\msado15.dll" no_namespace rename("EOF", "EndOfFile")

CSqlServerAccess::CSqlServerAccess()
{
	::CoInitialize(NULL); // ��ʼ��OLE/COM�⻷�� ��Ϊ����ADO�ӿ���׼��
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
	m_pConn->ConnectionTimeout = 5;				//����5�볬ʱ
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