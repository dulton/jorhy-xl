///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx�Ƽ����޹�˾(��Ȩ����) 
/// All rights reserved. 
/// 
/// @file      Client.h 
/// @brief     �ͻ���ģ��
///
/// @version   1.0 (�汾����)
/// @author    Jorhy (joorhy@gmail.com) 
/// @date      2013/09/24 15:11 
///
///
/// �޶�˵��������汾
///////////////////////////////////////////////////////////////////////////  
#ifndef __CLIENT_H_
#define __CLIENT_H_

#include "j_includes.h"
#include "x_socket.h"
#include "XlDType.h"
#include "XlCType.h"
#include "XlConfig.h"
/// ����Ĺ���:  �ͻ���ҵ������
class CXlClient : public J_Client
{
	struct VideoInfo
	{
		j_int32_t nChanId;
		j_string_t strHost;
	};
	typedef std::vector<VideoInfo> VideoInfoVec;
	struct VodInfo
	{
		GUID sessionId;
		j_int32_t nChanId;
		j_string_t strHost;
	};
	typedef std::vector<VodInfo> VodInfoVec;
	typedef std::vector<j_string_t> AlarmInfoVec;
public:
	CXlClient(j_socket_t nSock);
	~CXlClient();

public:
	///J_Client
	j_result_t ParserRequest(J_AsioDataBase *pAsioData);
	j_result_t Broken();

private:
	/// �������ݽӿ�
	j_result_t MakeTransData(J_AsioDataBase *pAsioData);
	j_result_t MakeAlarmData(J_AsioDataBase *pAsioData);
	/// �������ݽӿ�
	j_result_t ProcessRequest(J_AsioDataBase *pAsioData);
	j_result_t ProcessData(J_AsioDataBase *pAsioData);
	j_result_t ProcessAlarm(J_AsioDataBase *pAsioData);
	j_result_t ProcessConfig(J_AsioDataBase *pAsioData);

	/// �豸��ؽӿ�
	j_result_t StartView(const CliRealPlay &realPlay, J_AsioDataBase *pAsioData);
	j_result_t StopView(const CliRealPlay &realPlay, J_AsioDataBase *pAsioData);
	j_result_t StartVod(const CliStartVod &startVod, J_AsioDataBase *pAsioData);
	j_result_t StopVod(const CliStopVod &stopVod, J_AsioDataBase *pAsioData);
	j_result_t StartAlarm(j_string_t strHostId);
	j_result_t StopAlarm(j_string_t strHostId);
	j_result_t StopAll();
	/// ��������
	j_result_t OnLogin(J_AsioDataBase *pAsioData);
	j_result_t OnLogout(J_AsioDataBase *pAsioData);
	j_result_t OnHeartBeat(J_AsioDataBase *pAsioData);
	j_result_t OnRealPlay(J_AsioDataBase *pAsioData);
	j_result_t OnRealStop(J_AsioDataBase *pAsioData);
	j_result_t OnVodPlay(J_AsioDataBase *pAsioData);
	j_result_t OnVodStop(J_AsioDataBase *pAsioData);
	j_result_t OnSetTime(J_AsioDataBase *pAsioData);
	j_result_t OnStartAlarm(J_AsioDataBase *pAsioData);
	j_result_t OnStopAlarm(J_AsioDataBase *pAsioData);

private:
	//J_OS::CTCPSocket m_socket;						//�ͻ�������
	j_char_t *m_readBuff;								//�������󻺴���
	j_char_t *m_writeBuff;								//����ͻ�����
	j_char_t *m_dataBuff;								//��Ƶ���ͻ�����
	j_char_t *m_alarmBuff;								//�������ͻ�����

	j_int32_t m_ioCmdState;							//��������״̬
	j_int32_t m_ioDataState;							//��Ƶ����״̬
	j_int32_t m_ioAlarmState;							//��������״̬
	CRingBuffer m_ringBuffer;						//��Ƶ������
	J_StreamHeader m_streamHeader;			//��Ƶ����ͷ��Ϣ
	CRingBuffer m_alarmBuffer;						//����������
	J_StreamHeader m_alarmHeader;				//��������ͷ��Ϣ
	j_long_t m_nRefCnt;									//��Ƶ�����ü���
	j_long_t m_nAlarmRefCnt;							//���������ü���
	J_OS::CTLock m_vecLocker;						//ʵʱ��Ƶ������
	VideoInfoVec m_videoInfoVec;					//ʵʱ��Ƶ������Ϣ
	J_OS::CTLock m_vodLocker;						//��ʷ��Ƶ������
	VodInfoVec m_vodInfoVec;					//��ʷ��Ƶ������Ϣ
	J_OS::CTLock m_vecAlarmLocker;				//����������
	AlarmInfoVec m_alarmInfoVec;					//����������Ϣ
	CXlConfig m_config;
};
#endif // ~__CLIENT_H_
