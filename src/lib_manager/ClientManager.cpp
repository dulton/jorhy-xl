///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx�Ƽ����޹�˾(��Ȩ����) 
/// All rights reserved. 
/// 
/// @file         ClientManager.cpp 
/// @brief       �ͻ��˹���ģ��ʵ��
///
/// @version   1.0 (�汾����)
/// @author    Jorhy (joorhy@gmail.com) 
/// @date       2013/09/18 17:02 
///
///
/// �޶�˵��������汾
///////////////////////////////////////////////////////////////////////////  
#include "ClientManager.h"

CClientManager::CClientManager()
{

}

CClientManager::~CClientManager()
{

}

j_uint32_t CClientManager::Login(j_socket_t nSock, const j_char_t *pUserName, const j_char_t *pPassWord)
{
	return 0;
}

void CClientManager::Logout(j_uint32_t nUserId)
{

}

j_result_t CClientManager::RequestData(const j_char_t *pData, const j_int32_t &nLen, j_char_t **pRetData, j_int32_t &nRetLen)
{
	return J_OK;
}