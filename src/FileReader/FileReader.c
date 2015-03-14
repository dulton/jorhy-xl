#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>   
#include <assert.h>
#include <time.h>

#include "FileReader.h"

#define  TIMER_INV_M				1
#define  TIME_STEP						16
#define  I_FREAM_INTERVAL		100
#define	FRAME_INTERVAL		25
#define	FRAME_NUM_SEND	200

/// 定义结构体
struct s_media_info
{
	int i_channel;
	char *p_data;
	int i_len;
	time_t time_stamp;
	void *p_parm;
	struct s_media_info *p_next;
};

struct s_file_info
{
	time_t tm;
	int offset;
	struct s_file_info *next;
};

typedef struct
{ 
	char							cGpsStatus; 						//gps是否有效标识:A有效,V无效。
	char							cSpeedUnit; 						//速度单位
	unsigned short		usSpeed;							//速度值
	char							cLatitudeDegree;				//纬度值的度
	char							cLatitudeCent; 					//纬度值的分
	char							cLongitudeDegree;			//经度值的度
	char							cLongitudeCent;				//经度值的分
	long						lLatitudeSec;						//纬度值的秒
	long						lLongitudeSec;					//经度值的秒
	unsigned short		usGpsAngle;						//gps夹角
	char							cDirectionLatitude;			//纬度的方向
	char							cDirectionLongitude;		//经度的方向 
	char							reserve[4]; 
}GPSData_s;

/// 线程相关
static HANDLE s_hThreadTriger = INVALID_HANDLE_VALUE;		/// 触发器线程
static HANDLE s_hEventTriger	= INVALID_HANDLE_VALUE;			/// 触发器事件
static HANDLE s_hThreadSender = INVALID_HANDLE_VALUE;		/// 发送器线程
static HANDLE s_hEventSender[2];														/// 发送器事件
static HANDLE s_hThreadDecoder = INVALID_HANDLE_VALUE;		/// 解码线程
static HANDLE s_hEventDecoder[2];														/// 解码事件
static CRITICAL_SECTION s_mutex;
static CRITICAL_SECTION s_file_mux;
static CRITICAL_SECTION s_log_mux;

static BOOL s_bSenderRun = FALSE;

/// 回调相关
static AnalyseDataCallback s_pDataCallbackFunction = NULL;
static void *s_pDataParam = NULL;
static AnalyseAlarmCallback s_pAlarmCallbackFunction = NULL;
static void *s_pAlarmParam = NULL;
static AnalyseFileEndCallback s_pFileEndCallbackFunction = NULL;
static void *s_pFileEndParam = NULL;
static char *s_pData = NULL;
volatile static time_t s_tmStart = 0;
volatile static time_t s_tmSeekTime = 0;
volatile static BOOL s_bReadData = TRUE;

/// 文件相关
volatile static BOOL s_bStart = FALSE;
volatile static BOOL s_bInit = FALSE;
static long s_lFileHandle = 0;	
static FILE *s_pFileIdx = NULL;		/// index file
static FILE *s_pFileMdv = NULL;	/// media file
static struct s_file_info *s_begin_info = NULL;
static struct s_file_info *s_cur_info = NULL;
static struct s_file_info *s_next_info = NULL;
struct s_media_info *s_data_queue = NULL;

static int s_nInterval = TIMER_INV_M;
static int s_nStep = 1;
static int s_nSpeed = 0;
volatile static BOOL s_bPause = FALSE;

/// 通道相关
#define CHANNEL_NUM		16
static int s_cha_index[CHANNEL_NUM];

/// 日志相关
FILE * log_fp = NULL;


void CreateLog()
{
	if (log_fp == NULL)
	{
		char vodDir[512] = {0};
		char fileName[256] = {0};
		GetCurrentDirectory(512, vodDir);
		sprintf_s(vodDir, "%s/log", vodDir);
		sprintf_s(fileName, "%s/analyse.log", vodDir);

		if (_access(vodDir, 0) != 0)
		{
			if (!CreateDirectory(vodDir, NULL))
			{
				return;
			}
		}
		fopen_s(&log_fp, fileName, "wb+");
		InitializeCriticalSection (&s_log_mux);
	}
}

void ReleaseLog()
{
	if (log_fp)
	{
		fclose((FILE *)log_fp);
		log_fp = NULL;
		DeleteCriticalSection(&s_log_mux);
	}
}

