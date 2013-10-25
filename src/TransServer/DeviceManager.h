///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx科技有限公司(版权声明) 
/// All rights reserved. 
/// 
/// @file         DeviceManager.h 
/// @brief       前端设备管理模块
///
/// @version   1.0 (版本声明)
/// @author    Jorhy (joorhy@gmail.com) 
/// @date       2013/09/18 17:07 
///
///
/// 修订说明：最初版本
///////////////////////////////////////////////////////////////////////////  
#ifndef __DEVICE_MANAGER_H_
#define __DEVICE_MANAGER_H_
#include "j_includes.h"
#include "x_timer.h"
#include "x_lock.h"

/// 设备信心
struct HostInfo
{
	J_Host *pHost;				///< 设备对象
	j_boolean_t bRegister;	///< 是否注册成功
};

/// 本类的功能:  前端设备管理 
/// 本类完成对前端设备的管理，包括注册、设备验证、心跳保活等功能
class CDeviceManager
{
	typedef std::map<j_socket_t, HostInfo> ConnectMap;
	typedef std::map<j_string_t, J_Host *> DeviceMap;
public:
	CDeviceManager();
	~CDeviceManager();

public:
	/// 创建设备对象
	/// @param[in]		nHostType 设备类型,1-XlHost
	/// @param[in]		nSock 设备连接 
	/// @param[out]   NULL-失败,否则为Host对象
	/// @return			参见j_errtype.h 
	J_Host *CreateDevObj(j_int32_t nHostType, j_socket_t nSock);
	/// 获取设备对象
	/// @param[in]		strDevId 设备ID 
	/// @return			Host对象,NULL-未找到   
	J_Host *GetDeviceObj(j_socket_t nSock);
	/// 删除设备对象
	/// @param[in]		nSock 设备连接 
	/// @return		
	void ReleaseDevObj(j_socket_t nSock);
	/// 获取设备对象
	/// @param[in]		strDevId 设备ID 
	/// @return			Host对象,NULL-未找到   
	J_Host *GetDeviceObj(j_string_t strDevId);
	/// 设备校时
	/// @param[in]		sysTime 系统时间
	/// @return			参见j_errtype.h 
	j_result_t SetDeviceTime(j_time_t sysTime);

private:
	static void OnTimer(void *pUser)
	{
		(static_cast<CDeviceManager *>(pUser))->CheckDevice();
	}
	void CheckDevice();
	/// 添加设备
	/// @param[in]		pHost 设备对象 
	/// @return		    Host对象,=NULL-添加失败  
	j_result_t AddDevice(J_Host *pHost);
	/// 删除设备
	/// @param[in]		pHost 设备对象 
	/// @return
	void DelDevice(J_Host *pHost);

private:
	DeviceMap m_devMap;
	J_OS::CTimer m_timer;
	J_OS::CTLock m_locker;
	J_OS::CTLock m_devLocker;
	ConnectMap m_connMap;
};

JO_DECLARE_SINGLETON(DeviceManager)

#endif //~__DEVICE_MANAGER_H_