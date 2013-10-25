///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx�Ƽ����޹�˾(��Ȩ����) 
/// All rights reserved. 
/// 
/// @file         DeviceManager.h 
/// @brief       ǰ���豸����ģ��
///
/// @version   1.0 (�汾����)
/// @author    Jorhy (joorhy@gmail.com) 
/// @date       2013/09/18 17:07 
///
///
/// �޶�˵��������汾
///////////////////////////////////////////////////////////////////////////  
#ifndef __DEVICE_MANAGER_H_
#define __DEVICE_MANAGER_H_
#include "j_includes.h"
#include "x_timer.h"
#include "x_lock.h"

/// �豸����
struct HostInfo
{
	J_Host *pHost;				///< �豸����
	j_boolean_t bRegister;	///< �Ƿ�ע��ɹ�
};

/// ����Ĺ���:  ǰ���豸���� 
/// ������ɶ�ǰ���豸�Ĺ�������ע�ᡢ�豸��֤����������ȹ���
class CDeviceManager
{
	typedef std::map<j_socket_t, HostInfo> ConnectMap;
	typedef std::map<j_string_t, J_Host *> DeviceMap;
public:
	CDeviceManager();
	~CDeviceManager();

public:
	/// �����豸����
	/// @param[in]		nHostType �豸����,1-XlHost
	/// @param[in]		nSock �豸���� 
	/// @param[out]   NULL-ʧ��,����ΪHost����
	/// @return			�μ�j_errtype.h 
	J_Host *CreateDevObj(j_int32_t nHostType, j_socket_t nSock);
	/// ��ȡ�豸����
	/// @param[in]		strDevId �豸ID 
	/// @return			Host����,NULL-δ�ҵ�   
	J_Host *GetDeviceObj(j_socket_t nSock);
	/// ɾ���豸����
	/// @param[in]		nSock �豸���� 
	/// @return		
	void ReleaseDevObj(j_socket_t nSock);
	/// ��ȡ�豸����
	/// @param[in]		strDevId �豸ID 
	/// @return			Host����,NULL-δ�ҵ�   
	J_Host *GetDeviceObj(j_string_t strDevId);
	/// �豸Уʱ
	/// @param[in]		sysTime ϵͳʱ��
	/// @return			�μ�j_errtype.h 
	j_result_t SetDeviceTime(j_time_t sysTime);

private:
	static void OnTimer(void *pUser)
	{
		(static_cast<CDeviceManager *>(pUser))->CheckDevice();
	}
	void CheckDevice();
	/// ����豸
	/// @param[in]		pHost �豸���� 
	/// @return		    Host����,=NULL-���ʧ��  
	j_result_t AddDevice(J_Host *pHost);
	/// ɾ���豸
	/// @param[in]		pHost �豸���� 
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