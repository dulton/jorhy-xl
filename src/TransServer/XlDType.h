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
	xld_heartbeat = 4,				///< �������
	xld_get_alraminfo,				///< ������ȡ
	xld_get_devinfo,					///< �豸��Ϣ��ȡ
	xld_real_play,						///< ��ʼʵʱ��Ƶ����
	xld_real_stop,						///< ֹͣʵʱ��Ƶ����
	xld_vod_play,						///< ��ʼ¼��ط�
	xld_vod_stop,						///< ֹͣ¼��ط�
	xld_msg_info,						///< ϵͳ�쳣����
	xld_get_rcd_info,					///< ��ȡ¼����Ϣ
	xld_start_upload,					///< ��ʼ�ļ��ϴ�
	xld_uploading,						///< �ϴ��ļ�
	xld_stop_upload,					///< �����ļ��ϴ�
};

/// ��Ϣ���Ͷ���
enum DvrMsgType
{
	xld_alarm_complete = 0x00,		///< �����������
	xld_log_complete,						///< ��־�������
	xld_playvod_complete,				///< �ط����
	xld_disk_full,									///< ¼������
	xld_playreal_complete,				///< ʵʱ�������
	xld_rcd_info_complete,				///< ��ȡ¼���������
	xld_upload_success	 = 0x07,		///< �����ļ��ɹ�
	xld_upload_failed	,						///< �����ļ�ʧ��
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
	//bool bOnline;				///< ����״̬
	//bool bState;				///< �豸״̬
} DevHostInfo, *PLDevHostInfo;

/// ������Ϣ
typedef struct _tagDevAlarmInfo
{
	time_t tmTimeStamp;				///< ʱ���
	char bAlarm;								///< ������Ϣ
	double dLatitude;
	double dLongitude;
	double dGPSSpeed;
} DevAlarmInfo, *LPDevAlarmInfo;
typedef std::queue<DevAlarmInfo> DevAlarmInfoQueue;

/// ��־��Ϣ  
typedef struct _tagDevLogInfo
{
	char szID[32];				///< ����
	time_t tmTime;				///< ���ػ�ʱ��
	char bStatus;				///< �豸״̬,0-����,1-�ػ�
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

/// �ļ���Ϣ
typedef struct _tagDevFileInfo
{
	char fileName[512];			  ///< �ļ���
} DevFileInfo, *LPDevFileInfo;

/// ʱ������ 
typedef struct _tagDevSetTime
{
	time_t systime;				///< ��ǰϵͳʱ��
} DevSetTime, *LPDevSetTime;

/// ��ȡ��־��Ϣ
typedef struct _tagDevGetLogInfo
{
	char	  szID[32];		///< ����
	time_t tmStart;		///< ��ʼʱ��
	time_t tmEnd;		///< ����ʱ��
} DevGetLogInfo, *LPDevGetLogInfo;

/// ʵʱ����
typedef struct _tagDevRealPlay
{
	char hostId[32];				///< �豸ID
	unsigned char channel;			///< ͨ����
} DevRealPlay, *LPDevRealPlay;

/// ��ʼ��ʷ�ط�  
typedef struct _tagDevStartVod
{
	GUID sessionId;					///< �ػ�ID
	char hostId[32];						///< �豸ID
	unsigned char channel;			///< ͨ����	
	time_t tmStartTime;				///< ��ʼʱ��
	time_t tmEndTime;				///< ����ʱ��
} DevStartVod, *LPDevStartVod;

/// ֹͣ��ʷ�ط�  
typedef struct _tagDevStopVod 
{
	GUID sessionId;					///< �ػ�ID
	char hostId[32];						///< �豸ID
	char channel;					///< ͨ����	
} DevStopVod, *LPDevStopVod;

/// ��Ϣ
typedef struct _tagMsgInfo
{
	char hostId[32];								///< ����
	unsigned char channel;					///< ͨ����
	unsigned int nMsgCode;				///< ��Ϣ��
	unsigned char bReserve[16];			///< �ỰID
	//unsigned char bReserve[2048];	///< ϵͳ��Ϣ
} DevMsgInfo, *LPDevMsgInfo;

/// ��ȡ��ʷ��Ƶ��Ϣ
typedef struct _tagDevRcdInfo
{
	char szID[32];						///< ����ID
	time_t tmRecIntervalStartPt;		///< ¼�����俪ʼʱ��
	time_t tmRecIntervalEndPt;			///< ¼���������ʱ��
} DevRcdInfo, *LPDevRcdInfo;

/// ��ʼ�ļ��ϴ�
typedef struct _tagDevUploadStart
{ 
	int nUserId;									///< �û�ID
	int nFileId;										///< �ļ�ID
	char szID[32];			 					///< �豸ID
	char szFileName[512];					///< �ļ�����
} DevUploadStart, *LPDevUploadStart;

/// ֹͣ�ļ��ϴ�
typedef struct _tagDevUploadStop
{ 
	int nFileId;								///< �ļ�ID
	char szCheck[16];					///< MD5��У��
} DevUploadStop, *LPDevUploadStop;

#pragma pack(pop)

#endif//~__XLDTYPE_H_