///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx科技有限公司(版权声明) 
/// All rights reserved. 
/// 
/// @file         ClientManager.cpp 
/// @brief       客户端管理模块实现
///
/// @version   1.0 (版本声明)
/// @author    Jorhy (joorhy@gmail.com) 
/// @date       2013/09/18 17:02 
///
///
/// 修订说明：最初版本
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