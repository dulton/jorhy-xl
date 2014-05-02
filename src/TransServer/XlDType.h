///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx科技有限公司(版权声明) 
/// All rights reserved. 
/// 
/// @file      XlType.h 
/// @brief     XL设备结构定义模块
///
/// @version   1.0 (版本声明)
/// @author    Jorhy (joorhy@gmail.com) 
/// @date      2013/09/23 10:50 
///
///
/// 修订说明：最初版本
///////////////////////////////////////////////////////////////////////////  
#ifndef __XLDTYPE_H_
#define __XLDTYPE_H_
#include "j_includes.h"

/// 操作指令定义  
enum DvrCmdType
{
	xld_register = 0,					///< DVR主机注册
	xld_set_time,							///< 服务器校时
	xld_get_loginfo,					///< 获取DVR开关机日志
	xld_send_message,				///< 发送消息
	xld_heartbeat = 4,				///< 心跳检测
	xld_get_alraminfo,				///< 报警获取
	xld_get_devinfo,					///< 设备信息获取
	xld_real_play,						///< 开始实时视频播放
	xld_real_stop,						///< 停止实时视频播放
	xld_vod_play,						///< 开始录像回放
	xld_vod_stop,						///< 停止录像回放
	xld_msg_info,						///< 系统异常下载
	xld_get_rcd_info,					///< 获取录像信息
	xld_start_upload,					///< 开始文件上传
	xld_uploading,						///< 上传文件
	xld_stop_upload,					///< 结束文件上传
};

/// 消息类型定义
enum DvrMsgType
{
	xld_alarm_complete = 0x00,		///< 报警下载完成
	xld_log_complete,						///< 日志下载完成
	xld_playvod_complete,				///< 回放完成
	xld_disk_full,									///< 录像已满
	xld_playreal_complete,				///< 实时播放完成
	xld_rcd_info_complete,				///< 获取录像区间完成
	xld_upload_success	 = 0x07,		///< 传送文件成功
	xld_upload_failed	,						///< 传送文件失败
};

#pragma pack(push)
#pragma pack(1)

/// 指令头结构  
typedef struct _cmdHeader
{
	char beginCode;			///< 开始标志-0xFF
	char version;					///< 版本号
	char flag;						///< 状态码 0x00-请求,0xFF-应答
	unsigned char cmd;		///< 控制码,见CmdType定义
	int length;						///< 数据长度,不包括头数据和尾数据				
} CmdHeader, *LPCmdHeader;

/// 指令尾结构  
typedef struct _cmdTail
{
	char verify;					///< 校验码
	char endCode;				///< 结束标志-0xFE
} CmdTail, *LPCmdTail;
////////////////////////////////////////////////////////////////////////
/////                                   设备端                                      /////
////////////////////////////////////////////////////////////////////////

///设备信息
typedef struct _tagDevHostInfo
{
	char hostId[32];			///< 设备ID
	char vehicleNum[16];		///< 列车号
	char phoneNum[12];			///< 电话号码
	char totalChannels;			///< 通道数目
	//bool bOnline;				///< 在线状态
	//bool bState;				///< 设备状态
} DevHostInfo, *PLDevHostInfo;

/// 报警信息
typedef struct _tagDevAlarmInfo
{
	time_t tmTimeStamp;				///< 时间戳
	char bAlarm;								///< 报警信息
	double dLatitude;
	double dLongitude;
	double dGPSSpeed;
} DevAlarmInfo, *LPDevAlarmInfo;
typedef std::queue<DevAlarmInfo> DevAlarmInfoQueue;

/// 日志信息  
typedef struct _tagDevLogInfo
{
	char szID[32];				///< 车号
	time_t tmTime;				///< 开关机时间
	char bStatus;				///< 设备状态,0-开机,1-关机
} DevLogInfo, *LPDevLogInfo;
typedef std::queue<DevLogInfo> DevLogInfoQueue;

/// 回复命令码
typedef struct _tagDevRetVal
{
	char nRetVal;						///< 回复码 0-成功,1-失败
} DevRetVal, *LPDevRetVal;

/// 设备ID
typedef struct _tagDevEquipmentId
{
	char hostId[32];			   ///< 主机ID
} DevEquipmentId, *LPDevEquipmentId;

/// 文件信息
typedef struct _tagDevFileInfo
{
	char fileName[512];			  ///< 文件名
} DevFileInfo, *LPDevFileInfo;

/// 时间设置 
typedef struct _tagDevSetTime
{
	time_t systime;				///< 当前系统时间
} DevSetTime, *LPDevSetTime;

/// 获取日志信息
typedef struct _tagDevGetLogInfo
{
	char	  szID[32];		///< 车号
	time_t tmStart;		///< 开始时间
	time_t tmEnd;		///< 结束时间
} DevGetLogInfo, *LPDevGetLogInfo;

/// 实时播放
typedef struct _tagDevRealPlay
{
	char hostId[32];				///< 设备ID
	unsigned char channel;			///< 通道号
} DevRealPlay, *LPDevRealPlay;

/// 开始历史回放  
typedef struct _tagDevStartVod
{
	GUID sessionId;					///< 回话ID
	char hostId[32];						///< 设备ID
	unsigned char channel;			///< 通道号	
	time_t tmStartTime;				///< 开始时间
	time_t tmEndTime;				///< 结束时间
} DevStartVod, *LPDevStartVod;

/// 停止历史回放  
typedef struct _tagDevStopVod 
{
	GUID sessionId;					///< 回话ID
	char hostId[32];						///< 设备ID
	char channel;					///< 通道号	
} DevStopVod, *LPDevStopVod;

/// 消息
typedef struct _tagMsgInfo
{
	char hostId[32];								///< 车号
	unsigned char channel;					///< 通道号
	unsigned int nMsgCode;				///< 消息码
	unsigned char bReserve[16];			///< 会话ID
	//unsigned char bReserve[2048];	///< 系统消息
} DevMsgInfo, *LPDevMsgInfo;

/// 获取历史视频信息
typedef struct _tagDevRcdInfo
{
	char szID[32];						///< 车辆ID
	time_t tmRecIntervalStartPt;		///< 录像区间开始时间
	time_t tmRecIntervalEndPt;			///< 录像区间结束时间
} DevRcdInfo, *LPDevRcdInfo;

/// 开始文件上传
typedef struct _tagDevUploadStart
{ 
	int nUserId;									///< 用户ID
	int nFileId;										///< 文件ID
	char szID[32];			 					///< 设备ID
	char szFileName[512];					///< 文件名称
} DevUploadStart, *LPDevUploadStart;

/// 停止文件上传
typedef struct _tagDevUploadStop
{ 
	int nFileId;								///< 文件ID
	char szCheck[16];					///< MD5码校验
} DevUploadStop, *LPDevUploadStop;

#pragma pack(pop)

#endif//~__XLDTYPE_H_