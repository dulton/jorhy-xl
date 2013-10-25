///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx科技有限公司(版权声明) 
/// All rights reserved. 
/// 
/// @file      XlHost.h 
/// @brief     XL_DVR设备模块
///
/// @version   1.0 (版本声明)
/// @author    Jorhy (joorhy@gmail.com) 
/// @date      2013/09/22 17:40 
///
///
/// 修订说明：最初版本
///////////////////////////////////////////////////////////////////////////  
#ifndef __XLHOST_H_
#define __XLHOST_H_
#include "j_includes.h"
#include "x_socket.h"
/// 本类的功能:  XL设备接入类
class CXlHost : public J_Host
{
	struct ChannelInfo
	{
		J_Channel *pChannel;
		j_int32_t nRef;
	};
	typedef std::map<j_int32_t, ChannelInfo> ChannelMap;
public:
	CXlHost(j_socket_t nSock);
	~CXlHost();

public:
	///J_Host
	virtual j_result_t MakeChannel(const j_int32_t nChannelNum, J_Channel *&pObj);
	virtual j_boolean_t IsReady();
	virtual j_result_t GetHostId(j_string_t &strDevId);
	virtual j_result_t SetTime(j_time_t sysTime);
	virtual j_result_t GetDeviceInfo();
	virtual j_result_t EnableAlarm(CRingBuffer *pRingBuffer, j_boolean_t bEnable = true);
	virtual j_result_t ParserRequest(J_AsioDataBase *pAsioData);

private:
	j_result_t MakeRequest(j_char_t bCmd, j_char_t *pData, j_int32_t nLen, j_char_t *pBody);
	j_uint32_t CheckNum(j_char_t *pData, j_int32_t nLen);
	j_result_t MakeNetData(J_AsioDataBase *pAsioData, j_char_t *pDataBuff, j_int32_t nLen);
	j_result_t ProcessClientCmd(J_AsioDataBase *pAsioData);
	j_result_t ProcessDeviceCmd(J_AsioDataBase *pAsioData);
	j_result_t OnRegister(J_AsioDataBase *pAsioData);
	j_result_t OnHeartBeat(J_AsioDataBase *pAsioData);
	j_result_t OnAlarmInfo(J_AsioDataBase *pAsioData);
	j_result_t OnRealPlayData(J_AsioDataBase *pAsioData,  j_int32_t nDadaLen);
	j_result_t OnRealStopData(J_AsioDataBase *pAsioData);
	j_result_t OnVodPlayData(J_AsioDataBase *pAsioData,  j_int32_t nDadaLen);
	j_result_t OnVodStopData(J_AsioDataBase *pAsioData);
	j_result_t OnSetSysTime(J_AsioDataBase *pAsioData);
	j_result_t OnGetDevInfo(J_AsioDataBase *pAsioData);
	j_result_t OnGetLogInfo(J_AsioDataBase *pAsioData);
	//////////////////////////////////////////////////////////////////////////////////////
	j_result_t OnRealPlay(j_char_t *pData, j_int32_t nLen);
	j_result_t OnRealStop(j_char_t *pData, j_int32_t nLen);
	j_result_t OnVodPlay(j_char_t *pData, j_int32_t nLen);
	j_result_t OnVodStop(j_char_t *pData, j_int32_t nLen);

private:
	int AddRingBuffer(CRingBuffer *pRingBuffer)
	{
		TLock(m_vecLocker);
		m_vecRingBuffer.push_back(pRingBuffer);
		TUnlock(m_vecLocker);

		return J_OK;
	}
	int DelRingBuffer(CRingBuffer *pRingBuffer)
	{
		TLock(m_vecLocker);
		std::vector<CRingBuffer *>::iterator it = m_vecRingBuffer.begin();
		for (; it != m_vecRingBuffer.end(); it++)
		{
			if (*it == pRingBuffer)
			{
				m_vecRingBuffer.erase(it);
				TUnlock(m_vecLocker);

				return J_OK;
			}
		}
		TUnlock(m_vecLocker);
		return J_NOT_EXIST;
	}

private:
	j_boolean_t m_bReady;
	j_string_t m_hostId;
	j_int32_t m_lastBreatTime;
	J_OS::CTCPSocket m_cmdSocket;

	ChannelMap m_channelMap;
	J_OS::TLocker_t m_channelLocker;
	j_char_t *m_readBuff;
	j_char_t *m_writeBuff;
	j_int32_t m_ioState;				//1-读头数据,2-读数据区+尾数据
	J_OS::TLocker_t m_vecLocker;
	std::vector<CRingBuffer *> m_vecRingBuffer;
};

#endif