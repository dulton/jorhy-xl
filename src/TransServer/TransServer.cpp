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

#ifdef _DEBUG
#pragma comment (lib, "Debug\\core.lib")
#else
#pragma comment (lib, "Release\\core.lib")
#endif

CTcpServer4Device g_deviceServer;
CTcpServer4Client g_clientServer;

int main(int argc, char **argv)
{
	///帮助函数
	if (argc == 2 && strcmp(argv[1], "--help") == 0)
	{
		printf("--devport 	DVR连接端口	(默认8501)\n");
		printf("--cliport 	客户端连接端口	(默认8502)\n");
		printf("--dbaddr	数据库IP地址	(默认\"127.0.0.1\")\n");
		printf("--dbport  	数据库端口	(默认1433)\n");
		printf("--dbuser  	数据库登录名	(默认\"sa\")\n");
		printf("--dbpassword	数据库登录密码	(默认\"123456\")\n");
		return 0;
	}

	///初始化相关参数
	char strDbAddr[16] = {0};
	char strDbUser[32] = {0};
	char strDbPassWord[32] = {0};
	int nDbPort = 1433;
	int nDevPort = 8503;
	int nCliPort = 8502;
	strcpy(strDbAddr, "127.0.0.1");
	strcpy(strDbUser, "sa");
	strcpy(strDbPassWord, "123456");
	for (int i=1; i<argc; i+=2)
	{
		if (strcmp(argv[i], "--devport") == 0)
			nDevPort = atoi(argv[i + 1]);
		if (strcmp(argv[i], "--cliport") == 0)
			nCliPort = atoi(argv[i + 1]);
		if (strcmp(argv[i], "--dbaddr") == 0)
			strcpy(strDbAddr, argv[i + 1]);
		if (strcmp(argv[i], "--dbport") == 0)
			nDbPort = atoi(argv[i + 1]);
		if (strcmp(argv[i], "--dbuser") == 0)
			strcpy(strDbUser, argv[i + 1]);
		if (strcmp(argv[i], "--dbpassword") == 0)
			strcpy(strDbPassWord, argv[i + 1]);
		if (strcmp(argv[i], "--debug") == 0)
			J_OS::ENABLE_DEBUG();
	}

	///初始化数据库
	if (JoDataBaseObj->Connect(strDbAddr, nDbPort, strDbUser, strDbPassWord) != J_OK)
	{
		J_OS::LOGINFO("数据库连接失败, IP地址:%s,端口:%d,用户名:%s,密码:%s", strDbAddr, nDbPort, strDbUser, strDbPassWord);
		return -1;
	}
	JoDataBaseObj->Init();

	///启动设备监听服务
	if (g_deviceServer.StartService(nDevPort) != J_OK)
	{
		J_OS::LOGINFO("启动设备监听服务失败, 端口:%d", nDevPort);
		return -1;
	}

	///启动客户监听服务
	if (g_clientServer.StartService(nCliPort) != J_OK)
	{
		J_OS::LOGINFO("启动客户监听服务失败, 端口:%d", nCliPort);
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