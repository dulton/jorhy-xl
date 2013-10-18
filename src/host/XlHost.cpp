///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx科技有限公司(版权声明) 
/// All rights reserved. 
/// 
/// @file      XlHost.cpp 
/// @brief     XL_DVR设备模块实现
///
/// @version   1.0 (版本声明)
/// @author    Jorhy (joorhy@gmail.com) 
/// @date      2013/09/22 17:41 
///
///
/// 修订说明：最初版本
/////////////////////////////////////////////////////////////////////////// 
#include "XlHost.h"
#include "XlType.h"
#include "XlChannel.h"

#pragma comment (lib, "Debug\\core.lib")

CXlHost::CXlHost(j_socket_t nSock)
: m_cmdSocket(nSock)
{
	m_bReady = false;
	m_lastBreatTime = time(0);

	m_readBuff = new char[BUFFER_SIZE];			
	m_writeBuff = new char[BUFFER_SIZE];			
	m_ioState = xl_init_state;
}

CXlHost::~CXlHost()
{
	delete m_readBuff;
	delete m_writeBuff;
}
j_result_t CXlHost::MakeChannel(const j_int32_t nChannelNum, J_Channel *&pObj)
{
	J_Channel *pChannel = NULL;
	ChannelMap::iterator it = m_channelMap.find(nChannelNum);
	if (it != m_channelMap.end())
	{
		pChannel = it->second;
	}
	else
	{
		pChannel = new CXlChannel(this, nChannelNum);
		m_channelMap[nChannelNum] = pChannel;
	}
	pObj = pChannel;
	return J_OK;
}

j_boolean_t CXlHost::IsReady()
{
	if (time(0) - m_lastBreatTime > 5)
		m_bReady = false;

	return m_bReady;
}

j_result_t CXlHost::GetHostId(j_string_t &strDevId)
{
	strDevId = m_hostId;
	return J_OK;
}

j_result_t CXlHost::SetTime()
{
	return J_OK;
}

j_result_t CXlHost::GetDeviceInfo()
{
	return J_OK;
}

