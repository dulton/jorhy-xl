///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx�Ƽ����޹�˾(��Ȩ����) 
/// All rights reserved. 
/// 
/// @file         DbManager.h 
/// @brief       ���ݿ����ģ��
///
/// @version   1.0 (�汾����)
/// @author    Jorhy (joorhy@gmail.com) 
/// @date       2013/09/18 16:40 
///
///
/// �޶�˵��������汾
///////////////////////////////////////////////////////////////////////////  
#include "j_includes.h"

#ifndef __DB_MANAGER_H_
#define __DB_MANAGER_H_

/// ����Ĺ���:  ���ݿ����
/// ������һ�����ݿ������
/// �ڳ�����������Ҫ���ӡ�ɾ�����޸ĺͲ������ݿ���Ϣ�ĵط�
class CDbManager
{
public:
	CDbManager();
	~CDbManager();

public:
	/// ��ʼ�����ݿ�����
	/// @param[in]
	/// @param[in]  
	/// @param[out]   
	/// @return		  �μ�j_errtype.h 
	/// @see   
	/// @note    
	j_result_t Init();
	/// ע�����ݿ�����
	/// @param[in]
	/// @param[in]  
	/// @param[out]   
	/// @return		  �μ�j_errtype.h 
	/// @see   
	/// @note     
	j_result_t Deinit();
};
#endif //~__DB_MANAGER_H_