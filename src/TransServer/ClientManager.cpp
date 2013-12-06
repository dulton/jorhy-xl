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
#include "MsSqlServer.h"

JO_IMPLEMENT_SINGLETON(ClientManager)
#pragma comment(lib, "Debug\\core.lib")

CClientManager::CClientManager()
{
	m_timer.Create(1000, CClientManager::OnTimer, this);
}

CClientManager::~CClientManager()
{
	m_timer.Destroy();
}

j_result_t CClientManager::Login(const char *pUserName, const char *pPasswd, int nForce, int &nRet, J_Client *pClient)
{
	if (JoDataBaseObj->Login(pUserName, pPasswd, nForce, nRet) == J_OK)
	{
		TLock(m_lockerUser);
		UserMap::iterator it = m_userMap.find(pUserName);
		if (it != m_userMap.end() && nForce == 1)
		{
			it->second->SendMessage("", 0x01, xlc_force_login);
		}
		m_userMap[pUserName] = pClient;

		TUnlock(m_lockerUser);
	}

	return J_OK;
}

j_result_t CClientManager::Logout(const char *pUserName, J_Client *pClient)
{
	TLock(m_lockerUser);
	UserMap::iterator it = m_userMap.find(pUserName);
	if (it != m_userMap.end())
	{
		if (pClient == it->second)
		{
			JoDataBaseObj->Logout(pUserName);
			m_userMap.erase(pUserName);
		}
	}
	TUnlock(m_lockerUser);
	
	return J_OK;
}

j_result_t CClientManager::Notify(j_string_t strHostId, j_int32_t nType, j_int32_t nNo)
{
	TLock(m_locker);
	ClientMap::iterator it = m_clientMap.begin();
	for (; it!=m_clientMap.end(); ++it)
	{
		it->second->SendMessage(strHostId, nType, nNo);
	}
	TUnlock(m_locker);
	return J_OK;
}

J_Client *CClientManager::CreateClientObj(j_socket_t nSock)
{
	J_OS::LOGINFO("CClientManager::CreateClientObj Socket = %d", nSock.sock);
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
		pClient->Broken();
		delete pClient;
		m_clientMap.erase(it);
	}
	TUnlock(m_locker);
}

void CClientManager::CheckClient()
{
	TLock(m_locker);
	ClientMap::iterator it = m_clientMap.begin();
	for (; it!=m_clientMap.end(); ++it)
	{
		J_Client *pClient = it->second;
		if (pClient->GetState() != 0)
		{
			///处理客户端状态
		}
	}
	TUnlock(m_locker);
}