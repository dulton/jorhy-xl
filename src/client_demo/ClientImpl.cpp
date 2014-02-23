#include "stdafx.h"
#include "XlClientType.h"
#include "ClientImpl.h"
#include "ConfigDlg.h"

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
	m_configDlg = NULL;

	WSADATA wsaData; 
	WSAStartup(MAKEWORD(2,2), &wsaData);
}

CClientImpl::~CClientImpl()
{
	WSACleanup();
}

void CClientImpl::Login(DWORD dwAddr, int nPort, const char *pUser, const char *pPasswd, int nFlag)
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
		memcpy(loginBody.data.szAccountName, pUser, strlen(pUser));
		memcpy(loginBody.data.szPassword, pPasswd, strlen(pPasswd));
		loginBody.data.nForced = nFlag;
		MakeCommand(xlc_login, (char *)&loginBody.data, sizeof(LoginReq), (char *)&loginBody);
		send(m_sock, (char *)&loginBody, sizeof(LoginBody), 0);

		m_bRun = TRUE;
		m_recvThread = CreateThread(NULL, 0, ReciveProc, this, 0, NULL);
	}
}

void CClientImpl::Logout(const char *pUser, const char *pPasswd)
{
	struct LogoutBody {
		CmdHeader head;
		CmdTail tail;
	} logoutBody;
	memset(&logoutBody, 0, sizeof(LogoutBody)) ;
	MakeCommand(xlc_logout, NULL, 0, (char *)&logoutBody);
	send(m_sock, (char *)&logoutBody, sizeof(LogoutBody), 0);

	CloseHandle(m_recvThread);
	WaitForSingleObject(m_recvThread, INFINITE);
	m_bLogin = FALSE;
}

void CClientImpl::StartAlarm(const char *pEquipmentId)
{
	RealAlarmBody startAlarmBody;
	memset(&startAlarmBody, 0, sizeof(RealAlarmBody)) ;
	MakeCommand(xlc_start_real_alarm_info, (char *)pEquipmentId, sizeof(DVRId), (char *)&startAlarmBody);
	send(m_sock, (char *)&startAlarmBody, sizeof(RealAlarmBody), 0);
}

void CClientImpl::StopAlarm(const char *pEquipmentId)
{
	RealAlarmBody stoptAlarmBody;
	memset(&stoptAlarmBody, 0, sizeof(RealAlarmBody)) ;
	MakeCommand(xlc_stop_real_alarm_info, (char *)pEquipmentId, sizeof(DVRId), (char *)&stoptAlarmBody);
	send(m_sock, (char *)&stoptAlarmBody, sizeof(RealAlarmBody), 0);
}

void CClientImpl::RealPlay(const char *pDevId, char chId, HWND hWnd)
{
	RealViewBody realPlayBody;
	memset(&realPlayBody, 0, sizeof(RealViewBody));
	memcpy(realPlayBody.data.hostId, pDevId, strlen(pDevId));
	realPlayBody.data.channel = chId;

	MakeCommand(xlc_start_real_view, (char *)&realPlayBody.data, sizeof(RealViewReq), (char *)&realPlayBody);
	send(m_sock, (char *)&realPlayBody, sizeof(RealViewBody), 0);
	m_codec.StartRMDisplay(hWnd, chId );
}

void CClientImpl::RealStop(const char *pDevId, char chId)
{
	RealViewBody realStopBody;
	memset(&realStopBody, 0, sizeof(RealViewBody));
	memcpy(realStopBody.data.hostId, pDevId, strlen(pDevId));
	realStopBody.data.channel = chId;

	MakeCommand(xlc_stop_real_view, (char *)&realStopBody.data, sizeof(RealViewReq), (char *)&realStopBody);
	send(m_sock, (char *)&realStopBody, sizeof(RealViewBody), 0);
	m_codec.StopRMDisplay(chId);
}

void CClientImpl::VodPlay(const char *pDevId, char chId, GUID sessionId, HWND hWnd, __time64_t start, __time64_t end)
{
	struct VodPlayBody {
		CmdHeader head;
		VodPlayReq data;
		CmdTail tail;
	} vodPlayBody;
	memset(&vodPlayBody, 0, sizeof(VodPlayBody));
	memcpy(vodPlayBody.data.hostId, pDevId, strlen(pDevId));
	vodPlayBody.data.sessionID = sessionId;
	vodPlayBody.data.channel = chId;
	vodPlayBody.data.tmStartTime = start;
	vodPlayBody.data.tmEndTime = end;

	MakeCommand(xlc_start_vod_view, (char *)&vodPlayBody.data, sizeof(VodPlayReq), (char *)&vodPlayBody);
	send(m_sock, (char *)&vodPlayBody, sizeof(VodPlayBody), 0);
	m_vodCodec.StartPlayback(hWnd, chId );
}

