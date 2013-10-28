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
#include "XlType.h"
/// ����Ĺ���:  �ͻ���ҵ������
class CXlClient : public J_Client
{
	struct VideoInfo
	{
		j_int32_t nChanId;
		j_string_t strHost;
	};
	typedef std::vector<VideoInfo> VideoInfoVec;
	typedef std::vector<j_string_t> AlarmInfoVec;
public:
	CXlClient(j_socket_t nSock);
	~CXlClient();

public:
	///J_Client
	j_result_t ParserRequest(J_AsioDataBase *pAsioData);
	j_result_t Broken();

private:
	/// ���ýӿ�
	j_result_t MakeRequest(j_char_t bCmd, j_char_t *pData, j_int32_t nLen, j_char_t *pBody);
	j_result_t MakeResponse(j_char_t bCmd, j_char_t *pData, j_int32_t nLen, j_char_t *pBody);
	j_uint32_t CheckNum(j_char_t *pData, j_int32_t nLen);
	/// �������ݽӿ�
	j_result_t MakeNetData(J_AsioDataBase *pAsioData, j_char_t *pDataBuff, j_int32_t nLen);
	j_result_t MakeTransData(J_AsioDataBase *pAsioData);
	j_result_t MakeLogData(J_AsioDataBase *pAsioData);
	j_result_t MakeAlarmData(J_AsioDataBase *pAsioData);
	j_result_t MakeDvrListData(J_AsioDataBase *pAsioData);
	/// �������ݽӿ�
	j_result_t ProcessRequest(J_AsioDataBase *pAsioData);
	j_result_t ProcessData(J_AsioDataBase *pAsioData);
	j_result_t ProcessLog(J_AsioDataBase *pAsioData);
	j_result_t ProcessAlarm(J_AsioDataBase *pAsioData);
	j_result_t ProcessDvrList(J_AsioDataBase *pAsioData);
	/// �豸��ؽӿ�
	j_result_t StartView(j_string_t strHostId, j_int32_t nChanId, J_AsioDataBase *pAsioData);
	j_result_t StopView(j_string_t strHostId, j_int32_t nChanId, J_AsioDataBase *pAsioData);
	j_result_t StartAlarm(j_string_t strHostId, j_time_t tmStart, j_time_t tmEnd);
	j_result_t StopAlarm(j_string_t strHostId);
	j_result_t StopAll();
	j_result_t OnLogin(J_AsioDataBase *pAsioData);
	j_result_t OnLogout(J_AsioDataBase *pAsioData);
	j_result_t OnRealPlay(J_AsioDataBase *pAsioData);
	j_result_t OnRealStop(J_AsioDataBase *pAsioData);
	j_result_t OnVodPlay(J_AsioDataBase *pAsioData);
	j_result_t OnVodStop(J_AsioDataBase *pAsioData);
	j_result_t OnSetTime(J_AsioDataBase *pAsioData);
	j_result_t OnGetDevInfo(J_AsioDataBase *pAsioData);
	j_result_t OnGetLogInfo(J_AsioDataBase *pAsioData);
	j_result_t OnStartAlarm(J_AsioDataBase *pAsioData);
	j_result_t OnStopAlarm(J_AsioDataBase *pAsioData);
	/// ���ù�����ؽӿ�
	j_result_t OnGetTotleDvrInfo(J_AsioDataBase *pAsioData);
	j_result_t OnGetTotleUserInfo(J_AsioDataBase *pAsioData);
	j_result_t OnGetDvrList(J_AsioDataBase *pAsioData);
private:
	//J_OS::CTCPSocket m_socket;						//�ͻ�������
	j_char_t *m_readBuff;								//�������󻺴���
	j_char_t *m_writeBuff;								//����ͻ�����
	j_char_t *m_dataBuff;								//��Ƶ���ͻ�����
	j_char_t *m_alarmBuff;								//�������ͻ�����
	j_char_t *m_logBuff;									//��־���ͻ����� 
	j_char_t *m_dvrBuff;									//�豸��Ϣ���ͻ�����
	LogInfoQueue m_logInfoQueue;				//��ʷ��־�������
	AlarmInfoQueue m_alarmInfoQueue;		//��ʷ�����������
	DvrInfoQueue m_dvrInfoQueue;				//�豸��Ϣ�������
	j_int32_t m_ioCmdState;							//��������״̬
	j_int32_t m_ioDataState;							//��Ƶ����״̬
	j_int32_t m_ioAlarmState;							//��������״̬
	j_int32_t m_ioLogState;								//��־����״̬
	j_int32_t m_ioDvrListState;						//�豸��Ϣ����״̬
	CRingBuffer m_ringBuffer;						//��Ƶ������
	J_StreamHeader m_streamHeader;			//��Ƶ����ͷ��Ϣ
	CRingBuffer m_alarmBuffer;						//����������
	J_StreamHeader m_alarmHeader;				//��������ͷ��Ϣ
	j_long_t m_nRefCnt;									//��Ƶ�����ü���
	j_long_t m_nAlarmRefCnt;							//���������ü���
	J_OS::CTLock m_vecLocker;						//��Ƶ������
	VideoInfoVec m_videoInfoVec;					//��Ƶ������Ϣ
	J_OS::CTLock m_vecAlarmLocker;				//����������
	AlarmInfoVec m_alarmInfoVec;					//����������Ϣ
};
#endif // ~__CLIENT_H_
