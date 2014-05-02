#include "AccessOprator.h"

JO_IMPLEMENT_SINGLETON(AccessOprator)

CAccessOprator::CAccessOprator()
{
	::CoInitialize(NULL); // 初始化OLE/COM库环境 ，为访问ADO接口做准备
}

CAccessOprator::~CAccessOprator()
{
	::CoUninitialize();
}

j_result_t CAccessOprator::Connect(const j_char_t *pAddr, j_int16_t nPort, const j_char_t *pUa, const j_char_t *pPwd)
{
	try 
	{		
		//HRESULT hr = m_pConn.CreateInstance("ADODB.Connection");
		HRESULT hr = m_pConn.CreateInstance(__uuidof(Connection));
		if (FAILED(hr))
		{
			_com_error e(hr);
			J_OS::LOGINFO("CAccessOprator::Connect m_pConn.CreateInstance %s", e.ErrorMessage());
			return J_DB_ERROR;
		}
		//hr = m_pRec.CreateInstance("ADODB.Recordset");
		hr = m_pRec.CreateInstance(__uuidof(Recordset));
		if (FAILED(hr))
		{
			_com_error e(hr);
			J_OS::LOGINFO("CAccessOprator::Connect m_pRec.CreateInstance %s", e.ErrorMessage());
			return J_DB_ERROR;
		}
		//hr = m_pCmd.CreateInstance("ADODB.Command");
		hr = m_pCmd.CreateInstance(__uuidof(Command));
		if (FAILED(hr))
		{
			_com_error e(hr);
			J_OS::LOGINFO("CAccessOprator::Connect m_pCmd.CreateInstance %s", e.ErrorMessage());
			return J_DB_ERROR;
		}
		j_char_t connString[256] = {0};
		sprintf(connString,"Provider=Microsoft.ACE.OLEDB.12.0;Data Source=%s", pAddr);
		m_pConn->ConnectionString = connString;
		m_pConn->ConnectionTimeout = 5;				//连接5秒超时
		if (m_pConn->Open("", "", "", adModeUnknown) != S_OK)
		{
			J_OS::LOGINFO("CAccessOprator::Connect Open Error");
			return J_DB_ERROR;
		}
	}
	catch (...)
	{
		J_OS::LOGINFO("CAccessOprator::Connect Except");
		return J_DB_ERROR;
	}

	return J_OK;
}

j_result_t CAccessOprator::Init()
{
	try
	{

	}
	catch (...)
	{

	}
	return J_OK;
}

j_result_t CAccessOprator::Release()
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

j_string_t CAccessOprator::FindFilePath(const j_char_t *pEquipmentId)
{
	try
	{
		char strCmd[512] = {0};
		sprintf(strCmd, "SELECT FilePath FROM Equipment WHERE EquipmentID = '%s';", pEquipmentId);
		m_pRec = m_pConn->Execute((_bstr_t)strCmd, NULL, adCmdText);
		if (!m_pRec->EndOfFile)
		{
			return (char*)_bstr_t(m_pRec->GetCollect("FilePath"));
		}
	}
	catch(...){}

	return "";
}

j_result_t CAccessOprator::UpdateEquipment(const j_char_t *pEquipmentId, const j_char_t *pFilePath, j_boolean_t bFlag)
{
	try
	{
		char strCmd[512] = {0};
		if (bFlag)	/// ADD
		{
			sprintf(strCmd, "INSERT INTO Equipment(EquipmentID,FilePath)VALUES('%s','%s');", pEquipmentId, pFilePath);
			m_pRec = m_pConn->Execute((_bstr_t)strCmd, NULL, adCmdText);
		}
		else///DEL
		{
			sprintf(strCmd, "DELETE Equipment WHERE EquipmentID='%s';", pEquipmentId);
			m_pRec = m_pConn->Execute((_bstr_t)strCmd, NULL, adCmdText);
		}
	}
	catch (...)
	{
		return J_DB_ERROR;
	}
	return J_OK;
}

j_boolean_t CAccessOprator::HasDownLoaded(const j_char_t *pFileName)
{
	try
	{
		char strCmd[512] = {0};
		sprintf(strCmd, "SELECT * FROM FileInfo WHERE FileName = '%s';", pFileName);
		m_pRec = m_pConn->Execute((_bstr_t)strCmd, NULL, adCmdText);
		if (!m_pRec->EndOfFile)
		{
			return true;
		}
	}
	catch (...){}

	return false;
}

j_result_t CAccessOprator::UpdateFileInfo(const j_char_t *pEquipmentId, const j_char_t *pFileName, j_boolean_t bFlag)
{
	try
	{
		char strCmd[512] = {0};
		if (bFlag)	/// ADD
		{
			sprintf(strCmd, "INSERT INTO FileInfo(EquipmentID,FileName)VALUES('%s','%s');", pEquipmentId, pFileName);
			m_pRec = m_pConn->Execute((_bstr_t)strCmd, NULL, adCmdText);
		}
		else///DEL
		{
			sprintf(strCmd, "DELETE Equipment WHERE FileName='%s';", pFileName);
			m_pRec = m_pConn->Execute((_bstr_t)strCmd, NULL, adCmdText);
		}
	}
	catch (...)
	{
		return J_DB_ERROR;
	}

	return J_OK;
}