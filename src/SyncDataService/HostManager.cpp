#include "HostManager.h"

JO_IMPLEMENT_SINGLETON(HostManager)

CHostManager::CHostManager()
{

}

CHostManager::~CHostManager()
{

}

j_result_t CHostManager::GetReadyHostInfo(DownLoadFtpInfo &info)
{
	j_result_t nResult = J_UNKNOW;
	TLock(m_locker);
	HostMap::iterator it = m_hostMap.begin();
	if (it != m_hostMap.end())
	{
		info = it->second;
		nResult = J_OK;
	}
	TUnlock(m_locker);

	return nResult;
}

void CHostManager::UpdateHostInfo(const BroadCastFtpInfo &info)
{
	TLock(m_locker);
	j_string_t strHostId = info.host_id;
	HostMap::iterator it = m_hostMap.find(strHostId);
	if (it == m_hostMap.end())
	{
		DownLoadFtpInfo mInfo = {0};
		FillInfo(info, mInfo);
		m_hostMap[strHostId] = mInfo;
	}
	else
	{
		UpdateInfo(it->second);
	}
	TUnlock(m_locker);
}

void CHostManager::DelHost(const j_string_t &strHostId)
{
	TLock(m_locker);
	HostMap::iterator it = m_hostMap.find(strHostId);
	if (it != m_hostMap.end())
	{
		m_hostMap.erase(strHostId);
	}
	TUnlock(m_locker);
}

void CHostManager::CheckHost()
{
	TLock(m_locker);
	HostMap::iterator it = m_hostMap.begin();
	time_t curTime = time(0);
	for (; it != m_hostMap.end(); ++it)
	{
		if (curTime - it->second.updateTime > 5)
		{
			m_hostMap.erase(it);
			break;
		}
	}
	TUnlock(m_locker);
}

void CHostManager::FillInfo(const BroadCastFtpInfo &srcInfo, DownLoadFtpInfo &dstInfo)
{
	strcpy(dstInfo.strHostId, srcInfo.host_id);
	dstInfo.nFtpPort = srcInfo.ftp_port;
	strcpy(dstInfo.strFtpAddr, srcInfo.ftp_addr);
	strcpy(dstInfo.strUsesrName, srcInfo.user_name);
	strcpy(dstInfo.strPassWord, srcInfo.pass_word);
	dstInfo.updateTime = time(0);
}

void CHostManager::UpdateInfo(DownLoadFtpInfo &dstInfo)
{
	dstInfo.updateTime = time(0);
}