void WriteLogInfo(const char *format, ...)
{
	int nArgLen = 0;
	char log_buff[1024] = {0};
	va_list arg_ptr;
	time_t curTime;
	int nMilliseconds = 0;
	struct tm *pCurTime = NULL;
	SYSTEMTIME st;  
	char pBuff[128] = {0};

	EnterCriticalSection(&s_log_mux);
	va_start(arg_ptr, format);
	nArgLen = vsprintf_s(log_buff, sizeof(log_buff), format, arg_ptr);
	va_end(arg_ptr);

	curTime = time(0);
	localtime_s(pCurTime, &curTime);

	GetLocalTime(&st);
	nMilliseconds = st.wMilliseconds;

	sprintf_s(pBuff, "%04d%02d%02d%02d%02d%02d%03ld",
		1900+pCurTime->tm_year,
		pCurTime->tm_mon+1,
		pCurTime->tm_mday,
		pCurTime->tm_hour,
		pCurTime->tm_min,
		pCurTime->tm_sec,
		nMilliseconds);

	fwrite(pBuff, 1, strlen(pBuff), (FILE *)log_fp);
	fwrite(" : ", 1, 3, (FILE *)log_fp);
	fwrite(log_buff, 1, nArgLen, (FILE *)log_fp);
	fwrite("\r\n", 1, 2, (FILE *)log_fp);

	fflush((FILE *)log_fp);
	LeaveCriticalSection(&s_log_mux);
}

static void InitChannelIndex()
{
	int i = 0;
	for (i = 0; i < CHANNEL_NUM; i++)
		s_cha_index[i] = 1;
}

static void CloseAllFile()
{
	if (s_pFileIdx != NULL)
	{
		fclose(s_pFileIdx);
		s_pFileIdx = NULL;
	}

	if (s_pFileMdv != NULL)
	{
		fclose(s_pFileMdv);
		s_pFileMdv = NULL;
	}
}

static  void ReleaseAll()
{
	int i;
	if (s_hThreadTriger != INVALID_HANDLE_VALUE)
	{
		CloseHandle(s_hThreadTriger);
		s_hThreadTriger = INVALID_HANDLE_VALUE;
	}

	if (s_hThreadSender != INVALID_HANDLE_VALUE)
	{
		CloseHandle(s_hThreadSender);
		s_hThreadSender = INVALID_HANDLE_VALUE;
	}

	if (s_hThreadDecoder != INVALID_HANDLE_VALUE)
	{
		CloseHandle(s_hThreadDecoder);
		s_hThreadDecoder = INVALID_HANDLE_VALUE;
	}

	if (s_hEventTriger != INVALID_HANDLE_VALUE)
	{
		CloseHandle(s_hEventTriger);
		s_hEventTriger = INVALID_HANDLE_VALUE;
	}

	for (i=0; i<sizeof(s_hEventSender) / sizeof(HANDLE); i++)
	{
		if (s_hEventSender[i] != INVALID_HANDLE_VALUE)
		{
			CloseHandle(s_hEventSender[i]);
			s_hEventSender[i] = INVALID_HANDLE_VALUE;
		}
	}

	EnterCriticalSection(&s_mutex);
	s_pDataCallbackFunction = NULL;
	LeaveCriticalSection(&s_mutex);

	s_pAlarmCallbackFunction = NULL;
	s_pFileEndCallbackFunction = NULL;

	EnterCriticalSection(&s_file_mux);
	CloseAllFile();
	LeaveCriticalSection(&s_file_mux);
}

