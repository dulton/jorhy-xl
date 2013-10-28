///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx科技有限公司(版权声明) 
/// All rights reserved. 
/// 
/// @file      SqlServerAccess.cpp 
/// @brief     SqlServer数据库模块实现
///
/// @version   1.0 (版本声明)
/// @author    Jorhy (joorhy@gmail.com) 
/// @date      2013/09/22 11:11 
///
///
/// 修订说明：最初版本
///////////////////////////////////////////////////////////////////////////  
#include "MsSqlServer.h"

//#import "c:\\Program Files\\Common Files\\System\\ADO\\msado15.dll"no_namespace rename("EOF", "EndOfFile")
JO_IMPLEMENT_SINGLETON(SqlServerAccess)

CSqlServerAccess::CSqlServerAccess()
{
	::CoInitialize(NULL); // 初始化OLE/COM库环境 ，为访问ADO接口做准备
}

CSqlServerAccess::~CSqlServerAccess()
{
	::CoUninitialize();
}

j_result_t CSqlServerAccess::Connect(const j_char_t *pAddr, j_int16_t nPort, const j_char_t *pUa, const j_char_t *pPwd)
{
	try 
	{
		m_pConn.CreateInstance("ADODB.Connection");
		m_pRec.CreateInstance("ADODB.Recordset");
		m_pCmd.CreateInstance("ADODB.Command");

		j_char_t connString[256] = {0};
		sprintf(connString, "Provider=SQLOLEDB;Server=%s,%d;Database=xl; uid=%s; pwd=%s;", pAddr, nPort, pUa, pPwd);
		m_pConn->ConnectionString = connString;
		m_pConn->ConnectionTimeout = 5;				//连接5秒超时
		if (m_pConn->Open("", "", "", adConnectUnspecified) != S_OK)
			return J_DB_ERROR;
	}
	catch (...)
	{
		return J_DB_ERROR;
	}

	return J_OK;
}

j_result_t CSqlServerAccess::Release()
{
	try
	{
		m_pRec->Close();
		m_pConn->Close();
		m_pCmd.Release();
		m_pRec.Release();
		m_pConn.Release();
	}
	catch (...)
	{
		return J_DB_ERROR;
	}

	return J_OK;
}

j_result_t CSqlServerAccess::CheckUser(const char *pUserName, const char *pPasswd)
{
	try 
	{
		char strCmd[128] = {0};
		sprintf(strCmd, "SELECT * FROM UserInfo WHERE AccountName='%s' AND Password='%s';", pUserName, pPasswd);
		m_pRec = m_pConn->Execute((_bstr_t)strCmd, NULL, adCmdText);
		if (m_pRec->EndOfFile)
			return J_DB_ERROR;
	}
	catch(...)
	{
		return J_DB_ERROR;
	}

	return J_OK;
}

j_result_t CSqlServerAccess::UpdateDevInfo(const DevInfo &devInfo)
{
	try 
	{
		char strCmd[256] = {0};
		sprintf(strCmd, "UPDATE Equipment SET VehicleNO='%s',PhoneNum='%s',TotalChannels=%d WHERE EquipmentID='%s';",
			devInfo.vehicleNum, devInfo.phoneNum, devInfo.totalChannels & 0xFF, devInfo.hostId);
		m_pConn->Execute((_bstr_t)strCmd, NULL, adCmdText);
	}
	catch(...)
	{
		return J_DB_ERROR;
	}

	return J_OK;
}

j_result_t CSqlServerAccess::GetDevInfo(DevInfo &devInfo)
{
	try 
	{
		char strCmd[128] = {0};
		sprintf(strCmd, "SELECT * FROM Equipment WHERE EquipmentID='%s';", devInfo.hostId);
		m_pRec = m_pConn->Execute((_bstr_t)strCmd, NULL, adCmdText);
		if (m_pRec->EndOfFile)
			return J_UNKNOW;
		strcpy(devInfo.vehicleNum, (char*)_bstr_t(m_pRec->GetCollect("VehicleNO")));
		strcpy(devInfo.phoneNum, (char*)_bstr_t(m_pRec->GetCollect("PhoneNum")));
		devInfo.totalChannels = m_pRec->GetCollect("TotalChannels").intVal;
	}
	catch(...)
	{
		return J_DB_ERROR;
	}

	return J_OK;
}

