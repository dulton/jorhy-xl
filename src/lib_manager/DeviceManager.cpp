///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx�Ƽ����޹�˾(��Ȩ����) 
/// All rights reserved. 
/// 
/// @file      DeviceManager.cpp 
/// @brief     ǰ���豸����ģ��ʵ��
///
/// @version   1.0 (�汾����)
/// @author    Jorhy (joorhy@gmail.com) 
/// @date      2013/09/18 17:10 
///
///
/// �޶�˵��������汾
///////////////////////////////////////////////////////////////////////////  
#include "DeviceManager.h"

CDeviceManager::CDeviceManager()
{

}

CDeviceManager::~CDeviceManager()
{

}

j_uint32_t CDeviceManager::AddDevice(j_socket_t nSock)
{
	return 0;
}

void CDeviceManager::DelDevice(j_uint32_t nDevId)
{

}

j_result_t CDeviceManager::ParserData(j_uint32_t nDevId, const j_char_t *pData, const j_int32_t &nLen, j_char_t **pRetData, j_int32_t &nRetLen)
{
	return J_OK;
}

J_Host *CDeviceManager::GetDeviceObj(j_uint32_t nDevId)
{
	return NULL;
}

J_Host *CDeviceManager::GetDeviceObj(j_string_t strDevId)
{
	return NULL;
}