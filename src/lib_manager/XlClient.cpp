///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx科技有限公司(版权声明) 
/// All rights reserved. 
/// 
/// @file      Client.cpp 
/// @brief     客户端处理模块实现
///
/// @version   1.0 (版本声明)
/// @author    Jorhy (joorhy@gmail.com) 
/// @date      2013/09/24 15:13 
///
///
/// 修订说明：最初版本
///////////////////////////////////////////////////////////////////////////  
#include "XlClient.h"
#include "XlType.h"
#include "DeviceManager.h"

#pragma comment (lib, "Debug\\core.lib")

#define XL_BUFFER_SIZE (1024 * 1024)
CXlClient::CXlClient(j_socket_t nSock)
{
	m_readBuff = new char[XL_BUFFER_SIZE];			
	m_writeBuff = new char[XL_BUFFER_SIZE];		
	m_dataBuff = new char[XL_BUFFER_SIZE];	
	m_ioCmdState = xl_init_state;
	m_ioDataState = xl_init_state;
	m_nRefCnt = 0;
}

CXlClient::~CXlClient()
{
	delete m_readBuff;
	delete m_writeBuff;
	delete m_dataBuff;
}

j_result_t CXlClient::ParserRequest(J_AsioDataBase *pAsioData)
{
	if (pAsioData->ioType == J_AsioDataBase::j_command_e)
	{
		if (m_ioCmdState == xl_init_state)
		{
			pAsioData->ioCall = J_AsioDataBase::j_read_e;
			MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));

			m_ioCmdState = xl_read_head_state;
		}
		else if (m_ioCmdState == xl_read_head_state)
		{
			MakeNetData(pAsioData, m_readBuff + sizeof(CmdHeader), *((j_int32_t *)(m_readBuff + 4)) + sizeof(CmdTail));
			pAsioData->ioCall = J_AsioDataBase::j_read_e;

			m_ioCmdState = xl_read_data_state;
		}
		else if (m_ioCmdState == xl_read_data_state)
		{
			CmdHeader *pHeader = (CmdHeader *)m_readBuff;
			assert((pHeader->flag & 0xFF)== 0x00);
			switch (pHeader->cmd)
			{
			case xl_login:
				{
					int nBodyLen = sizeof(CmdHeader) + sizeof(LoginResp) + sizeof(CmdTail);
					LoginResp data = {0};
					data.ret = 0x00;
					MakeResponse(xl_login, (j_char_t *)&data, sizeof(LoginResp), m_writeBuff);
					pAsioData->ioCall = J_AsioDataBase::j_write_e;
					MakeNetData(pAsioData, m_writeBuff, nBodyLen);

					m_ioCmdState = xl_write_body_state;
				}
				break;
			case xl_logout:
				{
					int nBodyLen = sizeof(CmdHeader) + sizeof(LoginResp) + sizeof(CmdTail);
					LoginResp data = {0};
					data.ret = 0x00;
					MakeResponse(xl_logout, (j_char_t *)&data, sizeof(LoginResp), m_writeBuff);
					pAsioData->ioCall = J_AsioDataBase::j_write_e;
					MakeNetData(pAsioData, m_writeBuff, nBodyLen);

					m_ioCmdState = xl_write_body_state;
				}
				break;
			case xl_real_play:
				{
					RealPlayReq *pReps = (RealPlayReq *)(m_readBuff + sizeof(CmdHeader));
					J_OS::LOGINFO("hostId = %s, channel = %d", pReps->hostId, pReps->channel & 0xFF);
					if ((pHeader->flag & 0xFF) == 0x00)
					{
						J_Host *pHost = JoDeviceManager->GetDeviceObj(pReps->hostId);
						if (pHost != NULL)
						{
							pAsioData->ioRead.buf = m_readBuff;
							pAsioData->ioRead.bufLen = pAsioData->ioRead.finishedLen + sizeof(CmdHeader);
							pHost->ParserRequest(pAsioData);

							J_Channel *pChannel = NULL;
							pHost->MakeChannel(pReps->channel, pChannel);
							if (pChannel != NULL)
								pChannel->OpenStream(&m_ringBuffer);
						}
						pAsioData->ioCall = J_AsioDataBase::j_read_e;
						MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
						if (m_nRefCnt == 0)
							pAsioData->ioCall = J_AsioDataBase::j_read_write_e;
						atomic_inc(&m_nRefCnt);
						m_ioCmdState = xl_read_head_state;
					} 
				}
				break;
			case xl_real_stop:
				{
					RealStopReq *pReps = (RealStopReq *)(m_readBuff + sizeof(CmdHeader));
					J_OS::LOGINFO("hostId = %s, channel = %d", pReps->hostId, pReps->channel & 0xFF);

					J_Host *pHost = JoDeviceManager->GetDeviceObj(pReps->hostId);
					if (pHost != NULL)
					{
						pAsioData->ioRead.buf = m_readBuff;
						pAsioData->ioRead.bufLen = pAsioData->ioRead.finishedLen + sizeof(CmdHeader);
						pHost->ParserRequest(pAsioData);

						J_Channel *pChannel = NULL;
						pHost->MakeChannel(pReps->channel, pChannel);
						if (pChannel != NULL)
							pChannel->CloseStream(&m_ringBuffer);
					}

					int nBodyLen = sizeof(CmdHeader) + sizeof(RealPlayResp) + sizeof(CmdTail);
					RealPlayResp data = {0};
					data.ret = 0x00;
					MakeResponse(xl_real_stop, (j_char_t *)&data, sizeof(RealPlayResp), m_writeBuff);
					pAsioData->ioCall = J_AsioDataBase::j_write_e;
					MakeNetData(pAsioData, m_writeBuff, nBodyLen);
					if (m_nRefCnt > 0)
						atomic_dec(&m_nRefCnt);
					m_ioCmdState = xl_write_body_state;
				}
				break;
			default:
				break;
			}
			J_OS::LOGINFO("%d %X", pHeader->cmd, pHeader->flag & 0xFF);
		}
		else
		{	
			pAsioData->ioCall = J_AsioDataBase::j_read_e;
			MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));

			m_ioCmdState = xl_read_head_state;
		}
	}
	else
	{
		if (m_nRefCnt > 0)
		{
			if (m_ioDataState == xl_init_state)
			{
				pAsioData->ioType = J_AsioDataBase::j_data_e;
				pAsioData->hEvent = WSACreateEvent();

				pAsioData->ioCall = J_AsioDataBase::j_write_e;
				MakeTransData(pAsioData);
				m_ioDataState = xl_write_body_state;
			}
			else
			{
				pAsioData->ioCall = J_AsioDataBase::j_write_e;
				MakeTransData(pAsioData);
				m_ioDataState = xl_write_body_state;
			}
		}
		else
		{
			pAsioData->ioCall = J_AsioDataBase::j_keep_e;
		}
	}

	return J_OK;
}

