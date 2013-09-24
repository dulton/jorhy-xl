///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx�Ƽ����޹�˾(��Ȩ����) 
/// All rights reserved. 
/// 
/// @file      StreamManager.h 
/// @brief     ��ת������ģ��
///
/// @version   1.0 (�汾����)
/// @author    Jorhy (joorhy@gmail.com) 
/// @date      2013/09/24 11:37 
///
///
/// �޶�˵��������汾
///////////////////////////////////////////////////////////////////////////
#ifndef __STREAMMANAGER_H_
#define __STREAMMANAGER_H_
#include "j_includes.h"

#ifdef   LIBMANAGER_EXPORTS 
#define   JO_STREAMMAN_API           __declspec(dllexport) 
#else 
#define   JO_STREAMMAN_API           __declspec(dllimport) 
#endif 

/// ����Ĺ���:  ��ת��������
class JO_STREAMMAN_API CStreamManager
{
public:
	CStreamManager();
	~CStreamManager();

public:
	/// ��ʼ���ݴ���
	/// @param[in]
	/// @param[in]  
	/// @param[out]   
	/// @return		  �μ�j_errtype.h 
	/// @see   
	/// @note
	j_result_t StartTrans();
	/// ֹͣ��������
	/// @param[in]
	/// @param[in]  
	/// @param[out]   
	/// @return		  �μ�j_errtype.h 
	/// @see   
	/// @note    
	j_result_t StopTrans();
};
#endif//~__STREAMMANAGER_H_