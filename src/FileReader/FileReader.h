#ifndef __FILE_ANALYSE_H_
#define __FILE_ANALYSE_H_

#ifdef LIBANALYSE_EXPORTS
#define ANALYSE_API __declspec(dllexport)
#else
#define ANALYSE_API  __declspec(dllimport)
#endif

/// 错误码
#define ANALYSE_OK						 0
#define ANALYSE_ERROR				-1
#define ANALYSE_FILE_ERR			-2
#define ANALYSE_MEM_ERR		-3
#define ANALYSE_PARAM_ERR	-4

/// 数据类型
#define	ANALYSE_VIDEO			1
#define	ANALYSE_AUDIO			2

typedef struct _tagAVFILEInfo
{
	time_t tmBeg;																//文件开始时间（毫秒）
	time_t	tmEnd;																//文件总时长（毫秒）
	int nChannel;																	//通道信息
	int	nFrameRate;															//帧率信息
}AVFILEInfo_S, *LPAVFILEInfo_S;

typedef struct _tagAVFILEGPSInfo_S
{ 
	int bGpsValid;		//GPS信息是否有效
	int cSpeedUnit;	//速度单位，公里或者英里		1表示公里，2表示英里 
	unsigned short usGpsAngle;		//gps夹角
	unsigned short usSpeed;			//速度			
	double dbJingDu;	//经度		
	double dbWeiDu;		//纬度
	int	nReserve[4];	//保留参数
}AVFILEGPSInfo_S;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/// 回调函数
typedef void (WINAPI *AnalyseDataCallback)( long lType, int nChannel, time_t timeStamp, char *pData, int nLen, int bShow, void *pParam);
typedef void (WINAPI *AnalyseAlarmCallback)(double fLatitude, double fLongitude, double fSpeed, double fGpsAngle, time_t timeStamp, void *pParam);
typedef void (WINAPI *AnalyseFileEndCallback)(void *pParam);

/// 接口
ANALYSE_API int WINAPI ANALYSE_Initialize();
ANALYSE_API void WINAPI ANALYSE_UnInitialize();

ANALYSE_API int WINAPI ANALYSE_GetFileInfo(char *pFilePath, AVFILEInfo_S *pFileInfo);

ANALYSE_API void WINAPI ANALYSE_SetDataCallback(long lFileHandle, AnalyseDataCallback pCallbackFunction, void *pParam);
ANALYSE_API void WINAPI ANALYSE_SetAlarmCallback(long lFileHandle, AnalyseAlarmCallback pCallbackFunction, void *pParam);
ANALYSE_API void WINAPI ANALYSE_SetFileEndCallback(long lFileHandle, AnalyseFileEndCallback pCallbackFunction, void *pParam);

ANALYSE_API long WINAPI ANALYSE_OpenFile(char *pFilePath);
ANALYSE_API void WINAPI ANALYSE_CloseFile(long lFileHandle);

ANALYSE_API void WINAPI ANALYSE_Start(long lFileHandle);
ANALYSE_API void WINAPI ANALYSE_Stop(long lFileHandle);

ANALYSE_API void WINAPI ANALYSE_Pause(long lFileHandle);
ANALYSE_API void WINAPI ANALYSE_Resume(long lFileHandle);

ANALYSE_API void WINAPI ANALYSE_Fast(long lFileHandle);
ANALYSE_API void WINAPI ANALYSE_Slow(long lFileHandle);
ANALYSE_API void WINAPI ANALYSE_Normal(long lFileHandle);

ANALYSE_API int WINAPI ANALYSE_Seek(long lFileHandle, time_t timeStamp);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //~__FILE_ANALYSE_H_