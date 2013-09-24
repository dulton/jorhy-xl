///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx科技有限公司(版权声明) 
/// All rights reserved. 
/// 
/// @file      TcpServer4Client.cpp 
/// @brief     客户端TCP服务器模块实现
///
/// @version   1.0 (版本声明)
/// @author    Jorhy (joorhy@gmail.com) 
/// @date      2013/09/22 09:50 
///
///
/// 修订说明：最初版本
///////////////////////////////////////////////////////////////////////////  
#include "TcpServer4Client.h"

#pragma comment(lib, "Debug\\core.lib")
CTcpServer4Client::CTcpServer4Client()
{

}

CTcpServer4Client::~CTcpServer4Client()
{

}

j_result_t CTcpServer4Client::StartService(int nPort)
{
	memset(&m_asioData, 0, sizeof(m_asioData));
	m_asioData.ioAccept.peerPort = nPort;
	m_asioData.ioUser = this;
	m_asio.Listen(&m_asioData);

	return J_OK;
}

j_result_t CTcpServer4Client::StopService()
{
	return J_OK;
}

j_result_t CTcpServer4Client::OnAccept(const J_AsioDataBase *pAsioData, int nRet)
{
	return J_OK;
}

j_result_t CTcpServer4Client::OnRead(const J_AsioDataBase *pAsioData, int nRet)
{
	return J_OK;
}

j_result_t CTcpServer4Client::OnWrite(const J_AsioDataBase *pAsioData, int nRet)
{
	return J_OK;
}

j_result_t CTcpServer4Client::OnBroken(const J_AsioDataBase *pAsioData, int nRet)
{
	return J_OK;
}