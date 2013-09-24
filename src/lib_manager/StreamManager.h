///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx科技有限公司(版权声明) 
/// All rights reserved. 
/// 
/// @file      StreamManager.h 
/// @brief     流转发管理模块
///
/// @version   1.0 (版本声明)
/// @author    Jorhy (joorhy@gmail.com) 
/// @date      2013/09/24 11:37 
///
///
/// 修订说明：最初版本
///////////////////////////////////////////////////////////////////////////
#ifndef __STREAMMANAGER_H_
#define __STREAMMANAGER_H_
#include "j_includes.h"

#ifdef   LIBMANAGER_EXPORTS 
#define   JO_STREAMMAN_API           __declspec(dllexport) 
#else 
#define   JO_STREAMMAN_API           __declspec(dllimport) 
#endif 

/// 本类的功能:  流转发控制类
class JO_STREAMMAN_API CStreamManager
{
public:
	CStreamManager();
	~CStreamManager();

public:
	/// 开始数据传输
	/// @param[in]
	/// @param[in]  
	/// @param[out]   
	/// @return		  参见j_errtype.h 
	/// @see   
	/// @note
	j_result_t StartTrans();
	/// 停止传输数据
	/// @param[in]
	/// @param[in]  
	/// @param[out]   
	/// @return		  参见j_errtype.h 
	/// @see   
	/// @note    
	j_result_t StopTrans();
};
#endif//~__STREAMMANAGER_H_