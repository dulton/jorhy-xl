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
#include "ClientManager.h"

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

j_result_t CTcpServer4Client::OnAccept(J_AsioDataBase *pAsioData, int nRet)
{
	j_socket_t nSocket = pAsioData->ioAccept.subHandle;
	m_asio.AddUser(nSocket, this);
	J_Client *pClient = JoClientManager->CreateClientObj(nSocket);
	if (pClient != NULL)
	{
		J_AsioDataBase *pDataBase = new J_AsioDataBase;
		memset(pDataBase, 0, sizeof(J_AsioDataBase));
		//pDataBase->hEvent = WSACreateEvent();
		pDataBase->ioUser = this;
		pDataBase->ioType = J_AsioDataBase::j_command_e;
		pClient->ParserRequest(pDataBase);
		if (pDataBase->ioCall == J_AsioDataBase::j_read_e)
			m_asio.Read(nSocket, pDataBase);
		else if (pDataBase->ioCall == J_AsioDataBase::j_write_e)
			m_asio.Write(nSocket, pDataBase);
	}

	return J_OK;
}

j_result_t CTcpServer4Client::OnRead(J_AsioDataBase *pAsioData, int nRet)
{
	j_socket_t nSocket = pAsioData->ioHandle;
	J_Client *pClient = JoClientManager->GetClientObj(nSocket);
	if (pClient == NULL)
		return J_NOT_FOUND;

	pAsioData->ioUser = this;
	pClient->ParserRequest(pAsioData);
	if (pAsioData->ioCall == J_AsioDataBase::j_read_e)
		m_asio.Read(nSocket, pAsioData);
	else if (pAsioData->ioCall == J_AsioDataBase::j_write_e)
		m_asio.Write(nSocket, pAsioData);
	else
	{
		J_AsioDataBase *pDataBase = new J_AsioDataBase;
		memcpy(pDataBase, pAsioData, sizeof(J_AsioDataBase));
		pAsioData->ioCall = J_AsioDataBase::j_read_e;
		m_asio.Read(nSocket, pAsioData);

		if (pDataBase->ioCall == J_AsioDataBase::j_read_write_e)
		{
			pDataBase->ioType = J_AsioDataBase::j_data_e;
			pClient->ParserRequest(pDataBase);
		}
		else if (pDataBase->ioCall == J_AsioDataBase::j_read_write_log_e)
		{
			pDataBase->ioType = J_AsioDataBase::j_log_e;
			pClient->ParserRequest(pDataBase);
		}
		else if (pDataBase->ioCall == J_AsioDataBase::j_read_write_alarm_e)
		{
			pDataBase->ioType = J_AsioDataBase::j_alarm_e;
			pClient->ParserRequest(pDataBase);
		}
		else if (pDataBase->ioCall == J_AsioDataBase::j_read_write_dvrlist_e)
		{
			pDataBase->ioType = J_AsioDataBase::j_dvr_list_e;
			pClient->ParserRequest(pDataBase);
		}
		else if (pDataBase->ioCall == J_AsioDataBase::j_read_write_userlist_e)
		{
			pDataBase->ioType = J_AsioDataBase::j_user_list_e;
			pClient->ParserRequest(pDataBase);
		}
		else if (pDataBase->ioCall == J_AsioDataBase::j_read_write_departmentlist_e)
		{
			pDataBase->ioType = J_AsioDataBase::j_department_list_e;
			pClient->ParserRequest(pDataBase);
		}
		if (pDataBase->ioCall == J_AsioDataBase::j_write_e)
			m_asio.Write(nSocket, pDataBase);
		else
			delete pDataBase;
	}


	return J_OK;
}

j_result_t CTcpServer4Client::OnWrite(J_AsioDataBase *pAsioData, int nRet)
{
	j_socket_t nSocket = pAsioData->ioHandle;
	J_Client *pClient = JoClientManager->GetClientObj(nSocket);
	if (pClient == NULL)
		return J_NOT_FOUND;

	pAsioData->ioUser = this;
	pClient->ParserRequest(pAsioData);
	if (pAsioData->ioCall == J_AsioDataBase::j_read_e)
		m_asio.Read(nSocket, pAsioData);
	else if (pAsioData->ioCall == J_AsioDataBase::j_write_e)
		m_asio.Write(nSocket, pAsioData);
	else
		delete pAsioData;

	return J_OK;
}

j_result_t CTcpServer4Client::OnBroken(J_AsioDataBase *pAsioData, int nRet)
{
	j_socket_t nSocket = pAsioData->ioHandle;
	m_asio.DelUser(nSocket);
	JoClientManager->ReleaseClientObj(nSocket);
	J_OS::LOGINFO("CTcpServer4Client::OnBroken sock = %d", nSocket.sock);

	return J_OK;
}