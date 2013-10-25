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

#pragma comment(lib, "Debug\\core.lib")

CTcpServer4Device g_deviceServer;
CTcpServer4Client g_clientServer;

int main(int argc, char **argv)
{
	///��ʼ�����ݿ�
	if (JoDataBaseObj->Connect("127.0.0.1", 1433, "sa", "123456") != J_OK)
	{
		J_OS::LOGINFO("���ݿ�����ʧ��, IP��ַ:127.0.0.1,�˿�:1433,�û���:sa,����:123456");
		return -1;
	}

	///�����豸��������
	if (g_deviceServer.StartService(8503) != J_OK)
	{
		J_OS::LOGINFO("�����豸��������ʧ��, �˿�:8501");
		return -1;
	}

	///�����ͻ���������
	if (g_clientServer.StartService(8502) != J_OK)
	{
		J_OS::LOGINFO("�����ͻ���������ʧ��, �˿�:8502");
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