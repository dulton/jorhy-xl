/** 
*  Copyright (c) 2013, xx科技有限公司 
*  All rights reserved. 
* 
*  @file        TcpServer.cpp 
*  @author      Jorhy(joorhy@gmail.com) 
*  @date        2013/09/18 11:23 
*/  
#include "TcpServer4Device.h"

#pragma comment(lib, "Debug\\core.lib")
CTcpServer4Device::CTcpServer4Device()
{
	J_OS::LOGINFO("CTcpServer4Device::CTcpServer4Device()");
}

CTcpServer4Device::~CTcpServer4Device()
{
	J_OS::LOGINFO("CTcpServer4Device::~CTcpServer4Device()");
}

j_result_t CTcpServer4Device::StartService(int nPort)
{
	memset(&m_asioData, 0, sizeof(m_asioData));
	m_asioData.ioAccept.peerPort = nPort;
	m_asioData.ioUser = this;
	m_asio.Listen(&m_asioData);

	return J_OK;
}

j_result_t CTcpServer4Device::StopService()
{
	return J_OK;
}

j_result_t CTcpServer4Device::OnAccept(const J_AsioDataBase *pAsioData, int nRet)
{
	return J_OK;
}

j_result_t CTcpServer4Device::OnRead(const J_AsioDataBase *pAsioData, int nRet)
{
	return J_OK;
}

j_result_t CTcpServer4Device::OnWrite(const J_AsioDataBase *pAsioData, int nRet)
{
	return J_OK;
}

j_result_t CTcpServer4Device::OnBroken(const J_AsioDataBase *pAsioData, int nRet)
{
	return J_OK;
}