void CClientImpl::VodStop(const char *pDevId, char chId, GUID sessionId)
{
	struct VodStopBody {
		CmdHeader head;
		VodStopReq data;
		CmdTail tail;
	} vodStopBody;
	memset(&vodStopBody, 0, sizeof(VodStopBody));
	memcpy(vodStopBody.data.hostId, pDevId, strlen(pDevId));
	vodStopBody.data.channel = chId;
	vodStopBody.data.sessionID = sessionId;

	MakeCommand(xlc_stop_vod_view, (char *)&vodStopBody.data, sizeof(VodStopReq), (char *)&vodStopBody);
	send(m_sock, (char *)&vodStopBody, sizeof(VodStopBody), 0);
	m_vodCodec.StopPlayback(chId);
}

void CClientImpl::SetTime(__time64_t sysTime)
{
	//struct SysTimeBody {
	//	CmdHeader head;
	//	SetTimeReq data;
	//	CmdTail tail;
	//} sysTimeBody;
	//sysTimeBody.data.systime = sysTime;
	//MakeCommand(xl_set_time, (char *)&sysTimeBody.data, sizeof(SetTimeReq), (char *)&sysTimeBody);
	//send(m_sock, (char *)&sysTimeBody, sizeof(SysTimeBody), 0);
}

void CClientImpl::GetDevInfo(const char *pDevId)
{
	//struct DevInfoBody {
	//	CmdHeader head;
	//	HostInfoReq data;
	//	CmdTail tail;
	//} devInfoBody;
	//memset(&devInfoBody, 0, sizeof(DevInfoBody));
	//memcpy(devInfoBody.data.hostId, pDevId, strlen(pDevId));
	//MakeCommand(xl_get_devinfo, (char *)&devInfoBody.data, sizeof(HostInfoReq), (char *)&devInfoBody);
	//send(m_sock, (char *)&devInfoBody, sizeof(DevInfoBody), 0);
}

void CClientImpl::GetAlarmInfo(const char *pDevId, time_t tmStart, time_t tmEnd)
{
	struct AlarmInfoBody {
		CmdHeader head;
		AlarmInfoReq data;
		CmdTail tail;
	} alarmInfoBody;
	memset(&alarmInfoBody, 0, sizeof(AlarmInfoBody));
	memcpy(alarmInfoBody.data.hostId, pDevId, strlen(pDevId));
	MakeCommand(xlc_start_real_alarm_info, (char *)&alarmInfoBody.data, sizeof(AlarmInfoReq), (char *)&alarmInfoBody);
	send(m_sock, (char *)&alarmInfoBody, sizeof(AlarmInfoBody), 0);
}

void CClientImpl::StopAlarmInfo(const char *pDevId)
{
	struct AlarmStopBody {
		CmdHeader head;
		AlarmInfoReq data;
		CmdTail tail;
	} alarmStopBody;
	memset(&alarmStopBody, 0, sizeof(AlarmStopBody));
	memcpy(alarmStopBody.data.hostId, pDevId, strlen(pDevId));
	MakeCommand(xlc_stop_real_alarm_info, (char *)&alarmStopBody.data, sizeof(AlarmInfoReq), (char *)&alarmStopBody);
	send(m_sock, (char *)&alarmStopBody, sizeof(AlarmStopBody), 0);
}

