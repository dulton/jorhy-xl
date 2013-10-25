///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx科技有限公司(版权声明) 
/// All rights reserved. 
/// 
/// @file		  TcpServer4Client.h 
/// @brief       客户端TCP服务器模块
///
/// @version   1.0 (版本声明)
/// @author    Jorhy (joorhy@gmail.com) 
/// @date       2013/09/22 09:43 
///
///
/// 修订说明：最初版本
///////////////////////////////////////////////////////////////////////////  
#ifndef __TCP_SERVER4CLIENT_H_
#define __TCP_SERVER4CLIENT_H_
#include "j_includes.h"
#include "x_asio_win.h"
/// 本类的功能:  客户端请求
/// 完成客户端相关工作 
class CTcpServer4Client : public J_AsioUser
{
public:
	CTcpServer4Client();
	~CTcpServer4Client();

	///AsioUser
	virtual j_result_t OnAccept(J_AsioDataBase *pAsioData, int nRet);
	virtual j_result_t OnRead(J_AsioDataBase *pAsioData, int nRet);
	virtual j_result_t OnWrite(J_AsioDataBase *pAsioData, int nRet);
	virtual j_result_t OnBroken(J_AsioDataBase *pAsioData, int nRet);

	/// 启动服务
	/// @param[in]  nPort 服务端口号
	/// @param[in]  
	/// @param[out]   
	/// @return		  参见j_errtype.h 
	/// @see   
	/// @note    
	j_result_t StartService(int nPort);
	/// 停止服务
	/// @param[in]
	/// @param[in]  
	/// @param[out]   
	/// @return		  参见j_errtype.h 
	/// @see   
	/// @note    
	j_result_t StopService();

private:
	J_AsioDataBase m_asioData;
	CXAsio m_asio;
};
#endif //~__TCP_SERVER4CLIENT_H_