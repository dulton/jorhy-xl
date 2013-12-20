///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx�Ƽ����޹�˾(��Ȩ����) 
/// All rights reserved. 
/// 
/// @file      XlHost.h 
/// @brief     XL_DVR�豸ģ��
///
/// @version   1.0 (�汾����)
/// @author    Jorhy (joorhy@gmail.com) 
/// @date      2013/09/22 17:40 
///
///
/// �޶�˵��������汾
///////////////////////////////////////////////////////////////////////////  
#ifndef __XLHOST_H_
#define __XLHOST_H_
#include "j_includes.h"
#include "x_socket.h"
/// ����Ĺ���:  XL�豸������
class CXlHost : public J_Host
{
	struct ChannelInfo
	{
		J_Obj *pChannel;
		j_int32_t nRef;
	};
	typedef std::map<j_int32_t, ChannelInfo> ChannelMap;
public:
	CXlHost(j_socket_t nSock);
	~CXlHost();

public:
	///J_Host
	virtual j_result_t MakeChannel(const j_int32_t nChannelNum, J_Obj *&pObj);
	virtual j_boolean_t IsReady();
	virtual j_result_t GetHostId(j_string_t &strDevId);
	virtual j_result_t SetTime(j_time_t sysTime);
	virtual j_result_t GetDeviceInfo();
	virtual j_result_t EnableRcdInfo(CRingBuffer *pRingBuffer, j_boolean_t bEnable = true);
	virtual j_result_t EnableAlarm(CRingBuffer *pRingBuffer, j_boolean_t bEnable = true);
	virtual j_result_t ParserRequest(J_AsioDataBase *pAsioData);
	virtual j_result_t SendMessage(j_char_t *pData, j_int32_t nLen);

private:
	j_result_t ProcessClientCmd(J_AsioDataBase *pAsioData);
	j_result_t ProcessDeviceCmd(J_AsioDataBase *pAsioData);

	j_result_t OnRegister(J_AsioDataBase *pAsioData);
	j_result_t OnHeartBeat(J_AsioDataBase *pAsioData);
	j_result_t OnAlarmInfo(J_AsioDataBase *pAsioData);
	j_result_t OnRealPlayData(J_AsioDataBase *pAsioData,  j_int32_t nDadaLen);
	j_result_t OnRealStopData(J_AsioDataBase *pAsioData);
	j_result_t OnVodPlayData(J_AsioDataBase *pAsioData,  j_int32_t nDadaLen);
	j_result_t OnVodStopData(J_AsioDataBase *pAsioData);
	j_result_t OnRcdInfoData(J_AsioDataBase *pAsioData);
	j_result_t OnSetSysTime(J_AsioDataBase *pAsioData);
	j_result_t OnGetDevInfo(J_AsioDataBase *pAsioData);
	j_result_t OnGetLogInfo(J_AsioDataBase *pAsioData);
	j_result_t OnDefault(J_AsioDataBase *pAsioData);
	//////////////////////////////////////////////////////////////////////////////////////
	j_result_t OnRealPlay(j_int32_t nChannel);
	j_result_t OnRealStop(j_int32_t nChannel);
	j_result_t OnVodPlay(GUID sessionId, j_int32_t nChannel, j_time_t startTime, j_time_t endTime);
	j_result_t OnVodStop(GUID sessionId, j_int32_t nChannel);
	j_result_t OnGetRcdInfo(j_string_t hostId);

private:
	int AddRingBuffer(J_OS::TLocker_t &locker, std::vector<CRingBuffer *> &ringBufferVec, CRingBuffer *pRingBuffer)
	{
		TLock(locker);
		ringBufferVec.push_back(pRingBuffer);
		TUnlock(locker);

		return J_OK;
	}
	int DelRingBuffer(J_OS::TLocker_t &locker, std::vector<CRingBuffer *> &ringBufferVec, CRingBuffer *pRingBuffer)
	{
		TLock(locker);
		std::vector<CRingBuffer *>::iterator it = ringBufferVec.begin();
		for (; it != ringBufferVec.end(); it++)
		{
			if (*it == pRingBuffer)
			{
				ringBufferVec.erase(it);
				TUnlock(locker);

				return J_OK;
			}
		}
		TUnlock(locker);
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
	j_char_t *m_rcdBuffer;
	j_int32_t m_ioState;				//1-��ͷ����,2-��������+β����
	J_OS::TLocker_t m_vecLocker;
	std::vector<CRingBuffer *> m_vecRingBuffer;
	J_OS::TLocker_t m_vecRcdLocker;
	std::vector<CRingBuffer *> m_vecRcdRingBuffer;
};

#endif