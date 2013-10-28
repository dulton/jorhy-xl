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
#include "time.h"

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
	xl_start_real_alarm,			///< 开始报警记录下载
	xl_stop_real_alarm,			///< 停止报警记录下载
	xl_errcode_download,		///< 系统异常下载
	xl_login,							///< 登录
	xl_logout,							///< 注销

	xl_get_totle_dvr_info = 0x52,			///< 获取DVR总数信息
	xl_get_dvr_list,								///< 获取DVR信息列表
	xl_get_dvr_info,								///< 获取DVR信息
	xl_modify_dvr_info,						///< 修改DVR信息
	xl_delete_dvr,								///< 删除DVR	
	xl_get_totle_user_info,					///< 获取用户总数信息
	xl_get_user_list,								///< 获取用户信息列表
	xl_get_user_info,							///< 获取用户信息
	xl_modify_user_info,						///< 修改用户信息
	xl_delete_user,								///< 删除用户
	xl_get_department_list,					///< 获取单位信息列表
	xl_get_department_info,				///< 获取单位信息
	xl_modify_department_info,			///< 修改单位信息
	xl_delete_departmen,					///< 删除单位
};

#pragma pack(push)
#pragma pack(1)

////////////////////////////////////////////////////////////////////////
/////                                   客户端                                      /////
////////////////////////////////////////////////////////////////////////
/// DVR总数信息
typedef struct _tagDvrTotleInfo
{
	unsigned int uiTotal;   				///< DVR的总数
	unsigned int uiUnRegister; 		///< 未确认DVR数量
	unsigned int uiOnline;				///< 在线DVR总数
	unsigned int uiStock;   				///< 已确认但未分配给单位的DVR数量
} DVRTotleInfo,  *LPDVRTotleInfo;

/// DVR列表信息请求
typedef struct _tagDvrListReq
{
	int	nType;  				///< 请求类型
	long lDepartmentID;  	///< 所属单位ID
} DvrListReq, *LPDvrListReq;

/// DVR信息
typedef struct _tagDvrInfo
{
	char szEquipmentID[32];  		///< 设备ID
	char szVehicleNO[32];			///< 车号
	char szPhone[11];		///< 电话号码
	int nTotalChannels;				///< 通道数量
	char szReserve[32];  				///< 预留
	long lDepartmentID;  			///< 所属单位ID
	int nOnline;							///< 在离线状态
	int nState;  							///< 状态
} DVRInfo,  *LPDVRInfo;

/// DVR设备ID
typedef struct _tagDvrId
{
	char szEquipmentID[32];  		///< 设备ID 
} DVRId, *LPDvrId;

/// User总数信息
typedef struct _tagUserTotleInfo
 {
	unsigned int uiTotal;   					///< 用户的总数
	unsigned int uiOnline;					///< 在线用户总数
 } UserTotleInfo,  *LPUserTotleInfo;

/// User信息
typedef struct _tagUserInfo
{
	long lUserID;   						///< 用户ID
	char	szName[50];					///< 用户姓名
	char szAccountName[16];		///< 账户名
	char szPassword[16];  			///< 密码
	long lDepartmentID;  			///< 所属单位ID
	int nState;  							///< 状态
} UserInfo,  *LPUserInfo;

/// 用户ID
typedef struct _tagUserId
{
	long lUserID;   						///< 用户ID
} UserId, *LPUserId;

/// 用户类型
typedef struct _tagUserType
{
	int nType;   							///< 用户查询类型
} UserType, *LPUserType;

/// 单位信息
typedef struct _tagDepartmentInfo
{
	long  lDepartmentID;  		///< 单位ID
	char  szName[50];				///< 单位名称
	long  ParentID;					///< 父级单位
	int  OrderIndex;  				///< 单位顺序
	int  nState;  						///< 状态
} DepartmentInfo,  *LPDepartmentInfo;

/// 单位ID
typedef struct _tagDepartmentId
{
	long  lDepartmentID;  		///< 单位ID
} DepartmentId, *LPDepartmentId;
////////////////////////////////////////////////////////////////////////
/////                                   设备端                                      /////
////////////////////////////////////////////////////////////////////////
///设备信息
typedef struct _devInfo
{
	char hostId[32];					///< 设备ID
	char vehicleNum[16];		///< 列车号
	char phoneNum[12];			///< 电话号码
	char totalChannels;			///< 通道数目
} DevInfo, *PLDevInfo;

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
	char hostId[32];		///< 设备ID
	time_t tmStart;		///< 开始时间
	time_t tmEnd;		///< 结束时间
} GetLogInfoReq, *LPGetLogInfoReq;

/// 日志信息回复  
typedef struct _getLogInfoResp
{
	__int64 bStatus;			///< 设备状态,0-开机,1-关机
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
	char hostId[32];				///< 设备ID
	time_t tmStart;				///< 开始时间
	time_t tmEnd;				///< 结束时间
} AlarmInfoReq, *LPAlarmInfoReq;

/// 报警信息回复  
typedef struct _alarmInfoResp
{
	__int64 bAlarm;								///< 报警信息
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
	char hostId[32];									///< 设备ID
}HostInfoReq, *LPHostInfoReq;

/// 设备信息回复  
typedef DevInfo HostInfoResp, *LPHostInfoResp;

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
	__int64 channel;					///< 通道号	
	time_t tmStartTime;				///< 开始时间
	time_t tmEndTime;				///< 结束时间
} VodPlayReq, LPVodPlayReq;

/// 历史回放回复  
typedef struct _vodPlayResp
{

} VodPlayResp, *LPVodPlayResp;

/// 停止历史回放请求  
typedef struct _vodStopReq 
{
	char hostId[32];						///< 设备ID
	__int64 channel;					///< 通道号	
} VodStopReq, *LPVodStopReq;

/// 停止历史回放回复  
typedef struct _vodStopResp 
{
	char ret;
} VodStopResp, *LPVodStopResp;

/// 开始报警数据下载请求  
typedef struct _alarmStopReq
{
	char hostId[32];				///< 设备ID
} AlarmStopReq, *LPAlarmStopReq;

/// 开始报警数据下载回复  
typedef struct _alarmStopResp
{
	char ret;
} AlarmStopResp, *LPAlarmStopResp;

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

#endif//~__XLTYPE_H_