void CClientImpl::UploadFile(const char *pFilePath, const char *pDevId)
{
	//开始文件上传
	struct StartUploadBody {
		CmdHeader head;
		CliUploadStart data;
		CmdTail tail;
	} startUploadBody;
	memset(&startUploadBody, 0, sizeof(StartUploadBody));
	memcpy(startUploadBody.data.szID, pDevId, strlen(pDevId));
	memcpy(startUploadBody.data.szFileName, "j_include.h", strlen("j_include.h"));
	MakeCommand(xlc_start_upload, (char *)&startUploadBody.data, sizeof(CliUploadStart), (char *)&startUploadBody);
	send(m_sock, (char *)&startUploadBody, sizeof(StartUploadBody), 0);
	//文件上传
	FILE *pFile = NULL;
	fopen_s(&pFile, pFilePath, "rb+");
	if (pFile)
	{
		char pBuffer[2048] = {0};
		char pData[1024] = {0};
		while (1)
		{
			memset(pData, 0, sizeof(pFile));
			int nReadLen = fread(pData, 1, 1024, pFile);
			if (nReadLen > 0)
			{
				MakeCommand(xlc_upload, (char *)pData, nReadLen, (char *)pBuffer);
				int n = sizeof(CmdHeader) + sizeof(CmdTail) + nReadLen;
				send(m_sock, (char *)pBuffer, sizeof(CmdHeader) + sizeof(CmdTail) + nReadLen, 0);
			}
			else
			{
				fclose(pFile);
				break;
			}
		}
	}
	//停止文件上传
	struct StopUploadBody {
		CmdHeader head;
		CliUploadStop data;
		CmdTail tail;
	} stopUploadBody;
	memset(&stopUploadBody, 0, sizeof(StopUploadBody));
	memcpy(stopUploadBody.data.szID, pDevId, strlen(pDevId));
	//memcpy(stopUploadBody.data.szCheck, pFilePath, strlen(pFilePath));
	MakeCommand(xlc_stop_upload, (char *)&stopUploadBody.data, sizeof(CliUploadStop), (char *)&stopUploadBody);
	send(m_sock, (char *)&stopUploadBody, sizeof(StopUploadBody), 0);
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
	MakeCommand(xlc_get_loginfo, (char *)&logInfoBody.data, sizeof(GetLogInfoReq), (char *)&logInfoBody);
	send(m_sock, (char *)&logInfoBody, sizeof(LogInfoBody), 0);
}

void CClientImpl::GetDvrTotleInfo()
{
	struct DvrTotleInfoBody {
		CmdHeader head;
		CmdTail tail;
	} dvrTotleInfoBody;
	memset(&dvrTotleInfoBody, 0, sizeof(DvrTotleInfoBody));
	MakeCommand(xlc_get_dvr_summary, NULL, 0, (char *)&dvrTotleInfoBody);
	send(m_sock, (char *)&dvrTotleInfoBody, sizeof(DvrTotleInfoBody), 0);
}

void CClientImpl::GetUserTotleInfo()
{
	struct UserTotleInfoBody {
		CmdHeader head;
		CmdTail tail;
	} userTotleInfoBody;
	memset(&userTotleInfoBody, 0, sizeof(UserTotleInfoBody));
	MakeCommand(xlc_get_user_summary, NULL, 0, (char *)&userTotleInfoBody);
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
	MakeCommand(xlc_get_dvr_list, (char *)&dvrListBody.data, sizeof(DvrListReq), (char *)&dvrListBody);
	send(m_sock, (char *)&dvrListBody, sizeof(DvrListBody), 0);
}

void CClientImpl::GetUserList(int nType, long lDepartmentId)
{
	struct UserListBody {
		CmdHeader head;
		UserType data;
		CmdTail tail;
	} userListBody;
	memset(&userListBody, 0, sizeof(UserListBody));
	userListBody.data.nType = nType;
	userListBody.data.lDepartmentID = lDepartmentId;
	MakeCommand(xlc_get_user_list, (char *)&userListBody.data, sizeof(UserType), (char *)&userListBody);
	send(m_sock, (char *)&userListBody, sizeof(UserListBody), 0);
}

void CClientImpl::GetDepartmentList()
{
	struct DepartmentListBody {
		CmdHeader head;
		CmdTail tail;
	} departmentListBody;
	MakeCommand(xlc_get_department_list, NULL, 0, (char *)&departmentListBody);
	send(m_sock, (char *)&departmentListBody, sizeof(DepartmentListBody), 0);
}

void CClientImpl::AddDvrInfo(const DVRInfo &dvrInfo)
{
	struct AddDvrBody {
		CmdHeader head;
		DVRInfo data;
		CmdTail tail;
	} addDvrBody;
	MakeCommand(xlc_add_dvr_info, (char *)&dvrInfo, sizeof(DVRInfo), (char *)&addDvrBody);
	send(m_sock, (char *)&addDvrBody, sizeof(AddDvrBody), 0);
}

