#include "DataBus.h"

JO_IMPLEMENT_SINGLETON(DataBus)

CDataBus::CDataBus()
{
	m_timer.Create(1000, CDataBus::OnTimer, this);
}

CDataBus::~CDataBus()
{
	m_timer.Destroy();
}

j_result_t CDataBus::RegisterDevice(j_string_t strHostId, J_Host *pHost)
{
	TLock(m_locker);
	HostMap::iterator it = m_hostMap.find(strHostId);
	if (it != m_hostMap.end())
	{
		if (pHost == NULL)
		{
			m_hostMap.erase(it);
		}
	}
	else
	{
		if (pHost != NULL)
		{
			m_hostMap[strHostId] = pHost;
		}
	}
	TUnlock(m_locker);

	return J_OK;
}

j_result_t CDataBus::SubscribeMsg( j_string_t strHostId, j_int32_t nMsgType, J_Client *pClient, j_boolean_t bSub)
{
	TLock(m_locker);
	MsgMap::iterator itMsg = m_msgMap.find(strHostId);
	if (itMsg != m_msgMap.end())
	{
		MsgClientMap::iterator itMsgCli = itMsg->second.find(nMsgType);
		if (itMsgCli != itMsg->second.end())
		{
			if (bSub == false)
			{
				/// 退订消息
				MsgClientVec::iterator itCliVec = itMsgCli->second.begin();
				for (; itCliVec!=itMsgCli->second.end(); ++itCliVec)
				{
					if (*itCliVec == pClient)
					{
						itMsgCli->second.erase(itCliVec);
						break;
					}
				}
			}
			else
			{
				/// 订阅消息
				itMsgCli->second.push_back(pClient);
			}
		}
		else
		{
			/// 初次订阅消息,已经建立与消息的映射
			MsgClientVec cliVec;
			cliVec.push_back(pClient);
			itMsg->second[nMsgType] = cliVec;
		}
	}
	else
	{
		/// 初次订阅消息,没有建立与消息的映射
		MsgClientVec cliVec;
		cliVec.push_back(pClient);
		MsgClientMap cliMap;
		cliMap[nMsgType] = cliVec;
		m_msgMap[strHostId] = cliMap;
	}
	TUnlock(m_locker);

	return J_OK;
}

j_result_t CDataBus::Request(j_string_t strHostId, J_Client *pClient, CDataObj *pReqInfo, j_uint32_t nTimeOut)
{
	TLock(m_locker);
	RexMap::iterator it = m_rexMap.find(strHostId);
	if (it != m_rexMap.end())
	{
		/// 请求忙
	}
	else
	{
		/// 请求数据
		RexInfo info = {0};
		info.pClient = pClient;
		info.nTimeOut = nTimeOut;
		m_rexMap[strHostId] = info;
	}
	TUnlock(m_locker);
	return J_OK;
}

j_result_t CDataBus::Response(j_string_t strHostId, CDataObj *pRespInfo)
{
	TLock(m_locker);
	RexMap::iterator it = m_rexMap.find(strHostId);
	if (it != m_rexMap.end())
	{
		/// 回复数据
		it->second.pClient->SendRespInfo(pRespInfo);
		m_rexMap.erase(it);
	}
	TUnlock(m_locker);
	return J_OK;
}

j_result_t CDataBus::OnMessage(j_string_t strHostId, j_int32_t nMsgType, j_int32_t nSubType, CDataObj *pMsgInfo)
{
	TLock(m_locker);
	MsgMap::iterator itMsg = m_msgMap.find(strHostId);
	if (itMsg != m_msgMap.end())
	{
		MsgClientMap::iterator itMsgCli = itMsg->second.find(nMsgType);
		if (itMsgCli != itMsg->second.end())
		{
			MsgClientVec::iterator itCliVec = itMsgCli->second.begin();
			for (; itCliVec!=itMsgCli->second.end(); ++itCliVec)
			{
				/// 发送消息
				(*itCliVec)->SendMsgInfo(strHostId, nMsgType, nSubType);
			}
		}
	}

	itMsg = m_msgMap.find("all");
	if (itMsg != m_msgMap.end())
	{
		MsgClientMap::iterator itMsgCli = itMsg->second.find(nMsgType);
		if (itMsgCli != itMsg->second.end())
		{
			MsgClientVec::iterator itCliVec = itMsgCli->second.begin();
			for (; itCliVec!=itMsgCli->second.end(); ++itCliVec)
			{
				/// 发送消息
				(*itCliVec)->SendMsgInfo(strHostId, nMsgType, nSubType);
			}
		}
	}

	TUnlock(m_locker);

	return J_OK;
}

void CDataBus::CheckState()
{
	TLock(m_locker);
	RexMap::iterator it = m_rexMap.begin();
	for (; it != m_rexMap.end(); ++it)
	{
		/// 判断是否超时
		if (it->second.nTimeOut > 0)
		{
			it->second.nTimeOut -= 1000;
		}
		else
		{
			/// 超时处理
			m_timeOutMap[it->first] = it->second.pClient;
		}
	}
	TimeOutMap::iterator itTimeOut = m_timeOutMap.begin();
	for (; itTimeOut!=m_timeOutMap.end(); ++itTimeOut)
	{
		/// 发送超时消息
		/// 删除消息
		m_rexMap.erase(itTimeOut->first);
	}
	TUnlock(m_locker);
}