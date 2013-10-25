#pragma once
#include "windows.h"
#include "codec.h"

class CClientDemoImpl
{
public:
	CClientDemoImpl(CListBox *listBox);
	~CClientDemoImpl();

public:
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

private:
	static DWORD WINAPI ReciveProc(LPVOID lParam)
	{
		CClientDemoImpl *pThis = (CClientDemoImpl *)lParam;
		pThis->OnRecive();
		return 0;
	}
	void OnRecive();
	int MakeCommand(char bCmd, char *pData, int nLen, char *pBody);
	int CheckNum(char *pData, int nLen);
	void ShowInfo(const wchar_t *format, ...);

private:
	wchar_t m_dataBuff[8092];
	SOCKET m_sock;
	BOOL m_bLogin;
	BOOL m_bRun;
	HANDLE m_recvThread;
	CListBox *m_listBox;
	Codec m_codec;
	Codec m_vodCodec;
};