j_result_t CSqlServerAccess::InsertAlarmInfo(const char *pHostId, const AlarmInfo& alarmInfo)
{
	try 
	{
		char strCmd[512] = {0};
		sprintf(strCmd, "INSERT INTO Alarm(EquipmentID,Alarm,GPS_Latitude,GPS_Longitude,GPS_Speed,Speed,TimeStamp)VALUES('%s',%I64d,%f,%f,%f,%f,%d);",
			pHostId, alarmInfo.bAlarm & 0xFF, alarmInfo.gps.dLatitude, alarmInfo.gps.dLongitude, alarmInfo.gps.dGPSSpeed, alarmInfo.speed, alarmInfo.tmTimeStamp);
		m_pConn->Execute((_bstr_t)strCmd, NULL, adCmdText);
	}
	catch (...)
	{
		return J_DB_ERROR;
	}

	return J_OK;
}

time_t CSqlServerAccess::GetDevLogLastTime(const char *pHostId)
{
	time_t tmReturn = 0;
	try 
	{
		char strCmd[128] = {0};
		sprintf(strCmd, "SELECT MAX(TimeStamp) FROM EquipmentLog WHERE EquipmentID='%s';", pHostId);
		m_pRec = m_pConn->Execute((_bstr_t)strCmd, NULL, adCmdText);
		if (!m_pRec->EndOfFile)
		{
			_variant_t vIndex = (long)0;
			tmReturn = m_pRec->GetCollect(vIndex).llVal;
		}
	}
	catch (...)
	{

	}

	return tmReturn;
}

j_result_t CSqlServerAccess::InsertLogInfo(const char *pHostId, int nState, time_t tmTimeStamp)
{
	try
	{
		char strCmd[512] = {0};
		sprintf(strCmd, "INSERT INTO EquipmentLog(EquipmentID,State,TimeStamp)VALUES('%s',%d,%d);",
			pHostId, nState, tmTimeStamp);
		m_pConn->Execute((_bstr_t)strCmd, NULL, adCmdText);
	}
	catch (...)
	{
		return J_DB_ERROR;
	}

	return J_OK;
}

j_result_t CSqlServerAccess::GetLogInfoList(const char *pHostId, time_t tmStart, time_t tmEnd, LogInfoQueue &logInfoQueue)
{
	try
	{
		char strCmd[512] = {0};
		if (tmStart == -1 && tmEnd == -1)
			sprintf(strCmd, "SELECT * FROM EquipmentLog WHERE EquipmentID='%s';", pHostId);
		else
			sprintf(strCmd, "SELECT * FROM EquipmentLog WHERE EquipmentID='%s' AND TimeStamp BETWEEN %I64d AND %I64d;", pHostId, tmStart, tmEnd);
		m_pRec = m_pConn->Execute((_bstr_t)strCmd, NULL, adCmdText);
		GetLogInfoResp logInfo;
		while (!m_pRec->EndOfFile)
		{
			logInfo.bStatus = m_pRec->GetCollect("State").intVal;
			logInfo.tmTime = m_pRec->GetCollect("TimeStamp").llVal;
			logInfoQueue.push(logInfo);
			m_pRec->MoveNext();
		}
	}
	catch (...)
	{
		return J_DB_ERROR;
	}

	return J_OK;
}