static int InitFileIndex()
{
	char strType[5] = {0};
	int nLen = 0;  int nChannel = 0; int prvOffset = 0;
	time_t timeStamp = 0;

	if (s_pFileIdx != NULL)
	{
		// Read idxl
		memset (strType, 0, sizeof(strType));
		fread(strType, 1, 4, s_pFileIdx);
		// Read Totle Len
		fread(&nLen, 1, sizeof(int), s_pFileIdx);
		while (TRUE)
		{
			// Read Head Type
			memset (strType, 0, sizeof(strType));
			if (fread(strType, 1, 4, s_pFileIdx) <= 0)
			{
				break;
			}
			// Channel num
			fread(&nChannel, 1, sizeof(int), s_pFileIdx);
			// time stamp
			fread(&timeStamp, 1, sizeof(time_t), s_pFileIdx);
			// prev i frame offset
			fread(&prvOffset, 1, sizeof(int), s_pFileIdx);
			// Read Data Len
			fseek(s_pFileIdx, 8, SEEK_CUR);
			// 处理数据
			if (memcmp(strType, "ifrm", 4) == 0 && nChannel == 0)
			{
				timeStamp /= 1000;
				s_next_info = (struct s_file_info *)malloc(sizeof(struct s_file_info));
				s_next_info->tm = timeStamp;
				s_next_info->offset = prvOffset;
				s_next_info->next = NULL;
				if (s_begin_info == NULL)
				{
					s_begin_info = s_next_info;
				}
				if (s_cur_info != NULL)
				{
					s_cur_info->next = s_next_info;
				}
				s_cur_info = s_next_info;
			}
		}
		s_cur_info = s_begin_info;
		s_next_info = s_cur_info->next;
		s_tmStart = 0;
		s_bStart = FALSE;

		//s_nInterval = TIMER_INV_M;

		printf("offset = %d\n", s_cur_info->offset);
		s_bReadData = TRUE;
		if (fseek(s_pFileMdv, s_cur_info->offset, SEEK_SET) != 0)
		{
			printf("offset = %d  error\n", s_cur_info->offset);
			assert(FALSE);
		}
		
		return ANALYSE_OK;
	}

	return ANALYSE_FILE_ERR;
}

static void ReleaseFileIndex()
{
	struct s_file_info *info = NULL;
	for (s_cur_info = s_begin_info; s_cur_info != NULL; )
	{
		info = s_cur_info->next;
		free(s_cur_info);
		s_cur_info = info;
	}
	s_begin_info = NULL;
	s_cur_info = NULL;
	s_next_info = NULL;
}

static void ResumeThreadOnce(HANDLE hThread)
{
	while (ResumeThread(hThread) > 0)
		;
}

static void SuspendThreadOnce(HANDLE hThread)
{
	if (SuspendThread(hThread) > 0)
	{
		ResumeThread(hThread);
	}
}

static void ReadIFrame(int nLen, int nChannel, time_t timeStamp)
{
	struct s_media_info *p_info = NULL;
	struct s_media_info *p_info_queue = NULL;

	fread(s_pData, 1, nLen, s_pFileMdv);
	p_info = (struct s_media_info *)malloc(sizeof(struct s_media_info));
	if (p_info != NULL)
	{
		p_info->i_channel = nChannel;
		p_info->time_stamp = timeStamp;
		p_info->p_data = (char *)malloc(nLen);
		memcpy(p_info->p_data, s_pData, nLen);
		p_info->i_len = nLen;
		p_info->p_parm = s_pDataParam;
		p_info->p_next = NULL;
	}
	EnterCriticalSection(&s_mutex);
	for (p_info_queue = s_data_queue; p_info_queue->p_next != NULL; )
	{
		p_info_queue = p_info_queue->p_next;
	}
	p_info_queue->p_next = p_info;
	LeaveCriticalSection(&s_mutex);

	SetEvent(s_hEventDecoder[0]);
}

static void ReadPFrame(int nLen, int nChannel, time_t timeStamp)
{
	struct s_media_info *p_info = NULL;
	struct s_media_info *p_info_queue = NULL;

	fread(s_pData, 1, nLen, s_pFileMdv);
	if (s_nStep != TIME_STEP/* && CheckPFrame(nChannel)*/)
	{
		p_info = (struct s_media_info *)malloc(sizeof(struct s_media_info));
		if (p_info != NULL)
		{
			p_info->i_channel = nChannel;
			p_info->time_stamp = timeStamp;
			p_info->p_data = (char *)malloc(nLen);
			memcpy(p_info->p_data, s_pData, nLen);
			p_info->i_len = nLen;
			p_info->p_parm = s_pDataParam;
			p_info->p_next = NULL;
		}
		EnterCriticalSection(&s_mutex);
		for (p_info_queue = s_data_queue; p_info_queue->p_next != NULL; )
		{
			p_info_queue = p_info_queue->p_next;
		}
		p_info_queue->p_next = p_info;
		LeaveCriticalSection(&s_mutex);

		SetEvent(s_hEventDecoder[0]);
	}
}

