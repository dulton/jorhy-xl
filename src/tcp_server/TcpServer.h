///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx�Ƽ����޹�˾(��Ȩ����) 
/// All rights reserved. 
/// 
/// @file      TcpServer.h 
/// @brief     TCP����ģ��
///
/// @version   1.0 (�汾����)
/// @author    Jorhy (joorhy@gmail.com) 
/// @date      2013/09/18 17:12 
///
///
/// �޶�˵��������汾
///////////////////////////////////////////////////////////////////////////  

#ifndef __TCP_SERVER_H_
#define __TCP_SERVER_H_
#include "j_includes.h"
#include "x_asio_win.h"
/// ����Ĺ���:  TCP������
///  ������ܿͻ��ˡ�ǰ���豸������\n
/// ��ɿͻ��˺�ǰ���豸ӳ�䣬ת��������Ϣ��GPS��Ϣ
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

	/// ��������
	/// @param[in]  nPort ����˿ں�
	/// @param[in]  
	/// @param[out]   
	/// @return		  �μ�j_errtype.h 
	/// @see   
	/// @note    
	j_result_t StartService(int nPort);
	/// ֹͣ����
	/// @param[in]
	/// @param[in]  
	/// @param[out]   
	/// @return		  �μ�j_errtype.h 
	/// @see   
	/// @note    
	j_result_t StopService();
};

#endif //~__TCP_SERVER_H_