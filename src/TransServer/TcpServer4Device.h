///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx科技有限公司(版权声明) 
/// All rights reserved. 
/// 
/// @file			TcpServer4Dev.h 
/// @brief			设备端TCP服务模块
///
/// @version    1.0 (版本声明)
/// @author     Jorhy (joorhy@gmail.com) 
/// @date			2013/09/18 17:12 
///
///
/// 修订说明：最初版本
///////////////////////////////////////////////////////////////////////////  

#ifndef __TCP_SERVER4DEV_H_
#define __TCP_SERVER4DEV_H_
#include "j_includes.h"
#include "x_asio_win.h"
/// 本类的功能:  前端设备TCP服务器
/// 本类接受前端设备的请求，\n
/// 完成DVR注册、校时操作、快关机日志上传、心跳维持、实时报警、获取设备信息等功能
class CTcpServer4Device : public J_AsioUser
{
public:
	CTcpServer4Device();
	~CTcpServer4Device();

	///AsioUser
	virtual j_result_t OnAccept(J_AsioDataBase *pAsioData, int nRet);
	virtual j_result_t OnRead(J_AsioDataBase *pAsioData, int nRet);
	virtual j_result_t OnWrite(J_AsioDataBase *pAsioData, int nRet);
	virtual j_result_t OnBroken(J_AsioDataBase *pAsioData, int nRet);

	/// 启动服务
	/// @param[in]  nPort 服务端口号
	/// @return		  参见j_errtype.h 
	j_result_t StartService(int nPort);
	/// 停止服务
	/// @return		  参见j_errtype.h  
	j_result_t StopService();

	/// 发送数据
	j_result_t SentData(j_socket_t nSocket);

private:
	J_AsioDataBase m_asioData;
	CXAsio m_asio;
	j_char_t *m_buff;
	j_int32_t m_state;				//1-读头数据,2-读数据区+尾数据
};

#endif //~__TCP_SERVER4DEV_H_