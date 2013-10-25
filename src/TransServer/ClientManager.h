///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx�Ƽ����޹�˾(��Ȩ����) 
/// All rights reserved. 
/// 
/// @file         ClientManager.h 
/// @brief       �ͻ��˹���ģ��
///
/// @version   1.0 (�汾����)
/// @author    Jorhy (joorhy@gmail.com) 
/// @date       2013/09/18 16:59 
///
///
/// �޶�˵��������汾
///////////////////////////////////////////////////////////////////////////  
#ifndef __CLIENT_MANAGER_H_
#define __CLIENT_MANAGER_H_
#include "j_includes.h"
#include "x_lock.h"

/// ����Ĺ���:  �����ͻ�����Ϣ
/// �����Ǹ�����ģ��
/// �����ͻ��˵ĵ�¼���˳����������ݵȲ���
class CClientManager
{
	typedef std::map<j_socket_t, J_Client *>	ClientMap;
public:
	CClientManager();
	~CClientManager();

public:
	/// �����ͻ��˶���
	/// @param[in]		nSock �豸���� 
	/// @param[out]   NULL-ʧ��,����ΪClient����
	/// @return			�μ�j_errtype.h 
	J_Client *CreateClientObj(j_socket_t nSock);
	/// ��ȡ�ͻ��˶���
	/// @param[in]		nnSock �豸���� 
	/// @return			Client����,NULL-δ�ҵ�   
	J_Client *GetClientObj(j_socket_t nSock);
	///ɾ���ͻ��˶���
	/// @param[in]		nSock �豸���� 
	/// @return			
	void ReleaseClientObj(j_socket_t nSock);

private:
	J_OS::CTLock m_locker;
	ClientMap m_clientMap;
};

JO_DECLARE_SINGLETON(ClientManager)

#endif //~__CLIENT_MANAGER_H_