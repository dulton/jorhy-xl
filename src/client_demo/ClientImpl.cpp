#include "stdafx.h"
#include "XlType.h"
#include "ClientImpl.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

CClientImpl::CClientImpl()
{
	m_bLogin = FALSE;
	m_sock = INVALID_SOCKET;
	m_recvThread = INVALID_HANDLE_VALUE;
	m_videoListBox = NULL;
	m_configListBox = NULL;

	WSADATA wsaData; 
	WSAStartup(MAKEWORD(2,2), &wsaData);
}

CClientImpl::~CClientImpl()
{
	WSACleanup();
}

void CClientImpl::Login(DWORD dwAddr, int nPort, const char *pUser, const char *pPasswd)
{
	if (!m_bLogin)
	{
		m_sock = socket(AF_INET, SOCK_STREAM, 0);
		SOCKADDR_IN addr;
		addr.sin_addr.S_un.S_addr = htonl(dwAddr);
		addr.sin_family = AF_INET;
		addr.sin_port = htons(nPort);

		if(SOCKET_ERROR == connect(m_sock, (SOCKADDR*)&addr, sizeof(SOCKADDR_IN)))
		{
			closesocket(m_sock);
			m_sock = INVALID_SOCKET;
			return;
		}

		struct LoginBody {
			CmdHeader head;
			LoginReq data;
			CmdTail tail;
		} loginBody;
		memset(&loginBody, 0, sizeof(LoginBody)) ;
		memcpy(loginBody.data.user, pUser, strlen(pUser));
		memcpy(loginBody.data.passWord, pPasswd, strlen(pPasswd));
		MakeCommand(xl_login, (char *)&loginBody.data, sizeof(LoginReq), (char *)&loginBody);
		send(m_sock, (char *)&loginBody, sizeof(LoginBody), 0);

		m_bRun = TRUE;
		m_recvThread = CreateThread(NULL, 0, ReciveProc, this, 0, NULL);
	}
}

void CClientImpl::Logout(const char *pUser, const char *pPasswd)
{
	struct LogoutBody {
		CmdHeader head;
		LoginReq data;
		CmdTail tail;
	} logoutBody;
	memset(&logoutBody, 0, sizeof(LogoutBody)) ;
	memcpy(logoutBody.data.user, pUser, strlen(pUser));
	memcpy(logoutBody.data.passWord, pPasswd, strlen(pPasswd));
	MakeCommand(xl_logout, (char *)&logoutBody.data, sizeof(LogoutReq), (char *)&logoutBody);
	send(m_sock, (char *)&logoutBody, sizeof(LogoutBody), 0);

	CloseHandle(m_recvThread);
	WaitForSingleObject(m_recvThread, INFINITE);
	m_bLogin = FALSE;
}

void CClientImpl::RealPlay(const char *pDevId, char chId, HWND hWnd)
{
	struct RealPlayBody {
		CmdHeader head;
		RealPlayReq data;
		CmdTail tail;
	} realPlayBody;
	memset(&realPlayBody, 0, sizeof(RealPlayBody));
	memcpy(realPlayBody.data.hostId, pDevId, strlen(pDevId));
	realPlayBody.data.channel = chId;

	MakeCommand(xl_real_play, (char *)&realPlayBody.data, sizeof(RealPlayReq), (char *)&realPlayBody);
	send(m_sock, (char *)&realPlayBody, sizeof(RealPlayBody), 0);
	m_codec.StartRMDisplay(hWnd, chId );
}

void CClientImpl::RealStop(const char *pDevId, char chId)
{
	struct RealStopBody {
		CmdHeader head;
		RealStopReq data;
		CmdTail tail;
	} realStopBody;
	memset(&realStopBody, 0, sizeof(RealStopBody));
	memcpy(realStopBody.data.hostId, pDevId, strlen(pDevId));
	realStopBody.data.channel = chId;

	MakeCommand(xl_real_stop, (char *)&realStopBody.data, sizeof(RealStopReq), (char *)&realStopBody);
	send(m_sock, (char *)&realStopBody, sizeof(RealStopBody), 0);
	m_codec.StopRMDisplay(chId);
}

