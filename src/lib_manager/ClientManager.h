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

#ifdef   LIBMANAGER_EXPORTS 
#define   JO_CLIENTMAN_API           __declspec(dllexport) 
#else 
#define   JO_CLIENTMAN_API           __declspec(dllimport) 
#endif 

/// ����Ĺ���:  ����ͻ�����Ϣ
/// �����Ǹ�����ģ��
/// ����ͻ��˵ĵ�¼���˳����������ݵȲ���
class JO_CLIENTMAN_API CClientManager
{
public:
	CClientManager();
	~CClientManager();

public:
	/// ��¼
	/// @param[in]		nSock  ����Socket
	/// @param[in]		pUserName �û���
	/// @param[in]		pPassWord  ����
	/// @return		    �û�ID,=0-��¼ʧ��  
	j_uint32_t Login(j_socket_t nSock, const j_char_t *pUserName, const j_char_t *pPassWord);
	/// ע��
	/// @param[in]		nUserId �û�ID 
	/// @return		     
	void Logout(j_uint32_t nUserId);
	/// �û����󼰻ظ�
	/// @param[in]		pData��������
	/// @param[in]		nLen�������ݳ���
	/// @param[out]	pRetData�ظ�����,��Ҫ�ͷ�	
	/// @param[out]	nRetLen�ظ����ݳ���
	/// @return			�μ�j_errtype.h 
	j_result_t RequestData(const j_char_t *pData, const j_int32_t &nLen, j_char_t **pRetData, j_int32_t &nRetLen);
};
#endif //~__CLIENT_MANAGER_H_