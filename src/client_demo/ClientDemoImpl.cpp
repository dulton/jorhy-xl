#include "stdafx.h"
#include "XlType.h"
#include "ClientDemoImpl.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

CClientDemoImpl::CClientDemoImpl(CListBox *listBox)
{
	m_bLogin = FALSE;
	m_sock = INVALID_SOCKET;
	m_recvThread = INVALID_HANDLE_VALUE;
	m_listBox = listBox;

	WSADATA wsaData; 
	WSAStartup(MAKEWORD(2,2), &wsaData);

	//m_codec = Initial();
}

CClientDemoImpl::~CClientDemoImpl()
{
	//Release(m_codec);

	WSACleanup();
}

void CClientDemoImpl::Login(DWORD dwAddr, int nPort, const char *pUser, const char *pPasswd)
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

void CClientDemoImpl::Logout(const char *pUser, const char *pPasswd)
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

void CClientDemoImpl::RealPlay(const char *pDevId, char chId, HWND hWnd)
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

void CClientDemoImpl::RealStop(const char *pDevId, char chId)
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

void CClientDemoImpl::VodPlay(const char *pDevId, char chId, HWND hWnd, __time64_t start, __time64_t end)
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

void CClientDemoImpl::VodStop(const char *pDevId, char chId)
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

void CClientDemoImpl::SetTime(__time64_t sysTime)
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

void CClientDemoImpl::GetDevInfo(const char *pDevId)
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

void CClientDemoImpl::GetAlarmInfo(const char *pDevId, time_t tmStart, time_t tmEnd)
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

void CClientDemoImpl::StopAlarmInfo(const char *pDevId)
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

void CClientDemoImpl::GetLogInfo(const char *pDevId, time_t tmStart, time_t tmEnd)
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

int CClientDemoImpl::MakeCommand(char bCmd, char *pData, int nLen, char *pBody)
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

int CClientDemoImpl::CheckNum(char *pData, int nLen)
{
	int nCheckNum = 0xFE;
	for (int i=0; i<nLen; ++i)
		nCheckNum += pData[i];

	return (nCheckNum % 256);
}

void CClientDemoImpl::OnRecive()
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
		if((pHeader->cmd & 0xFF) == xl_real_play)
		{
			RealPlayReq *pReps = (RealPlayReq *)(pRecvBuff + sizeof(CmdHeader));
			//wchar_t hostId[64] = {0};
			//wsprintf(hostId, L"%s", pReps->hostId);
			//ShowInfo(L"host hostId = %s, channel = %d", hostId, pReps->channel & 0xFF);
			AFX_MANAGE_STATE(AfxGetStaticModuleState());
			m_codec.InputStream((PBYTE)pRecvBuff + sizeof(CmdHeader), pHeader->length);
		}
		else if ((pHeader->cmd & 0xFF) == xl_vod_play)
		{
			RealPlayReq *pReps = (RealPlayReq *)(pRecvBuff + sizeof(CmdHeader));
			//wchar_t hostId[64] = {0};
			//wsprintf(hostId, L"%s", pReps->hostId);
			//ShowInfo(L"host hostId = %s, channel = %d", hostId, pReps->channel & 0xFF);
			AFX_MANAGE_STATE(AfxGetStaticModuleState());
			m_vodCodec.InputStream((PBYTE)pRecvBuff + sizeof(CmdHeader), pHeader->length);
		}
		else
		{
			ShowInfo(L"cmd=%d len=%d\n", pHeader->cmd & 0xFF, pHeader->length);
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
					ShowInfo(defInfo);
				}
				break;
			case xl_get_loginfo:
				{
					GetLogInfoResp *pReps = (GetLogInfoResp *)(pRecvBuff + sizeof(CmdHeader));
					wchar_t logInfo[256] = {0};
					wsprintf(logInfo, L"state=%I64d, time_stamp=%d\n", pReps->bStatus & 0xFF, pReps->tmTime);
					ShowInfo(logInfo);
				}
				break;
			case xl_get_alraminfo:
				{
					AlarmInfoResp *pReps = (AlarmInfoResp *)(pRecvBuff + sizeof(CmdHeader));
					CString alarmInfo;
					alarmInfo.Format(L"Alarm=%I64d, Latitude=%f, Longitude=%f, GPSSpeed=%f, Speed=%f, TimeStamp=%I64d\n", 
						pReps->bAlarm & 0xFF, pReps->gps.dLatitude, pReps->gps.dLongitude, pReps->gps.dGPSSpeed, pReps->speed, pReps->tmTimeStamp);
					m_listBox->InsertString(0, alarmInfo);
				}
				break;
			default:
				break;
			}
		}

	}
	delete pRecvBuff;
}

void CClientDemoImpl::ShowInfo(const wchar_t *format, ...)
{
	static int sNum = 0;
	int nArgLen = 0;
	wchar_t dataBuff[8092] = {0};
	va_list arg_ptr;
	va_start(arg_ptr, format);
	nArgLen = vswprintf(dataBuff, 8092, format, arg_ptr);
	va_end(arg_ptr);
	wsprintf(m_dataBuff, L"Info %d : %s", sNum++, dataBuff);
	m_listBox->InsertString(0, m_dataBuff);
}