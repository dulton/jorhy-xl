///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx�Ƽ����޹�˾(��Ȩ����) 
/// All rights reserved. 
/// 
/// @file      XlCType.h 
/// @brief     �ͻ��˽ṹ����
///
/// @version   1.0 (�汾����)
/// @author    Jorhy (joorhy@gmail.com) 
/// @date      2013/11/01 09:04 
/// �޶�˵��������汾
///////////////////////////////////////////////////////////////////////////  
#ifndef __XLCTYPE_H_
#define __XLCTYPE_H_

/// ����״̬
enum CliCmdState
{
	xl_init_state = 0,
	xl_read_head_state,
	xl_read_data_state,
	xl_write_body_state,
};

/// ����ָ���  
enum CliCmdType
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
	xlc_get_dvr_summary = 0x81,	///< ��ȡDVR������Ϣ
	xlc_get_dvr_list,							///< ��ȡDVR���ػ���־
	xlc_get_dvr_info,						///< ��ȡ����DVR��Ϣ
	xlc_add_dvr_info,						///< ����DVR ����ʹ�ã���Ԥ���ӿڣ�
	xlc_mod_dvr_info,						///< �޸�DVR����Ϣ
	xlc_del_dvr_info,						///< ɾ��DVR
	xlc_get_user_summary,				///< ��ȡ�û�������Ϣ
	xlc_get_user_list,						///< ��ȡ�û���Ϣ�б�
	xlc_get_user_info,						///< ��ȡ�����û���Ϣ
	xlc_add_user_info,						///< �����û�����Ϣ
	xlc_mod_user_info,					///< �޸��û�����Ϣ
	xlc_del_user_info,						///< ɾ���û�
	xlc_get_department_list,			///< ��ȡ��λ��Ϣ�б�
	xlc_get_department_info,			///< ��ȡ������λ��Ϣ
	xlc_add_department_info,			///< ���ӵ�λ����Ϣ
	xlc_mod_department_info,		///< �޸ĵ�λ����Ϣ
	xlc_del_department_info,			///< ɾ����λ
	xlc_get_alarm_info,
};

#pragma pack(push)
#pragma pack(1)
////////////////////////////////////////////////////////////////////////
/////                                   �ͻ���                                      /////
////////////////////////////////////////////////////////////////////////
/// �û���¼
typedef struct _tagCliUserLogin
{
	char userName[16];		///<�˻���
	char passWord[16];  		///< ����
	int  nForced;					///<1 ǿ�Ƶ�¼��0 ��ǿ�Ƶ�¼
} CliUserLogin,  *LPCliUserLogin;

///�豸��Ϣ
typedef struct _tagCliDevInfo
{
	char hostId[32];					///< �豸ID
	char vehicleNum[16];		///< �г���
	char phoneNum[12];			///< �绰����
	char totalChannels;			///< ͨ����Ŀ
} CliDevInfo, *PLCliDevInfo;

/// ������Ϣ
typedef struct _tagCliAlarmInfo
{
	char pHostId[32];							///< �豸ID
	__int64 bAlarm;								///< ������Ϣ
	struct  
	{
		double dLatitude;
		double dLongitude;
		double dGPSSpeed;
	} gps;												///< GPS��Ϣ
	double	speed;									///< ����
	time_t tmTimeStamp;						///< ʱ���
} CliAlarmInfo, *LPCliAlarmInfo;
typedef std::queue<CliAlarmInfo> CliAlarmInfoQueue;

/// ��־��Ϣ  
typedef struct _tagCliLogInfo
{
	char pHostId[32];			///< �豸ID
	__int64 bStatus;			///< �豸״̬,0-����,1-�ػ�
	time_t tmTime;				///< ���ػ�ʱ��
} CliLogInfo, *LPCliLogInfo;
typedef std::queue<CliLogInfo> CliLogInfoQueue;

/// �豸�����ظ�������
typedef struct _tagCliRetValue
{
	char  pHostId[32];		///< �豸ID
	int nRetVal;				///< �ظ��� 0-�ɹ�,1-ʧ��
} CliRetValue, *LPCliRetValue;

/// ���ò����ظ�������
typedef struct _tagCliRetValue2
{
	int nRetVal;						///< �ظ��� 0-�ɹ�,1-ʧ��
} CliRetValue2, *LPCliRetValue2;

/// ʵʱ����ֹͣ�ظ�
typedef struct _tagCliRealViewRetValue
{
	char  pHostId[32];		///< �豸ID
	int nChannelId;			///< ͨ���� 
	int nRetVal;				///< �ظ��� 0-�ɹ�,1-ʧ��
} CliRealViewRetValue, *LPCliRealViewRetValue;

/// ��ʷ����ֹͣ�ظ�
typedef struct _tagCliVodStopRetValue
{
	GUID sessionId;		///< �ػ�ID
	char  pHostId[32];		///< �豸ID
	int nChannelId;			///< ͨ���� 
	int nRetVal;				///< �ظ��� 0-�ɹ�,1-ʧ��
} CliVodStopRetValue, *LPCliVodStopRetValue;

