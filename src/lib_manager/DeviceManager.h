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

#ifdef   LIBMANAGER_EXPORTS 
#define   JO_DEVICEMAN_API           __declspec(dllexport) 
#else 
#define   JO_DEVICEMAN_API           __declspec(dllimport) 
#endif 

/// ����Ĺ���:  ǰ���豸���� 
/// ������ɶ�ǰ���豸�Ĺ�������ע�ᡢ�豸��֤����������ȹ���
class JO_DEVICEMAN_API CDeviceManager
{
public:
	CDeviceManager();
	~CDeviceManager();

public:
	/// ����豸
	/// @param[in]		nSock  ����Socket
	/// @return		    �豸ID,=0-���ʧ��  
	j_uint32_t AddDevice(j_socket_t nSock);
	/// ɾ���豸
	/// @param[in]		nDevId �豸ID 
	/// @return
	void DelDevice(j_uint32_t nDevId);
	/// ��������
	/// @param[in]		pData��������
	/// @param[in]		nLen�������ݳ���
	/// @param[out]	pRetData�ظ�����,��Ҫ�ͷ�	
	/// @param[out]	nRetLen�ظ����ݳ���
	/// @return			�μ�j_errtype.h 
	j_result_t ParserData(j_uint32_t nDevId, const j_char_t *pData, const j_int32_t &nLen, j_char_t **pRetData, j_int32_t &nRetLen);
	/// ��ȡ�豸����
	/// @param[in]		nDevId �豸ID 
	/// @return			Host����,NULL-δ�ҵ�   
	J_Host *GetDeviceObj(j_uint32_t nDevId);
	/// ��ȡ�豸����
	/// @param[in]		strDevId �豸ID 
	/// @return			Host����,NULL-δ�ҵ�   
	J_Host *GetDeviceObj(j_string_t strDevId);
};
#endif //~__DEVICE_MANAGER_H_