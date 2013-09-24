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
#ifndef __XLTYPE_H_
#define __XLTYPE_H_
#include "j_includes.h"

/// 操作指令定义  
enum CmdType
{
	xl_register = 0,					///< DVR主机注册
	xl_set_time,						///< 服务器校时
	xl_get_loginfo,					///< 获取DVR开关机日志
	xl_heartbeat,						///< 心跳检测
	xl_get_alraminfo,				///< 报警获取
	xl_get_devinfo,					///< 设备信息获取
	xl_real_view,						///< 实时视频浏览
	xl_vod_view,						///< 录像回放
	xl_vod_download,				///< 录像下载
	xl_alarm_download,			///< 报警记录下载
	xl_errcode_download,		///< 系统异常下载
};

/// 指令头结构  
typedef struct _cmdHeader
{
	char beginCode;			///< 开始标志-0xFF
	char version;					///< 版本号
	char flag;						///< 状态码 0x00-请求,0xFF-应答
	char cmd;						///< 控制码,见CmdType定义
	int length;						///< 数据长度,不包括头数据和尾数据				
} CmdHeader, *LPCmdHeader;

/// 指令尾结构  
typedef struct _cmdTail
{
	char verify;					///< 校验码
	char endCode;				///< 结束标志-0xFE
} CmdTail, *LPCmdTail;

/// 注册请求  
typedef struct _registerReq
{

} RegisterReq, *LPRegisterResp;

/// 注册回复
typedef struct _registerResp
{
	char hostId[32];			   ///< 主机ID
} RegisterResp, *LPRegisterResp;

/// 时间设置请求  
typedef struct _setTimeReq
{
	time_t systime;				///< 当前系统时间
} SetTimeReq, *LPSetTimeReq;

/// 时间设置回复  
typedef struct _setTimeResp
{
	char ret;						///< 回复码 0-成功,1-失败
} SetTimeResp, *LPSetTimeResp;

/// 日志信息请求 
typedef struct _getLogInfoReq
{
	union
	{
		char all;					///< 数据为空
		struct 
		{
			time_t tmStart;		///< 开始时间
			time_t tmEnd;		///< 结束时间
		} info;
	};
} GetLogInfoReq, *LPGetLogInfoReq;

/// 日志信息回复  
typedef struct _getLogInfoResp
{
	char bStatus;				///< 设备状态,0-开机,1-关机
	time_t tmTime;				///< 开关机时间
} GetLogInfoResp, *LPGetLogInfoResp;

/// 心跳检测请求  
typedef struct _heartBeatReq
{

} HeartBeatReq, *LPHeartBeatReq;

/// 心跳检测回复  
typedef struct _heartBeatResp
{
	char state;					///< 状态 0-正常,1-异常
} HeartBeatResp, *LPHeartBeatResp;

/// 报警信息请求  
typedef struct _alarmInfoReq
{

} AlarmInfoReq, *LPAlarmInfoReq;

/// 报警信息回复  
typedef struct _alarmInfoResp
{
	unsigned char bAlarm;						///< 报警信息
	struct  
	{
		double dLatitude;
		double dLongitude;
		double dGPSSpeed;
	} gps;												///< GPS信息
	double	speed;									///< 车速
	time_t tmTimeStamp;						///< 时间戳
} AlarmInfoResp, *LPAlarmInfoResp;

/// 设备信息请求  
typedef struct _hostInfoReq
{

}HostInfoReq, *LPHostInfoReq;

/// 设备信息回复  
typedef struct _hostInfoResp
{
	char hostId[32];									///< 设备ID
	char vehicleNum[8];							///< 车辆号
	char phoneNum[11];							///< 手机号
	unsigned char bTotleChannels;			///< 未知
} HostInfoResp, *LPHostInfoResp;

/// 实时预览请求 
typedef struct _realViewReq
{
	char hostId[32];						///< 设备ID
	unsigned char channel;			///< 通道号				
} RealViewReq, *LPRealViewReq;

/// 实时预览回复  
typedef struct _realViewResp
{

} RealViewResp, *LPRealViewResp;

/// 历史回放请求  
typedef struct _vodReq
{
	char hostId[32];						///< 设备ID
	unsigned char channel;			///< 通道号	
	time_t tmStartTime;				///< 开始时间
} VodReq, LPVodReq;

/// 历史回放回复  
typedef struct _vodResp
{

} VodResp, *LPVodResp;

/// 录像下载请求  
typedef struct _vodDownLoadReq
{
	char hostId[32];						///< 设备ID
	unsigned char channel;			///< 通道号	
	time_t tmStartTime;				///< 开始时间
	time_t tmEndTime;				///< 结束时间
} VodDownLoadReq, *LPVodDownLoadReq;

/// 录像下载回复  
typedef struct _vodDownLoadResp
{

} VodDownLoadResp, *LPVodDownLoadResp;

/// 报警数据下载请求  
typedef struct _alarmDownLoadReq
{
	time_t tmStart;						///< 开始时间
	time_t tmEnd;						///< 结束时间
} AlarmDownLoadReq, *LPAlarmDownLoadReq;

/// 报警数据下载回复  
typedef struct _alarmDownLoadResp
{

} AlarmDownLoadResp, *LPAlarmDownLoadResp;

/// 系统异常请求  
typedef struct _sysErrorCodeReq
{

} SysErrorCodeReq, *LPSysErrorCodeReq;

/// 系统异常回复  
typedef struct _sysErrorCodeResp
{
	char ret;					///< 0-0xFF的异常信息码
} SysErrorCodeResp, *LPSysErrorCodeResp;

#endif//~__XLTYPE_H_