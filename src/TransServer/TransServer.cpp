///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx�Ƽ����޹�˾(��Ȩ����) 
/// All rights reserved. 
/// 
/// @file      nvr.cpp 
/// @brief     NVRӦ�ó���ģ��
///
/// @version   1.0 (�汾����)
/// @author    Jorhy (joorhy@gmail.com) 
/// @date      2013/09/18 17:11 
///
///
/// �޶�˵��������汾
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
	///��������
	if (argc == 2 && strcmp(argv[1], "--help") == 0)
	{
		printf("--devport 	DVR���Ӷ˿�	(Ĭ��8501)\n");
		printf("--cliport 	�ͻ������Ӷ˿�	(Ĭ��8502)\n");
		printf("--dbaddr	���ݿ�IP��ַ	(Ĭ��\"127.0.0.1\")\n");
		printf("--dbport  	���ݿ�˿�	(Ĭ��1433)\n");
		printf("--dbuser  	���ݿ��¼��	(Ĭ��\"sa\")\n");
		printf("--dbpassword	���ݿ��¼����	(Ĭ��\"123456\")\n");
		return 0;
	}

	///��ʼ����ز���
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

	///��ʼ�����ݿ�
	if (JoDataBaseObj->Connect(strDbAddr, nDbPort, strDbUser, strDbPassWord) != J_OK)
	{
		J_OS::LOGINFO("���ݿ�����ʧ��, IP��ַ:%s,�˿�:%d,�û���:%s,����:%s", strDbAddr, nDbPort, strDbUser, strDbPassWord);
		return -1;
	}
	JoDataBaseObj->Init();

	///�����豸��������
	if (g_deviceServer.StartService(nDevPort) != J_OK)
	{
		J_OS::LOGINFO("�����豸��������ʧ��, �˿�:%d", nDevPort);
		return -1;
	}

	///�����ͻ���������
	if (g_clientServer.StartService(nCliPort) != J_OK)
	{
		J_OS::LOGINFO("�����ͻ���������ʧ��, �˿�:%d", nCliPort);
		return -1;
	}

	while (true)
	{
		Sleep(100);
	}

	///ֹͣ�ͻ���������
	g_clientServer.StopService();
	///ֹͣ�豸��������
	g_deviceServer.StopService();
	///ע�����ݿ�����
	JoDataBaseObj->Release();

	return 0;
}