///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx科技有限公司(版权声明) 
/// All rights reserved. 
/// 
/// @file      XlHost.h 
/// @brief     XL_DVR设备模块
///
/// @version   1.0 (版本声明)
/// @author    Jorhy (joorhy@gmail.com) 
/// @date      2013/09/22 17:40 
///
///
/// 修订说明：最初版本
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

/// 本类的功能:  XL设备接入类
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
	/// 生成命令消息体
	/// @param[in]		bCmd命令码
	/// @param[in]		pData数据
	/// @param[in]		nLen数据长度
	/// @param[out]   pBody消息体数据
	/// @return			参见j_errtype.h 
	j_result_t MakeCommand(j_char_t bCmd, j_char_t *pData, j_int32_t nLen, j_char_t *pBody);
	/// 计算校验和
	/// @param[in]		pData数据
	/// @param[in]		nLen数据长度 
	/// @return			校验和  
	j_uint32_t CheckNum(j_char_t *pData, j_int32_t nLen);
	/// 生成网络数据
	/// @param[out]		pAsioData 异步数据
	/// @param[in]			pDataBuff 数据缓存
	/// @param[in]			nLen 数据长度
	/// @return		  参见j_errtype.h 
	j_result_t MakeNetData(J_AsioDataBase *pAsioData, j_char_t *pDataBuff, j_int32_t nLen);

private:
	j_boolean_t m_bReady;
	j_string_t m_hostId;
	j_int32_t m_lastBreatTime;
	J_OS::CTCPSocket m_cmdSocket;

	ChannelMap m_channelMap;
	j_char_t *m_readBuff;
	j_char_t *m_writeBuff;
	j_int32_t m_ioState;				//1-读头数据,2-读数据区+尾数据
};

#endif