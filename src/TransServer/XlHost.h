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
	/// ͨ����Ϣ
	struct ChannelInfo
	{
		J_Obj *pChannel;
		j_int32_t nRef;
	};
	typedef std::map<j_int32_t, ChannelInfo> ChannelMap;
	/// �ļ��ϴ��Ự��Ϣ
	struct FileUploadInfo
	{
		int nUserId;
		int nFileId;
	};
	typedef std::vector<FileUploadInfo> VecFileUploadInfo;
public:
	CXlHost(j_socket_t nSock);
	~CXlHost();

public:
	///J_Host
	virtual j_result_t MakeChannel(const j_int32_t nChannelNum, J_Obj *&pObj);
	virtual j_boolean_t IsReady();
	virtual j_result_t OnBroken();
	virtual j_result_t GetHostId(j_string_t &strDevId);
	virtual j_result_t SetTime(j_time_t sysTime);
	virtual j_result_t GetDeviceInfo();
	virtual j_result_t EnableRcdInfo(CRingBuffer *pRingBuffer, j_boolean_t bEnable = true);
	virtual j_result_t EnableAlarm(CRingBuffer *pRingBuffer, j_boolean_t bEnable = true);
	virtual j_result_t ParserRequest(J_AsioDataBase *pAsioData);
	virtual j_result_t SendMessage(j_char_t *pData, j_int32_t nLen);
	/// �ͻ��˹���
	virtual j_result_t AddClient(J_Client *pClient)
	{
		TLock(m_vecClientLocker);
		m_vecClient.push_back(pClient);
		TUnlock(m_vecClientLocker);

		return J_OK;
	}
	virtual j_result_t DelClient(J_Client *pClient)
	{
		TLock(m_vecClientLocker);
		std::vector<J_Client *>::iterator it = m_vecClient.begin();
		for (; it != m_vecClient.end(); ++it)
		{
			if (*it == pClient)
			{
				m_vecClient.erase(it);
				break;
			}
		}
		TUnlock(m_vecClientLocker);

		return J_OK;
	}
	j_result_t ReConnect(j_int32_t nChannel) 
	{
		return OnRealPlay(nChannel);
	}
	/// �ļ��ϴ�
	virtual j_result_t OnStartUpload(j_int32_t nUserId, j_int32_t nFileId, j_string_t pFileName);
	virtual j_result_t OnUploading(j_int32_t nFileId, j_char_t *pData, j_int32_t nLen);
	virtual j_result_t OnStopUpload(j_int32_t nFileId, j_char_t *pMD5);

private:
	/// �ͻ��˴�������Ϣ
	j_result_t ProcessClientCmd(J_AsioDataBase *pAsioData);
	/// �����豸�����紫������Ϣ
	j_result_t ProcessDeviceCmd(J_AsioDataBase *pAsioData);

	/// ע����Ϣ
	j_result_t OnRegister(J_AsioDataBase *pAsioData);
	/// �����ı���Ϣ
	j_result_t OnSendMessage(J_AsioDataBase *pAsioData);
	/// ������Ϣ
	j_result_t OnHeartBeat(J_AsioDataBase *pAsioData);
	/// ������Ϣ(����GPS)
	j_result_t OnAlarmInfo(J_AsioDataBase *pAsioData);
	/// ʵʱ��Ƶ����
	j_result_t OnRealPlayData(J_AsioDataBase *pAsioData,  j_int32_t nDadaLen);
	/// ֹͣʵʱ��Ƶ��������
	j_result_t OnRealStopData(J_AsioDataBase *pAsioData);
	/// ��ʷ��Ƶ����
	j_result_t OnVodPlayData(J_AsioDataBase *pAsioData,  j_int32_t nDadaLen);
	/// ֹͣ��ʷ��Ƶ��������
	j_result_t OnVodStopData(J_AsioDataBase *pAsioData);
	/// ϵͳ��Ϣ
	j_result_t OnMsgInfo(J_AsioDataBase *pAsioData);
	/// ��ѯ��ʷ¼�����䷴������
	j_result_t OnRcdInfoData(J_AsioDataBase *pAsioData);
	/// ����ʱ�ӵķ�����Ϣ
	j_result_t OnSetSysTime(J_AsioDataBase *pAsioData);
	/// ��ȡ�豸��Ϣ�ķ�����Ϣ
	j_result_t OnGetDevInfo(J_AsioDataBase *pAsioData);
	/// ��ȡ��־�ķ�����Ϣ
	j_result_t OnGetLogInfo(J_AsioDataBase *pAsioData);
	/// δ������Ϣ����
	j_result_t OnDefault(J_AsioDataBase *pAsioData);
	//////////////////////////////////////////////////////////////////////////////////////
	/// �ͻ��˷���������ʵʱ��Ƶ
	j_result_t OnRealPlay(j_int32_t nChannel);
	/// �ͻ��˷�����ֹͣʵʱ��Ƶ
	j_result_t OnRealStop(j_int32_t nChannel);
	/// �ͻ��˷�����������ʷ��Ƶ
	j_result_t OnVodPlay(GUID sessionId, j_int32_t nChannel, j_time_t startTime, j_time_t endTime);
	/// �ͻ��˷�����ֹͣʵʱ��Ƶ
	j_result_t OnVodStop(GUID sessionId, j_int32_t nChannel);
	/// �ͻ��˷����Ĳ�ѯ��ʷ¼������
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
	VecFileUploadInfo m_vecFileUploadInfo;
	J_OS::TLocker_t m_fileUploadLocker;
	j_char_t *m_readBuff;
	j_char_t *m_writeBuff;
	j_char_t *m_rcdBuffer;
	j_char_t *m_fileBuffer;
	j_int32_t m_ioState;				//1-��ͷ����,2-��������+β����
	J_OS::TLocker_t m_vecLocker;
	std::vector<CRingBuffer *> m_vecRingBuffer;
	J_OS::TLocker_t m_vecRcdLocker;
	std::vector<CRingBuffer *> m_vecRcdRingBuffer;

	J_OS::TLocker_t m_vecClientLocker;
	std::vector<J_Client *> m_vecClient;
};

#endif