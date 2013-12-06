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
#ifndef __XLDTYPE_H_
#define __XLDTYPE_H_
#include "j_includes.h"

/// ����ָ���  
enum DvrCmdType
{
	xld_register = 0,					///< DVR����ע��
	xld_set_time,							///< ������Уʱ
	xld_get_loginfo,					///< ��ȡDVR���ػ���־
	xld_send_message,				///< ������Ϣ
	xld_heartbeat = 4,					///< �������
	xld_get_alraminfo,				///< ������ȡ
	xld_get_devinfo,					///< �豸��Ϣ��ȡ
	xld_real_play,						///< ��ʼʵʱ��Ƶ����
	xld_real_stop,						///< ֹͣʵʱ��Ƶ����
	xld_vod_play,						///< ��ʼ¼��ط�
	xld_vod_stop,						///< ֹͣ¼��ط�
	xld_errcode_download,			///< ϵͳ�쳣����
	xld_get_rcd_info,					///< ��ȡ¼����Ϣ
};

#pragma pack(push)
#pragma pack(1)

/// ָ��ͷ�ṹ  
typedef struct _cmdHeader
{
	char beginCode;			///< ��ʼ��־-0xFF
	char version;					///< �汾��
	char flag;						///< ״̬�� 0x00-����,0xFF-Ӧ��
	unsigned char cmd;		///< ������,��CmdType����
	int length;						///< ���ݳ���,������ͷ���ݺ�β����				
} CmdHeader, *LPCmdHeader;

/// ָ��β�ṹ  
typedef struct _cmdTail
{
	char verify;					///< У����
	char endCode;				///< ������־-0xFE
} CmdTail, *LPCmdTail;
////////////////////////////////////////////////////////////////////////
/////                                   �豸��                                      /////
////////////////////////////////////////////////////////////////////////

///�豸��Ϣ
typedef struct _tagDevHostInfo
{
	char hostId[32];			///< �豸ID
	char vehicleNum[16];		///< �г���
	char phoneNum[12];			///< �绰����
	char totalChannels;			///< ͨ����Ŀ
	bool bOnline;				///< ����״̬
	bool bState;				///< �豸״̬
} DevHostInfo, *PLDevHostInfo;

/// ������Ϣ
typedef struct _tagDevAlarmInfo
{
	__int64 bAlarm;								///< ������Ϣ
	struct  
	{
		double dLatitude;
		double dLongitude;
		double dGPSSpeed;
	} gps;												///< GPS��Ϣ
	double	speed;									///< ����
	time_t tmTimeStamp;						///< ʱ���
} DevAlarmInfo, *LPDevAlarmInfo;
typedef std::queue<DevAlarmInfo> DevAlarmInfoQueue;

/// ��־��Ϣ  
typedef struct _tagDevLogInfo
{
	__int64 bStatus;			///< �豸״̬,0-����,1-�ػ�
	time_t tmTime;				///< ���ػ�ʱ��
} DevLogInfo, *LPDevLogInfo;
typedef std::queue<DevLogInfo> DevLogInfoQueue;

/// �ظ�������
typedef struct _tagDevRetVal
{
	char nRetVal;						///< �ظ��� 0-�ɹ�,1-ʧ��
} DevRetVal, *LPDevRetVal;

/// �豸ID
typedef struct _tagDevEquipmentId
{
	char hostId[32];			   ///< ����ID
} DevEquipmentId, *LPDevEquipmentId;

/// ʱ������ 
typedef struct _tagDevSetTime
{
	time_t systime;				///< ��ǰϵͳʱ��
} DevSetTime, *LPDevSetTime;

/// ��ȡ��־��Ϣ
typedef struct _tagDevGetLogInfo
{
	time_t tmStart;		///< ��ʼʱ��
	time_t tmEnd;		///< ����ʱ��
} DevGetLogInfo, *LPDevGetLogInfo;

/// ʵʱ����
typedef struct _tagDevRealPlay
{
	char hostId[32];						///< �豸ID
	unsigned char channel;			///< ͨ����				
} DevRealPlay, *LPDevRealPlay;

/// ��ʼ��ʷ�ط�  
typedef struct _tagDevStartVod
{
	GUID sessionId;					///< �ػ�ID
	char hostId[32];						///< �豸ID
	__int64 channel;					///< ͨ����	
	time_t tmStartTime;				///< ��ʼʱ��
	time_t tmEndTime;				///< ����ʱ��
} DevStartVod, *LPDevStartVod;

/// ֹͣ��ʷ�ط�  
typedef struct _tagDevStopVod 
{
	GUID sessionId;					///< �ػ�ID
	char hostId[32];						///< �豸ID
	__int64 channel;					///< ͨ����	
} DevStopVod, *LPDevStopVod;

typedef struct _tagDevRcdInfo
{
	char szID[32];							///< ����ID
	time_t tmRecIntervalStartPt;		///< ¼�����俪ʼʱ��
	time_t tmRecIntervalEndPt;		///< ¼���������ʱ��
} DevRcdInfo, *LPDevRcdInfo;

#pragma pack(pop)

#endif//~__XLDTYPE_H_