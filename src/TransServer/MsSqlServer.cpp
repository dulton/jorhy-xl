///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx�Ƽ����޹�˾(��Ȩ����) 
/// All rights reserved. 
/// 
/// @file      SqlServerAccess.cpp 
/// @brief     SqlServer���ݿ�ģ��ʵ��
///
/// @version   1.0 (�汾����)
/// @author    Jorhy (joorhy@gmail.com) 
/// @date      2013/09/22 11:11 
///
///
/// �޶�˵��������汾
///////////////////////////////////////////////////////////////////////////  
#include "MsSqlServer.h"

//#import "c:\\Program Files\\Common Files\\System\\ADO\\msado15.dll"no_namespace rename("EOF", "EndOfFile")
JO_IMPLEMENT_SINGLETON(SqlServerAccess)

CSqlServerAccess::CSqlServerAccess()
{
	::CoInitialize(NULL); // ��ʼ��OLE/COM�⻷�� ��Ϊ����ADO�ӿ���׼��
}

CSqlServerAccess::~CSqlServerAccess()
{
	::CoUninitialize();
}

j_result_t CSqlServerAccess::Connect(const j_char_t *pAddr, j_int16_t nPort, const j_char_t *pUa, const j_char_t *pPwd)
{
	m_pConn.CreateInstance("ADODB.Connection");
	m_pRec.CreateInstance("ADODB.Recordset");
	m_pCmd.CreateInstance("ADODB.Command");

	j_char_t connString[256] = {0};
	sprintf(connString, "Provider=SQLOLEDB;Server=%s,%d;Database=xl; uid=%s; pwd=%s;", pAddr, nPort, pUa, pPwd);
	m_pConn->ConnectionString = connString;
	m_pConn->ConnectionTimeout = 5;				//����5�볬ʱ
	if (m_pConn->Open("", "", "", adConnectUnspecified) != S_OK)
		return J_DB_ERROR;

	return J_OK;
}

j_result_t CSqlServerAccess::Release()
{
	m_pRec->Close();
	m_pConn->Close();
	m_pCmd.Release();
	m_pRec.Release();
	m_pConn.Release();

	return J_OK;
}

j_result_t CSqlServerAccess::CheckUser(const char *pUserName, const char *pPasswd)
{
	char strCmd[128] = {0};
	sprintf(strCmd, "SELECT * FROM UserInfo WHERE AccountName='%s' AND Password='%s';", pUserName, pPasswd);
	m_pRec = m_pConn->Execute((_bstr_t)strCmd, NULL, adCmdText);
	if (m_pRec->EndOfFile)
		return J_UNKNOW;

	//m_pRec->MoveNext();
	return J_OK;
}

j_result_t CSqlServerAccess::UpdateDevInfo(const DevInfo &devInfo)
{
	char strCmd[256] = {0};
	sprintf(strCmd, "UPDATE Equipment SET VehicleNO='%s',PhoneNum='%s',TotalChannels=%d WHERE EquipmentID='%s';",
		devInfo.vehicleNum, devInfo.phoneNum, devInfo.totalChannels & 0xFF, devInfo.hostId);
	m_pConn->Execute((_bstr_t)strCmd, NULL, adCmdText);

	return J_OK;
}

j_result_t CSqlServerAccess::GetDevInfo(DevInfo &devInfo)
{
	char strCmd[128] = {0};
	sprintf(strCmd, "SELECT * FROM Equipment WHERE EquipmentID='%s';", devInfo.hostId);
	m_pRec = m_pConn->Execute((_bstr_t)strCmd, NULL, adCmdText);
	if (m_pRec->EndOfFile)
		return J_UNKNOW;
	strcpy(devInfo.vehicleNum, (char*)_bstr_t(m_pRec->GetCollect("VehicleNO")));
	strcpy(devInfo.phoneNum, (char*)_bstr_t(m_pRec->GetCollect("PhoneNum")));
	devInfo.totalChannels = m_pRec->GetCollect("TotalChannels").intVal;

	return J_OK;
}

j_result_t CSqlServerAccess::InsertAlarmInfo(const char *pHostId, const AlarmInfo& alarmInfo)
{
	char strCmd[512] = {0};
	sprintf(strCmd, "INSERT INTO Alarm(EquipmentID,Alarm,GPS_Latitude,GPS_Longitude,GPS_Speed,Speed,TimeStamp)VALUES('%s',%I64d,%f,%f,%f,%f,%d);",
		pHostId, alarmInfo.bAlarm & 0xFF, alarmInfo.gps.dLatitude, alarmInfo.gps.dLongitude, alarmInfo.gps.dGPSSpeed, alarmInfo.speed, alarmInfo.tmTimeStamp);
	m_pConn->Execute((_bstr_t)strCmd, NULL, adCmdText);

	return J_OK;
}

time_t CSqlServerAccess::GetDevLogLastTime(const char *pHostId)
{
	char strCmd[128] = {0};
	sprintf(strCmd, "SELECT MAX(TimeStamp) FROM EquipmentLog WHERE EquipmentID='%s';", pHostId);
	m_pRec = m_pConn->Execute((_bstr_t)strCmd, NULL, adCmdText);
	if (m_pRec->EndOfFile)
		return 0;

	_variant_t vIndex = (long)0;
	return m_pRec->GetCollect(vIndex).llVal;
}

j_result_t CSqlServerAccess::InsertLogInfo(const char *pHostId, int nState, time_t tmTimeStamp)
{
	char strCmd[512] = {0};
	sprintf(strCmd, "INSERT INTO EquipmentLog(EquipmentID,State,TimeStamp)VALUES('%s',%d,%d);",
		pHostId, nState, tmTimeStamp);
	m_pConn->Execute((_bstr_t)strCmd, NULL, adCmdText);

	return J_OK;
}

j_result_t CSqlServerAccess::GetLogInfoList(const char *pHostId, time_t tmStart, time_t tmEnd, LogInfoQueue &logInfoQueue)
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
	return J_OK;
}

j_result_t CSqlServerAccess::GetAlarmInfoList(const char *pHostId, time_t tmStart, time_t tmEnd, AlarmInfoQueue &alarmInfoQueue)
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
	return J_OK;
}