#include "XlChannel.h"

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

j_result_t CXlChannel::InputData(const j_char_t *pData, j_int32_t nLen)
{
	J_StreamHeader streamHeader = {0};
	streamHeader.dataLen = nLen;

	TLock(m_vecLocker);
	std::vector<CRingBuffer *>::iterator it = m_vecRingBuffer.begin();
	for (; it != m_vecRingBuffer.end(); it++)
	{
		(*it)->PushBuffer(pData, streamHeader);
	}
	TUnlock(m_vecLocker);

	J_OS::LOGINFO("channel = %d length = %d", m_nChannelNum, nLen);
	return J_OK;
}


