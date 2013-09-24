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

#ifdef   LIBMANAGER_EXPORTS 
#define   JO_DEVICEMAN_API           __declspec(dllexport) 
#else 
#define   JO_DEVICEMAN_API           __declspec(dllimport) 
#endif 

/// 本类的功能:  前端设备管理 
/// 本类完成对前端设备的管理，包括注册、设备验证、心跳保活等功能
class JO_DEVICEMAN_API CDeviceManager
{
public:
	CDeviceManager();
	~CDeviceManager();

public:
	/// 添加设备
	/// @param[in]		nSock  连接Socket
	/// @return		    设备ID,=0-添加失败  
	j_uint32_t AddDevice(j_socket_t nSock);
	/// 删除设备
	/// @param[in]		nDevId 设备ID 
	/// @return
	void DelDevice(j_uint32_t nDevId);
	/// 解析数据
	/// @param[in]		pData请求数据
	/// @param[in]		nLen请求数据长度
	/// @param[out]	pRetData回复数据,需要释放	
	/// @param[out]	nRetLen回复数据长度
	/// @return			参见j_errtype.h 
	j_result_t ParserData(j_uint32_t nDevId, const j_char_t *pData, const j_int32_t &nLen, j_char_t **pRetData, j_int32_t &nRetLen);
	/// 获取设备对象
	/// @param[in]		nDevId 设备ID 
	/// @return			Host对象,NULL-未找到   
	J_Host *GetDeviceObj(j_uint32_t nDevId);
	/// 获取设备对象
	/// @param[in]		strDevId 设备ID 
	/// @return			Host对象,NULL-未找到   
	J_Host *GetDeviceObj(j_string_t strDevId);
};
#endif //~__DEVICE_MANAGER_H_