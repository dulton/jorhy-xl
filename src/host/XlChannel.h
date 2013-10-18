///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx�Ƽ����޹�˾(��Ȩ����) 
/// All rights reserved. 
/// 
/// @file      XlChannel.h 
/// @brief     XL_DVRͨ��ģ��
///
/// @version   1.0 (�汾����)
/// @author    Jorhy (joorhy@gmail.com) 
/// @date      2013/10/17 20:47 
///
///
/// �޶�˵��������汾
///////////////////////////////////////////////////////////////////////////  
#ifndef __XLCHANNEL_H_
#define __XLCHANNEL_H_
#include "j_includes.h"
/// ����Ĺ���:  ��������
class CXlChannel : public J_Channel
{
public:
	CXlChannel(J_Obj *pOwner, j_int32_t nChannel);
	~CXlChannel();

public:
	///J_Channel
	virtual j_result_t OpenStream(CRingBuffer *pRingBuffer);
	virtual j_result_t CloseStream(CRingBuffer *pRingBuffer);

public:
	/// ��������
	/// @param[in]				pData ������
	/// @param[in]				nLen ���ݳ��� 
	/// @return					�μ�j_errtype.h 
	j_result_t InputData(const j_char_t *pData, j_int32_t nLen);

private:
	int AddRingBuffer(CRingBuffer *pRingBuffer)
	{
		TLock(m_vecLocker);
		m_vecRingBuffer.push_back(pRingBuffer);
		TUnlock(m_vecLocker);

		return J_OK;
	}
	int DelRingBuffer(CRingBuffer *pRingBuffer)
	{
		TLock(m_vecLocker);
		std::vector<CRingBuffer *>::iterator it = m_vecRingBuffer.begin();
		for (; it != m_vecRingBuffer.end(); it++)
		{
			if (*it == pRingBuffer)
			{
				m_vecRingBuffer.erase(it);
				TUnlock(m_vecLocker);

				return J_OK;
			}
		}
		TUnlock(m_vecLocker);
		return J_NOT_EXIST;
	}
private:
	j_int32_t m_nChannelNum;
	J_OS::TLocker_t m_vecLocker;
	std::vector<CRingBuffer *> m_vecRingBuffer;
};

#endif //~__XLCHANNEL_H_
