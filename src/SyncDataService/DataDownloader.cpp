#include "DataDownloader.h"
#include "HostManager.h"
#include "AccessOprator.h"

#include "direct.h"

CDataDownloader::CDataDownloader()
: m_bStart(false)
{
	m_bStartHost = false;
	m_bStartFile = false;
	memset(&m_param, 0, sizeof(j_thread_parm));
}

CDataDownloader::~CDataDownloader()
{

}

j_result_t CDataDownloader::Start(const char *pFilePath)
{
	if (!m_bStart)
	{
		m_bStart = true;
		m_strPath = pFilePath;
		m_param.entry = CDataDownloader::OnWorkFunc;
		m_param.data = this;
		m_workTread.Create(m_param);
	}

	return J_OK;
}

j_result_t CDataDownloader::Stop()
{
	if (m_bStart)
	{
		m_bStart = false;
		m_workTread.Release();
	}
	return J_OK;
}

void CDataDownloader::OnReturn(j_boolean_t bFinish)
{
	if (bFinish)
	{
		EndDownLoad();
		//JoAccessObj->UpdateFileInfo(m_info.strHostId, m_strRemoteFile.c_str());
		//J_OS::LOGINFO("Load Flie Success %s-%s", m_info.strHostId, m_strRemoteFile.c_str());
	}
	else
	{
		m_bStartHost = false;
		m_bStartFile = false;
		JoHostManager->DelHost(m_info.strHostId);
		m_ftpHelper.Logout();
		remove(m_strLocalFile.c_str());
		//J_OS::LOGINFO("Load Flie Failed %s-%s", m_info.strHostId, m_strRemoteFile.c_str());
	}
}

void CDataDownloader::OnWork()
{
	while (m_bStart)
	{
		j_sleep(1);
		if (!m_bStartHost)
		{
			if (!GetFileList())
				continue;
		}
		else
		{
			StartDownLoad();
		}
	}
}

j_boolean_t CDataDownloader::GetFileList()
{
	if (JoHostManager->GetReadyHostInfo(m_info) == J_OK)
	{
		if (m_ftpHelper.Login(m_info.strFtpAddr, m_info.nFtpPort, m_info.strUsesrName, m_info.strPassWord) != J_OK)
		{
			m_bStartHost = false;
			JoHostManager->DelHost(m_info.strHostId);
			return false;
		}

		if (m_ftpHelper.ListFile("", m_vecFileName) != J_OK)
		{
			m_bStartHost = false;
			JoHostManager->DelHost(m_info.strHostId);
			m_ftpHelper.Logout();
			return false;
		}

		m_bStartHost = true;
		return true;
	}
	return false;
}

void CDataDownloader::StartDownLoad()
{
	if (!m_bStartFile)
	{
		VecFileName::iterator it = m_vecFileName.begin();
		if (it == m_vecFileName.end())
		{
			m_bStartFile = false;
			m_bStartHost = false;
		}
		else
		{
			m_strRemoteFile = it->c_str();
			//if (!JoAccessObj->HasDownLoaded(m_strRemoteFile.c_str()))
			if (true)
			{
				j_string_t strLocalPath = m_strPath + "\\" + m_info.strHostId;
				if (_access(strLocalPath.c_str(), 0) == -1)
				{
					mkdir(strLocalPath.c_str());
				}
				m_strLocalFile = strLocalPath + "\\" + m_strRemoteFile;
				//J_OS::LOGINFO("Begin DownLoad %s", m_strLocalFile.c_str());
				if (m_ftpHelper.DownLoadFile(m_strRemoteFile.c_str(), m_strLocalFile.c_str(), CDataDownloader::OnDownLoadReturn, this) != J_OK)
				{
					EndDownLoad();
				}
			}	
			else
			{
				EndDownLoad();
			}
		}
	}
}

void  CDataDownloader::EndDownLoad()
{
	TLock(m_locker);
	VecFileName::iterator it = m_vecFileName.begin();
	for (; it != m_vecFileName.end(); ++it)
	{
		if (m_strRemoteFile == *it)
		{
			m_vecFileName.erase(it);
			m_bStartFile = false;
			break;
		}
	}
	TUnlock(m_locker);
}