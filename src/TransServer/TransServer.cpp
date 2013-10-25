///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx科技有限公司(版权声明) 
/// All rights reserved. 
/// 
/// @file      nvr.cpp 
/// @brief     NVR应用程序模块
///
/// @version   1.0 (版本声明)
/// @author    Jorhy (joorhy@gmail.com) 
/// @date      2013/09/18 17:11 
///
///
/// 修订说明：最初版本
///////////////////////////////////////////////////////////////////////////  
#include "TcpServer4Client.h"
#include "TcpServer4Device.h"
#include "MsSqlServer.h"

#pragma comment(lib, "Debug\\core.lib")

CTcpServer4Device g_deviceServer;
CTcpServer4Client g_clientServer;

int main(int argc, char **argv)
{
	///初始化数据库
	if (JoDataBaseObj->Connect("127.0.0.1", 1433, "sa", "123456") != J_OK)
	{
		J_OS::LOGINFO("数据库连接失败, IP地址:127.0.0.1,端口:1433,用户名:sa,密码:123456");
		return -1;
	}

	///启动设备监听服务
	if (g_deviceServer.StartService(8503) != J_OK)
	{
		J_OS::LOGINFO("启动设备监听服务失败, 端口:8501");
		return -1;
	}

	///启动客户监听服务
	if (g_clientServer.StartService(8502) != J_OK)
	{
		J_OS::LOGINFO("启动客户监听服务失败, 端口:8502");
		return -1;
	}

	while (true)
	{
		Sleep(100);
	}

	///停止客户监听服务
	g_clientServer.StopService();
	///停止设备监听服务
	g_deviceServer.StopService();
	///注销数据库连接
	JoDataBaseObj->Release();

	return 0;
}