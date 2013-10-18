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

/// 命令状态
enum CmdState
{
	xl_init_state = 0,
	xl_read_head_state,
	xl_read_data_state,
	xl_write_body_state,
};

/// 操作指令定义  
enum CmdType
{
	xl_register = 0,					///< DVR主机注册
	xl_set_time,						///< 服务器校时
	xl_get_loginfo,					///< 获取DVR开关机日志
	xl_heartbeat = 4,				///< 心跳检测
	xl_get_alraminfo,				///< 报警获取
	xl_get_devinfo,					///< 设备信息获取
	xl_real_play,						///< 开始实时视频播放
	xl_real_stop,						///< 停止实时视频播放
	xl_vod_play,						///< 开始录像回放
	xl_vod_stop,						///< 停止录像回放
	xl_start_alarm_download,	///< 开始报警记录下载
	xl_stop_alarm_download,	///< 停止报警记录下载
	xl_errcode_download,		///< 系统异常下载
	xl_login,							///< 登录
	xl_logout,							///< 注销
};

#pragma pack(push)
#pragma pack(1)
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

} RegisterReq, *LPRegisterReq;

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
	BOOL bAlarm;								///< 报警信息
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

/// 实时播放请求 
typedef struct _realPlayReq
{
	char hostId[32];						///< 设备ID
	unsigned char channel;			///< 通道号				
} RealPlayReq, *LPRealPlayReq;

/// 实时播放回复  
typedef struct _realPlayResp
{
	char ret;								///< 0-成功, 1-失败
} RealPlayResp, *LPRealPlayResp;

/// 停止实时播放请求 
typedef _realPlayReq RealStopReq, *LPRealPlayReq;

/// 停止实时播放回复  
typedef _realPlayResp RealStopResp, *LPRealStopResp;

/// 历史回放请求  
typedef struct _vodPlayReq
{
	char hostId[32];						///< 设备ID
	unsigned char channel;			///< 通道号	
	time_t tmStartTime;				///< 开始时间
	time_t tmEndTime;				///< 结束时间
} VodPlayReq, LPVodPlayReq;

/// 历史回放回复  
typedef struct _vodPlayResp
{

} VodPlayResp, *LPVodPlayResp;

/// 停止历史回放请求  
typedef _vodPlayReq VodStopReq, *LPVodStopReq;

/// 停止历史回放回复  
typedef _vodPlayResp VodStopResp, *LPVodStopResp;

/// 开始报警数据下载请求  
typedef struct _alarmDownLoadReq
{
	time_t tmStart;						///< 开始时间
	time_t tmEnd;						///< 结束时间
} AlarmDownLoadReq, *LPAlarmDownLoadReq;

/// 开始报警数据下载回复  
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

/// 登录请求  
typedef struct _loginReq
{
	char user[16];			///< 用户名
	char passWord[32];	///< 密码
} LoginReq, *LPLoginReq;

/// 登录回复  
typedef struct _loginResp
{
	char ret;					///< 0-成功,1-失败
} LoginResp, *LPLoginResp;

/// 注销请求  
typedef _loginReq LogoutReq, *LPLogoutReq;

/// 注销回复  
typedef _loginResp LogoutResp, *LPLogoutResp;

#pragma pack(pop)

/*typedef struct _tagBlockHeader
{
	unsigned char szID[32];
	unsigned int uChannelID;
	USERDATAINFO DataInfo;
	VIDEOINFO VideoInfo;
	time_t tmTimeStamp;
} BlockHeader, *LPBlockHeader;*/

#endif//~__XLTYPE_H_