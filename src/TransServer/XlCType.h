///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx科技有限公司(版权声明) 
/// All rights reserved. 
/// 
/// @file      XlCType.h 
/// @brief     客户端结构定义
///
/// @version   1.0 (版本声明)
/// @author    Jorhy (joorhy@gmail.com) 
/// @date      2013/11/01 09:04 
/// 修订说明：最初版本
///////////////////////////////////////////////////////////////////////////  
#ifndef __XLCTYPE_H_
#define __XLCTYPE_H_

/// 命令状态
enum CliCmdState
{
	xl_init_state = 0,
	xl_read_head_state,
	xl_read_data_state,
	xl_write_body_state,
};

/// 操作指令定义  
enum CliCmdType
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
	xlc_get_dvr_summary = 0x81,	///< 获取DVR总数信息
	xlc_get_dvr_list,							///< 获取DVR开关机日志
	xlc_get_dvr_info,						///< 获取单个DVR信息
	xlc_add_dvr_info,						///< 增加DVR （不使用，但预留接口）
	xlc_mod_dvr_info,						///< 修改DVR的信息
	xlc_del_dvr_info,						///< 删除DVR
	xlc_get_user_summary,				///< 获取用户总数信息
	xlc_get_user_list,						///< 获取用户信息列表
	xlc_get_user_info,						///< 获取单个用户信息
	xlc_add_user_info,						///< 增加用户的信息
	xlc_mod_user_info,					///< 修改用户的信息
	xlc_del_user_info,						///< 删除用户
	xlc_get_department_list,			///< 获取单位信息列表
	xlc_get_department_info,			///< 获取单个单位信息
	xlc_add_department_info,			///< 增加单位的信息
	xlc_mod_department_info,		///< 修改单位的信息
	xlc_del_department_info,			///< 删除单位
	xlc_get_alarm_info,
};

#pragma pack(push)
#pragma pack(1)
////////////////////////////////////////////////////////////////////////
/////                                   客户端                                      /////
////////////////////////////////////////////////////////////////////////
/// 用户登录
typedef struct _tagCliUserLogin
{
	char userName[16];		///<账户名
	char passWord[16];  		///< 密码
	int  nForced;					///<1 强制登录；0 非强制登录
} CliUserLogin,  *LPCliUserLogin;

///设备信息
typedef struct _tagCliDevInfo
{
	char hostId[32];					///< 设备ID
	char vehicleNum[16];		///< 列车号
	char phoneNum[12];			///< 电话号码
	char totalChannels;			///< 通道数目
} CliDevInfo, *PLCliDevInfo;

/// 报警信息
typedef struct _tagCliAlarmInfo
{
	char pHostId[32];							///< 设备ID
	__int64 bAlarm;								///< 报警信息
	struct  
	{
		double dLatitude;
		double dLongitude;
		double dGPSSpeed;
	} gps;												///< GPS信息
	double	speed;									///< 车速
	time_t tmTimeStamp;						///< 时间戳
} CliAlarmInfo, *LPCliAlarmInfo;
typedef std::queue<CliAlarmInfo> CliAlarmInfoQueue;

/// 日志信息  
typedef struct _tagCliLogInfo
{
	char pHostId[32];			///< 设备ID
	__int64 bStatus;			///< 设备状态,0-开机,1-关机
	time_t tmTime;				///< 开关机时间
} CliLogInfo, *LPCliLogInfo;
typedef std::queue<CliLogInfo> CliLogInfoQueue;

/// 设备操作回复命令码
typedef struct _tagCliRetValue
{
	char  pHostId[32];		///< 设备ID
	int nRetVal;				///< 回复码 0-成功,1-失败
} CliRetValue, *LPCliRetValue;

/// 配置操作回复命令码
typedef struct _tagCliRetValue2
{
	int nRetVal;						///< 回复码 0-成功,1-失败
} CliRetValue2, *LPCliRetValue2;

/// 实时播放停止回复
typedef struct _tagCliRealViewRetValue
{
	char  pHostId[32];		///< 设备ID
	int nChannelId;			///< 通道号 
	int nRetVal;				///< 回复码 0-成功,1-失败
} CliRealViewRetValue, *LPCliRealViewRetValue;

/// 历史播放停止回复
typedef struct _tagCliVodStopRetValue
{
	GUID sessionId;		///< 回话ID
	char  pHostId[32];		///< 设备ID
	int nChannelId;			///< 通道号 
	int nRetVal;				///< 回复码 0-成功,1-失败
} CliVodStopRetValue, *LPCliVodStopRetValue;

/// 设备ID
typedef struct _tagCliEquipmentId
{
	char hostId[32];			   ///< 主机ID
} CliEquipmentId, *LPCliEquipmentId;

