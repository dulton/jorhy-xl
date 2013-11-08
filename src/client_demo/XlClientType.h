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
	xlc_message = 0x51,				///< 消息
	xlc_login,								///< 用户登录
	xlc_logout,							///< 注销登录
	xlc_start_real_alarm_info,		///< 开启获取实时报警信息
	xlc_stop_real_alarm_info,		///< 停止获取实时报警信息
	xlc_start_real_view,				///< 开启实时视频浏览
	xlc_stop_real_view,				///< 停止实时视频浏览
	xlc_start_vod_view,				///< 开始录像回放（下载）
	xlc_stop_vod_view,				///< 停止录像回放（下载）
	xlc_heart_beat,						///< 客户端在线心跳
	///////////////////////////////////////////////////////////////////
	xlc_get_dvr_summary,			///< 获取DVR总数信息
	xlc_get_dvr_list,						///< 获取DVR开关机日志
	xlc_get_dvr_info,					///< 获取单个DVR信息
	xlc_add_dvr_info,					///< 增加DVR （不使用，但预留接口）
	xlc_mod_dvr_info,					///< 修改DVR的信息
	xlc_del_dvr_info,					///< 删除DVR
	xlc_get_user_summary,			///< 获取用户总数信息
	xlc_get_user_list,					///< 获取用户信息列表
	xlc_get_user_info,					///< 获取单个用户信息
	xlc_add_user_info,					///< 增加用户的信息
	xlc_mod_user_info,				///< 修改用户的信息
	xlc_del_user_info,					///< 删除用户
	xlc_get_department_list,		///< 获取单位信息列表
	xlc_get_department_info,		///< 获取单个单位信息
	xlc_add_department_info,		///< 增加单位的信息
	xlc_mod_department_info,	///< 修改单位的信息
	xlc_del_department_info,		///< 删除单位
	xlc_get_dvr_state,					///< 获取DVR在离线信息	
	xlc_get_alarm_info,				///< 开始报警记录下载
	xlc_xxxxxxxxxxxxxx,				///< 停止报警记录下载
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

//#pragma pack(4)
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
	long lDepartmentID;  	///< 所属单位ID
	int	nType;  				///< 请求类型
} DvrListReq, *LPDvrListReq;

/// DVR信息
typedef struct _tagDvrInfo
{
	char szEquipmentID[32];  		///< 设备ID
	char szVehicleNO[16];			///< 车号
	char szPhone[16];					///< 电话号码
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
	long lDepartmentID;				///< 单位ID
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
} AlarmInfoReq, *LPAlarmInfoReq;

/// 报警信息回复  
typedef struct _alarmInfoResp
{
	char hostId[32];								///< 设备信息
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
typedef struct _realViewReq
{
	char hostId[32];						///< 设备ID
	int channel;			///< 通道号				
} RealViewReq, *LPRealViewReqReq;

/// 实时播放回复  
typedef struct _realViewResp
{
	char ret;								///< 0-成功, 1-失败
} RealViewResp, *LPRealViewRespResp;

/// 实时视频消息体
struct RealViewBody 
{
	CmdHeader head;
	RealViewReq data;
	CmdTail tail;
};

/// 历史回放请求  
typedef struct _vodPlayReq
{
	GUID sessionID;					///< 回话ID
	char hostId[32];						///< 设备ID
	int channel;							///< 通道号	
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
	GUID sessionID;					///< 回话ID
	char hostId[32];						///< 设备ID
	int channel;							///< 通道号	
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
	char szAccountName[16];		//账户名
	char szPassword[16];  				//密码
	int  nForced;								//1 强制登录,0 非强制登录
} LoginReq, *LPLoginReq;

/// 登录回复  
typedef struct _loginResp
{
	int ret;					///< 1 登录成功
								///< 2 用户名错误
								///< 3 密码错误
								///< 4 用户服务器用户满
								///< 5 客户端版本过低或过高
								///< 6 用户已在其他位置进行登录
								///< 7 登录失败
} LoginResp, *LPLoginResp;

/// 注销回复  
typedef _loginResp LogoutResp, *LPLogoutResp;
///< 1 注销成功
///< 0 注销失败

/// 实时报警消息体
struct RealAlarmBody 
{
	CmdHeader head;
	DVRId data;
	CmdTail tail;
};

#pragma pack(pop)

#endif//~__XLTYPE_H_