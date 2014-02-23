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
	unsigned char szID[32];	 //车号
	unsigned char uChannelID;	 //通道号
	time_t tmTimeStamp;	 // 戳
	double dLatitude;	 //维度
	double dLongtude;	 //经度
	double dGPSSpeed;	 //GPS速度
	double dSpeed;	 //速度
	unsigned char bAlarm;	 //报警值
	unsigned long ulVideoSize;	 //视频帧长度
	bool bIsKeyFrame;	 //视频帧是否是关键帧
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
	unsigned char bAlarm; //报警信息
	GPS gps;//GPS信息
	double dSpeed;//车速度
	time_t tmTimeStamp;//时间戳
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
	unsigned char bStatus;	//状态,0,为开机， 1为关机
	time_t tmTime;			//时间，开关机时间
}LOGCONTEXT, *LPLOGCONTEXT;

typedef struct _tagLogTime
{
	time_t tmStart;//若取得所有的日志tmStart为-1
	time_t tmEnd;//若取得所有的日志tmEnd为-1
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