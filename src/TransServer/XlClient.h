///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx科技有限公司(版权声明) 
/// All rights reserved. 
/// 
/// @file      Client.h 
/// @brief     客户端模块
///
/// @version   1.0 (版本声明)
/// @author    Jorhy (joorhy@gmail.com) 
/// @date      2013/09/24 15:11 
///
///
/// 修订说明：最初版本
///////////////////////////////////////////////////////////////////////////  
#ifndef __CLIENT_H_
#define __CLIENT_H_

#include "j_includes.h"
#include "x_socket.h"
#include "XlDType.h"
#include "XlCType.h"
#include "XlConfig.h"
/// 本类的功能:  客户端业务处理类
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

	///j_string_t optHostId;								操作的设备
	///j_int32_t  optCommand;						操作的指令
	///j_time_t   optTime;								操作的时间
	typedef std::map<CliOptKey, j_time_t> OptionMap;
public:
	CXlClient(j_socket_t nSock);
	~CXlClient();

public:
	///J_Client
	j_result_t SendMessage(j_string_t strHostId, j_int32_t nType, j_int32_t nNo);
	j_result_t ParserRequest(J_AsioDataBase *pAsioData);
	j_result_t Broken();
	j_int32_t GetState();

private:
	/// 生成数据接口
	j_result_t MakeTransData(J_AsioDataBase *pAsioData);
	j_result_t MakeAlarmData(J_AsioDataBase *pAsioData);
	j_result_t MakeRcdData(J_AsioDataBase *pAsioData);
	/// 分析数据接口
	j_result_t ProcessRequest(J_AsioDataBase *pAsioData);
	j_result_t ProcessData(J_AsioDataBase *pAsioData);
	j_result_t ProcessAlarm(J_AsioDataBase *pAsioData);
	j_result_t ProcessRcd(J_AsioDataBase *pAsioData);
	j_result_t ProcessConfig(J_AsioDataBase *pAsioData);

	/// 设备相关接口
	j_result_t StartView(const CliRealPlay &realPlay, J_AsioDataBase *pAsioData);
	j_result_t StopView(const CliRealPlay &realPlay, J_AsioDataBase *pAsioData);
	j_result_t StartVod(const CliStartVod &startVod, J_AsioDataBase *pAsioData);
	j_result_t StopVod(const CliStopVod &stopVod, J_AsioDataBase *pAsioData);
	j_result_t StartAlarm(j_string_t strHostId);
	j_result_t StopAlarm(j_string_t strHostId);
	j_result_t StopAll();
	j_result_t StopAllReal();
	j_result_t StopAllVod();
	j_result_t StopAllAlarm();
	/// 请求处理函数
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
	j_result_t OnSendMsg(J_AsioDataBase *pAsioData);
	j_result_t OnGetRctInfo(J_AsioDataBase *pAsioData);

private:
	j_char_t m_userName[32];							//用户名
	J_OS::CTCPSocket m_socket;						//客户端连接
	j_char_t *m_readBuff;								//命令请求缓存区
	j_char_t *m_writeBuff;								//命令发送缓存区
	j_char_t *m_dataBuff;								//视频发送缓存区
	j_char_t *m_alarmBuff;								//报警发送缓存区
	j_char_t *m_rcdBuff;									//录像信息发送缓存区

	j_int32_t m_ioCmdState;							//命令请求状态
	j_int32_t m_ioDataState;							//视频发送状态
	j_int32_t m_ioAlarmState;							//报警发送状态
	j_int32_t m_ioRcdState;								//报警发送状态
	CRingBuffer m_ringBuffer;						//视频流队列
	J_StreamHeader m_streamHeader;			//视频队列头信息
	CRingBuffer m_alarmBuffer;						//报警流队列
	J_StreamHeader m_alarmHeader;				//报警队列头信息
	CRingBuffer m_rcdBuffer;							//报警流队列
	J_StreamHeader m_rcdHeader;					//报警队列头信息
	j_long_t m_nRefCnt;									//视频流引用计数
	j_long_t m_nAlarmRefCnt;							//报警流引用计数
	J_OS::CTLock m_vecLocker;						//实时视频访问锁
	VideoInfoVec m_videoInfoVec;					//实时视频访问信息
	J_OS::CTLock m_vodLocker;						//历史视频访问锁
	VodInfoVec m_vodInfoVec;						//历史视频访问信息
	J_OS::CTLock m_vecAlarmLocker;				//报警访问锁
	AlarmInfoVec m_alarmInfoVec;					//报警访问信息
	j_int32_t m_state;										//客户端状态
	j_int32_t m_lastBreatTime;
	CXlConfig m_config;
	///操作集合
	OptionMap m_optionMap;
};
#endif // ~__CLIENT_H_
