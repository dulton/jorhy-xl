///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx�Ƽ����޹�˾(��Ȩ����) 
/// All rights reserved. 
/// 
/// @file		  TcpServer4Client.h 
/// @brief       �ͻ���TCP������ģ��
///
/// @version   1.0 (�汾����)
/// @author    Jorhy (joorhy@gmail.com) 
/// @date       2013/09/22 09:43 
///
///
/// �޶�˵��������汾
///////////////////////////////////////////////////////////////////////////  
#ifndef __TCP_SERVER4CLIENT_H_
#define __TCP_SERVER4CLIENT_H_
#include "j_includes.h"
#include "x_asio_win.h"
/// ����Ĺ���:  �ͻ�������
/// ��ɿͻ�����ع��� 
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
#endif //~__TCP_SERVER4CLIENT_H_