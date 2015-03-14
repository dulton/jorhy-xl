/** 
*  Copyright (c) 2013, xx科技有限公司 
*  All rights reserved. 
* 
*  @file        TcpServer.cpp 
*  @author      Jorhy(joorhy@gmail.com) 
*  @date        2013/09/18 11:23 
*/  
#include "TcpServer4Device.h"
#include "DeviceManager.h"

#define BUFFER_SIZE (1024 * 1024)

CTcpServer4Device::CTcpServer4Device()
{
	m_buff = new char[BUFFER_SIZE];			//不需要释放
	m_state = 0;
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

j_result_t CTcpServer4Device::OnAccept(J_AsioDataBase *pAsioData, int nRet)
{
	u_long ret = 0;
	ioctlsocket(pAsioData->ioAccept.subHandle, FIONBIO, &ret);
	j_socket_t nSocket = pAsioData->ioAccept.subHandle;
	m_asio.AddUser(nSocket, this);
	J_Host *pHost = JoDeviceManager->CreateDevObj(1, nSocket);
	if (pHost != NULL)
	{
		J_AsioDataBase *pDataBase = new J_AsioDataBase;
		memset(pDataBase, 0, sizeof(J_AsioDataBase));
		pDataBase->ioUser = this;
		pDataBase->ioType = J_AsioDataBase::j_data_e;

		pHost->ParserRequest(pDataBase);
		if (pDataBase->ioCall == J_AsioDataBase::j_read_e)
			m_asio.Read(nSocket, pDataBase);
		else if (pDataBase->ioCall == J_AsioDataBase::j_write_e)
			m_asio.Write(nSocket, pDataBase);
	}
	return J_OK;
}

j_result_t CTcpServer4Device::OnRead(J_AsioDataBase *pAsioData, int nRet)
{
	j_socket_t nSocket = pAsioData->ioHandle;
	J_Host *pHost = JoDeviceManager->GetDeviceObj(nSocket);
	if (pHost == NULL)
		return J_NOT_FOUND;

	pAsioData->ioUser = this;
	pHost->ParserRequest(pAsioData);
	if (pAsioData->ioCall == J_AsioDataBase::j_read_e)
		m_asio.Read(nSocket, pAsioData);
	else if (pAsioData->ioCall == J_AsioDataBase::j_write_e)
		m_asio.Write(nSocket, pAsioData);

	return J_OK;
}

j_result_t CTcpServer4Device::OnWrite(J_AsioDataBase *pAsioData, int nRet)
{
	j_socket_t nSocket = pAsioData->ioHandle;
	J_Host *pHost = JoDeviceManager->GetDeviceObj(nSocket);
	if (pHost == NULL)
		return J_NOT_FOUND;

begin_write:
	pAsioData->ioUser = this;
	pHost->ParserRequest(pAsioData);
	if (pAsioData->ioCall == J_AsioDataBase::j_read_e)
		m_asio.Read(nSocket, pAsioData);
	else if (pAsioData->ioCall == J_AsioDataBase::j_write_e)
	{
		if (pAsioData->ioWrite.bufLen == 0)
		{
			j_sleep(1);
			goto begin_write;
		}
		m_asio.Write(nSocket, pAsioData);
	}
	else if (pAsioData->ioCall == J_AsioDataBase::j_write_end_e)
	{
		delete pAsioData;
		pAsioData = NULL;
	}

	return J_OK;
}

j_result_t CTcpServer4Device::OnBroken(J_AsioDataBase *pAsioData, int nRet)
{
	j_socket_t nSocket = pAsioData->ioHandle;
	JoDeviceManager->ReleaseDevObj(nSocket);
	m_asio.DelUser(nSocket);
	j_close_socket(nSocket.sock);
	J_OS::LOGINFO("CTcpServer4Device::OnBroken");
	m_state = 0;

	return J_OK;
}

j_result_t CTcpServer4Device::SentData(j_socket_t nSocket)
{
	J_Host *pHost = JoDeviceManager->GetDeviceObj(nSocket);
	if (pHost == NULL)
		return J_NOT_FOUND;


	J_AsioDataBase *pDataBase = new J_AsioDataBase;
	memset(pDataBase, 0, sizeof(J_AsioDataBase));
	pDataBase->ioUser = this;
	pDataBase->ioType = J_AsioDataBase::j_upload_e;
	pDataBase->ioUser = this;

	pHost->ParserRequest(pDataBase);
	if (pDataBase->ioCall == J_AsioDataBase::j_read_e)
		m_asio.Read(nSocket, pDataBase);
	else if (pDataBase->ioCall == J_AsioDataBase::j_write_e)
		m_asio.Write(nSocket, pDataBase);
	else if (pDataBase->ioCall == J_AsioDataBase::j_write_end_e)
	{
		delete pDataBase;
		pDataBase = NULL;
	}

	return J_OK;
}