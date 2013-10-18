#include "stdafx.h"
#include "XlType.h"
#include "ClientDemoImpl.h"
#include "XDvr1000Base.h"

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

void CClientDemoImpl::Login(const char *pUser, const char *pPasswd)
{
	if (!m_bLogin)
	{
		m_sock = socket(AF_INET, SOCK_STREAM, 0);
		SOCKADDR_IN addr;
		addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
		addr.sin_family = AF_INET;
		addr.sin_port = htons(8502);

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
		retValue = recv(m_sock, pRecvBuff + sizeof(CmdHeader), pHeader->length + sizeof(CmdTail), 0);
		if(SOCKET_ERROR == retValue || 0 == retValue)
		{
			closesocket(m_sock);
			m_bLogin = FALSE;	
			break;
		}
		ASSERT(retValue == pHeader->length + sizeof(CmdTail));
		if((pHeader->cmd & 0xFF) == xl_real_play)
		{
			RealPlayReq *pReps = (RealPlayReq *)(pRecvBuff + sizeof(CmdHeader));
			wchar_t hostId[64] = {0};
			wsprintf(hostId, L"%s", pReps->hostId);
			ShowInfo(L"host hostId = %s, channel = %d", hostId, pReps->channel & 0xFF);
			AFX_MANAGE_STATE(AfxGetStaticModuleState());
			m_codec.InputStream((PBYTE)pRecvBuff + sizeof(CmdHeader), pHeader->length);
		}
		ShowInfo(L"cmd=%d len=%d\n", pHeader->cmd & 0xFF, pHeader->length);

		switch (pHeader->cmd)
		{
		case xl_logout:
			m_bRun = FALSE;
			closesocket(m_sock);
			break;
		default:
			break;
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