/// 时间设置 
typedef struct _tagCliSetTime
{
	char pHostId[32];			///< 设备ID
	time_t systime;				///< 当前系统时间
} CliSetTime, *LPCliSetTime;

/// 获取日志信息
typedef struct _tagCliGetLogInfo
{
	char pHostId[32];	///< 设备ID
	time_t tmStart;		///< 开始时间
	time_t tmEnd;		///< 结束时间
} CliGetLogInfo, *LPCliGetLogInfo;

/// 实时报警
typedef struct _tagCliRealAlarmInfo
{
	char  hostId[32];		///< 设备ID
} CliRealAlarmInfo, *LPCliRealAlarmInfo;

/// 实时播放
typedef struct _tagCliRealPlay
{
	char hostId[32];						///< 设备ID
	unsigned char channel;			///< 通道号				
} CliRealPlay, *LPCliRealPlay;

/// 开始历史回放  
typedef struct _tagCliStartVod
{
	GUID sessionId;					///< 回话ID
	char hostId[32];						///< 设备ID
	int		channel;					///< 通道号	
	time_t tmStartTime;				///< 开始时间
	time_t tmEndTime;				///< 结束时间
} CliStartVod, *LPCliStartVod;

/// 停止历史回放  
typedef struct _tagCliStopVod 
{
	GUID sessionId;					///< 回话ID
	char hostId[32];						///< 设备ID
	int channel;							///< 通道号	
} CliStopVod, *LPCliStopVod;

/// 设备相关
/// DVR总数信息
typedef struct _tagCliHostTotleInfo
{
	unsigned int uiTotal;   				///< DVR的总数
	unsigned int uiUnRegister; 		///< 未注册DVR数量
	unsigned int uiOnline;				///< 在线DVR总数
	unsigned int uiStock;   				///< 已注册但未分配给用户的DVR数量
} CliHostTotleInfo,  *LPCliHostTotleInfo;

/// DVR信息
typedef struct _tagCliHostInfo
{
	char szEquipmentID[32];  		///< 设备ID
	char szVehicleNO[16];			///< 车号
	char szPhone[16];					///< 电话号码
	int nTotalChannels;				///< 通道数量
	char szReserve[32];  				///< 预留
	long lDepartmentID;  			///< 所属单位ID
	int nOnline;							///< 在离线状态
	int nState;  							///< 状态
} CliHostInfo,  *LPCliHostInfo;
typedef std::queue<CliHostInfo> CliHostInfoQueue;

/// DVR设备ID
typedef struct _tagCliHostId
{
	char szEquipmentID[32];  		///< 设备ID 
} CliHostId, *LPCliHostId;

/// DVR列表信息请求
typedef struct _tagCliGetHostList
{
	int	nType;  				///< 请求类型
	long lDepartmentID;  	///< 所属单位ID
} CliGetHostList, *LPCliGetHostList;

///用户相关
/// User总数信息
typedef struct _tagCliUserTotleInfo
{
	unsigned int uiTotal;   					///< 用户的总数
	unsigned int uiOnline;					///< 在线用户总数
} CliUserTotleInfo,  *LPCliUserTotleInfo;

/// User信息
typedef struct _tagCliUserInfo
{
	long lUserID;   						///< 用户ID
	char	szName[50];					///< 用户姓名
	char szAccountName[16];		///< 账户名
	char szPassword[16];  			///< 密码
	long lDepartmentID;  			///< 所属单位ID
	int nState;  							///< 状态
} CliUserInfo,  *LPCliUserInfo;
typedef std::queue<CliUserInfo> CliUserInfoQueue;

/// 用户ID
typedef struct _tagCliUserId
{
	long lUserID;   						///< 用户ID
} CliUserId, *LPCliUserId;

/// 用户类型
typedef struct _tagCliUserType
{
	int nType;   							///< 用户查询类型
} CliUserType, *LPCliUserType;

///单位相关
/// 单位信息
typedef struct _tagCliDepartmentInfo
{
	long  lDepartmentID;  		///< 单位ID
	char  szName[50];				///< 单位名称
	long  ParentID;					///< 父级单位
	int  OrderIndex;  				///< 单位顺序
	int  nState;  						///< 状态
} CliDepartmentInfo,  *LPCliDepartmentInfo;
typedef std::queue<CliDepartmentInfo> CliDepartmentInfoQueue;

/// 单位ID
typedef struct _tagCliDepartmentId
{
	long  lDepartmentID;  		///< 单位ID
} CliDepartmentId, *LPCliDepartmentId;

/// 配置回复码
typedef struct _tagCliConfigRetVal
{
	int nRetVal;
} CliConfigRetVal, *LPCliConfigRetVal;

#pragma pack(pop)

#endif // !__XLCTYPE_H_
