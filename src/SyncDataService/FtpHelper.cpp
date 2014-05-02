#include "FtpHelper.h"

CFtpNotificationHelper::CFtpNotificationHelper()
{
	m_totleSize = 0;
	m_pUsesr = NULL;
	m_callback = NULL;
}

CFtpNotificationHelper::~CFtpNotificationHelper()
{

}

void CFtpNotificationHelper::OnBeginReceivingData()
{
	
}

void CFtpNotificationHelper::OnEndReceivingData(long lReceivedBytes)
{
	if (m_callback)
	{
		if (lReceivedBytes == m_totleSize)
		{
			m_callback(m_pUsesr, true);
		}
		else
		{
			m_callback(m_pUsesr, false);
		}
		m_totleSize = 0;
		m_pUsesr = NULL;
		m_callback = NULL;
	}
}

void CFtpNotificationHelper::OnPreReceiveFile(const tstring& strSourceFile, const tstring& strTargetFile, long lFileSize)
{
	if (lFileSize == 0)
	{
		if (m_callback)
		{
			m_callback(m_pUsesr, false);
		}
		m_pUsesr = NULL;
		m_callback = NULL;
	}
	m_totleSize = lFileSize;
}

CFtpHelper::CFtpHelper()
{

}

CFtpHelper::~CFtpHelper()
{

}

j_result_t CFtpHelper::Login(const char *pHostName, int nPort, const char *pUsername, const char *pPassword)
{
	m_logonInfo.SetHost(ConvertToTString(pHostName), nPort, ConvertToTString(pUsername), ConvertToTString(pPassword));
	if (m_client.Login(m_logonInfo))
	{
		return J_OK;
	}
	return J_UNKNOW;
}

j_result_t CFtpHelper::Logout()
{
	m_client.Logout();

	return J_OK;
}

j_result_t CFtpHelper::ListFile(const char *pFilePath, VecFileName &vecFileName)
{
	vecFileName.clear();
	TStringVector fileList;
	if (m_client.NameList(ConvertToTString(pFilePath), fileList))
	{
		if (fileList.empty())
			return J_UNKNOW;

		TStringVector::iterator it = fileList.begin();
		for (; it != fileList.end(); ++it)
		{
			vecFileName.push_back(ConvertToSTDString(it->c_str()));
		}
		return J_OK;
	}
	return J_UNKNOW;
}

j_result_t CFtpHelper::DownLoadFile(const char *pRemoteFile, const char *pLocalFile, StateCallback pCallback, void *pUser)
{
	m_notifactionHelper.SetStateCallback(pUser, pCallback);
	m_client.AttachObserver(&m_notifactionHelper);
	if (m_client.DownloadFile(ConvertToTString(pRemoteFile), ConvertToTString(pLocalFile)))
	{
		return J_OK;
	}
	return J_UNKNOW;
}

void CFtpHelper::ClearDownLoad()
{
	m_client.DetachObserver(&m_notifactionHelper);
}

tstring CFtpHelper::ConvertToTString(const char *strSource)
{  
	int nLen = strlen(strSource)   +   1;      
	int   nwLen   =   MultiByteToWideChar(CP_ACP,   0,   strSource,   nLen,   NULL,   0); 
	TCHAR   strTarget[512];     
	MultiByteToWideChar(CP_ACP,   0,   strSource,   nLen,   strTarget,   nwLen);

	return strTarget;
}

j_string_t CFtpHelper::ConvertToSTDString(const wchar_t *strSource)
{
	char strTarget[512];        
	int nLen = wcslen(strSource) + 1;        

	WideCharToMultiByte(CP_ACP,   0,   strSource,   nLen,   strTarget,   2 * nLen,   NULL,   NULL);  

	return strTarget;
}