static BOOL ReadBFrame(int nLen, double *f_latitude, double *f_longitude, double *f_speed, double *f_angle)
{
	short b_type = 0;
	short b_unknow = 0;
	GPSData_s gps_data = {0};

	fread(&b_type, 1, sizeof(short), s_pFileMdv);
	fread(&b_unknow, 1, sizeof(short), s_pFileMdv);

	if (b_type == 1)
	{
		fseek(s_pFileMdv, nLen - 4, SEEK_CUR);
	}
	else
	{
		fread(&gps_data, 1, sizeof(GPSData_s), s_pFileMdv);
		*f_latitude = (double)gps_data.cLatitudeDegree + (double)gps_data.cLatitudeCent / 60 + (double)gps_data.lLatitudeSec / 3600;
		*f_longitude = (double)gps_data.cLongitudeDegree + (double)gps_data.cLongitudeCent / 60 + (double)gps_data.lLongitudeSec / 3600;
		*f_speed = (double)gps_data.usSpeed / 100;
		*f_angle = (double)gps_data.usGpsAngle / 100;

		return TRUE;
	}

	return FALSE;
}

static void ReadFrameInfo(int *pChannel, time_t *pTimeStamp, int *pOffset, int *pLen)
{
	// Channel num
	fread(pChannel, 1, sizeof(int), s_pFileMdv);
	// time stamp
	fread(pTimeStamp, 1, sizeof(time_t), s_pFileMdv);
	*pTimeStamp /= 1000;
	// prev i frame offset
	fread(pOffset, 1, sizeof(int), s_pFileMdv);
	// Read Data Len
	fread(pLen, 1, sizeof(int), s_pFileMdv);
}

static unsigned __stdcall OnTriger(void *param)
{
	while (TRUE)
	{	
		if (s_tmStart == 0)
		{
			SetEvent(s_hEventSender[0]);
			continue;
		}
		/// 毫秒级定时器
		if (WaitForSingleObject(s_hEventTriger, s_nInterval) == WAIT_OBJECT_0)
		{
			break;
		}

		if (s_tmStart != 0)
		{
			s_tmStart += s_nStep;
		}

		SetEvent(s_hEventSender[0]);
	}

	return 0;
}

static unsigned __stdcall OnSender(void *param)
{
	int i = 0;
	char strType[5] = {0};
	int nChannel = 0; int prvOffset = 0; int nLen = 0;
	time_t timeStamp = 0;
	double f_latitude;
	double f_longitude;
	double f_speed;
	double f_angle;

	while(TRUE)
	{
		if (WaitForMultipleObjects(sizeof(s_hEventSender)/sizeof(HANDLE), s_hEventSender, FALSE, INFINITE) == (WAIT_OBJECT_0 + 1))
		{
			/// 退出线程
			break;
		}

		while (TRUE)
		{
			__try
			{
				EnterCriticalSection(&s_file_mux);
				// Read Head Type
				if (s_bReadData && s_pFileMdv != NULL)
				{
					memset (strType, 0, sizeof(strType));
					if (fread(strType, 1, 4, s_pFileMdv) > 0)
					{
						ReadFrameInfo(&nChannel, &timeStamp, &prvOffset, &nLen);
					}
					else
					{
						SuspendThreadOnce(s_hThreadTriger);
						s_tmStart = 0;
						s_bReadData = TRUE;
						assert(s_pFileEndCallbackFunction != NULL);
						if (s_pFileEndCallbackFunction != NULL)
						{
							s_pFileEndCallbackFunction(s_pFileEndParam);
						}
						//continue;
						LeaveCriticalSection(&s_file_mux);
						break;
					}
					s_bReadData = FALSE;
					if (s_tmStart == 0)
					{
						s_tmStart = timeStamp;
					}
				}
				else if (((s_tmStart >= timeStamp && s_tmStart > 0) || s_tmStart == 0 || (s_tmSeekTime != 0 && timeStamp < s_tmSeekTime)) 
					&& (s_pFileMdv != NULL))
				{
					//s_tmStart = timeStamp;
					if (memcmp(strType, "ifrm", 4) == 0)
					{
						ReadIFrame(nLen, nChannel, timeStamp);
					}
					else if (memcmp(strType, "pfrm", 4) == 0)
					{
						ReadPFrame(nLen, nChannel, timeStamp);
					}
					else if (memcmp(strType, "bfrm", 4) == 0)
					{
						if (ReadBFrame(nLen, &f_latitude, &f_longitude, &f_speed, &f_angle) == TRUE)
						{
							assert (s_pAlarmCallbackFunction != NULL);
							if (s_pAlarmCallbackFunction != NULL)
							{
								s_pAlarmCallbackFunction(f_latitude, f_longitude, f_speed, f_angle, timeStamp, s_pAlarmParam);
							}
						}
					}
					else if (memcmp(strType, "afrm", 4) == 0)
					{
						fread(s_pData, 1, nLen, s_pFileMdv);
						assert (s_pDataCallbackFunction != NULL);
						if (s_pDataCallbackFunction != NULL && (s_nStep == 1 && s_nInterval == TIMER_INV_M))
						{
							s_pDataCallbackFunction(ANALYSE_AUDIO, nChannel, timeStamp, s_pData, nLen, TRUE, s_pDataParam);
						}	
					}
					else
					{
						assert(FALSE);
						printf ("assert(FALSE);  ----1\n");
						//abort();
						s_bReadData = TRUE;
						LeaveCriticalSection(&s_file_mux);
						break;
					}
					s_bReadData = TRUE;
				}
				else
				{
					LeaveCriticalSection(&s_file_mux);
					break;
				}
				LeaveCriticalSection(&s_file_mux);
			} 
			__except( EXCEPTION_EXECUTE_HANDLER ) { 
				assert (FALSE);
				printf ("assert(FALSE);  ----2\n");
				LeaveCriticalSection(&s_file_mux);
			}
		}

		if (s_tmSeekTime != 0)
		{
			s_tmSeekTime = 0;
			s_tmStart = timeStamp;
		}
	}

	return 0;
}

