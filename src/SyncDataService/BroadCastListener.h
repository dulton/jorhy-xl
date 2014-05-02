#ifndef __BROADCAST_LISTENER_H_
#define __BROADCAST_LISTENER_H_
#include "j_includes.h"
#include "x_thread.h"

class CBroadCastListener
{
public:
	CBroadCastListener();
	~CBroadCastListener();

	/// 启动服务
	/// @param[in]  nPort 服务端口号
	/// @return		  参见j_errtype.h  
	j_result_t StartService(int nPort);
	/// 停止服务
	/// @return		  参见j_errtype.h    
	j_result_t StopService();

private:
	static unsigned X_JO_API OnWorkFunc(void *pParam)
	{
		CBroadCastListener *pThis = static_cast<CBroadCastListener *>(pParam);
		if (pThis != NULL)
			pThis->OnWork();

		return 0;
	}
	void OnWork();

private:
	SOCKET m_broadCastListener;
	j_char_t m_recvBuff[1500];

	j_thread_parm m_param;
	CJoThread m_workTread;
	bool m_bStart;
};

#endif //~__BROADCAST_LISTENER_H_