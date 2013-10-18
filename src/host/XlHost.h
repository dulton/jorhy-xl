///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx�Ƽ����޹�˾(��Ȩ����) 
/// All rights reserved. 
/// 
/// @file      XlHost.h 
/// @brief     XL_DVR�豸ģ��
///
/// @version   1.0 (�汾����)
/// @author    Jorhy (joorhy@gmail.com) 
/// @date      2013/09/22 17:40 
///
///
/// �޶�˵��������汾
///////////////////////////////////////////////////////////////////////////  
#ifndef __XLHOST_H_
#define __XLHOST_H_
#include "j_includes.h"
#include "x_socket.h"

#ifdef   XLHOST_EXPORTS 
#define   JO_XLHOST_API           __declspec(dllexport) 
#else 
#define   JO_XLHOST_API           __declspec(dllimport) 
#endif 

/// ����Ĺ���:  XL�豸������
class JO_XLHOST_API CXlHost : public J_Host
{
	typedef std::map<j_int32_t, J_Channel *> ChannelMap;
public:
	CXlHost(j_socket_t nSock);
	~CXlHost();

public:
	///J_Host
	virtual j_result_t MakeChannel(const j_int32_t nChannelNum, J_Channel *&pObj);
	virtual j_boolean_t IsReady();
	virtual j_result_t GetHostId(j_string_t &strDevId);
	virtual j_result_t SetTime();
	virtual j_result_t GetDeviceInfo();
	virtual j_result_t ParserRequest(J_AsioDataBase *pAsioData);

private:
	/// ����������Ϣ��
	/// @param[in]		bCmd������
	/// @param[in]		pData����
	/// @param[in]		nLen���ݳ���
	/// @param[out]   pBody��Ϣ������
	/// @return			�μ�j_errtype.h 
	j_result_t MakeCommand(j_char_t bCmd, j_char_t *pData, j_int32_t nLen, j_char_t *pBody);
	/// ����У���
	/// @param[in]		pData����
	/// @param[in]		nLen���ݳ��� 
	/// @return			У���  
	j_uint32_t CheckNum(j_char_t *pData, j_int32_t nLen);
	/// ������������
	/// @param[out]		pAsioData �첽����
	/// @param[in]			pDataBuff ���ݻ���
	/// @param[in]			nLen ���ݳ���
	/// @return		  �μ�j_errtype.h 
	j_result_t MakeNetData(J_AsioDataBase *pAsioData, j_char_t *pDataBuff, j_int32_t nLen);

private:
	j_boolean_t m_bReady;
	j_string_t m_hostId;
	j_int32_t m_lastBreatTime;
	J_OS::CTCPSocket m_cmdSocket;

	ChannelMap m_channelMap;
	j_char_t *m_readBuff;
	j_char_t *m_writeBuff;
	j_int32_t m_ioState;				//1-��ͷ����,2-��������+β����
};

#endif