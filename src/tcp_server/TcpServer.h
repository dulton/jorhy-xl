///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx科技有限公司(版权声明) 
/// All rights reserved. 
/// 
/// @file      TcpServer.h 
/// @brief     TCP服务模块
///
/// @version   1.0 (版本声明)
/// @author    Jorhy (joorhy@gmail.com) 
/// @date      2013/09/18 17:12 
///
///
/// 修订说明：最初版本
///////////////////////////////////////////////////////////////////////////  

#ifndef __TCP_SERVER_H_
#define __TCP_SERVER_H_
#include "j_includes.h"
#include "x_asio_win.h"
/// 本类的功能:  TCP服务器
///  本类接受客户端、前端设备的请求，\n
/// 完成客户端和前端设备映射，转发报警信息和GPS信息
class CTcpServer : public J_AsioUser
{
public:
	CTcpServer();
	~CTcpServer();

	///AsioUser
	virtual j_result_t OnAccept(const J_AsioDataBase *pAsioData, int nRet);
	virtual j_result_t OnRead(const J_AsioDataBase *pAsioData, int nRet);
	virtual j_result_t OnWrite(const J_AsioDataBase *pAsioData, int nRet);
	virtual j_result_t OnBroken(const J_AsioDataBase *pAsioData, int nRet);

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
};

#endif //~__TCP_SERVER_H_