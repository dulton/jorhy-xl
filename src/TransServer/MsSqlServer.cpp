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
#include <iostream>
using namespace std;

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
		//HRESULT hr = m_pConn.CreateInstance("ADODB.Connection");
		HRESULT hr = m_pConn.CreateInstance(__uuidof(Connection));
		if (FAILED(hr))
		{
			_com_error e(hr);
			J_OS::LOGINFO("CSqlServerAccess::Connect m_pConn.CreateInstance %s", e.ErrorMessage());
			return J_DB_ERROR;
		}
		//hr = m_pRec.CreateInstance("ADODB.Recordset");
		hr = m_pRec.CreateInstance(__uuidof(Recordset));
		if (FAILED(hr))
		{
			_com_error e(hr);
			J_OS::LOGINFO("CSqlServerAccess::Connect m_pRec.CreateInstance %s", e.ErrorMessage());
			return J_DB_ERROR;
		}
		//hr = m_pCmd.CreateInstance("ADODB.Command");
		hr = m_pCmd.CreateInstance(__uuidof(Command));
		if (FAILED(hr))
		{
			_com_error e(hr);
			J_OS::LOGINFO("CSqlServerAccess::Connect m_pCmd.CreateInstance %s", e.ErrorMessage());
			return J_DB_ERROR;
		}
		j_char_t connString[256] = {0};
		sprintf(connString, "Provider=SQLOLEDB;Server=%s,%d;Database=xl; uid=%s; pwd=%s;", pAddr, nPort, pUa, pPwd);
		m_pConn->ConnectionString = connString;
		m_pConn->ConnectionTimeout = 5;				//连接5秒超时
		if (m_pConn->Open("", "", "", adConnectUnspecified) != S_OK)
		{
			J_OS::LOGINFO("CSqlServerAccess::Connect Open Error");
			return J_DB_ERROR;
		}
	}
	catch (...)
	{
		J_OS::LOGINFO("CSqlServerAccess::Connect Except");
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

j_result_t CSqlServerAccess::Login(const char *pUserName, const char *pPasswd, int nForce, int &nRet)
{
	try 
	{
		char strCmd[128] = {0};
		sprintf(strCmd, "SELECT * FROM UserInfo WHERE AccountName='%s';", pUserName);
		m_pRec = m_pConn->Execute((_bstr_t)strCmd, NULL, adCmdText);
		if (m_pRec->EndOfFile)
		{
			nRet = 1;
			return J_DB_ERROR;
		}
		else
		{
			if (strcmp(pPasswd, (char*)_bstr_t(m_pRec->GetCollect("Password"))) != 0)
			{
				nRet = 2;
				return J_DB_ERROR;
			}
			else if (nForce)
			{
				sprintf(strCmd, "UPDATE UserInfo SET Online=1 WHERE AccountName='%s';", pUserName);
				m_pConn->Execute((_bstr_t)strCmd, NULL, adCmdText);
			}
			else if (m_pRec->GetCollect("Online").intVal == 1)
			{
				nRet = 6;
				return J_DB_ERROR;
			}
			else
			{
				nRet = 0;
				memset(strCmd, 0, sizeof(strCmd));
				sprintf(strCmd, "UPDATE UserInfo SET Online=1 WHERE AccountName='%s';", pUserName);
				m_pConn->Execute((_bstr_t)strCmd, NULL, adCmdText);
			}
		}
	}
	catch(...)
	{
		return J_DB_ERROR;
	}

	return J_OK;
}

j_result_t CSqlServerAccess::Logout(const char *pUserName)
{
	try 
	{
		char strCmd[128] = {0};
		sprintf(strCmd, "UPDATE UserInfo SET Online=0 WHERE AccountName='%s';", pUserName);
		m_pConn->Execute((_bstr_t)strCmd, NULL, adCmdText);
	}
	catch(...)
	{
		return J_DB_ERROR;
	}
	return J_OK;
}

j_result_t CSqlServerAccess::UpdateDevInfo(const DevHostInfo &devInfo)
{
	try 
	{
		char strCmd[256] = {0};
		if (devInfo.bOnline == true)
		{
			sprintf(strCmd, "UPDATE Equipment SET VehicleNO='%s',PhoneNum='%s',TotalChannels=%d,Online=%d WHERE EquipmentID='%s';",
				devInfo.vehicleNum, devInfo.phoneNum, devInfo.totalChannels & 0xFF, devInfo.bOnline, devInfo.hostId);
		}
		else
		{
			sprintf(strCmd, "UPDATE Equipment SET Online=%d WHERE EquipmentID='%s';",
				devInfo.bOnline, devInfo.hostId);
		}
		m_pConn->Execute((_bstr_t)strCmd, NULL, adCmdText);
	}
	catch(...)
	{
		return J_DB_ERROR;
	}

	return J_OK;
}

j_result_t CSqlServerAccess::GetDevInfo(DevHostInfo &devInfo)
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

j_result_t CSqlServerAccess::InsertAlarmInfo(const char *pHostId, const DevAlarmInfo& alarmInfo)
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

j_result_t CSqlServerAccess::GetLogInfoList(const char *pHostId, time_t tmStart, time_t tmEnd, DevLogInfoQueue &logInfoQueue)
{
	//try
	//{
	//	char strCmd[512] = {0};
	//	if (tmStart == -1 && tmEnd == -1)
	//		sprintf(strCmd, "SELECT * FROM EquipmentLog WHERE EquipmentID='%s';", pHostId);
	//	else
	//		sprintf(strCmd, "SELECT * FROM EquipmentLog WHERE EquipmentID='%s' AND TimeStamp BETWEEN %I64d AND %I64d;", pHostId, tmStart, tmEnd);
	//	m_pRec = m_pConn->Execute((_bstr_t)strCmd, NULL, adCmdText);
	//	LogInfo logInfo;
	//	while (!m_pRec->EndOfFile)
	//	{
	//		logInfo.bStatus = m_pRec->GetCollect("State").intVal;
	//		logInfo.tmTime = m_pRec->GetCollect("TimeStamp").llVal;
	//		logInfoQueue.push(logInfo);
	//		m_pRec->MoveNext();
	//	}
	//}
	//catch (...)
	//{
	//	return J_DB_ERROR;
	//}

	return J_OK;
}

j_result_t CSqlServerAccess::GetAlarmInfoList(const char *pHostId, time_t tmStart, time_t tmEnd, CliAlarmInfoQueue &alarmInfoQueue)
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
		CliAlarmInfo alarmInfo;
		while (!m_pRec->EndOfFile)
		{
			strcpy((char *)alarmInfo.pHostId, pHostId);
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

j_result_t CSqlServerAccess::GetDvrTotleInfo(CliHostTotleInfo &dvrTotleInfo)
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

j_result_t CSqlServerAccess::GetUserTotleInfo(CliUserTotleInfo &userTotleInfo)
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

j_result_t CSqlServerAccess::GetDvrList(int nType, long lDepartmentId, CliHostInfoQueue &dvrInfoQueue)
{
	try
	{
		char strCmd[512] = {0};
		sprintf(strCmd, "SELECT * FROM Equipment WHERE DepartmentID=%d;", lDepartmentId);
		m_pRec = m_pConn->Execute((_bstr_t)strCmd, NULL, adCmdText);
		CliHostInfo dvrInfo;
		while (!m_pRec->EndOfFile)
		{
			memset (&dvrInfo, 0, sizeof(CliHostInfo));
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

j_result_t CSqlServerAccess::GetUserList(int nType, CliUserInfoQueue &userInfoQueue)
{
	try
	{
		char strCmd[512] = {0};
		sprintf(strCmd, "SELECT * FROM UserInfo;");
		m_pRec = m_pConn->Execute((_bstr_t)strCmd, NULL, adCmdText);
		CliUserInfo userInfo;
		while (!m_pRec->EndOfFile)
		{
			memset (&userInfo, 0, sizeof(CliUserInfo));
			userInfo.lUserID = m_pRec->GetCollect("UserID").llVal;
			strcpy(userInfo.szName, (char*)_bstr_t(m_pRec->GetCollect("Name")));
			strcpy(userInfo.szAccountName, (char*)_bstr_t(m_pRec->GetCollect("AccountName")));
			strcpy(userInfo.szPassword, (char*)_bstr_t(m_pRec->GetCollect("Password")));
			userInfo.lDepartmentID = m_pRec->GetCollect("DepartmentID").llVal;
			userInfo.nState = m_pRec->GetCollect("State").intVal;
			userInfoQueue.push(userInfo);
			m_pRec->MoveNext();
		}
	}
	catch (...)
	{
		return J_DB_ERROR;
	}

	return J_OK;
}

j_result_t CSqlServerAccess::GetDepartmentList(CliDepartmentInfoQueue &departmentInfoQueue)
{
	try
	{
		char strCmd[512] = {0};
		sprintf(strCmd, "SELECT * FROM Department;");
		m_pRec = m_pConn->Execute((_bstr_t)strCmd, NULL, adCmdText);
		CliDepartmentInfo departmentInfo;
		while (!m_pRec->EndOfFile)
		{
			memset (&departmentInfo, 0, sizeof(CliDepartmentInfo));
			departmentInfo.lDepartmentID = m_pRec->GetCollect("DepartmentID").llVal;
			strcpy(departmentInfo.szName, (char*)_bstr_t(m_pRec->GetCollect("Name")));
			departmentInfo.ParentID = m_pRec->GetCollect("ParentID").llVal;
			departmentInfo.OrderIndex = m_pRec->GetCollect("OrderIndex").intVal;
			departmentInfo.nState = m_pRec->GetCollect("State").intVal;
			departmentInfoQueue.push(departmentInfo);
			m_pRec->MoveNext();
		}
	}
	catch (...)
	{
		return J_DB_ERROR;
	}

	return J_OK;
}

j_result_t CSqlServerAccess::GetDvrInfo(const CliHostId &dvrId, CliHostInfo &info)
{
	try
	{
		char strCmd[512] = {0};
		sprintf(strCmd, "SELECT * FROM Equipment WHERE EquipmentID='%s';", dvrId.szEquipmentID);
		m_pRec = m_pConn->Execute((_bstr_t)strCmd, NULL, adCmdText);
		if (!m_pRec->EndOfFile)
		{
			strcpy(info.szEquipmentID, (char*)_bstr_t(m_pRec->GetCollect("EquipmentID")));
			strcpy(info.szVehicleNO, (char*)_bstr_t(m_pRec->GetCollect("VehicleNO")));
			strcpy(info.szPhone, (char*)_bstr_t(m_pRec->GetCollect("PhoneNum")));
			info.nTotalChannels = m_pRec->GetCollect("TotalChannels").intVal;
			info.lDepartmentID = m_pRec->GetCollect("DepartmentID").llVal;
			info.nOnline = m_pRec->GetCollect("Online").intVal;
			info.nState = m_pRec->GetCollect("State").intVal;
		}
	}
	catch (...)
	{
		return J_DB_ERROR;
	}

	return J_OK;
}

j_result_t CSqlServerAccess::GetUserInfo(const CliUserId &userId, CliUserInfo &info)
{
	try
	{
		char strCmd[512] = {0};
		sprintf(strCmd, "SELECT * FROM UserInfo WHERE UserID=%d;", userId.lUserID);
		m_pRec = m_pConn->Execute((_bstr_t)strCmd, NULL, adCmdText);
		if (!m_pRec->EndOfFile)
		{
			info.lUserID = m_pRec->GetCollect("UserID").llVal;
			strcpy(info.szName, (char*)_bstr_t(m_pRec->GetCollect("Name")));
			strcpy(info.szAccountName, (char*)_bstr_t(m_pRec->GetCollect("AccountName")));
			strcpy(info.szPassword, (char*)_bstr_t(m_pRec->GetCollect("Password")));
			info.lDepartmentID = m_pRec->GetCollect("DepartmentID").llVal;
			info.nState = m_pRec->GetCollect("State").intVal;
		}
	}
	catch (...)
	{
		return J_DB_ERROR;
	}

	return J_OK;
}

j_result_t CSqlServerAccess::GetDepartmentInfo(const CliDepartmentId &departmengId, CliDepartmentInfo &info)
{
	try
	{
		char strCmd[512] = {0};
		sprintf(strCmd, "SELECT * FROM Department WHERE DepartmentID=%d;", departmengId.lDepartmentID);
		m_pRec = m_pConn->Execute((_bstr_t)strCmd, NULL, adCmdText);
		if (!m_pRec->EndOfFile)
		{
			info.lDepartmentID = m_pRec->GetCollect("DepartmentID").llVal;
			strcpy(info.szName, (char*)_bstr_t(m_pRec->GetCollect("Name")));
			info.ParentID = m_pRec->GetCollect("ParentID").llVal;
			info.OrderIndex = m_pRec->GetCollect("OrderIndex").intVal;
			info.nState = m_pRec->GetCollect("State").intVal;
		}
	}
	catch (...)
	{
		return J_DB_ERROR;
	}

	return J_OK;
}

j_result_t CSqlServerAccess::AddDvrInfo(const CliHostInfo &info)
{
	try
	{
		char strCmd[512] = {0};
		sprintf(strCmd, "INSERT INTO Equipment (EquipmentID,VehicleNO,PhoneNum,TotalChannels,DepartmentID,Online,State) VALUES ('%s','%s','%s',%d,%d,%d,%d);", 
			info.szEquipmentID, info.szVehicleNO, info.szPhone, info.nTotalChannels, info.lDepartmentID, info.nOnline, info.nState);
		m_pRec = m_pConn->Execute((_bstr_t)strCmd, NULL, adCmdText);
	}
	catch (...)
	{
		return J_DB_ERROR;
	}

	return J_OK;
}

j_result_t CSqlServerAccess::AddUserInfo(const CliUserInfo &info)
{
	try
	{
		char strCmd[512] = {0};
		sprintf(strCmd, "INSERT INTO UserInfo (UserID,Name,AccountName,Password,DepartmentID,State) VALUES (%d,'%s','%s','%s',%d,%d);", 
			info.lUserID, info.szName, info.szAccountName, info.szPassword, info.lDepartmentID, info.nState);
		m_pRec = m_pConn->Execute((_bstr_t)strCmd, NULL, adCmdText);
	}
	catch (...)
	{
		return J_DB_ERROR;
	}

	return J_OK;
}

j_result_t CSqlServerAccess::AddDepartmentInfo(const CliDepartmentInfo &info)
{
	try
	{
		char strCmd[512] = {0};
		sprintf(strCmd, "INSERT INTO Department (DepartmentID,Name,ParentID,OrderIndex,State) VALUES (%d,'%s',%d,%d,%d);", 
			info.lDepartmentID, info.szName, info.ParentID, info.OrderIndex, info.nState);
		m_pRec = m_pConn->Execute((_bstr_t)strCmd, NULL, adCmdText);
	}
	catch (...)
	{
		return J_DB_ERROR;
	}

	return J_OK;
}

j_result_t CSqlServerAccess::ModDvrInfo(const CliHostInfo &info)
{
	try
	{
		char strCmd[512] = {0};
		sprintf(strCmd, "UPDATE Equipment SET VehicleNO='%s',PhoneNum='%s',TotalChannels=%d,DepartmentID=%d,Online=%d,State=%d WHERE EquipmentID='%s';", 
			info.szVehicleNO, info.szPhone, info.nTotalChannels, info.lDepartmentID, info.nOnline, info.nState, info.szEquipmentID);
		m_pRec = m_pConn->Execute((_bstr_t)strCmd, NULL, adCmdText);
	}
	catch (...)
	{
		return J_DB_ERROR;
	}

	return J_OK;
}

j_result_t CSqlServerAccess::ModUserInfo(const CliUserInfo &info)
{
	try
	{
		char strCmd[512] = {0};
		sprintf(strCmd, "UPDATE UserInfo SET Name='%s',AccountName='%s',Password='%s',DepartmentID=%d,State=%d WHERE UserID=%d;", 
			info.szName, info.szAccountName, info.szPassword, info.lDepartmentID, info.nState, info.lUserID);
		m_pRec = m_pConn->Execute((_bstr_t)strCmd, NULL, adCmdText);
	}
	catch (...)
	{
		return J_DB_ERROR;
	}

	return J_OK;
}

j_result_t CSqlServerAccess::ModDepartmentInfo(const CliDepartmentInfo &info)
{
	try
	{
		char strCmd[512] = {0};
		sprintf(strCmd, "UPDATE Department SET Name='%s',ParentID=%d,OrderIndex=%d,State=%d WHERE DepartmentID=%d;", 
			info.szName, info.ParentID, info.OrderIndex, info.nState, info.lDepartmentID);
		m_pRec = m_pConn->Execute((_bstr_t)strCmd, NULL, adCmdText);
	}
	catch (...)
	{
		return J_DB_ERROR;
	}

	return J_OK;
}

j_result_t CSqlServerAccess::DelDvrInfo(const CliHostId &dvrId)
{
	try
	{
		char strCmd[512] = {0};
		sprintf(strCmd, "DELETE Equipment WHERE EquipmentID='%s';", dvrId.szEquipmentID);
		m_pRec = m_pConn->Execute((_bstr_t)strCmd, NULL, adCmdText);
	}
	catch (...)
	{
		return J_DB_ERROR;
	}

	return J_OK;
}

j_result_t CSqlServerAccess::DelUserInfo(const CliUserId &userId)
{
	try
	{
		char strCmd[512] = {0};
		sprintf(strCmd, "DELETE UserInfo WHERE UserID=%d;", userId.lUserID);
		m_pRec = m_pConn->Execute((_bstr_t)strCmd, NULL, adCmdText);
	}
	catch (...)
	{
		return J_DB_ERROR;
	}

	return J_OK;
}

j_result_t CSqlServerAccess::DelDepartmentInfo(const CliDepartmentId &departmengId)
{
	try
	{
		char strCmd[512] = {0};
		sprintf(strCmd, "DELETE Department WHERE DepartmentID=%d;", departmengId.lDepartmentID);
		m_pRec = m_pConn->Execute((_bstr_t)strCmd, NULL, adCmdText);
	}
	catch (...)
	{
		return J_DB_ERROR;
	}

	return J_OK;
}