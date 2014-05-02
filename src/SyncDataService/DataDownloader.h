#ifndef __DATA_DOWNLOADER_H_
#define __DATA_DOWNLOADER_H_
#include "j_includes.h"
#include "x_thread.h"
#include "FtpHelper.h"
#include "XLSyncType.h"

class CDataDownloader
{
public:
	CDataDownloader();
	~CDataDownloader();

public: 
	/// 启动
	/// @param[in]	  pFilePath 文件存储路径
	/// @return		  参见j_errtype.h  
	j_result_t Start(const char *pFilePath);
	/// 停止
	/// @return		  参见j_errtype.h    
	j_result_t Stop();

private:
	static void OnDownLoadReturn(void *pUsesr, j_boolean_t bFinish)
	{
		CDataDownloader *pThis = static_cast<CDataDownloader *>(pUsesr);
		if (pThis != NULL)
			pThis->OnReturn(bFinish);
	}
	void OnReturn( j_boolean_t bFinish);
	static unsigned X_JO_API OnWorkFunc(void *pParam)
	{
		CDataDownloader *pThis = static_cast<CDataDownloader *>(pParam);
		if (pThis != NULL)
			pThis->OnWork();

		return 0;
	}
	void OnWork();

private:
	j_boolean_t GetFileList();
	void StartDownLoad();
	void EndDownLoad();

private:
	j_thread_parm m_param;
	CJoThread m_workTread;
	j_boolean_t m_bStart;
	j_boolean_t m_bStartHost;
	j_boolean_t m_bStartFile;
	DownLoadFtpInfo m_info;
	CFtpHelper m_ftpHelper;
	VecFileName m_vecFileName;
	j_string_t m_strPath;
	j_string_t m_strRemoteFile;
	j_string_t m_strLocalFile;
};

#endif //~__DATA_DOWNLOADER_H_