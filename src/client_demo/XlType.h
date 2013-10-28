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
#include "time.h"

/// ����ָ���  
enum CmdType
{
	xl_register = 0,					///< DVR����ע��
	xl_set_time,						///< ������Уʱ
	xl_get_loginfo,					///< ��ȡDVR���ػ���־
	xl_heartbeat = 4,				///< �������
	xl_get_alraminfo,				///< ������ȡ
	xl_get_devinfo,					///< �豸��Ϣ��ȡ
	xl_real_play,						///< ��ʼʵʱ��Ƶ����
	xl_real_stop,						///< ֹͣʵʱ��Ƶ����
	xl_vod_play,						///< ��ʼ¼��ط�
	xl_vod_stop,						///< ֹͣ¼��ط�
	xl_start_real_alarm,			///< ��ʼ������¼����
	xl_stop_real_alarm,			///< ֹͣ������¼����
	xl_errcode_download,		///< ϵͳ�쳣����
	xl_login,							///< ��¼
	xl_logout,							///< ע��

	xl_get_totle_dvr_info = 0x52,			///< ��ȡDVR������Ϣ
	xl_get_dvr_list,								///< ��ȡDVR��Ϣ�б�
	xl_get_dvr_info,								///< ��ȡDVR��Ϣ
	xl_modify_dvr_info,						///< �޸�DVR��Ϣ
	xl_delete_dvr,								///< ɾ��DVR	
	xl_get_totle_user_info,					///< ��ȡ�û�������Ϣ
	xl_get_user_list,								///< ��ȡ�û���Ϣ�б�
	xl_get_user_info,							///< ��ȡ�û���Ϣ
	xl_modify_user_info,						///< �޸��û���Ϣ
	xl_delete_user,								///< ɾ���û�
	xl_get_department_list,					///< ��ȡ��λ��Ϣ�б�
	xl_get_department_info,				///< ��ȡ��λ��Ϣ
	xl_modify_department_info,			///< �޸ĵ�λ��Ϣ
	xl_delete_departmen,					///< ɾ����λ
};

#pragma pack(push)
#pragma pack(1)

////////////////////////////////////////////////////////////////////////
/////                                   �ͻ���                                      /////
////////////////////////////////////////////////////////////////////////
/// DVR������Ϣ
typedef struct _tagDvrTotleInfo
{
	unsigned int uiTotal;   				///< DVR������
	unsigned int uiUnRegister; 		///< δȷ��DVR����
	unsigned int uiOnline;				///< ����DVR����
	unsigned int uiStock;   				///< ��ȷ�ϵ�δ�������λ��DVR����
} DVRTotleInfo,  *LPDVRTotleInfo;

/// DVR�б���Ϣ����
typedef struct _tagDvrListReq
{
	int	nType;  				///< ��������
	long lDepartmentID;  	///< ������λID
} DvrListReq, *LPDvrListReq;

/// DVR��Ϣ
typedef struct _tagDvrInfo
{
	char szEquipmentID[32];  		///< �豸ID
	char szVehicleNO[32];			///< ����
	char szPhone[11];		///< �绰����
	int nTotalChannels;				///< ͨ������
	char szReserve[32];  				///< Ԥ��
	long lDepartmentID;  			///< ������λID
	int nOnline;							///< ������״̬
	int nState;  							///< ״̬
} DVRInfo,  *LPDVRInfo;

/// DVR�豸ID
typedef struct _tagDvrId
{
	char szEquipmentID[32];  		///< �豸ID 
} DVRId, *LPDvrId;

/// User������Ϣ
typedef struct _tagUserTotleInfo
 {
	unsigned int uiTotal;   					///< �û�������
	unsigned int uiOnline;					///< �����û�����
 } UserTotleInfo,  *LPUserTotleInfo;

/// User��Ϣ
typedef struct _tagUserInfo
{
	long lUserID;   						///< �û�ID
	char	szName[50];					///< �û�����
	char szAccountName[16];		///< �˻���
	char szPassword[16];  			///< ����
	long lDepartmentID;  			///< ������λID
	int nState;  							///< ״̬
} UserInfo,  *LPUserInfo;

/// �û�ID
typedef struct _tagUserId
{
	long lUserID;   						///< �û�ID
} UserId, *LPUserId;

/// �û�����
typedef struct _tagUserType
{
	int nType;   							///< �û���ѯ����
} UserType, *LPUserType;

/// ��λ��Ϣ
typedef struct _tagDepartmentInfo
{
	long  lDepartmentID;  		///< ��λID
	char  szName[50];				///< ��λ����
	long  ParentID;					///< ������λ
	int  OrderIndex;  				///< ��λ˳��
	int  nState;  						///< ״̬
} DepartmentInfo,  *LPDepartmentInfo;

