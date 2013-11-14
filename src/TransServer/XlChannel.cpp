#include "XlChannel.h"
#include "XlDType.h"

CXlChannel::CXlChannel(J_Obj *pOwner, j_int32_t nChannel)
{
	m_nChannelNum = nChannel;
}

CXlChannel::~CXlChannel()
{

}

j_result_t CXlChannel::OpenStream(CRingBuffer *pRingBuffer)
{
	AddRingBuffer(pRingBuffer);
	return J_OK;
}

j_result_t CXlChannel::CloseStream(CRingBuffer *pRingBuffer)
{
	DelRingBuffer(pRingBuffer);
	return J_OK;
}

j_result_t CXlChannel::OpenVod(const j_guid_t &sessionId, CRingBuffer *pRingBuffer)
{
	TLock(m_mapLocker);
	std::map<j_guid_t, CRingBuffer *>::iterator it = m_mapRingBuffer.find(sessionId);
	if (it == m_mapRingBuffer.end())
	{
		m_mapRingBuffer[sessionId] = pRingBuffer;
	}
	TUnlock(m_mapLocker);

	return J_OK;
}

j_result_t CXlChannel::CloseVod(const j_guid_t &sessionId, CRingBuffer *pRingBuffer)
{
	TLock(m_mapLocker);
	std::map<j_guid_t, CRingBuffer *>::iterator it = m_mapRingBuffer.find(sessionId);
	if (it != m_mapRingBuffer.end())
	{
		m_mapRingBuffer.erase(it);
	}
	TUnlock(m_mapLocker);

	return J_OK;
}

j_result_t CXlChannel::InputData(const j_int32_t nType, const j_char_t *pData, j_int32_t nLen)
{
	J_StreamHeader streamHeader = {0};
	streamHeader.dataLen = nLen;

	if (nType == 0)
	{
		TLock(m_vecLocker);
		std::vector<CRingBuffer *>::iterator it = m_vecRingBuffer.begin();
		for (; it != m_vecRingBuffer.end(); it++)
		{
			(*it)->PushBuffer(pData, streamHeader);
		}
		TUnlock(m_vecLocker);
	}
	else
	{
		GUID *pGuid = (GUID *)(pData + sizeof(CmdHeader));
		TLock(m_mapLocker);
		std::map<j_guid_t, CRingBuffer *>::iterator it = m_mapRingBuffer.find(*pGuid);
		if (it != m_mapRingBuffer.end())
		{
			it->second->PushBuffer(pData, streamHeader);
		}
		TUnlock(m_mapLocker);
	}
	//J_OS::LOGINFO("channel = %d length = %d", m_nChannelNum, nLen);
	return J_OK;
}