j_result_t CXlHost::ParserRequest(J_AsioDataBase *pAsioData)
{
	if (pAsioData->ioType == J_AsioDataBase::j_data_e)
	{
		if (m_ioState == xl_init_state)
		{
			MakeCommand(xl_register, NULL, 0, (j_char_t *)m_writeBuff);
			pAsioData->ioCall = J_AsioDataBase::j_write_e;
			MakeNetData(pAsioData, m_writeBuff, sizeof(CmdHeader) + sizeof(CmdTail));
		
			m_ioState = xl_write_body_state;
		}
		else if (m_ioState == xl_read_head_state)
		{
			pAsioData->ioCall = J_AsioDataBase::j_read_e;
			MakeNetData(pAsioData, m_readBuff + sizeof(CmdHeader), *((j_int32_t *)(m_readBuff + 4)) + sizeof(CmdTail));

			m_ioState = xl_read_data_state;
		}
		else if (m_ioState == xl_read_data_state)
		{
			CmdHeader *pHeader = (CmdHeader *)m_readBuff;
			switch (pHeader->cmd)
			{
			case xl_register:
				{
					RegisterResp *pReps = (RegisterResp *)(m_readBuff + sizeof(CmdHeader));
					m_hostId = pReps->hostId;
					m_bReady = true;
					J_OS::LOGINFO("hostId =%s", pReps->hostId);
					MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
					pAsioData->ioCall = J_AsioDataBase::j_read_e;

					m_ioState = xl_read_head_state;
				}
				break;
			case xl_heartbeat:
				m_lastBreatTime = time(0);
				MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
				pAsioData->ioCall = J_AsioDataBase::j_read_e;

				m_ioState = xl_read_head_state;
				break;
			case xl_get_alraminfo:
				{
					AlarmInfoResp *pAlarmInfo = (AlarmInfoResp *)(m_readBuff + sizeof(CmdHeader));
					J_OS::LOGINFO("GPS(%f %f %f) %f %d", pAlarmInfo->gps.dLatitude, pAlarmInfo->gps.dLongitude,
						pAlarmInfo->gps.dGPSSpeed, pAlarmInfo->speed, pAlarmInfo->tmTimeStamp);

					MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
					pAsioData->ioCall = J_AsioDataBase::j_read_e;

					m_ioState = xl_read_head_state;
				}
				break;
			case xl_real_play:
				{
					RealPlayReq *pReps = (RealPlayReq *)(m_readBuff + sizeof(CmdHeader));
					J_OS::LOGINFO("host hostId = %s, channel = %d", pReps->hostId, pReps->channel & 0xFF);
					ChannelMap::iterator it = m_channelMap.find(pReps->channel & 0xFF);
					if (it != m_channelMap.end())
					{
						CXlChannel *pXlChannel = dynamic_cast<CXlChannel *>(it->second);
						if (pXlChannel != NULL)
							pXlChannel->InputData(m_readBuff, pHeader->length + sizeof(CmdHeader) + sizeof(CmdTail));
					}
					///视频数据
					pAsioData->ioCall = J_AsioDataBase::j_read_e;
					MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));

					m_ioState = xl_read_head_state;
				}
				break;
			case xl_real_stop:
				{
					RealStopReq *pReps = (RealStopReq *)(m_readBuff + sizeof(CmdHeader));
					J_OS::LOGINFO("hostId = %s, channel = %d", pReps->hostId, pReps->channel & 0xFF);
					///返回数据
					MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
					pAsioData->ioCall = J_AsioDataBase::j_read_e;

					m_ioState = xl_read_head_state;
				}
				break;
			default:
				break;
			}
			J_OS::LOGINFO("%d %X", pAsioData->ioRead.finishedLen, pHeader->flag & 0xFF);
		}
		else
		{
			pAsioData->ioCall = J_AsioDataBase::j_read_e;
			MakeNetData(pAsioData, m_readBuff, sizeof(CmdHeader));
			pAsioData->ioCall = J_AsioDataBase::j_read_e;

			m_ioState = xl_read_head_state;
		}
	}
	else
	{
		j_char_t *pDataBuff = pAsioData->ioRead.buf;
		CmdHeader *pHeader = (CmdHeader *)pDataBuff;
		switch (pHeader->cmd)
		{
		case xl_real_play:
			{
				RealPlayReq *pReps = (RealPlayReq *)(pDataBuff + sizeof(CmdHeader));
				J_OS::LOGINFO("host hostId = %s, channel = %d", pReps->hostId, pReps->channel & 0xFF);
				m_cmdSocket.Write_n(pDataBuff, pAsioData->ioRead.bufLen);
			}
			break;
		case xl_real_stop:
			{
				RealStopReq *pReps = (RealStopReq *)(pDataBuff + sizeof(CmdHeader));
				J_OS::LOGINFO("hostId = %s, channel = %d", pReps->hostId, pReps->channel & 0xFF);
				m_cmdSocket.Write_n(pDataBuff, pAsioData->ioRead.bufLen);
			}
			break;
		default:
			break;
		}
	}

	return J_OK;
}

j_result_t CXlHost::MakeCommand(j_char_t bCmd, j_char_t *pData, j_int32_t nLen, j_char_t *pBody)
{
	//填充头信息
	CmdHeader *pHeader = (CmdHeader *)pBody;
	pHeader->beginCode = 0xFF;
	pHeader->version = 0x0A;
	pHeader->flag = 0x00;
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

j_uint32_t CXlHost::CheckNum(j_char_t *pData, j_int32_t nLen)
{
	j_uint32_t nCheckNum = 0xFE;
	for (int i=0; i<nLen; ++i)
		nCheckNum += pData[i];

	return (nCheckNum % 256);
}

j_result_t CXlHost::MakeNetData(J_AsioDataBase *pAsioData, j_char_t *pDataBuff, j_int32_t nLen)
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