static BOOL IsNeedShow(int nChannel)
{
	BOOL b_result = TRUE;
	if (s_nStep == 2)
	{
		b_result = ((s_cha_index[nChannel] % 2) == 1);
	}
	else if (s_nStep == 4)
	{
		b_result = ((s_cha_index[nChannel] % 8) == 1);
	}
	else if (s_nStep == 8)
	{
		b_result = ((s_cha_index[nChannel] % 16) == 1);
	}
	s_cha_index[nChannel]++;

	return b_result;
}

static unsigned __stdcall OnDecoder(void *param)
{
	int i = 0;
	struct s_media_info *data_info = NULL;
	struct s_media_info *tmp_info = NULL;
	static BOOL b_need_show = TRUE;
	while(TRUE)
	{
		if (WaitForMultipleObjects(sizeof(s_hEventDecoder)/sizeof(HANDLE), s_hEventDecoder, FALSE, INFINITE) == (WAIT_OBJECT_0 + 1))
		{
			/// 退出线程
			break;
		}

		EnterCriticalSection(&s_mutex);
		for (data_info = s_data_queue->p_next; data_info != NULL; )
		{
			tmp_info = data_info->p_next;
			if (s_pDataCallbackFunction != NULL)
			{
				if (s_tmSeekTime == 0)
				{
					b_need_show = IsNeedShow(data_info->i_channel);
					s_pDataCallbackFunction(ANALYSE_VIDEO, data_info->i_channel, data_info->time_stamp, data_info->p_data, data_info->i_len, b_need_show, s_pDataParam);
				}
				else
				{
					s_pDataCallbackFunction(ANALYSE_VIDEO, data_info->i_channel, s_tmSeekTime, data_info->p_data, data_info->i_len, FALSE, s_pDataParam);
				}
			}
			free(data_info->p_data);
			free(data_info);
			data_info = tmp_info;
		}
		s_data_queue->p_next = NULL;
		LeaveCriticalSection(&s_mutex);
	}

	return 0;
}

