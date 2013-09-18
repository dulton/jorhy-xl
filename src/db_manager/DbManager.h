///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx科技有限公司(版权声明) 
/// All rights reserved. 
/// 
/// @file         DbManager.h 
/// @brief       数据库管理模块
///
/// @version   1.0 (版本声明)
/// @author    Jorhy (joorhy@gmail.com) 
/// @date       2013/09/18 16:40 
///
///
/// 修订说明：最初版本
///////////////////////////////////////////////////////////////////////////  
#include "j_includes.h"

#ifndef __DB_MANAGER_H_
#define __DB_MANAGER_H_

/// 本类的功能:  数据库操作
/// 本类是一个数据库操作类
/// 在程序中用在需要增加、删除、修改和查找数据库信息的地方
class CDbManager
{
public:
	CDbManager();
	~CDbManager();

public:
	/// 初始化数据库连接
	/// @param[in]
	/// @param[in]  
	/// @param[out]   
	/// @return		  参见j_errtype.h 
	/// @see   
	/// @note    
	j_result_t Init();
	/// 注销数据库连接
	/// @param[in]
	/// @param[in]  
	/// @param[out]   
	/// @return		  参见j_errtype.h 
	/// @see   
	/// @note     
	j_result_t Deinit();
};
#endif //~__DB_MANAGER_H_