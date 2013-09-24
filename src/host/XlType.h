///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx�Ƽ����޹�˾(��Ȩ����) 
/// All rights reserved. 
/// 
/// @file      XlType.h 
/// @brief     XL�豸�ṹ����ģ��
///
/// @version   1.0 (�汾����)
/// @author    Jorhy (joorhy@gmail.com) 
/// @date      2013/09/23 10:50 
///
///
/// �޶�˵��������汾
///////////////////////////////////////////////////////////////////////////  
#ifndef __XLTYPE_H_
#define __XLTYPE_H_
#include "j_includes.h"

/// ����ָ���  
enum CmdType
{
	xl_register = 0,					///< DVR����ע��
	xl_set_time,						///< ������Уʱ
	xl_get_loginfo,					///< ��ȡDVR���ػ���־
	xl_heartbeat,						///< �������
	xl_get_alraminfo,				///< ������ȡ
	xl_get_devinfo,					///< �豸��Ϣ��ȡ
	xl_real_view,						///< ʵʱ��Ƶ���
	xl_vod_view,						///< ¼��ط�
	xl_vod_download,				///< ¼������
	xl_alarm_download,			///< ������¼����
	xl_errcode_download,		///< ϵͳ�쳣����
};

/// ָ��ͷ�ṹ  
typedef struct _cmdHeader
{
	char beginCode;			///< ��ʼ��־-0xFF
	char version;					///< �汾��
	char flag;						///< ״̬�� 0x00-����,0xFF-Ӧ��
	char cmd;						///< ������,��CmdType����
	int length;						///< ���ݳ���,������ͷ���ݺ�β����				
} CmdHeader, *LPCmdHeader;

/// ָ��β�ṹ  
typedef struct _cmdTail
{
	char verify;					///< У����
	char endCode;				///< ������־-0xFE
} CmdTail, *LPCmdTail;

/// ע������  
typedef struct _registerReq
{

} RegisterReq, *LPRegisterResp;

/// ע��ظ�
typedef struct _registerResp
{
	char hostId[32];			   ///< ����ID
} RegisterResp, *LPRegisterResp;

/// ʱ����������  
typedef struct _setTimeReq
{
	time_t systime;				///< ��ǰϵͳʱ��
} SetTimeReq, *LPSetTimeReq;

/// ʱ�����ûظ�  
typedef struct _setTimeResp
{
	char ret;						///< �ظ��� 0-�ɹ�,1-ʧ��
} SetTimeResp, *LPSetTimeResp;

/// ��־��Ϣ���� 
typedef struct _getLogInfoReq
{
	union
	{
		char all;					///< ����Ϊ��
		struct 
		{
			time_t tmStart;		///< ��ʼʱ��
			time_t tmEnd;		///< ����ʱ��
		} info;
	};
} GetLogInfoReq, *LPGetLogInfoReq;

/// ��־��Ϣ�ظ�  
typedef struct _getLogInfoResp
{
	char bStatus;				///< �豸״̬,0-����,1-�ػ�
	time_t tmTime;				///< ���ػ�ʱ��
} GetLogInfoResp, *LPGetLogInfoResp;

/// �����������  
typedef struct _heartBeatReq
{

} HeartBeatReq, *LPHeartBeatReq;

/// �������ظ�  
typedef struct _heartBeatResp
{
	char state;					///< ״̬ 0-����,1-�쳣
} HeartBeatResp, *LPHeartBeatResp;

/// ������Ϣ����  
typedef struct _alarmInfoReq
{

} AlarmInfoReq, *LPAlarmInfoReq;

/// ������Ϣ�ظ�  
typedef struct _alarmInfoResp
{
	unsigned char bAlarm;						///< ������Ϣ
	struct  
	{
		double dLatitude;
		double dLongitude;
		double dGPSSpeed;
	} gps;												///< GPS��Ϣ
	double	speed;									///< ����
	time_t tmTimeStamp;						///< ʱ���
} AlarmInfoResp, *LPAlarmInfoResp;

/// �豸��Ϣ����  
typedef struct _hostInfoReq
{

}HostInfoReq, *LPHostInfoReq;

/// �豸��Ϣ�ظ�  
typedef struct _hostInfoResp
{
	char hostId[32];									///< �豸ID
	char vehicleNum[8];							///< ������
	char phoneNum[11];							///< �ֻ���
	unsigned char bTotleChannels;			///< δ֪
} HostInfoResp, *LPHostInfoResp;

/// ʵʱԤ������ 
typedef struct _realViewReq
{
	char hostId[32];						///< �豸ID
	unsigned char channel;			///< ͨ����				
} RealViewReq, *LPRealViewReq;

/// ʵʱԤ���ظ�  
typedef struct _realViewResp
{

} RealViewResp, *LPRealViewResp;

/// ��ʷ�ط�����  
typedef struct _vodReq
{
	char hostId[32];						///< �豸ID
	unsigned char channel;			///< ͨ����	
	time_t tmStartTime;				///< ��ʼʱ��
} VodReq, LPVodReq;

/// ��ʷ�طŻظ�  
typedef struct _vodResp
{

} VodResp, *LPVodResp;

/// ¼����������  
typedef struct _vodDownLoadReq
{
	char hostId[32];						///< �豸ID
	unsigned char channel;			///< ͨ����	
	time_t tmStartTime;				///< ��ʼʱ��
	time_t tmEndTime;				///< ����ʱ��
} VodDownLoadReq, *LPVodDownLoadReq;

/// ¼�����ػظ�  
typedef struct _vodDownLoadResp
{

} VodDownLoadResp, *LPVodDownLoadResp;

/// ����������������  
typedef struct _alarmDownLoadReq
{
	time_t tmStart;						///< ��ʼʱ��
	time_t tmEnd;						///< ����ʱ��
} AlarmDownLoadReq, *LPAlarmDownLoadReq;

/// �����������ػظ�  
typedef struct _alarmDownLoadResp
{

} AlarmDownLoadResp, *LPAlarmDownLoadResp;

/// ϵͳ�쳣����  
typedef struct _sysErrorCodeReq
{

} SysErrorCodeReq, *LPSysErrorCodeReq;

/// ϵͳ�쳣�ظ�  
typedef struct _sysErrorCodeResp
{
	char ret;					///< 0-0xFF���쳣��Ϣ��
} SysErrorCodeResp, *LPSysErrorCodeResp;

#endif//~__XLTYPE_H_