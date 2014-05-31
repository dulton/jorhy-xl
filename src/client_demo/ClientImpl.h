#pragma once
#include "windows.h"
#include "codec.h"
#include "XlClientType.h"
#include "ConfigDlg.h"

class CClientImpl
{
public:
	CClientImpl();
	~CClientImpl();

public:
	void HeartBreat();
	///视频界面
	void Login(DWORD dwAddr, int nPort, const char *pUser, const char *pPasswd, int nFlag);
	void Logout(const char *pUser, const char *pPasswd);
	void StartAlarm(const char *pEquipmentId);
	void StopAlarm(const char *pEquipmentId);
	void RealPlay(const char *pDevId, char chId, HWND hWnd);
	void RealStop(const char *pDevId, char chId);
	void VodPlay(const char *pDevId, char chId, GUID sessionId, HWND hWnd, __time64_t start, __time64_t end);
	void VodStop(const char *pDevId, char chId, GUID sessionId);
	void SetTime(__time64_t sysTime);
	void GetDevInfo(const char *pDevId);
	void GetLogInfo(const char *pDevId, time_t tmStart, time_t tmEnd);
	void GetAlarmInfo(const char *pDevId, time_t tmStart, time_t tmEnd);
	void StopAlarmInfo(const char *pDevId);
	void UploadFile(const char *pFilePath, const char *pDevId);

	///配置界面
	void GetDvrTotleInfo();
	void GetUserTotleInfo();
	void GetDvrList(int nType, long lDepartmentId);
	void GetUserList(int nType, long lDepartmentId);
	void GetDepartmentList();
	void AddDvrInfo(const DVRInfo &dvrInfo);
	void GetDvrInfo(const DVRInfo &dvrInfo);
	void ModifyDvrInfo(const DVRInfo &dvrInfo);
	void DeleteDvrInfo(const DVRInfo &dvrInfo);
	void AddUserInfo(const UserInfo &userInfo);
	void GetUserInfo(const UserInfo &userInfo);
	void ModifyUserInfo(const UserInfo &userInfo);
	void DeleteUserInfo(const UserInfo &userInfo);
	void AddDepartmentInfo(const DepartmentInfo &departmentInfo);
	void GetDepartmentInfo(const DepartmentInfo &departmentInfo);
	void ModifyDepartmentInfo(const DepartmentInfo &departmentInfo);
	void DeleteDepartmentInfo(const DepartmentInfo &departmentInfo);

public:
	void SetVideoListBox(CListBox *pListBox) { m_videoListBox = pListBox; }
	void SetConfigListBox(CListBox *pListBox) { m_configListBox = pListBox; }
	void SetConfigDlg(CConfigDlg *pConfigDlg) { m_configDlg = pConfigDlg; }

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
	CConfigDlg *m_configDlg;
	Codec m_codec;
	Codec m_vodCodec;
};