int WINAPI ANALYSE_GetFileInfo(char *pFilePath, AVFILEInfo_S *pFileInfo)
{
	char strType[5] = {0};
	int nLen = 0;  int nChannel = 0; int nTotleChan = 0;
	time_t timeStamp = 0;
	time_t startTime = 0;
	time_t endTime = 0;

	FILE *pFileIdx = NULL;
	fopen_s(&pFileIdx, pFilePath, "rb+");
	if (pFileIdx == NULL)
	{
		assert(FALSE);
		return ANALYSE_FILE_ERR;
	}

	memset (pFileInfo, 0, sizeof(AVFILEInfo_S));

	if (pFileIdx != NULL)
	{
		// Read idxl
		memset (strType, 0, sizeof(strType));
		fread(strType, 1, 4, pFileIdx);
		// Read Totle Len
		fread(&nLen, 1, sizeof(int), pFileIdx);
		while (TRUE)
		{
			// Read Head Type
			memset (strType, 0, sizeof(strType));
			if (fread(strType, 1, 4, pFileIdx) <= 0)
			{
				break;
			}
			// Channel num
			fread(&nChannel, 1, sizeof(int), pFileIdx);
			if (nChannel > nTotleChan)
			{
				nTotleChan = nChannel;
			}
			// time stamp
			fread(&timeStamp, 1, sizeof(time_t), pFileIdx);
			// Read Data Len
			fseek(pFileIdx, 12, SEEK_CUR);
			// 处理数据
			if (memcmp(strType, "ifrm", 4) == 0 && nChannel == 0)
			{
				endTime = timeStamp / 1000;
				if (startTime == 0)
				{
					startTime = endTime;
				}
			}
		}
	}

	pFileInfo->tmBeg = startTime;
	pFileInfo->tmEnd = endTime;
	pFileInfo->nChannel = nTotleChan + 1;

	fclose(pFileIdx);

	return ANALYSE_OK;
}

