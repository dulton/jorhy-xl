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