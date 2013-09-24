///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx�Ƽ����޹�˾(��Ȩ����) 
/// All rights reserved. 
/// 
/// @file			TcpServer4Dev.h 
/// @brief			�豸��TCP����ģ��
///
/// @version    1.0 (�汾����)
/// @author     Jorhy (joorhy@gmail.com) 
/// @date			2013/09/18 17:12 
///
///
/// �޶�˵��������汾
///////////////////////////////////////////////////////////////////////////  

#ifndef __TCP_SERVER4DEV_H_
#define __TCP_SERVER4DEV_H_
#include "j_includes.h"
#include "x_asio_win.h"

#ifdef   TCPSERVER_EXPORTS 
#define   JO_TCPSERVER_API           __declspec(dllexport) 
#else 
#define   JO_TCPSERVER_API           __declspec(dllimport) 
#endif 

/// ����Ĺ���:  ǰ���豸TCP������
/// �������ǰ���豸������\n
/// ���DVRע�ᡢУʱ��������ػ���־�ϴ�������ά�֡�ʵʱ��������ȡ�豸��Ϣ�ȹ���
class JO_TCPSERVER_API CTcpServer4Device : public J_AsioUser
{
public:
	CTcpServer4Device();
	~CTcpServer4Device();

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

private:
	J_AsioDataBase m_asioData;
	CXAsio m_asio;
};

#endif //~__TCP_SERVER4DEV_H_