/// ��λID
typedef struct _tagDepartmentId
{
	long  lDepartmentID;  		///< ��λID
} DepartmentId, *LPDepartmentId;
////////////////////////////////////////////////////////////////////////
/////                                   �豸��                                      /////
////////////////////////////////////////////////////////////////////////
///�豸��Ϣ
typedef struct _devInfo
{
	char hostId[32];					///< �豸ID
	char vehicleNum[16];		///< �г���
	char phoneNum[12];			///< �绰����
	char totalChannels;			///< ͨ����Ŀ
} DevInfo, *PLDevInfo;

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

} RegisterReq, *LPRegisterReq;

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
	char hostId[32];		///< �豸ID
	time_t tmStart;		///< ��ʼʱ��
	time_t tmEnd;		///< ����ʱ��
} GetLogInfoReq, *LPGetLogInfoReq;

/// ��־��Ϣ�ظ�  
typedef struct _getLogInfoResp
{
	__int64 bStatus;			///< �豸״̬,0-����,1-�ػ�
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
	char hostId[32];				///< �豸ID
	time_t tmStart;				///< ��ʼʱ��
	time_t tmEnd;				///< ����ʱ��
} AlarmInfoReq, *LPAlarmInfoReq;

/// ������Ϣ�ظ�  
typedef struct _alarmInfoResp
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
} AlarmInfoResp, *LPAlarmInfoResp;

/// �豸��Ϣ����  
typedef struct _hostInfoReq
{
	char hostId[32];									///< �豸ID
}HostInfoReq, *LPHostInfoReq;

/// �豸��Ϣ�ظ�  
typedef DevInfo HostInfoResp, *LPHostInfoResp;

/// ʵʱ�������� 
typedef struct _realPlayReq
{
	char hostId[32];						///< �豸ID
	unsigned char channel;			///< ͨ����				
} RealPlayReq, *LPRealPlayReq;

/// ʵʱ���Żظ�  
typedef struct _realPlayResp
{
	char ret;								///< 0-�ɹ�, 1-ʧ��
} RealPlayResp, *LPRealPlayResp;

/// ֹͣʵʱ�������� 
typedef _realPlayReq RealStopReq, *LPRealPlayReq;

/// ֹͣʵʱ���Żظ�  
typedef _realPlayResp RealStopResp, *LPRealStopResp;

/// ��ʷ�ط�����  
typedef struct _vodPlayReq
{
	char hostId[32];						///< �豸ID
	__int64 channel;					///< ͨ����	
	time_t tmStartTime;				///< ��ʼʱ��
	time_t tmEndTime;				///< ����ʱ��
} VodPlayReq, LPVodPlayReq;

/// ��ʷ�طŻظ�  
typedef struct _vodPlayResp
{

} VodPlayResp, *LPVodPlayResp;

/// ֹͣ��ʷ�ط�����  
typedef struct _vodStopReq 
{
	char hostId[32];						///< �豸ID
	__int64 channel;					///< ͨ����	
} VodStopReq, *LPVodStopReq;

/// ֹͣ��ʷ�طŻظ�  
typedef struct _vodStopResp 
{
	char ret;
} VodStopResp, *LPVodStopResp;

/// ��ʼ����������������  
typedef struct _alarmStopReq
{
	char hostId[32];				///< �豸ID
} AlarmStopReq, *LPAlarmStopReq;

/// ��ʼ�����������ػظ�  
typedef struct _alarmStopResp
{
	char ret;
} AlarmStopResp, *LPAlarmStopResp;

/// ϵͳ�쳣����  
typedef struct _sysErrorCodeReq
{
	
} SysErrorCodeReq, *LPSysErrorCodeReq;

/// ϵͳ�쳣�ظ�  
typedef struct _sysErrorCodeResp
{
	char ret;					///< 0-0xFF���쳣��Ϣ��
} SysErrorCodeResp, *LPSysErrorCodeResp;

/// ��¼����  
typedef struct _loginReq
{
	char user[16];			///< �û���
	char passWord[32];	///< ����
} LoginReq, *LPLoginReq;

/// ��¼�ظ�  
typedef struct _loginResp
{
	char ret;					///< 0-�ɹ�,1-ʧ��
} LoginResp, *LPLoginResp;

/// ע������  
typedef _loginReq LogoutReq, *LPLogoutReq;

/// ע���ظ�  
typedef _loginResp LogoutResp, *LPLogoutResp;

#pragma pack(pop)

#endif//~__XLTYPE_H_