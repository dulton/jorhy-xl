#include "windows.h"
#include "stdio.h"
#include "FileReader.h"

#include <iostream>                              //包含头文件
#include <stdlib.h>

#ifdef _DEBUG
#pragma comment (lib, "Debug\\FileReader.lib")
#else
#pragma comment (lib, "Release\\FileReader.lib")
#endif

#define  CHANNEL 0

void WINAPI data_callback(long lType, int nCh, time_t timeStamp, char *pData, int nLen, BOOL bShow, void *parm)
{
	printf("type = %d channel = %d tm = %I64d len = %d\n", lType, nCh, timeStamp, nLen);
}

void WINAPI alarm_callback(double fLatitude, double fLongitude, double nSpeed, double angle, time_t _tm, void *parm)
{
	//printf("latitude = %f longitude = %f speed = %f, %f, %I64d\n", fLatitude, fLongitude, nSpeed, angle, _tm);
}

void WINAPI file_end_callback(void *parm)
{
	printf("file end\n");
}

BOOL b_run = true;
BOOL b_run_init = true;
AVFILEInfo_S channelInfo = {0};
int main(int argc, char **argv)
{
	while (b_run_init)
	{
		b_run = true;
		ANALYSE_GetFileInfo("D:\\JoorhyPrj\\dll\\mdv_03_18\\H20140318-083323NP00FF_12035.idx", &channelInfo);
		ANALYSE_Initialize();
		ANALYSE_Initialize();
		ANALYSE_Initialize();
		ANALYSE_SetDataCallback(1, data_callback, (void *)1);
		ANALYSE_SetAlarmCallback(1, alarm_callback, (void *)2);
		ANALYSE_SetFileEndCallback(1, file_end_callback, (void *)3);
		ANALYSE_OpenFile("D:\\JoorhyPrj\\dll\\mdv_03_18\\H20140318-083323NP00FF_12035.mdv");
		while(b_run)
		{
			int ch = getchar();
			switch (ch)
			{
			case 's':
				ANALYSE_Start(1);
				break;
			case 't':
				ANALYSE_Stop(1);
				break;
			case 'p':
				ANALYSE_Pause(1);
				break;
			case 'r':
				ANALYSE_Resume(1);
				break;
			case 'f':
				ANALYSE_Fast(1);
				break;
			case 'l':
				ANALYSE_Slow(1);
				break;
			case 'm':
				ANALYSE_Normal(1);
				break;
			case 'k':
				ANALYSE_Seek(1, 1395102488734);
				break;
			case 'n':
				ANALYSE_CloseFile(1);
				ANALYSE_CloseFile(1);
				ANALYSE_CloseFile(1);
				ANALYSE_GetFileInfo("E:\\讯利视频\\H20131014-164008NP20FF_1401076.idx", &channelInfo);
				ANALYSE_OpenFile("E:\\讯利视频\\H20131014-164008NP20FF_1401076.mdv");
				ANALYSE_OpenFile("E:\\讯利视频\\H20131014-164008NP20FF_1401076.mdv");
				ANALYSE_OpenFile("E:\\讯利视频\\H20131014-164008NP20FF_1401076.mdv");
				ANALYSE_Start(1);
				break;
			case 'o':
				ANALYSE_OpenFile("E:\\讯利视频\\mdv_03_18\\H20140318-082723NP00FF_12035.mdv");
				break;
			case 'c':
				ANALYSE_CloseFile(1);
				break;
			case 'e':
				b_run = FALSE;
				break;
			}
		}
		//ANALYSE_Stop(1);
		ANALYSE_CloseFile(1);
		ANALYSE_CloseFile(1);
		ANALYSE_CloseFile(1);
		ANALYSE_UnInitialize();
		ANALYSE_UnInitialize();
		ANALYSE_UnInitialize();
		
		int ch = getchar();
	}


	return 0;
}