void CClientImpl::GetDvrInfo(const DVRInfo &dvrInfo)
{
	struct GetDvrBody {
		CmdHeader head;
		DVRId data;
		CmdTail tail;
	} getDvrBody;
	MakeCommand(xlc_get_dvr_info, (char *)&dvrInfo.szEquipmentID, sizeof(DVRId), (char *)&getDvrBody);
	send(m_sock, (char *)&getDvrBody, sizeof(GetDvrBody), 0);
}

void CClientImpl::ModifyDvrInfo(const DVRInfo &dvrInfo)
{
	struct ModifyDvrBody {
		CmdHeader head;
		DVRInfo data;
		CmdTail tail;
	} modifyDvrBody;
	MakeCommand(xlc_mod_dvr_info, (char *)&dvrInfo, sizeof(DVRInfo), (char *)&modifyDvrBody);
	send(m_sock, (char *)&modifyDvrBody, sizeof(ModifyDvrBody), 0);
}

void CClientImpl::DeleteDvrInfo(const DVRInfo &dvrInfo)
{
	struct DeleteDvrBody {
		CmdHeader head;
		DVRId data;
		CmdTail tail;
	} deleteDvrBody;
	MakeCommand(xlc_del_dvr_info, (char *)&dvrInfo.szEquipmentID, sizeof(DVRId), (char *)&deleteDvrBody);
	send(m_sock, (char *)&deleteDvrBody, sizeof(DeleteDvrBody), 0);
}

void CClientImpl::AddUserInfo(const UserInfo &userInfo)
{
	struct AddUserBody {
		CmdHeader head;
		UserInfo data;
		CmdTail tail;
	} addUserBody;
	MakeCommand(xlc_add_user_info, (char *)&userInfo, sizeof(UserInfo), (char *)&addUserBody);
	send(m_sock, (char *)&addUserBody, sizeof(AddUserBody), 0);
}

void CClientImpl::GetUserInfo(const UserInfo &userInfo)
{
	struct GetUserBody {
		CmdHeader head;
		UserId data;
		CmdTail tail;
	} getUserBody;
	MakeCommand(xlc_get_user_info, (char *)&userInfo.lUserID, sizeof(UserId), (char *)&getUserBody);
	send(m_sock, (char *)&getUserBody, sizeof(GetUserBody), 0);
}

void CClientImpl::ModifyUserInfo(const UserInfo &userInfo)
{
	struct ModifyUserBody {
		CmdHeader head;
		UserInfo data;
		CmdTail tail;
	} modifyUserBody;
	MakeCommand(xlc_mod_user_info, (char *)&userInfo, sizeof(UserInfo), (char *)&modifyUserBody);
	send(m_sock, (char *)&modifyUserBody, sizeof(ModifyUserBody), 0);
}

void CClientImpl::DeleteUserInfo(const UserInfo &userInfo)
{
	struct DeleteUserBody {
		CmdHeader head;
		UserId data;
		CmdTail tail;
	} deleteUserBody;
	MakeCommand(xlc_del_user_info, (char *)&userInfo.lUserID, sizeof(UserId), (char *)&deleteUserBody);
	send(m_sock, (char *)&deleteUserBody, sizeof(DeleteUserBody), 0);
}

void CClientImpl::AddDepartmentInfo(const DepartmentInfo &departmentInfo)
{
	struct AddDepartmentBody {
		CmdHeader head;
		DepartmentInfo data;
		CmdTail tail;
	} addDepartmentBody;
	MakeCommand(xlc_add_department_info, (char *)&departmentInfo, sizeof(DepartmentInfo), (char *)&addDepartmentBody);
	send(m_sock, (char *)&addDepartmentBody, sizeof(AddDepartmentBody), 0);
}

void CClientImpl::GetDepartmentInfo(const DepartmentInfo &departmentInfo)
{
	struct GetDepartmentBody {
		CmdHeader head;
		DepartmentId data;
		CmdTail tail;
	} getDepartmentBody;
	MakeCommand(xlc_get_department_info, (char *)&departmentInfo.lDepartmentID, sizeof(DepartmentId), (char *)&getDepartmentBody);
	send(m_sock, (char *)&getDepartmentBody, sizeof(GetDepartmentBody), 0);
}

