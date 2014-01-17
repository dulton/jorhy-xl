#ifndef __J_VIDEOADAPTER_H_
#define __J_VIDEOADAPTER_H_

#include "j_obj.h"
#include "j_type.h"
#include "x_ringbuffer.h"
#include "x_log.h"
#include "x_errtype.h"
#include "x_lock.h"

struct J_AsioUser : virtual public J_Obj
{
	///连接事件完成(用于网络IO)
	///@param[in]	asioData IO数据集
	///@param[in]	错误码,见x_error_type.h	
	virtual j_result_t OnAccept(J_AsioDataBase *pAsioData, j_result_t nRet) = 0;

	///读事件完成
	///@param[in]	asioData IO数据集
	///@param[in]	错误码,见x_error_type.h
	virtual j_result_t OnRead(J_AsioDataBase *pAsioData, j_result_t nRet) = 0;

	///写事件完成
	///@param[in]	asioData IO数据集
	///@param[in]	错误码,见x_error_type.h
	virtual j_result_t OnWrite(J_AsioDataBase *pAsioData, j_result_t nRet) = 0;

	///断线事件完成(用于网络IO)
	///@param[in]	错误码,见x_error_type.h
	virtual j_result_t OnBroken(J_AsioDataBase *pAsioData, j_result_t nRet) = 0;
};

struct J_DbAccess : virtual public J_Obj
{
	///连接数据库
	///@param[in]		pAddr IP地址
	///@param[in]		nPort 端口号
	///@param[in]		pUa 用户名
	///@param[in]		pPwd 密码
	///@return			参见x_errtype.h
	virtual j_result_t Connect(const j_char_t *pAddr, j_int16_t nPort, const j_char_t *pUa, const j_char_t *pPwd) = 0;
	///初始化数据库
	///@return			参见x_errtype.h
	virtual j_result_t Init() = 0;
	///释放数据库连接
	///@return			参见x_errtype.h
	virtual j_result_t Release() = 0;
};

struct J_Channel : virtual public J_Obj 
{
	///打开实时视频
	///@param[in]		pRingBuffer 流队列对象
	///@return			参见x_errtype.h
	virtual j_result_t OpenStream(CRingBuffer *pRingBuffer) = 0;
	///关闭实时视频
	///@param[in]		pRingBuffer 流队列对象
	///@return			参见x_errtype.h
	virtual j_result_t CloseStream(CRingBuffer *pRingBuffer) = 0;
};

struct J_Vod : virtual public J_Obj 
{
	///打开历史视频
	///@param[in]		sessionId 回话ID
	///@param[in]		pRingBuffer 流队列对象
	///@return			参见x_errtype.h
	virtual j_result_t OpenVod(const j_guid_t &sessionId, CRingBuffer *pRingBuffer) = 0;
	///关闭历史视频
	///@param[in]		sessionId 回话ID
	///@param[in]		pRingBuffer 流队列对象
	///@return			参见x_errtype.h
	virtual j_result_t CloseVod(const j_guid_t &sessionId, CRingBuffer *pRingBuffer) = 0;
};

struct J_Host : virtual public J_Obj 
{
	///获取通道对象
	///@param[in]		nChannelNum 通道号 
	///@param[out]	pObj 通道对象
	///@return			参见x_errtype.h
	virtual j_result_t MakeChannel(const j_int32_t nChannelNum, J_Obj *&pObj) = 0;
	///判断设备是否就绪
	///@return			true-就绪,false-未就绪
	virtual j_boolean_t IsReady() = 0;
	///获取设备ID
	///@param[out]	strDevId 设备ID
	///@return			参见x_errtype.h
	virtual j_result_t GetHostId(j_string_t &strDevId) = 0;
	///设备校时
	///@param[in]		sysTime 系统时间
	///@return			参见x_errtype.h
	virtual j_result_t SetTime(j_time_t sysTime) = 0;
	///获取设备信息
	///@return			参见x_errtype.h
	virtual j_result_t GetDeviceInfo() = 0;
	///接收录像信息
	///@param[in]		pRingBuffer 流队列对象
	///@param[in]		bEnable true-启动,false-停止
	///@return			参见x_errtype.h
	virtual j_result_t EnableRcdInfo(CRingBuffer *pRingBuffer, j_boolean_t bEnable = true) = 0;
	///启动报警
	///@param[in]		pRingBuffer 流队列对象
	///@param[in]		bEnable true-启动,false-停止
	///@return			参见x_errtype.h
	virtual j_result_t EnableAlarm(CRingBuffer *pRingBuffer, j_boolean_t bEnable = true) = 0;
	///用户请求及回复
	///@param[in][out]	pAsioData异步数据	
	///@return				参见x_errtype.h
	virtual j_result_t ParserRequest(J_AsioDataBase *pAsioData) = 0;
	///发送消息
	///@param[in]			pData 数据
	///@param[in]			nLen 数据长度
	///@return				参见x_errtype.h
	virtual j_result_t SendMessage(j_char_t *pData, j_int32_t nLen) = 0;
};

struct J_Client : virtual public J_Obj 
{
	/// 发送消息
	/// @param[in]			strHostId 设备ID
	/// @param[in]			nType 消息类型
	/// @param[in]			nNo 消息编号
	/// @param[in]			nChannel 通道号
	virtual j_result_t SendMsgInfo(j_string_t strHostId, j_int32_t nType, j_int32_t nNo, j_int32_t nChannel = 0) = 0;
	/// 用户请求及回复
	/// @param[in][out]	pAsioData异步数据	
	/// @return				参见j_errtype.h 
	virtual j_result_t ParserRequest(J_AsioDataBase *pAsioData) = 0;
	/// 用户断线
	/// @return				参见j_errtype.h 
	virtual j_result_t Broken() = 0;
	/// 获取状态
	/// @return				客户端状态 
	virtual j_int32_t GetState() = 0;
};

#endif //~__J_VIDEOADAPTER_H_
