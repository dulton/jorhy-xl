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
#include "XlClient.h"

JO_IMPLEMENT_SINGLETON(ClientManager)
#pragma comment(lib, "Debug\\core.lib")

CClientManager::CClientManager()
{

}

CClientManager::~CClientManager()
{

}

J_Client *CClientManager::CreateClientObj(j_socket_t nSock)
{
	J_Client *pClient = NULL;
	try 
	{
		pClient = new CXlClient(nSock);
	}
	catch(...)
	{
		J_OS::LOGINFO("CClientManager::CreateClientObj Error");
	}
	if (pClient != NULL)
	{
		TLock(m_locker);
		m_clientMap[nSock] = pClient;
		TUnlock(m_locker);
	}
	
	return pClient;
}

J_Client *CClientManager::GetClientObj(j_socket_t nSock)
{
	J_Client *pClient = NULL;
	TLock(m_locker);
	ClientMap::iterator it = m_clientMap.find(nSock);
	if (it != m_clientMap.end())
	{
		pClient = it->second;
	}
	TUnlock(m_locker);

	return pClient;
}

void CClientManager::ReleaseClientObj(j_socket_t nSock)
{
	J_Client *pClient = NULL;
	TLock(m_locker);
	ClientMap::iterator it = m_clientMap.find(nSock);
	if (it != m_clientMap.end())
	{
		pClient = it->second;
		delete pClient;
		m_clientMap.erase(it);
	}
	TUnlock(m_locker);
}

j_uint32_t CClientManager::Login(j_socket_t nSock, const j_char_t *pUserName, const j_char_t *pPassWord)
{
	return 0;
}

void CClientManager::Logout(j_uint32_t nUserId)
{

}