void CClientImpl::VodPlay(const char *pDevId, char chId, HWND hWnd, __time64_t start, __time64_t end)
{
	struct VodPlayBody {
		CmdHeader head;
		VodPlayReq data;
		CmdTail tail;
	} vodPlayBody;
	memset(&vodPlayBody, 0, sizeof(VodPlayBody));
	memcpy(vodPlayBody.data.hostId, pDevId, strlen(pDevId));
	vodPlayBody.data.channel = chId;
	vodPlayBody.data.tmStartTime = start;
	vodPlayBody.data.tmEndTime = end;

	MakeCommand(xl_vod_play, (char *)&vodPlayBody.data, sizeof(VodPlayReq), (char *)&vodPlayBody);
	send(m_sock, (char *)&vodPlayBody, sizeof(VodPlayBody), 0);
	m_vodCodec.StartPlayback(hWnd, chId );
}

void CClientImpl::VodStop(const char *pDevId, char chId)
{
	struct VodStopBody {
		CmdHeader head;
		VodStopReq data;
		CmdTail tail;
	} vodStopBody;
	memset(&vodStopBody, 0, sizeof(VodStopBody));
	memcpy(vodStopBody.data.hostId, pDevId, strlen(pDevId));
	vodStopBody.data.channel = chId;

	MakeCommand(xl_vod_stop, (char *)&vodStopBody.data, sizeof(VodStopReq), (char *)&vodStopBody);
	send(m_sock, (char *)&vodStopBody, sizeof(VodStopBody), 0);
	m_vodCodec.StopPlayback(chId);
}

void CClientImpl::SetTime(__time64_t sysTime)
{
	struct SysTimeBody {
		CmdHeader head;
		SetTimeReq data;
		CmdTail tail;
	} sysTimeBody;
	sysTimeBody.data.systime = sysTime;
	MakeCommand(xl_set_time, (char *)&sysTimeBody.data, sizeof(SetTimeReq), (char *)&sysTimeBody);
	send(m_sock, (char *)&sysTimeBody, sizeof(SysTimeBody), 0);
}

void CClientImpl::GetDevInfo(const char *pDevId)
{
	struct DevInfoBody {
		CmdHeader head;
		HostInfoReq data;
		CmdTail tail;
	} devInfoBody;
	memset(&devInfoBody, 0, sizeof(DevInfoBody));
	memcpy(devInfoBody.data.hostId, pDevId, strlen(pDevId));
	MakeCommand(xl_get_devinfo, (char *)&devInfoBody.data, sizeof(HostInfoReq), (char *)&devInfoBody);
	send(m_sock, (char *)&devInfoBody, sizeof(DevInfoBody), 0);
}

void CClientImpl::GetAlarmInfo(const char *pDevId, time_t tmStart, time_t tmEnd)
{
	struct AlarmInfoBody {
		CmdHeader head;
		AlarmInfoReq data;
		CmdTail tail;
	} alarmInfoBody;
	memset(&alarmInfoBody, 0, sizeof(AlarmInfoBody));
	alarmInfoBody.data.tmStart = tmStart;
	alarmInfoBody.data.tmEnd = tmEnd;
	memcpy(alarmInfoBody.data.hostId, pDevId, strlen(pDevId));
	MakeCommand(xl_start_real_alarm, (char *)&alarmInfoBody.data, sizeof(AlarmInfoReq), (char *)&alarmInfoBody);
	send(m_sock, (char *)&alarmInfoBody, sizeof(AlarmInfoBody), 0);
}

void CClientImpl::StopAlarmInfo(const char *pDevId)
{
	struct AlarmStopBody {
		CmdHeader head;
		AlarmStopReq data;
		CmdTail tail;
	} alarmStopBody;
	memset(&alarmStopBody, 0, sizeof(AlarmStopBody));
	memcpy(alarmStopBody.data.hostId, pDevId, strlen(pDevId));
	MakeCommand(xl_stop_real_alarm, (char *)&alarmStopBody.data, sizeof(AlarmStopReq), (char *)&alarmStopBody);
	send(m_sock, (char *)&alarmStopBody, sizeof(AlarmStopBody), 0);
}

