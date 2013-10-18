///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx科技有限公司(版权声明) 
/// All rights reserved. 
/// 
/// @file      Client.h 
/// @brief     客户端模块
///
/// @version   1.0 (版本声明)
/// @author    Jorhy (joorhy@gmail.com) 
/// @date      2013/09/24 15:11 
///
///
/// 修订说明：最初版本
///////////////////////////////////////////////////////////////////////////  
#ifndef __CLIENT_H_
#define __CLIENT_H_

#ifdef   LIBMANAGER_EXPORTS 
#define   JO_XLCLIENT_API           __declspec(dllexport) 
#else 
#define   JO_XLCLIENT_API           __declspec(dllimport) 
#endif 

#include "j_includes.h"
/// 本类的功能:  客户端业务处理类
class JO_XLCLIENT_API CXlClient : public J_Client
{
public:
	CXlClient(j_socket_t nSock);
	~CXlClient();

public:
	///J_Client
	j_result_t ParserRequest(J_AsioDataBase *pAsioData);

private:
	/// 生成命令消息体
	/// @param[in]		bCmd命令码
	/// @param[in]		pData数据
	/// @param[in]		nLen数据长度
	/// @param[out]   pBody消息体数据
	/// @return			参见j_errtype.h 
	j_result_t MakeResponse(j_char_t bCmd, j_char_t *pData, j_int32_t nLen, j_char_t *pBody);
	/// 计算校验和
	/// @param[in]		pData数据
	/// @param[in]		nLen数据长度 
	/// @return			校验和  
	j_uint32_t CheckNum(j_char_t *pData, j_int32_t nLen);
	/// 生成网络数据
	/// @param[out]		pAsioData 异步数据
	/// @param[in]			pDataBuff 数据缓存
	/// @param[in]			nLen 数据长度
	/// @return				参见j_errtype.h 
	j_result_t MakeNetData(J_AsioDataBase *pAsioData, j_char_t *pDataBuff, j_int32_t nLen);
	/// 生成传输数据
	/// @param[out]		pAsioData 异步数据
	/// @return				参见j_errtype.h 
	j_result_t MakeTransData(J_AsioDataBase *pAsioData);
private:
	j_char_t *m_readBuff;
	j_char_t *m_writeBuff;
	j_char_t *m_dataBuff;
	j_int32_t m_ioCmdState;					//1-读头数据,2-读数据区+尾数据
	j_int32_t m_ioDataState;					//1-读头数据,2-读数据区+尾数据
	CRingBuffer m_ringBuffer;
	J_StreamHeader m_streamHeader;
	j_long_t m_nRefCnt; 
};
#endif // ~__CLIENT_H_
