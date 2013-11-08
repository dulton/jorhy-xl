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
	xlc_message = 0x51,				///< ��Ϣ
	xlc_login,								///< �û���¼
	xlc_logout,							///< ע����¼
	xlc_start_real_alarm_info,		///< ������ȡʵʱ������Ϣ
	xlc_stop_real_alarm_info,		///< ֹͣ��ȡʵʱ������Ϣ
	xlc_start_real_view,				///< ����ʵʱ��Ƶ���
	xlc_stop_real_view,				///< ֹͣʵʱ��Ƶ���
	xlc_start_vod_view,				///< ��ʼ¼��طţ����أ�
	xlc_stop_vod_view,				///< ֹͣ¼��طţ����أ�
	xlc_heart_beat,						///< �ͻ�����������
	///////////////////////////////////////////////////////////////////
	xlc_get_dvr_summary,			///< ��ȡDVR������Ϣ
	xlc_get_dvr_list,						///< ��ȡDVR���ػ���־
	xlc_get_dvr_info,					///< ��ȡ����DVR��Ϣ
	xlc_add_dvr_info,					///< ����DVR ����ʹ�ã���Ԥ���ӿڣ�
	xlc_mod_dvr_info,					///< �޸�DVR����Ϣ
	xlc_del_dvr_info,					///< ɾ��DVR
	xlc_get_user_summary,			///< ��ȡ�û�������Ϣ
	xlc_get_user_list,					///< ��ȡ�û���Ϣ�б�
	xlc_get_user_info,					///< ��ȡ�����û���Ϣ
	xlc_add_user_info,					///< �����û�����Ϣ
	xlc_mod_user_info,				///< �޸��û�����Ϣ
	xlc_del_user_info,					///< ɾ���û�
	xlc_get_department_list,		///< ��ȡ��λ��Ϣ�б�
	xlc_get_department_info,		///< ��ȡ������λ��Ϣ
	xlc_add_department_info,		///< ���ӵ�λ����Ϣ
	xlc_mod_department_info,	///< �޸ĵ�λ����Ϣ
	xlc_del_department_info,		///< ɾ����λ
	xlc_get_dvr_state,					///< ��ȡDVR��������Ϣ	
	xlc_get_alarm_info,				///< ��ʼ������¼����
	xlc_xxxxxxxxxxxxxx,				///< ֹͣ������¼����
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

//#pragma pack(4)
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
	long lDepartmentID;  	///< ������λID
	int	nType;  				///< ��������
} DvrListReq, *LPDvrListReq;

/// DVR��Ϣ
typedef struct _tagDvrInfo
{
	char szEquipmentID[32];  		///< �豸ID
	char szVehicleNO[16];			///< ����
	char szPhone[16];					///< �绰����
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
	long lDepartmentID;				///< ��λID
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
} AlarmInfoReq, *LPAlarmInfoReq;

/// ������Ϣ�ظ�  
typedef struct _alarmInfoResp
{
	char hostId[32];								///< �豸��Ϣ
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
typedef struct _realViewReq
{
	char hostId[32];						///< �豸ID
	int channel;			///< ͨ����				
} RealViewReq, *LPRealViewReqReq;

/// ʵʱ���Żظ�  
typedef struct _realViewResp
{
	char ret;								///< 0-�ɹ�, 1-ʧ��
} RealViewResp, *LPRealViewRespResp;

/// ʵʱ��Ƶ��Ϣ��
struct RealViewBody 
{
	CmdHeader head;
	RealViewReq data;
	CmdTail tail;
};

/// ��ʷ�ط�����  
typedef struct _vodPlayReq
{
	GUID sessionID;					///< �ػ�ID
	char hostId[32];						///< �豸ID
	int channel;							///< ͨ����	
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
	GUID sessionID;					///< �ػ�ID
	char hostId[32];						///< �豸ID
	int channel;							///< ͨ����	
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
	char szAccountName[16];		//�˻���
	char szPassword[16];  				//����
	int  nForced;								//1 ǿ�Ƶ�¼,0 ��ǿ�Ƶ�¼
} LoginReq, *LPLoginReq;

/// ��¼�ظ�  
typedef struct _loginResp
{
	int ret;					///< 1 ��¼�ɹ�
								///< 2 �û�������
								///< 3 �������
								///< 4 �û��������û���
								///< 5 �ͻ��˰汾���ͻ����
								///< 6 �û���������λ�ý��е�¼
								///< 7 ��¼ʧ��
} LoginResp, *LPLoginResp;

/// ע���ظ�  
typedef _loginResp LogoutResp, *LPLogoutResp;
///< 1 ע���ɹ�
///< 0 ע��ʧ��

/// ʵʱ������Ϣ��
struct RealAlarmBody 
{
	CmdHeader head;
	DVRId data;
	CmdTail tail;
};

#pragma pack(pop)

#endif//~__XLTYPE_H_