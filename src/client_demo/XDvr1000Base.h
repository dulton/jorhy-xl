#pragma once

#ifndef _XLDLLHEADER
#define DLLHEADER _declspec(dllimport)
#else
#define DLLHEADER _declspec(dllexport)
#endif


#ifdef __cplusplus
extern "C"
{
	//添加删除， 输入视频流
	DLLHEADER LONG WINAPI Initial();
	DLLHEADER BOOL WINAPI Release(LONG lHandle);
	DLLHEADER BOOL WINAPI InputStream(LONG lHandle, PBYTE pBuffer, ULONG ulSize);

	//实时视频查看
	DLLHEADER BOOL WINAPI StartRMDisplay(LONG lHandle, HWND hWnd, UINT uChannelID);	
	DLLHEADER BOOL WINAPI StopRMDisplay(LONG lHandle, UINT uChannelID);

	//远程回放
	DLLHEADER BOOL WINAPI StartPlayback(LONG lHandle, HWND hWnd, UINT uChannelID);
	DLLHEADER BOOL WINAPI StopPlayback(LONG lHandle, UINT uChannelID);

	//本地录像, 导出录像
	DLLHEADER BOOL WINAPI StartRec(LONG lHandle, CString& strFileName);
	DLLHEADER BOOL WINAPI SetRecChannel(LONG lHandle, UINT uChannelID);
	DLLHEADER BOOL WINAPI StopRec(LONG lHandle);

	//文件播放
	DLLHEADER BOOL WINAPI StartPlayFile(LONG lHandle, CString& strFileName);
	DLLHEADER BOOL WINAPI SetPlayFileChannel(LONG lHandle, UINT uChannelID, HWND hWnd);
	DLLHEADER BOOL WINAPI StopPlayFile(LONG lHandle);
}


#endif