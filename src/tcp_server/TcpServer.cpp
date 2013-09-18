/** 
*  Copyright (c) 2013, xx科技有限公司 
*  All rights reserved. 
* 
*  @file        TcpServer.cpp 
*  @author      Jorhy(joorhy@gmail.com) 
*  @date        2013/09/18 11:23 
*/  
#include "TcpServer.h"

CTcpServer::CTcpServer()
{

}

CTcpServer::~CTcpServer()
{

}

j_result_t CTcpServer::StartService(int nPort)
{
	return J_OK;
}

j_result_t CTcpServer::StopService()
{
	return J_OK;
}

j_result_t CTcpServer::OnAccept(const J_AsioDataBase *pAsioData, int nRet)
{
	return J_OK;
}

j_result_t CTcpServer::OnRead(const J_AsioDataBase *pAsioData, int nRet)
{
	return J_OK;
}

j_result_t CTcpServer::OnWrite(const J_AsioDataBase *pAsioData, int nRet)
{
	return J_OK;
}

j_result_t CTcpServer::OnBroken(const J_AsioDataBase *pAsioData, int nRet)
{
	return J_OK;
}