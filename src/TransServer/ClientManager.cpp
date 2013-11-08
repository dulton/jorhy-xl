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
#include "XlClient.h"

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
			///����ͻ���״̬
		}
	}
	TUnlock(m_locker);
}