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

};
#endif //~__DEVICE_MANAGER_H_