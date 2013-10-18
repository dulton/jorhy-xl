///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx�Ƽ����޹�˾(��Ȩ����) 
/// All rights reserved. 
/// 
/// @file      Client.h 
/// @brief     �ͻ���ģ��
///
/// @version   1.0 (�汾����)
/// @author    Jorhy (joorhy@gmail.com) 
/// @date      2013/09/24 15:11 
///
///
/// �޶�˵��������汾
///////////////////////////////////////////////////////////////////////////  
#ifndef __CLIENT_H_
#define __CLIENT_H_

#ifdef   LIBMANAGER_EXPORTS 
#define   JO_XLCLIENT_API           __declspec(dllexport) 
#else 
#define   JO_XLCLIENT_API           __declspec(dllimport) 
#endif 

#include "j_includes.h"
/// ����Ĺ���:  �ͻ���ҵ������
class JO_XLCLIENT_API CXlClient : public J_Client
{
public:
	CXlClient(j_socket_t nSock);
	~CXlClient();

public:
	///J_Client
	j_result_t ParserRequest(J_AsioDataBase *pAsioData);

private:
	/// ����������Ϣ��
	/// @param[in]		bCmd������
	/// @param[in]		pData����
	/// @param[in]		nLen���ݳ���
	/// @param[out]   pBody��Ϣ������
	/// @return			�μ�j_errtype.h 
	j_result_t MakeResponse(j_char_t bCmd, j_char_t *pData, j_int32_t nLen, j_char_t *pBody);
	/// ����У���
	/// @param[in]		pData����
	/// @param[in]		nLen���ݳ��� 
	/// @return			У���  
	j_uint32_t CheckNum(j_char_t *pData, j_int32_t nLen);
	/// ������������
	/// @param[out]		pAsioData �첽����
	/// @param[in]			pDataBuff ���ݻ���
	/// @param[in]			nLen ���ݳ���
	/// @return				�μ�j_errtype.h 
	j_result_t MakeNetData(J_AsioDataBase *pAsioData, j_char_t *pDataBuff, j_int32_t nLen);
	/// ���ɴ�������
	/// @param[out]		pAsioData �첽����
	/// @return				�μ�j_errtype.h 
	j_result_t MakeTransData(J_AsioDataBase *pAsioData);
private:
	j_char_t *m_readBuff;
	j_char_t *m_writeBuff;
	j_char_t *m_dataBuff;
	j_int32_t m_ioCmdState;					//1-��ͷ����,2-��������+β����
	j_int32_t m_ioDataState;					//1-��ͷ����,2-��������+β����
	CRingBuffer m_ringBuffer;
	J_StreamHeader m_streamHeader;
	j_long_t m_nRefCnt; 
};
#endif // ~__CLIENT_H_
