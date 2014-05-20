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
		NetHostInfo info = {0};
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
	TLock(m_locker);
	ConnectMap::iterator it = m_connMap.find(nSock);
	if (it != m_connMap.end())
	{
		pHost = it->second.pHost;
	}
	TUnlock(m_locker);

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

j_result_t CDeviceManager::AddDevice(J_Host *pHost, j_socket_t sock)
{
	if (pHost != NULL)
	{
		j_string_t strDevId;
		if (pHost->GetHostId(strDevId) == J_OK)
		{
			DeviceMap::iterator hostIt = m_devMap.find(strDevId);
			if (hostIt != m_devMap.end())
			{
				j_close_socket(hostIt->second.sock.sock);
				ReleaseDevObj(hostIt->second.sock);
			}
			TLock(m_devLocker);
			//设备添加成功
			LocalHostInfo info = {0};
			info.pHost = pHost;
			info.sock = sock;
			m_devMap[strDevId] = info;
			TUnlock(m_devLocker);
		}
	}
	
	return J_OK;
}

void CDeviceManager::DelDevice(J_Host *pHost)
{
	TLock(m_devLocker);
	DeviceMap::iterator it = m_devMap.begin();
	for (; it!=m_devMap.end(); ++it)
	{
		if (it->second.pHost == pHost)
		{
			pHost->OnBroken();
			delete pHost;
			m_devMap.erase(it);
			break;
		}
	}
	TUnlock(m_devLocker);
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
				pHost->GetDeviceInfo();
				pHost->SetTime(time(0));
				AddDevice(pHost, it->first);
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
	TLock(m_devLocker);
	DeviceMap::iterator it = m_devMap.find(strDevId);
	if (it != m_devMap.end())
	{
		TUnlock(m_devLocker);
		return it->second.pHost;
	}
	TUnlock(m_devLocker);

	return NULL;
}

j_result_t CDeviceManager::SetDeviceTime(j_time_t sysTime)
{
	TLock(m_devLocker);
	DeviceMap::iterator it = m_devMap.begin();
	for (; it!=m_devMap.end(); ++it)
	{
		J_Host *pHost = it->second.pHost;
		if (pHost->IsReady())
		{
			pHost->SetTime(sysTime);
		}
	}
	TUnlock(m_devLocker);

	return J_OK;
}