void CClientImpl::GetLogInfo(const char *pDevId, time_t tmStart, time_t tmEnd)
{
	struct LogInfoBody {
		CmdHeader head;
		GetLogInfoReq data;
		CmdTail tail;
	} logInfoBody;
	memset(&logInfoBody, 0, sizeof(LogInfoBody));
	memcpy(logInfoBody.data.hostId, pDevId, strlen(pDevId));
	logInfoBody.data.tmStart = tmStart;
	logInfoBody.data.tmEnd = tmEnd;
	MakeCommand(xl_get_loginfo, (char *)&logInfoBody.data, sizeof(GetLogInfoReq), (char *)&logInfoBody);
	send(m_sock, (char *)&logInfoBody, sizeof(LogInfoBody), 0);
}

void CClientImpl::GetDvrTotleInfo()
{
	struct DvrTotleInfoBody {
		CmdHeader head;
		CmdTail tail;
	} dvrTotleInfoBody;
	memset(&dvrTotleInfoBody, 0, sizeof(DvrTotleInfoBody));
	MakeCommand(xl_get_totle_dvr_info, NULL, 0, (char *)&dvrTotleInfoBody);
	send(m_sock, (char *)&dvrTotleInfoBody, sizeof(DvrTotleInfoBody), 0);
}

void CClientImpl::GetUserTotleInfo()
{
	struct UserTotleInfoBody {
		CmdHeader head;
		CmdTail tail;
	} userTotleInfoBody;
	memset(&userTotleInfoBody, 0, sizeof(UserTotleInfoBody));
	MakeCommand(xl_get_totle_user_info, NULL, 0, (char *)&userTotleInfoBody);
	send(m_sock, (char *)&userTotleInfoBody, sizeof(UserTotleInfoBody), 0);
}

void CClientImpl::GetDvrList(int nType, long lDepartmentId)
{
	struct DvrListBody {
		CmdHeader head;
		DvrListReq data;
		CmdTail tail;
	} dvrListBody;
	memset(&dvrListBody, 0, sizeof(DvrListReq));
	dvrListBody.data.nType = nType;
	dvrListBody.data.lDepartmentID = lDepartmentId;
	MakeCommand(xl_get_dvr_list, (char *)&dvrListBody.data, sizeof(DvrListReq), (char *)&dvrListBody);
	send(m_sock, (char *)&dvrListBody, sizeof(DvrListBody), 0);
}

void CClientImpl::GetUserList(int nType)
{
	struct UserListBody {
		CmdHeader head;
		UserType data;
		CmdTail tail;
	} userListBody;
	memset(&userListBody, 0, sizeof(UserListBody));
	userListBody.data.nType = nType;
	MakeCommand(xl_get_user_list, (char *)&userListBody.data, sizeof(UserType), (char *)&userListBody);
	send(m_sock, (char *)&userListBody, sizeof(UserListBody), 0);
}

int CClientImpl::MakeCommand(char bCmd, char *pData, int nLen, char *pBody)
{
	//填充头信息
	CmdHeader *pHeader = (CmdHeader *)pBody;
	pHeader->beginCode = 0xFF;
	pHeader->version = 0x0A;
	pHeader->flag = 0x00;
	pHeader->cmd = bCmd;
	pHeader->length = nLen;
	//填充数据区
	if (nLen > 0)
		memcpy(pBody + sizeof(CmdHeader), pData, nLen);
	//填充尾信息
	CmdTail *pTail = (CmdTail *)(pBody + sizeof(CmdHeader) + nLen);
	pTail->verify = CheckNum(pBody, sizeof(CmdHeader) + nLen);
	pTail->endCode = 0xFE;

	return 0;
}

int CClientImpl::CheckNum(char *pData, int nLen)
{
	int nCheckNum = 0xFE;
	for (int i=0; i<nLen; ++i)
		nCheckNum += pData[i];

	return (nCheckNum % 256);
}

