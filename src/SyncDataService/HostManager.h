#ifndef __HOST_MANAGER_H_
#define __HOST_MANAGER_H_

#include "j_includes.h"
#include "x_timer.h"
#include "x_lock.h"
#include "XLSyncType.h"

class CHostManager
{
public:
	CHostManager();
	~CHostManager();

public:
	j_result_t GetReadyHostInfo(DownLoadFtpInfo &info);
	void UpdateHostInfo(const BroadCastFtpInfo &info);
	void DelHost(const j_string_t &strHostId);

private:
	static void OnTimer(void *pUser)
	{
		(static_cast<CHostManager *>(pUser))->CheckHost();
	}
	void CheckHost();
	void FillInfo(const BroadCastFtpInfo &srcInfo, DownLoadFtpInfo &dstInfo);
	void UpdateInfo(DownLoadFtpInfo &dstInfo);

private:
	HostMap m_hostMap;
	J_OS::CTimer m_timer;
	J_OS::CTLock m_locker;
};

JO_DECLARE_SINGLETON(HostManager)

#endif //~__HOST_MANAGER_H_