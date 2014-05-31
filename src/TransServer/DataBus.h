#ifndef __DATA_BUS_H_
#define __DATA_BUS_H_
#include "j_includes.h"
#include "x_timer.h"
#include "x_lock.h"
#include "DataObj.h"

class CDataBus
{
	/// �豸ע��
	typedef std::map<j_string_t, J_Host *> HostMap;
	/// ��Ϣ����
	typedef std::vector<J_Client *> MsgClientVec;
	typedef std::map<j_int32_t, MsgClientVec> MsgClientMap;
	typedef std::map<j_string_t, MsgClientMap> MsgMap;
	/// ��Ϣ���󼰻ظ�
	struct RexInfo
	{
		J_Client *pClient;
		j_int32_t nTimeOut;
	};
	typedef std::map<j_string_t, RexInfo> RexMap;
	/// ��Ϣ��ʱ
	typedef std::map<j_string_t, J_Client *> TimeOutMap;
public:
	CDataBus();
	~CDataBus();

public:
	/// ע���豸����
	/// @param[in]		strHostId �豸ID
	/// @param[in]		pHost �豸���� 
	/// @return			�μ�j_errtype.h 
	j_result_t RegisterDevice(j_string_t strHostId, J_Host *pHost);
	/// �����豸��Ϣ
	/// @param[in]		pClient �����߶���
	/// @param[in]		strHostId �豸ID
	/// @param[in]		nMsgType ��Ϣ���� 
	/// @param[in]		bSub true-��ʾ����, false-��ʾ�˶�
	/// @return			�μ�j_errtype.h 
	j_result_t SubscribeMsg(j_string_t strHostId, j_int32_t nMsgType, J_Client *pClient, j_boolean_t bSub = true);
	/// ����������Ϣ
	/// @param[in]		strHostId �豸ID
	/// @param[in]		pReqInfo ������Ϣ����
	/// @param[in]		nTimeOut ����ʱʱ��,��λms 
	/// @return			�μ�j_errtype.h 
	j_result_t Request(j_string_t strHostId, J_Client *pClient, CDataObj *pReqInfo, j_uint32_t nTimeOut = 3000);
	/// ����Ӧ����Ϣ
	/// @param[in]		strHostId �豸ID
	/// @param[in]		pRespInfo Ӧ����Ϣ����
	/// @return			�μ�j_errtype.h 
	j_result_t Response(j_string_t strHostId, CDataObj *pRespInfo);
	/// �¼���Ϣ
	/// @param[in]		strHostId �豸ID
	/// @param[in]		nMsgType ��Ϣ����
	/// @param[in]		nSubType ��Ϣ������
	/// @param[in]		pMsgInfo �¼���Ϣ����
	/// @return			�μ�j_errtype.h 
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