void CClientImpl::OnRecive()
{
	FD_SET fdRead;
	timeval timeVal;
	timeVal.tv_sec = 1;
	timeVal.tv_usec = 0;
	char *pRecvBuff = new char[1024 * 1024];
	while (m_bRun)
	{
		FD_ZERO(&fdRead);
		FD_SET(m_sock, &fdRead);
		if(0 == select(0, &fdRead, NULL, NULL, &timeVal))
		{
			FD_CLR(m_sock, &fdRead);
			continue;
		}
		FD_CLR(m_sock, &fdRead);

		int retValue = recv(m_sock,pRecvBuff, sizeof(CmdHeader), 0);
		if(SOCKET_ERROR == retValue || 0 == retValue)
		{
			closesocket(m_sock);
			m_bLogin = FALSE;	
			break;
		}
		ASSERT(retValue == sizeof(CmdHeader));
		CmdHeader *pHeader = (CmdHeader *)pRecvBuff;

		int nTotleLen = pHeader->length + sizeof(CmdTail);
		int nRecvLen = 0;
		while (nTotleLen > 0)
		{
			nRecvLen = recv(m_sock, pRecvBuff + retValue, nTotleLen, 0);
			if(SOCKET_ERROR == nRecvLen || 0 == nRecvLen)
			{
				closesocket(m_sock);
				m_bLogin = FALSE;	
				break;
			}
			retValue += nRecvLen;
			nTotleLen -= nRecvLen;
		}
		ASSERT(retValue == pHeader->length + sizeof(CmdTail) + sizeof(CmdHeader));
		switch (pHeader->cmd & 0xFF)
		{
		case xl_real_play: case xl_real_stop: case xl_vod_play: case xl_vod_stop:
		case xl_start_real_alarm: case xl_stop_real_alarm: case xl_set_time: case xl_get_loginfo:
		case xl_get_alraminfo: case xl_get_devinfo: case xl_login: case xl_logout:
			ProcessVideoData(pHeader, pRecvBuff);
			break;
		case xl_get_totle_dvr_info: case xl_get_totle_user_info: case xl_get_dvr_list: case xl_get_user_list:
			ProcessConfigData(pHeader, pRecvBuff);
			break;
		default:
			break;
		}
	}
	delete pRecvBuff;
}

void CClientImpl::ProcessVideoData(CmdHeader *pHeader,  char *pRecvBuff)
{
	if((pHeader->cmd & 0xFF) == xl_real_play)
	{
		RealPlayReq *pReps = (RealPlayReq *)(pRecvBuff + sizeof(CmdHeader));
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		m_codec.InputStream((PBYTE)pRecvBuff + sizeof(CmdHeader), pHeader->length);
	}
	else if ((pHeader->cmd & 0xFF) == xl_vod_play)
	{
		RealPlayReq *pReps = (RealPlayReq *)(pRecvBuff + sizeof(CmdHeader));
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		m_vodCodec.InputStream((PBYTE)pRecvBuff + sizeof(CmdHeader), pHeader->length);
	}
	else
	{
		ShowVideoInfo(L"cmd=%d len=%d\n", pHeader->cmd & 0xFF, pHeader->length);
		switch (pHeader->cmd)
		{
		case xl_logout:
			m_bRun = FALSE;
			closesocket(m_sock);
			break;
		case xl_set_time:
			break;
		case xl_get_devinfo:
			{
				HostInfoResp *pReps = (HostInfoResp *)(pRecvBuff + sizeof(CmdHeader));
				wchar_t defInfo[256] = {0};
				wsprintf(defInfo, L"host hostId=%S, vehicle=%S, phone=%S, channelNum=%d\n", 
					pReps->hostId, pReps->vehicleNum, pReps->phoneNum, pReps->totalChannels & 0xFF);
				ShowVideoInfo(defInfo);
			}
			break;
		case xl_get_loginfo:
			{
				GetLogInfoResp *pReps = (GetLogInfoResp *)(pRecvBuff + sizeof(CmdHeader));
				wchar_t logInfo[256] = {0};
				wsprintf(logInfo, L"state=%I64d, time_stamp=%d\n", pReps->bStatus & 0xFF, pReps->tmTime);
				ShowVideoInfo(logInfo);
			}
			break;
		case xl_get_alraminfo:
			{
				AlarmInfoResp *pReps = (AlarmInfoResp *)(pRecvBuff + sizeof(CmdHeader));
				CString alarmInfo;
				alarmInfo.Format(L"Alarm=%I64d, Latitude=%f, Longitude=%f, GPSSpeed=%f, Speed=%f, TimeStamp=%I64d\n", 
					pReps->bAlarm & 0xFF, pReps->gps.dLatitude, pReps->gps.dLongitude, pReps->gps.dGPSSpeed, pReps->speed, pReps->tmTimeStamp);
				m_videoListBox->InsertString(0, alarmInfo);
			}
			break;
		default:
			break;
		}
	}
}

