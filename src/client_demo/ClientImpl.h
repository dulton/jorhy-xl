#pragma once
#include "windows.h"
#include "codec.h"
#include "XlType.h"

class CClientImpl
{
public:
	CClientImpl();
	~CClientImpl();

public:
	///视频界面
	void Login(DWORD dwAddr, int nPort, const char *pUser, const char *pPasswd);
	void Logout(const char *pUser, const char *pPasswd);
	void RealPlay(const char *pDevId, char chId, HWND hWnd);
	void RealStop(const char *pDevId, char chId);
	void VodPlay(const char *pDevId, char chId, HWND hWnd, __time64_t start, __time64_t end);
	void VodStop(const char *pDevId, char chId);
	void SetTime(__time64_t sysTime);
	void GetDevInfo(const char *pDevId);
	void GetLogInfo(const char *pDevId, time_t tmStart, time_t tmEnd);
	void GetAlarmInfo(const char *pDevId, time_t tmStart, time_t tmEnd);
	void StopAlarmInfo(const char *pDevId);

	///配置界面
	void GetDvrTotleInfo();
	void GetUserTotleInfo();
	void GetDvrList(int nType, long lDepartmentId);
	void GetUserList(int nType);

public:
	void SetVideoListBox(CListBox *pListBox) { m_videoListBox = pListBox; }
	void SetConfigListBox(CListBox *pListBox) { m_configListBox = pListBox; }

private:
	static DWORD WINAPI ReciveProc(LPVOID lParam)
	{
		CClientImpl *pThis = (CClientImpl *)lParam;
		pThis->OnRecive();
		return 0;
	}
	void OnRecive();
	void ProcessVideoData(CmdHeader *pHeader,  char *pRecvBuff);
	void ProcessConfigData(CmdHeader *pHeader,  char *pRecvBuff);
	int MakeCommand(char bCmd, char *pData, int nLen, char *pBody);
	int CheckNum(char *pData, int nLen);
	void ShowVideoInfo(const wchar_t *format, ...);
	void ShowConfigInfo(const wchar_t *format, ...);

private:
	wchar_t m_dataBuff[8092];
	SOCKET m_sock;
	BOOL m_bLogin;
	BOOL m_bRun;
	HANDLE m_recvThread;
	CListBox *m_videoListBox;
	CListBox *m_configListBox;
	Codec m_codec;
	Codec m_vodCodec;
};