///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx科技有限公司(版权声明) 
/// All rights reserved. 
/// 
/// @file      DeviceManager.cpp 
/// @brief     前端设备管理模块实现
///
/// @version   1.0 (版本声明)
/// @author    Jorhy (joorhy@gmail.com) 
/// @date      2013/09/18 17:10 
///
///
/// 修订说明：最初版本
///////////////////////////////////////////////////////////////////////////  
#include "DeviceManager.h"
#include "XlHost.h"

#pragma comment(lib,"Debug\\XlHost.lib")

JO_IMPLEMENT_SINGLETON(DeviceManager)

CDeviceManager::CDeviceManager()
{
	m_timer.Create(1000, CDeviceManager::OnTimer, this);
}

CDeviceManager::~CDeviceManager()
{
	m_timer.Destroy();
}

J_Host *CDeviceManager::CreateDevObj(j_int32_t nHostType, j_socket_t nSock)
{
	J_Host *pHost = NULL;
	try 
	{
		switch (nHostType)
		{
		case 1://XlHost
			pHost = new CXlHost(nSock);
			break;
		default:
			break;
		}
	}
	catch(...)
	{
		J_OS::LOGINFO("CDeviceManager::CreateDevObj Error");
	}

	if (pHost != NULL)
	{
		HostInfo info = {0};
		info.pHost = pHost;
		info.bRegister = false;
		TLock(m_locker);
		m_connMap[nSock] = info;
		TUnlock(m_locker);
	}

	return pHost;
}

J_Host *CDeviceManager::GetDeviceObj(j_socket_t nSock)
{
	J_Host *pHost = NULL;
	ConnectMap::iterator it = m_connMap.find(nSock);
	if (it != m_connMap.end())
	{
		pHost = it->second.pHost;
	}

	return pHost;
}

void CDeviceManager::ReleaseDevObj(j_socket_t nSock)
{
	TLock(m_locker);
	ConnectMap::iterator it = m_connMap.find(nSock);
	if (it != m_connMap.end())
	{
		DelDevice(it->second.pHost);
		m_connMap.erase(it);
	}
	TUnlock(m_locker);
}

j_result_t CDeviceManager::AddDevice(J_Host *pHost)
{
	if (pHost != NULL)
	{
		j_string_t strDevId;
		if (pHost->GetHostId(strDevId) == J_OK)
		{
			//设备添加成功
			m_devMap[strDevId] = pHost;
		}
	}
	
	return J_OK;
}

void CDeviceManager::DelDevice(J_Host *pHost)
{
	DeviceMap::iterator it = m_devMap.begin();
	for (; it!=m_devMap.end(); ++it)
	{
		if (it->second == pHost)
		{
			delete pHost;
			m_devMap.erase(it);
			break;
		}
	}
}

void CDeviceManager::CheckDevice()
{
	TLock(m_locker);
	ConnectMap::iterator it = m_connMap.begin();
	for (; it!=m_connMap.end(); ++it)
	{
		J_Host *pHost = it->second.pHost;
		if (pHost->IsReady())
		{
			if (!it->second.bRegister)
			{
				AddDevice(pHost);
				it->second.bRegister = true;
			}
		}
		else
		{
			if (it->second.bRegister)
			{
				DelDevice(pHost);
				j_close_socket(it->first.sock);
				m_connMap.erase(it);
				TUnlock(m_locker);
				break;
			}
		}
	}
	TUnlock(m_locker);
}

J_Host *CDeviceManager::GetDeviceObj(j_string_t strDevId)
{
	DeviceMap::iterator it = m_devMap.find(strDevId);
	if (it != m_devMap.end())
		return it->second;

	return NULL;
}