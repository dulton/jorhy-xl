#ifndef __DATA_BUS_H_
#define __DATA_BUS_H_
#include "j_includes.h"
#include "x_timer.h"
#include "x_lock.h"
#include "DataObj.h"

class CDataBus
{
	/// 设备注册
	typedef std::map<j_string_t, J_Host *> HostMap;
	/// 消息订阅
	typedef std::vector<J_Client *> MsgClientVec;
	typedef std::map<j_int32_t, MsgClientVec> MsgClientMap;
	typedef std::map<j_string_t, MsgClientMap> MsgMap;
	/// 消息请求及回复
	struct RexInfo
	{
		J_Client *pClient;
		j_int32_t nTimeOut;
	};
	typedef std::map<j_string_t, RexInfo> RexMap;
	/// 消息超时
	typedef std::map<j_string_t, J_Client *> TimeOutMap;
public:
	CDataBus();
	~CDataBus();

public:
	/// 注册设备对象
	/// @param[in]		strHostId 设备ID
	/// @param[in]		pHost 设备对象 
	/// @return			参见j_errtype.h 
	j_result_t RegisterDevice(j_string_t strHostId, J_Host *pHost);
	/// 订阅设备消息
	/// @param[in]		pClient 订阅者对象
	/// @param[in]		strHostId 设备ID
	/// @param[in]		nMsgType 消息类型 
	/// @param[in]		bSub true-表示订阅, false-表示退订
	/// @return			参见j_errtype.h 
	j_result_t SubscribeMsg(j_string_t strHostId, j_int32_t nMsgType, J_Client *pClient, j_boolean_t bSub = true);
	/// 发送请求消息
	/// @param[in]		strHostId 设备ID
	/// @param[in]		pReqInfo 请求消息内容
	/// @param[in]		nTimeOut 请求超时时间,单位ms 
	/// @return			参见j_errtype.h 
	j_result_t Request(j_string_t strHostId, J_Client *pClient, CDataObj *pReqInfo, j_uint32_t nTimeOut = 3000);
	/// 请求应答消息
	/// @param[in]		strHostId 设备ID
	/// @param[in]		pRespInfo 应答消息内容
	/// @return			参见j_errtype.h 
	j_result_t Response(j_string_t strHostId, CDataObj *pRespInfo);
	/// 事件消息
	/// @param[in]		strHostId 设备ID
	/// @param[in]		nMsgType 消息类型
	/// @param[in]		nSubType 消息子类型
	/// @param[in]		pMsgInfo 事件消息内容
	/// @return			参见j_errtype.h 
	j_result_t OnMessage(j_string_t strHostId, j_int32_t nMsgType, j_int32_t nSubType, CDataObj *pMsgInfo = NULL);

private:
	static void OnTimer(void *pUser)
	{
		(static_cast<CDataBus *>(pUser))->CheckState();
	}
	void CheckState();

private:
	J_OS::CTimer m_timer;
	J_OS::CTLock m_locker;

	HostMap m_hostMap;
	MsgMap m_msgMap;
	RexMap m_rexMap;
	TimeOutMap m_timeOutMap;
};

JO_DECLARE_SINGLETON(DataBus)

#endif //~__DATA_BUS_H_