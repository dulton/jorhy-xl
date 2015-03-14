#ifndef __FTP_HELPER_H_
#define __FTP_HELPER_H_
#include "j_includes.h"
#include "FTPClient.h"
#include <vector>
#include <string>

typedef std::vector<j_string_t> VecFileName;
typedef void (* StateCallback)(void *pUsesr, bool bSuccess);

using namespace nsFTP;

class CFtpNotificationHelper : public CFTPClient::CNotification
{
public:
	CFtpNotificationHelper();
	~CFtpNotificationHelper();

public:
	virtual void OnBeginReceivingData();
	virtual void OnEndReceivingData(long /*lReceivedBytes*/);
	virtual void OnPreReceiveFile(const tstring& /*strSourceFile*/, const tstring& /*strTargetFile*/, long /*lFileSize*/);

public:
	void SetStateCallback(void *pUser, StateCallback pCallback)
	{
		m_pUsesr = pUser;
		m_callback = pCallback;
	}

private:
	long m_totleSize;
	StateCallback m_callback;
	void *m_pUsesr;
};

class CFtpHelper
{
public:
	CFtpHelper();
	~CFtpHelper();

public:
	j_result_t Login(const char *pHostName, int nPort, const char *pUsername, const char *pPassword);
	j_result_t Logout();
	j_result_t ListFile(const char *pFilePath, VecFileName &vecFileName);
	j_result_t DownLoadFile(const char *pRemoteFile, const char *pLocalFile, StateCallback pCallback, void *pUser);
	void ClearDownLoad();

public:
	static tstring ConvertToTString(const char *strSource);
	static j_string_t ConvertToSTDString(const wchar_t *strSource);

private:
	CFTPClient m_client;
	CLogonInfo m_logonInfo;
	CFtpNotificationHelper m_notifactionHelper;
};

#endif //~__FTP_HELPER_H_