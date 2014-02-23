#pragma once

#define DEFAULT_BUFFERSIZE (1024 * 512)

#define CMD_REGISTER		0		
#define CMD_SETIME			1
#define CMD_GETLOG			2
#define CMD_STOPGETLOG		3
#define CMD_HEADDET			4
#define CMD_GETALARM		5
#define CMD_GETDEVICE		6
#define CMD_RMVIDEO			7
#define CMD_STOPRMVIDEO		8
#define CMD_RECVIDEO		9
#define CMD_STOPRECVIDEO	10
#define CMD_ALARMDOWN		11

typedef struct _tagGPS
{
	double dLatitude;
	double dLongtude;
	double dGPSSpeed;
}GPS, *LPGPS;

typedef struct _tagDataInfoSeg
{
	unsigned char bAlarm;
	GPS gps;
	double bSpeed;
	unsigned char bReserve[64];
}USERDATAINFO, *LPUSERDATAINFO;

typedef struct _tagVideoInfoSeg
{
	unsigned long ulVideoSize;
	unsigned long ulLastVideoSize;
	bool bIsKeyFrame;
}VIDEOINFO, *LPVIDEOINFO;

/*typedef struct _tagBlockHeader
{
	unsigned char szID[32];
	unsigned int uChannelID;
	USERDATAINFO DataInfo;
	VIDEOINFO VideoInfo;
	time_t tmTimeStamp;
}BLOCKHEADER,*LPBLOCKHEADER;*/

#pragma pack(push)
#pragma pack(1)
typedef struct _tagBlockHeader
{
	unsigned char szID[32];	 //����
	unsigned char uChannelID;	 //ͨ����
	time_t tmTimeStamp;	 // ��
	double dLatitude;	 //ά��
	double dLongtude;	 //����
	double dGPSSpeed;	 //GPS�ٶ�
	double dSpeed;	 //�ٶ�
	unsigned char bAlarm;	 //����ֵ
	unsigned long ulVideoSize;	 //��Ƶ֡����
	bool bIsKeyFrame;	 //��Ƶ֡�Ƿ��ǹؼ�֡
}BLOCKHEADER,*LPBLOCKHEADER;

#pragma pack(pop)

typedef struct _tagFileHeader
{
	time_t tmStartTime;
	unsigned int uDuration;
	unsigned char szHash[16];
	unsigned long dwIndexTable[310];
}FILEHEADER, *LPFILEHEADER;

typedef struct _tagAlarm
{
	unsigned char bAlarm; //������Ϣ
	GPS gps;//GPS��Ϣ
	double dSpeed;//���ٶ�
	time_t tmTimeStamp;//ʱ���
}ALARM, *LPALARM;

typedef struct _tagCmdLine
{
	unsigned char* lpBuffer;
	unsigned long ulSize;
}CMDLINE, *LPCMDLINE;

typedef struct _tagEquipmentInfo
{
	char szID[32];
	char szVehicleNum[16];
	char szPhoneNum[12];
	unsigned char bTotalChannels;
}EQUIPMENTINFO,*LPEQUIPMENTINFO;

typedef struct _tagLogConext
{
	unsigned char bStatus;	//״̬,0,Ϊ������ 1Ϊ�ػ�
	time_t tmTime;			//ʱ�䣬���ػ�ʱ��
}LOGCONTEXT, *LPLOGCONTEXT;

typedef struct _tagLogTime
{
	time_t tmStart;//��ȡ�����е���־tmStartΪ-1
	time_t tmEnd;//��ȡ�����е���־tmEndΪ-1
}LOGTIME, *LPLOGTIME;


typedef struct _tagVideoReq
{
	char szID[32];
	unsigned char bChannelID;
}VIDEOREQ, *LPVIDEOREQ;

typedef struct _tagRecVideoReq
{
	char szID[32];
	unsigned char bChannelID;
	time_t tmStartTime;
	time_t tmEndTime;
}RECVIDEOREQ, *LPRECVIDEOREQ;