/// �豸ID
typedef struct _tagCliEquipmentId
{
	char hostId[32];			   ///< ����ID
} CliEquipmentId, *LPCliEquipmentId;

/// ʱ������ 
typedef struct _tagCliSetTime
{
	char pHostId[32];			///< �豸ID
	time_t systime;				///< ��ǰϵͳʱ��
} CliSetTime, *LPCliSetTime;

/// ��ȡ��־��Ϣ
typedef struct _tagCliGetLogInfo
{
	char pHostId[32];	///< �豸ID
	time_t tmStart;		///< ��ʼʱ��
	time_t tmEnd;		///< ����ʱ��
} CliGetLogInfo, *LPCliGetLogInfo;

/// ʵʱ����
typedef struct _tagCliRealAlarmInfo
{
	char  hostId[32];		///< �豸ID
} CliRealAlarmInfo, *LPCliRealAlarmInfo;

/// ʵʱ����
typedef struct _tagCliRealPlay
{
	char hostId[32];						///< �豸ID
	unsigned char channel;			///< ͨ����				
} CliRealPlay, *LPCliRealPlay;

/// ��ʼ��ʷ�ط�  
typedef struct _tagCliStartVod
{
	GUID sessionId;					///< �ػ�ID
	char hostId[32];						///< �豸ID
	int		channel;					///< ͨ����	
	time_t tmStartTime;				///< ��ʼʱ��
	time_t tmEndTime;				///< ����ʱ��
} CliStartVod, *LPCliStartVod;

/// ֹͣ��ʷ�ط�  
typedef struct _tagCliStopVod 
{
	GUID sessionId;					///< �ػ�ID
	char hostId[32];						///< �豸ID
	int channel;							///< ͨ����	
} CliStopVod, *LPCliStopVod;

/// �豸���
/// DVR������Ϣ
typedef struct _tagCliHostTotleInfo
{
	unsigned int uiTotal;   				///< DVR������
	unsigned int uiUnRegister; 		///< δע��DVR����
	unsigned int uiOnline;				///< ����DVR����
	unsigned int uiStock;   				///< ��ע�ᵫδ������û���DVR����
} CliHostTotleInfo,  *LPCliHostTotleInfo;

/// DVR��Ϣ
typedef struct _tagCliHostInfo
{
	char szEquipmentID[32];  		///< �豸ID
	char szVehicleNO[16];			///< ����
	char szPhone[16];					///< �绰����
	int nTotalChannels;				///< ͨ������
	char szReserve[32];  				///< Ԥ��
	long lDepartmentID;  			///< ������λID
	int nOnline;							///< ������״̬
	int nState;  							///< ״̬
} CliHostInfo,  *LPCliHostInfo;
typedef std::queue<CliHostInfo> CliHostInfoQueue;

/// DVR�豸ID
typedef struct _tagCliHostId
{
	char szEquipmentID[32];  		///< �豸ID 
} CliHostId, *LPCliHostId;

/// DVR�б���Ϣ����
typedef struct _tagCliGetHostList
{
	int	nType;  				///< ��������
	long lDepartmentID;  	///< ������λID
} CliGetHostList, *LPCliGetHostList;

///�û����
/// User������Ϣ
typedef struct _tagCliUserTotleInfo
{
	unsigned int uiTotal;   					///< �û�������
	unsigned int uiOnline;					///< �����û�����
} CliUserTotleInfo,  *LPCliUserTotleInfo;

/// User��Ϣ
typedef struct _tagCliUserInfo
{
	long lUserID;   						///< �û�ID
	char	szName[50];					///< �û�����
	char szAccountName[16];		///< �˻���
	char szPassword[16];  			///< ����
	long lDepartmentID;  			///< ������λID
	int nState;  							///< ״̬
} CliUserInfo,  *LPCliUserInfo;
typedef std::queue<CliUserInfo> CliUserInfoQueue;

/// �û�ID
typedef struct _tagCliUserId
{
	long lUserID;   						///< �û�ID
} CliUserId, *LPCliUserId;

/// �û�����
typedef struct _tagCliUserType
{
	int nType;   							///< �û���ѯ����
} CliUserType, *LPCliUserType;

///��λ���
/// ��λ��Ϣ
typedef struct _tagCliDepartmentInfo
{
	long  lDepartmentID;  		///< ��λID
	char  szName[50];				///< ��λ����
	long  ParentID;					///< ������λ
	int  OrderIndex;  				///< ��λ˳��
	int  nState;  						///< ״̬
} CliDepartmentInfo,  *LPCliDepartmentInfo;
typedef std::queue<CliDepartmentInfo> CliDepartmentInfoQueue;

/// ��λID
typedef struct _tagCliDepartmentId
{
	long  lDepartmentID;  		///< ��λID
} CliDepartmentId, *LPCliDepartmentId;

/// ���ûظ���
typedef struct _tagCliConfigRetVal
{
	int nRetVal;
} CliConfigRetVal, *LPCliConfigRetVal;

#pragma pack(pop)

#endif // !__XLCTYPE_H_
