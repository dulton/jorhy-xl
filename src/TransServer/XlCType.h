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

/// 消息类型
enum CliMsgType
{
	xlc_msg_user = 0x01,			///< 用户消息
	xlc_msg_dev,						///< 设备消息
	xlc_msg_disk,						///< 磁盘消息
	xlc_msg_upload,					///< 上传文件消息
};

/// 用户消息子类型
enum CliMsgSubTypeForUser
{
	xlc_force_login = 0x01,			///< 用户强制下线
	xlc_user_deleted,						///< 用户被删除
};

/// 设备消息子类型
enum CliMsgSubTypeForDev
{
	xlc_alarm_complete = 0x01,				///< 报警下载完成
	xlc_log_complete,								///< 日志下载完成
	xlc_playvod_complete,						///< 回放完成
	xlc_rcd_info_complete,						///< 获取录像区间完成
};

/// 磁盘消息子类型
enum CliMsgSubTypeForDisk
{
	xlc_disk_full = 0x01,								///< 录像已满
};

/// 文件上传消息子类型
enum CliMsgSubTypeUpload
{
	xlc_upload_success = 0x01,				///< 传输成功
	xlc_upload_failed,								///< 传输失败
};
	
/// 操作指令定义  
enum CliCmdType
{
	xlc_message = 0x51,			///< 消息
	xlc_login,								///< 用户登录
	xlc_logout,							///< 注销登录
	xlc_start_real_alarm_info,	///< 开启获取实时报警信息
	xlc_stop_real_alarm_info,	///< 停止获取实时报警信息
	xlc_start_real_view,				///< 开启实时视频浏览
	xlc_stop_real_view,				///< 停止实时视频浏览
	xlc_start_vod_view,				///< 开始录像回放（下载）
	xlc_stop_vod_view,				///< 停止录像回放（下载）
	xlc_heart_beat,						///< 客户端在线心跳
	xlc_send_msg,						///< 发送文本消息
	xlc_get_rcd_info,					///< 获取录像信息
	///////////////////////////////////////////////////////////////////
	xlc_get_loginfo,
	///////////////////////////////////////////////////////////////////
	xlc_start_upload = 0x60,			///< 开始上传文件
	xlc_uploading,							///< 上传数据
	xlc_stop_upload,					///< 结束上传文件
	xlc_upload_resp,					///< 文件上传反馈
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
	///历史视频完成,内部使用
	xlc_playvod_complete_inner = 0xF0,		///< 历史回放完成
	xlc_rcdinfo_complete_inner = 0xF1,		///< 获取历史区间完成
};

struct CliOptKey
{
	CliOptKey(const char *pHostId, j_int32_t nCommand)
	{
		m_hostId = pHostId;
		m_nCommand = nCommand;
	}
	bool operator<(const CliOptKey &other) const
	{
		bool b_ret = false;
		if (m_hostId != other.m_hostId)
			b_ret = m_hostId < other.m_hostId;
		else
			b_ret = m_nCommand < other.m_nCommand;
		return b_ret;
	}
	j_string_t m_hostId;
	j_int32_t m_nCommand;
};

#pragma pack(push)
#pragma pack(1)
////////////////////////////////////////////////////////////////////////
/////                                   客户端                                      /////
////////////////////////////////////////////////////////////////////////

typedef struct _tagCliMessage
{
	unsigned int uiType;   	///< 消息类型
	unsigned int uiNO;		///< 消息编号
	char hostID[32];		///< 设备ID 
	int nChannelID;			///< 通道号 
} CliMessage, *LPCliMessage;

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
	char pHostId[32];						///< 设备ID
	int bAlarm;								///< 报警信息
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
	int bStatus;			///< 设备状态,0-开机,1-关机
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
	int nRetVal;			///< 回复码 0-成功,1-失败
} CliRealViewRetValue, *LPCliRealViewRetValue;

/// 历史播放停止回复
typedef struct _tagCliVodStopRetValue
{
	GUID sessionId;			///< 会话ID
	char  pHostId[32];		///< 设备ID
	int nChannelId;				///< 通道号 
	int nRetVal;					///< 回复码 0-成功,1-失败
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

/// 消息转发
typedef struct _tagCliSendMsg
{
	char hostId[32];						///< 设备ID
	char pData[1];							///< 消息体
} CliSendMsg, *LPCliSendMsg;

/// 开始文件上传
typedef struct _tagUploadStart
{ 
	int nUserId;										///< 用户ID
	int nFileId;											///< 文件ID
	WCHAR szID[32];			 					///< 设备ID
	WCHAR szFileName[512];				///< 文件名称
} CliUploadStart, *LPCliUploadStart;

/// 停止文件上传
typedef struct _tagUploadStop
{ 
	char szID[32];						///< 设备ID
	int nFileId;								///< 文件ID
	char szCheck[16];					///< MD5码校验
} CliUploadStop, *LPCliUploadStop;

/// 文件上传反馈
typedef struct _tagUploadResp
{ 
	char szID[32];						///< 设备ID
	char message[128];				
} CliUploadResp, *LPCliUploadResp;

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

typedef struct _tagCliRcdInfo
{
	char hostId[32];					///< 车辆ID
	time_t tmRecIntervalStartPt;		///< 录像区间开始时间
	time_t tmRecIntervalEndPt;			///< 录像区间结束时间
} CliRcdInfo, *LPCliRcdInfo;

#pragma pack(pop)

#endif // !__XLCTYPE_H_
