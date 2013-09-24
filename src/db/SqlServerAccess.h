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

#import "c:\\Program Files\\Common Files\\System\\ADO\\msado15.dll" no_namespace rename("EOF", "EndOfFile")
/// ����Ĺ���:  SqlServer���ݿ������
/// ���ݿ����ɾ�Ĳ� 

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
	_ConnectionPtr m_pConn;		/// ���Ӷ���  
	_RecordsetPtr m_pRec;				/// ��¼������  
	_CommandPtr m_pCmd;			/// �������
};
#endif // ~__SQLSERVERACCESS_H_