void CClientImpl::ProcessConfigData(CmdHeader *pHeader,  char *pRecvBuff)
{
	ShowVideoInfo(L"cmd=%d len=%d\n", pHeader->cmd & 0xFF, pHeader->length);
	switch (pHeader->cmd)
	{
	case xl_get_totle_dvr_info:
		{
			DVRTotleInfo *pResp = (DVRTotleInfo *)(pRecvBuff + sizeof(CmdHeader));
			wchar_t totleDvrInfo[256] = {0};
			wsprintf(totleDvrInfo, L"uiTotal=%d, uiUnRegister=%d uiOnline=%d, uiStock=%d\n", pResp->uiTotal, pResp->uiUnRegister, pResp->uiOnline, pResp->uiStock);
			ShowConfigInfo(totleDvrInfo);
		}
		break;
	case xl_get_totle_user_info:
		{
			UserTotleInfo *pResp = (UserTotleInfo *)(pRecvBuff + sizeof(CmdHeader));
			wchar_t totleUserInfo[256] = {0};
			wsprintf(totleUserInfo, L"uiTotal=%d uiOnline=%d\n", pResp->uiTotal, pResp->uiOnline);
			ShowConfigInfo(totleUserInfo);
		}
		break;
	case xl_get_dvr_list:
		{
			DVRInfo *pResp = (DVRInfo *)(pRecvBuff + sizeof(CmdHeader));
			wchar_t dvrInfo[256] = {0};
			wsprintf(dvrInfo, L"szEquipmentID=%S szVehicleNO=%S szPhone=%S\n", pResp->szEquipmentID, pResp->szVehicleNO, pResp->szPhone);
			ShowConfigInfo(dvrInfo);
		}
		break;
	case xl_get_user_list:
		{
			UserInfo *pResp = (UserInfo *)(pRecvBuff + sizeof(CmdHeader));
			wchar_t userInfo[256] = {0};
			wsprintf(userInfo, L"lUserID=%d szName=%S szAccountName=%S\n", pResp->lUserID, pResp->szName, pResp->szAccountName);
			ShowConfigInfo(userInfo);
		}
	}
}

void CClientImpl::ShowVideoInfo(const wchar_t *format, ...)
{
	static int sNum = 0;
	int nArgLen = 0;
	wchar_t dataBuff[8092] = {0};
	va_list arg_ptr;
	va_start(arg_ptr, format);
	nArgLen = vswprintf(dataBuff, 8092, format, arg_ptr);
	va_end(arg_ptr);
	wsprintf(m_dataBuff, L"Info %d : %s", sNum++, dataBuff);
	if (m_videoListBox != NULL)
		m_videoListBox->InsertString(0, m_dataBuff);
}
void CClientImpl::ShowConfigInfo(const wchar_t *format, ...)
{
	static int sNum = 0;
	int nArgLen = 0;
	wchar_t dataBuff[8092] = {0};
	va_list arg_ptr;
	va_start(arg_ptr, format);
	nArgLen = vswprintf(dataBuff, 8092, format, arg_ptr);
	va_end(arg_ptr);
	wsprintf(m_dataBuff, L"Info %d : %s", sNum++, dataBuff);
	if (m_configListBox != NULL)
		m_configListBox->InsertString(0, m_dataBuff);
}