void CClientImpl::ModifyDepartmentInfo(const DepartmentInfo &departmentInfo)
{
	struct ModifyDepartmentBody {
		CmdHeader head;
		DepartmentInfo data;
		CmdTail tail;
	} modifyDepartmentBody;
	MakeCommand(xlc_mod_department_info, (char *)&departmentInfo, sizeof(DepartmentInfo), (char *)&modifyDepartmentBody);
	send(m_sock, (char *)&modifyDepartmentBody, sizeof(ModifyDepartmentBody), 0);
}

void CClientImpl::DeleteDepartmentInfo(const DepartmentInfo &departmentInfo)
{
	struct DeleteDepartmentBody {
		CmdHeader head;
		DepartmentId data;
		CmdTail tail;
	} deleteDepartmentBody;
	MakeCommand(xlc_del_department_info, (char *)&departmentInfo.lDepartmentID, sizeof(DepartmentId), (char *)&deleteDepartmentBody);
	send(m_sock, (char *)&deleteDepartmentBody, sizeof(DeleteDepartmentBody), 0);
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
	unsigned int nCheckNum = 0xFE;
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
		case xlc_start_real_view: case xlc_stop_real_view: case xlc_start_vod_view: case xlc_stop_vod_view:
		case xlc_start_real_alarm_info: case xlc_stop_real_alarm_info: /*case xl_set_time: case xl_get_loginfo:*/
		case xlc_get_alarm_info: /*case xl_get_devinfo: */case xlc_login: case xlc_logout: case 5: case 7: case 9:
			ProcessVideoData(pHeader, pRecvBuff);
			break;
		case xlc_get_dvr_summary: case xlc_get_user_summary: case xlc_get_dvr_list: case xlc_get_user_list:
		case xlc_get_department_list: case xlc_get_dvr_info: case xlc_get_user_info: case xlc_get_department_info:
		case xlc_add_dvr_info: case xlc_add_user_info: case xlc_add_department_info:
		case xlc_mod_dvr_info: case xlc_mod_user_info: case xlc_mod_department_info:
		case xlc_del_dvr_info: case xlc_del_user_info: case xlc_del_department_info:
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
	if((pHeader->cmd & 0xFF) == xlc_start_real_view)
	{
		RealViewReq *pReps = (RealViewReq *)(pRecvBuff + sizeof(CmdHeader));
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		int x = CheckNum(pRecvBuff, pHeader->length + sizeof(CmdHeader));
		int y = *(pRecvBuff + pHeader->length + sizeof(CmdHeader)) & 0xFF;
		int z = *(pRecvBuff + pHeader->length + sizeof(CmdHeader) + 1) & 0xFF;
		if (CheckNum(pRecvBuff, pHeader->length + sizeof(CmdHeader)) != (*(pRecvBuff + sizeof(CmdHeader) + pHeader->length) & 0xFF))
			ShowVideoInfo(L"cmd=%d len=%d\n", pHeader->cmd & 0xFF, pHeader->length);
		m_codec.InputStream((PBYTE)pRecvBuff + sizeof(CmdHeader), pHeader->length);
		//ShowVideoInfo(L"cmd=%d len=%d\n", pHeader->cmd & 0xFF, pHeader->length);
	}
	else if ((pHeader->cmd & 0xFF) == xlc_start_vod_view)
	{
		GUID *sessid = (GUID *)(pRecvBuff + sizeof(CmdHeader));
		RealViewReq *pReps = (RealViewReq *)(pRecvBuff + sizeof(CmdHeader) + sizeof(GUID));
		ShowVideoInfo(L"cmd=%d len=%d\n", pHeader->cmd & 0xFF, pHeader->length);
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		m_vodCodec.InputStream((PBYTE)pRecvBuff + sizeof(CmdHeader) + sizeof(GUID), pHeader->length - sizeof(GUID));
	}
	else
	{
		ShowVideoInfo(L"cmd=%d len=%d\n", pHeader->cmd & 0xFF, pHeader->length);
		switch (pHeader->cmd)
		{
		case xlc_logout:
			m_bRun = FALSE;
			closesocket(m_sock);
			break;
		case xlc_login:
			{
				LoginResp *pLoginInfo = (LoginResp *)(pRecvBuff + sizeof(CmdHeader));
				ShowVideoInfo(L"ret=%d\n", pLoginInfo->ret);
			}
			break;
		/*case xl_set_time:
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
			break;*/
		case xlc_start_real_alarm_info:
			{
				AlarmInfoResp *pReps = (AlarmInfoResp *)(pRecvBuff + sizeof(CmdHeader));
				CString alarmInfo;
				alarmInfo.Format(L"HostId=%S, %d, %f, %f, %f, %f, %I64d\n", 
					pReps->hostId, pReps->bAlarm & 0xFF, pReps->gps.dLatitude, pReps->gps.dLongitude, pReps->gps.dGPSSpeed, pReps->speed, pReps->tmTimeStamp);
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
	ShowConfigInfo(L"cmd=%d len=%d\n", pHeader->cmd & 0xFF, pHeader->length);
	switch (pHeader->cmd)
	{
	case xlc_get_dvr_summary:
		{
			DVRTotleInfo *pResp = (DVRTotleInfo *)(pRecvBuff + sizeof(CmdHeader));
			wchar_t totleDvrInfo[256] = {0};
			wsprintf(totleDvrInfo, L"uiTotal=%d, uiUnRegister=%d uiOnline=%d, uiStock=%d\n", pResp->uiTotal, pResp->uiUnRegister, pResp->uiOnline, pResp->uiStock);
			ShowConfigInfo(totleDvrInfo);
		}
		break;
	case xlc_get_user_summary:
		{
			UserTotleInfo *pResp = (UserTotleInfo *)(pRecvBuff + sizeof(CmdHeader));
			wchar_t totleUserInfo[256] = {0};
			wsprintf(totleUserInfo, L"uiTotal=%d uiOnline=%d\n", pResp->uiTotal, pResp->uiOnline);
			ShowConfigInfo(totleUserInfo);
		}
		break;
	case xlc_get_dvr_list:
		{
			DVRInfo *pResp = (DVRInfo *)(pRecvBuff + sizeof(CmdHeader));
			wchar_t dvrInfo[256] = {0};
			wsprintf(dvrInfo, L"szEquipmentID=%S szVehicleNO=%S szPhone=%S\n", pResp->szEquipmentID, pResp->szVehicleNO, pResp->szPhone);
			ShowConfigInfo(dvrInfo);
		}
		break;
	case xlc_get_user_list:
		{
			UserInfo *pResp = (UserInfo *)(pRecvBuff + sizeof(CmdHeader));
			wchar_t userInfo[256] = {0};
			wsprintf(userInfo, L"lUserID=%d szName=%S szAccountName=%S\n", pResp->lUserID, pResp->szName, pResp->szAccountName);
			ShowConfigInfo(userInfo);
		}
		break;
	case xlc_get_department_list:
		{
			DepartmentInfo *pResp = (DepartmentInfo *)(pRecvBuff + sizeof(CmdHeader));
			wchar_t userInfo[256] = {0};
			wsprintf(userInfo, L"lDepartmentID=%d szName=%S \n", pResp->lDepartmentID, pResp->szName);
			ShowConfigInfo(userInfo);
		}
		break;
	case xlc_get_dvr_info:
		{
			DVRInfo *pResp = (DVRInfo *)(pRecvBuff + sizeof(CmdHeader)); 
			if (m_configDlg != NULL)
				m_configDlg->SetDvrInfo(*pResp);
		}
		break;
	case xlc_get_user_info:
		{
			UserInfo *pResp = (UserInfo *)(pRecvBuff + sizeof(CmdHeader)); 
			if (m_configDlg != NULL)
				m_configDlg->SetUserInfo(*pResp);
		}
		break;
	case xlc_get_department_info:
		{
			DepartmentInfo *pResp = (DepartmentInfo *)(pRecvBuff + sizeof(CmdHeader)); 
			if (m_configDlg != NULL)
				m_configDlg->SetDepartmentInfo(*pResp);
		}
		break;
	case xlc_add_dvr_info: case xlc_add_user_info: case xlc_add_department_info:
	case xlc_mod_dvr_info: case xlc_mod_user_info: case xlc_mod_department_info:
	case xlc_del_dvr_info: case xlc_del_user_info: case xlc_del_department_info:
		{
			wchar_t info[256] = {0};
			wsprintf(info, L"return_code=%c\n", *(pRecvBuff + sizeof(CmdHeader)) & 0xFF);
			ShowConfigInfo(info);
		}
		break;
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