j_result_t CXlClient::MakeResponse(j_char_t bCmd, j_char_t *pData, j_int32_t nLen, j_char_t *pBody)
{
	//填充头信息
	CmdHeader *pHeader = (CmdHeader *)pBody;
	pHeader->beginCode = 0xFF;
	pHeader->version = 0x0A;
	pHeader->flag = 0xFF;
	pHeader->cmd = bCmd;
	pHeader->length = nLen;
	//填充数据区
	if (nLen > 0)
		memcpy(pBody + sizeof(CmdHeader), pData, nLen);
	//填充尾信息
	CmdTail *pTail = (CmdTail *)(pBody + sizeof(CmdHeader) + nLen);
	pTail->verify = CheckNum(pBody, sizeof(CmdHeader) + nLen);
	pTail->endCode = 0xFE;

	return J_OK;
}

j_uint32_t CXlClient::CheckNum(j_char_t *pData, j_int32_t nLen)
{
	j_uint32_t nCheckNum = 0xFE;
	for (int i=0; i<nLen; ++i)
		nCheckNum += pData[i];

	return (nCheckNum % 256);
}

j_result_t CXlClient::MakeNetData(J_AsioDataBase *pAsioData, j_char_t *pDataBuff, j_int32_t nLen)
{
	if (pAsioData->ioCall == J_AsioDataBase::j_read_e)
	{
		pAsioData->ioRead.buf = pDataBuff;
		pAsioData->ioRead.bufLen = nLen;
		pAsioData->ioRead.finishedLen = 0;
		pAsioData->ioRead.whole = true;
		pAsioData->ioRead.shared = true;
	}
	else if (pAsioData->ioCall == J_AsioDataBase::j_write_e)
	{
		pAsioData->ioWrite.buf = pDataBuff;
		pAsioData->ioWrite.bufLen = nLen;
		pAsioData->ioWrite.finishedLen = 0;
		pAsioData->ioWrite.whole = true;
		pAsioData->ioWrite.shared = true;
	}
	return J_OK;
}

j_result_t CXlClient::MakeTransData(J_AsioDataBase *pAsioData)
{
	j_result_t nResult = J_OK;
	memset(&m_streamHeader, 0, sizeof(J_StreamHeader));
	do 
	{
		nResult = m_ringBuffer.PopBuffer(m_dataBuff, m_streamHeader);
		if (nResult == J_OK)
		{
			pAsioData->ioWrite.buf = m_dataBuff;
			pAsioData->ioWrite.bufLen = m_streamHeader.dataLen;
			pAsioData->ioWrite.finishedLen = 0;
			pAsioData->ioWrite.whole = true;
			pAsioData->ioWrite.shared = true;
		}
	} while (nResult != J_OK);

	return J_OK;
}