j_result_t CSqlServerAccess::GetAlarmInfoList(const char *pHostId, time_t tmStart, time_t tmEnd, AlarmInfoQueue &alarmInfoQueue)
{
	try
	{
		char strCmd[512] = {0};
		if (tmStart == -1 && tmEnd == -1)
			sprintf(strCmd, "SELECT * FROM Alarm WHERE EquipmentID='%s';", pHostId);
		else if (tmEnd == -1)
			sprintf(strCmd, "SELECT * FROM Alarm WHERE EquipmentID='%s' AND TimeStamp >= %I64d;", pHostId, tmStart);
		else
			sprintf(strCmd, "SELECT * FROM Alarm WHERE EquipmentID='%s' AND TimeStamp BETWEEN %I64d AND %I64d;", pHostId, tmStart, tmEnd);
		m_pRec = m_pConn->Execute((_bstr_t)strCmd, NULL, adCmdText);
		AlarmInfo alarmInfo;
		while (!m_pRec->EndOfFile)
		{
			alarmInfo.bAlarm = m_pRec->GetCollect("Alarm").intVal;
			alarmInfo.gps.dLatitude = m_pRec->GetCollect("GPS_Latitude").dblVal;
			alarmInfo.gps.dLongitude = m_pRec->GetCollect("GPS_Longitude").dblVal;
			alarmInfo.gps.dGPSSpeed = m_pRec->GetCollect("GPS_Speed").dblVal;
			alarmInfo.speed = m_pRec->GetCollect("Speed").dblVal;
			alarmInfo.tmTimeStamp = m_pRec->GetCollect("TimeStamp").llVal;
			alarmInfoQueue.push(alarmInfo);
			m_pRec->MoveNext();
		}
	}
	catch (...)
	{
		return J_DB_ERROR;
	}

	return J_OK;
}

j_result_t CSqlServerAccess::GetDvrTotleInfo(DVRTotleInfo &dvrTotleInfo)
{
	try 
	{
		char strCmd[512] = {0};
		sprintf(strCmd, "SELECT COUNT(*) FROM Equipment;");
		m_pRec = m_pConn->Execute((_bstr_t)strCmd, NULL, adCmdText);
		if (!m_pRec->EndOfFile)
		{
			_variant_t vIndex = (long)0;
			dvrTotleInfo.uiTotal = m_pRec->GetCollect(vIndex).llVal;
		}
	}
	catch(...)
	{
		return J_DB_ERROR;
	}
	return J_OK;
}

j_result_t CSqlServerAccess::GetUserTotleInfo(UserTotleInfo &userTotleInfo)
{
	try 
	{
		char strCmd[512] = {0};
		sprintf(strCmd, "SELECT COUNT(*) FROM UserInfo;");
		m_pRec = m_pConn->Execute((_bstr_t)strCmd, NULL, adCmdText);
		if (!m_pRec->EndOfFile)
		{
			_variant_t vIndex = (long)0;
			userTotleInfo.uiTotal = m_pRec->GetCollect(vIndex).llVal;
		}
	}
	catch(...)
	{
		return J_DB_ERROR;
	}
	return J_OK;
}

j_result_t CSqlServerAccess::GetDvrList(int nType, long lDepartmentId, DvrInfoQueue &dvrInfoQueue)
{
	try
	{
		char strCmd[512] = {0};
		sprintf(strCmd, "SELECT * FROM Equipment WHERE DepartmentID=%d;", lDepartmentId);
		m_pRec = m_pConn->Execute((_bstr_t)strCmd, NULL, adCmdText);
		DVRInfo dvrInfo;
		while (!m_pRec->EndOfFile)
		{
			memset (&dvrInfo, 0, sizeof(DVRInfo));
			strcpy(dvrInfo.szEquipmentID, (char*)_bstr_t(m_pRec->GetCollect("EquipmentID")));
			strcpy(dvrInfo.szVehicleNO, (char*)_bstr_t(m_pRec->GetCollect("VehicleNO")));
			strcpy(dvrInfo.szPhone, (char*)_bstr_t(m_pRec->GetCollect("PhoneNum")));
			dvrInfo.nTotalChannels = m_pRec->GetCollect("TotalChannels").intVal;
			dvrInfo.lDepartmentID = m_pRec->GetCollect("DepartmentID").llVal;
			dvrInfo.nOnline = m_pRec->GetCollect("Online").intVal;
			dvrInfo.nState = m_pRec->GetCollect("State").intVal;
			dvrInfoQueue.push(dvrInfo);
			m_pRec->MoveNext();
		}
	}
	catch (...)
	{
		return J_DB_ERROR;
	}

	return J_OK;
}