#include "BroadCastListener.h"
#include "XLSyncType.h"
#include "HostManager.h"

CBroadCastListener::CBroadCastListener()
: m_bStart(false)
{
	WSADATA wsaData;          //指向WinSocket信息结构的指针
	WSAStartup(MAKEWORD( 2, 2), &wsaData );

	memset(&m_param, 0, sizeof(j_thread_parm));
}

CBroadCastListener::~CBroadCastListener()
{
	WSACleanup();
}

j_result_t CBroadCastListener::StartService(int nPort)
{
	if (!m_bStart)
	{
		m_broadCastListener = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		BOOL bReuseaddr = TRUE;
		setsockopt(m_broadCastListener, SOL_SOCKET , SO_REUSEADDR, (const char*)&bReuseaddr, sizeof(BOOL));

		SOCKADDR_IN sin = {0};
		sin.sin_family = AF_INET;
		sin.sin_port = htons(nPort);
		sin.sin_addr.s_addr = INADDR_ANY;
		if(bind( m_broadCastListener, (SOCKADDR FAR *)&sin, sizeof(sin)) != 0)
		{
			J_OS::LOGINFO("Can't bind socket to local port! Program stop.\n");//初始化失败返回-1
			return J_SOCKET_ERROR;
		}
		m_bStart = true;
		m_param.entry = CBroadCastListener::OnWorkFunc;
		m_param.data = this;
		m_workTread.Create(m_param);
	}

	return J_OK;
}

j_result_t CBroadCastListener::StopService()
{
	if (m_bStart)
	{
		m_bStart = false;
		m_workTread.Release();
	}
	return J_OK;
}

void CBroadCastListener::OnWork()
{
	//BroadCastFtpInfo info = {0};
	//strcpy(info.ftp_addr, "127.0.0.1");
	//info.ftp_port = 8504;
	//strcpy(info.host_id, "111");
	//strcpy(info.user_name, "joo");
	//strcpy(info.pass_word, "1");
	//JoHostManager->UpdateHostInfo(info);
	SOCKADDR_IN	sockAddrInClient = {0};
	int iAddrLen = sizeof(sockAddrInClient);
	int nSockAddrSize;
	int nRecvSize;
	while(m_bStart)
	{
		nSockAddrSize = sizeof ( SOCKADDR_IN );
		memset (&sockAddrInClient, 0, sizeof(sockAddrInClient));
		memset (m_recvBuff, 0, sizeof(sockAddrInClient));
		nRecvSize = recvfrom (m_broadCastListener, m_recvBuff, 1400, 0, (SOCKADDR FAR *) &sockAddrInClient, &nSockAddrSize);
		if(nRecvSize == SOCKET_ERROR)
		{
			j_sleep(1000);
			continue;
		}
		else
		{
			BroadCastFtpInfo *pInfo = (BroadCastFtpInfo *)m_recvBuff;
			//J_OS::LOGINFO("BroadCast Recive %s ", pInfo->host_id);
			JoHostManager->UpdateHostInfo(*pInfo);
		}
	}
}