int WINAPI ANALYSE_Initialize()
{
	int i;
	if (s_bSenderRun == TRUE)
	{
		return ANALYSE_OK;
	}

	if (log_fp == NULL)
	{
		CreateLog();
	}

	InitializeCriticalSection (&s_mutex);
	InitializeCriticalSection (&s_file_mux);
	InitChannelIndex();

	s_bSenderRun = TRUE;
	s_pData = (char *)malloc(1024 * 1024);
	if (s_pData == NULL)
	{
		assert(FALSE);
		return ANALYSE_MEM_ERR;
	}
	/// 创建线程
	s_hThreadTriger = (HANDLE)_beginthreadex(NULL, 0, OnTriger, NULL, CREATE_SUSPENDED, NULL);
	if (s_hThreadTriger == INVALID_HANDLE_VALUE)
	{
		assert(FALSE);
		ReleaseAll();
		return ANALYSE_ERROR;
	}

	s_hThreadSender = (HANDLE)_beginthreadex(NULL, 0, OnSender, NULL, CREATE_SUSPENDED, NULL);
	if (s_hThreadTriger == INVALID_HANDLE_VALUE)
	{
		assert(FALSE);
		ReleaseAll();
		return ANALYSE_ERROR;
	}

	s_hThreadDecoder = (HANDLE)_beginthreadex(NULL, 0, OnDecoder, NULL, CREATE_SUSPENDED, NULL);
	if (s_hThreadDecoder == INVALID_HANDLE_VALUE)
	{
		assert(FALSE);
		ReleaseAll();
		return ANALYSE_ERROR;
	}

	/// 创建事件
	s_hEventTriger = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (s_hEventTriger == INVALID_HANDLE_VALUE)
	{
		assert(FALSE);
		ReleaseAll();
		return ANALYSE_ERROR;
	}

	s_hEventSender[0] = INVALID_HANDLE_VALUE;
	s_hEventSender[1] = INVALID_HANDLE_VALUE;

	for (i=0; i<sizeof(s_hEventSender) / sizeof(HANDLE); i++)
	{
		s_hEventSender[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (s_hEventSender[i] == INVALID_HANDLE_VALUE)
		{
			assert(FALSE);
			ReleaseAll();
			return ANALYSE_ERROR;
		}
	}

	s_hEventDecoder[0] = INVALID_HANDLE_VALUE;
	s_hEventDecoder[1] = INVALID_HANDLE_VALUE;

	for (i=0; i<sizeof(s_hEventDecoder) / sizeof(HANDLE); i++)
	{
		s_hEventDecoder[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (s_hEventDecoder[i] == INVALID_HANDLE_VALUE)
		{
			assert(FALSE);
			ReleaseAll();
			return ANALYSE_ERROR;
		}
	}

	s_data_queue = (struct s_media_info *)malloc(sizeof(struct s_media_info));
	s_data_queue->p_data = NULL;
	s_data_queue->p_next = NULL;

	ResumeThreadOnce (s_hThreadSender);
	ResumeThreadOnce (s_hThreadDecoder);

	ReleaseLog();

	return ANALYSE_OK;
}

void WINAPI ANALYSE_UnInitialize()
{
	ANALYSE_Stop(1);
	ANALYSE_CloseFile(1);
	if (s_bSenderRun == TRUE)
	{
		SetEvent(s_hEventSender[1]);
		SetEvent(s_hEventTriger);

		ReleaseAll();

		if (s_pData != NULL)
		{
			free(s_pData);
			s_pData = NULL;
		}

		if (s_data_queue != NULL)
		{
			free (s_data_queue);
			s_data_queue = NULL;
		}

		DeleteCriticalSection (&s_mutex);
		DeleteCriticalSection (&s_file_mux);
		s_bSenderRun = FALSE;
	}
}

void WINAPI ANALYSE_SetDataCallback(long lFileHandle, AnalyseDataCallback pCallbackFunction, void *pParam)
{
	assert(pCallbackFunction != NULL);
	s_pDataCallbackFunction = pCallbackFunction;
	s_pDataParam = pParam;
}

void WINAPI ANALYSE_SetAlarmCallback(long lFileHandle, AnalyseAlarmCallback pCallbackFunction, void *pParam)
{
	assert(pCallbackFunction != NULL);
	s_pAlarmCallbackFunction = pCallbackFunction;
	s_pAlarmParam = pParam;
}

void WINAPI ANALYSE_SetFileEndCallback(long lFileHandle, AnalyseFileEndCallback pCallbackFunction, void *pParam)
{
	assert(pCallbackFunction != NULL);
	s_pFileEndCallbackFunction = pCallbackFunction;
	s_pFileEndParam = pParam;
}


void WINAPI ANALYSE_Start(long lFileHandle)
{
	if (s_bStart == FALSE && s_pFileMdv != NULL)
	{
		EnterCriticalSection(&s_file_mux);
		s_bStart = TRUE;
		fseek(s_pFileMdv, s_begin_info->offset, SEEK_SET);
		LeaveCriticalSection(&s_file_mux);
	}

	ResumeThreadOnce (s_hThreadTriger);
	s_bPause = FALSE;
}

void WINAPI ANALYSE_Stop(long lFileHandle)
{	
	struct s_media_info *data_info = NULL;
	struct s_media_info *tmp_info = NULL;

	if (s_bStart)
	{
		SuspendThreadOnce(s_hThreadTriger);

		EnterCriticalSection(&s_mutex);
		for (data_info = s_data_queue->p_next; data_info != NULL; )
		{
			tmp_info = data_info->p_next;
			free(data_info->p_data);
			free(data_info);
			data_info = tmp_info;
		}
		s_data_queue->p_next = NULL;

		s_tmStart = 0;
		s_bReadData = TRUE;
		s_bStart = FALSE;
		LeaveCriticalSection(&s_mutex);
	}
}

long WINAPI ANALYSE_OpenFile(char *pFilePath)
{
	char pFilePathIdx[256] = {0};
	char *p = strstr(pFilePath, ".");

	if (p == NULL)
	{
		assert(FALSE);
		return ANALYSE_FILE_ERR;
	}

	if (s_pFileMdv != NULL)
	{
		return s_lFileHandle;
	}

	strncpy_s(pFilePathIdx, sizeof(pFilePathIdx), pFilePath, p - pFilePath);
	strcat_s(pFilePathIdx, sizeof(pFilePathIdx), ".idx");

	EnterCriticalSection(&s_file_mux);

	fopen_s(&s_pFileIdx, pFilePathIdx, "rb+");
	if (s_pFileIdx == NULL)
	{
		assert(FALSE);
		CloseAllFile();
		return ANALYSE_FILE_ERR;
	}

	fopen_s(&s_pFileMdv, pFilePath, "rb+");
	if (s_pFileMdv == NULL)
	{
		assert(FALSE);
		CloseAllFile();
		return ANALYSE_FILE_ERR;
	}
	InitFileIndex();

	LeaveCriticalSection(&s_file_mux);

	return ++s_lFileHandle;
}

void WINAPI ANALYSE_CloseFile(long lFileHandle)
{
	ANALYSE_Stop(0);
	if (s_pFileMdv != NULL)
	{
		//SuspendThreadOnce(s_hThreadTriger);
		EnterCriticalSection(&s_file_mux);
		ReleaseFileIndex();
		CloseAllFile();
		LeaveCriticalSection(&s_file_mux);

		s_pDataParam = NULL;
		s_pAlarmParam = NULL;
		s_pFileEndParam = NULL;
	}
}

void WINAPI ANALYSE_Pause(long lFileHandle)
{
	if (s_bStart)
	{
		SuspendThreadOnce(s_hThreadTriger);
		s_bPause = TRUE;
	}
}

void WINAPI ANALYSE_Resume(long lFileHandle)
{
	if (s_bStart)
	{
		ResumeThreadOnce (s_hThreadTriger);
		s_bPause = FALSE;
	}
}

static void CalcSpeed()
{
	InitChannelIndex();
	switch (s_nSpeed)
	{
	case 0: s_nStep = 1; s_nInterval = 1; break;// normal
	case 1: s_nStep = 2; s_nInterval = 1; break;// fast x2
	case 2: s_nStep = 4; s_nInterval = 1; break;// fast x4
	case 3: s_nStep = 8; s_nInterval = 1; break;// fast x8
	case 4: s_nStep = 16; s_nInterval = 1; break;// fast x16
	case -1: s_nStep = 1; s_nInterval = 2; break;// slow x2
	case -2: s_nStep = 1; s_nInterval = 4; break;// slow x4
	case -3: s_nStep = 1; s_nInterval = 8; break;// slow x8
	case -4: s_nStep = 1; s_nInterval = 16; break;// slow x8
	default: s_nStep = 1; s_nInterval = 1; break; // default
	}
}

void WINAPI ANALYSE_Fast(long lFileHandle)
{
	if (s_bStart)
	{
		SuspendThreadOnce(s_hThreadTriger);
		if (s_nSpeed < 4) 
		{
			s_nSpeed++;
			CalcSpeed();
		}
		s_tmStart = 0;
		if (!s_bPause)
		{
			ResumeThreadOnce (s_hThreadTriger);
		}
	}
}

void WINAPI ANALYSE_Slow(long lFileHandle)
{
	if (s_bStart)
	{
		SuspendThreadOnce(s_hThreadTriger);
		if (s_nSpeed > -4)
		{
			s_nSpeed--;
			CalcSpeed();
		}
		s_tmStart = 0;
		if (!s_bPause)
		{
			ResumeThreadOnce (s_hThreadTriger);
		}
	}
}

void WINAPI ANALYSE_Normal(long lFileHandle)
{
	if (s_bStart)
	{
		SuspendThreadOnce(s_hThreadTriger);
		s_tmStart = 0;
		s_nSpeed = 0;
		CalcSpeed();
		ResumeThreadOnce (s_hThreadTriger);
	}
}

int WINAPI ANALYSE_Seek(long lFileHandle, time_t timeStamp)
{
	struct s_media_info *data_info = NULL;
	struct s_media_info *tmp_info = NULL;

	if (s_bStart)
	{
		SuspendThreadOnce(s_hThreadTriger);

		EnterCriticalSection(&s_mutex);
		for (data_info = s_data_queue->p_next; data_info != NULL; )
		{
			tmp_info = data_info->p_next;
			free(data_info->p_data);
			free(data_info);
			data_info = tmp_info;
		}
		s_data_queue->p_next = NULL;
		LeaveCriticalSection(&s_mutex);

		for (s_cur_info = s_begin_info; s_cur_info->next != NULL; s_cur_info = s_cur_info->next)
		{
			if (abs((int)(s_cur_info->tm - timeStamp)) < 4000)
			{
				fseek(s_pFileMdv, s_cur_info->offset, SEEK_SET);
				s_bReadData = TRUE;
				s_tmStart = 0;
				s_tmSeekTime = timeStamp;
				if (!s_bPause)
				{
					ResumeThreadOnce (s_hThreadTriger);
				}
				return ANALYSE_OK;
			}
		}

		if (!s_bPause)
		{
			ResumeThreadOnce (s_hThreadTriger);
		}
	}

	return ANALYSE_PARAM_ERR;
}