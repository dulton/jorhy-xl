#pragma once

#pragma comment(lib, "AMESDK.lib")
#include "AMESDK.H"

class Codec
{
private:
	//实时视频和录像回放
	DEVICE_HANDLE m_szRMChannel[8], m_szRecChannel[8], m_szRecFileChannel[8];
	
	//本地录像和导出文件
	BOOL m_bRecChannel[8];
	HANDLE m_lLocalRec;
	CCriticalSection m_csLocalRec;


	HCRYPTPROV m_hProv;
	HCRYPTHASH m_hHash;	

	//录像文件播放
	HANDLE m_lRecFile,m_lTrdRecFile;
	BOOL m_bNeedExit;
	static DWORD WINAPI RecPlayProc(LPVOID lParam);

public:
	Codec();
	virtual ~Codec();

	//输入视频
	BOOL InputStream(PBYTE pBuffer, ULONG ulSize);

	//实时浏览
	BOOL StartRMDisplay(HWND hWnd, UINT uChannelID);	
	BOOL StopRMDisplay(UINT uChannelID);

	//远程回放
	BOOL StartPlayback(HWND hWnd, UINT uChannelID);
	BOOL StopPlayback(UINT uChannelID);

	//本地录像, 导出录像
	BOOL StartRec(CString& strFileName);
	BOOL SetRecChannel(UINT uChannelID);
	BOOL StopRec();

	//文件播放
	BOOL StartPlayFile(CString& strFileName);
	BOOL SetPlayFileChannel(UINT uChannelID, HWND hWnd);
	BOOL StopPlayFile();
};