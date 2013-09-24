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

#ifdef   LIBMANAGER_EXPORTS 
#define   JO_DBM_API           __declspec(dllexport) 
#else 
#define   JO_DBM_API           __declspec(dllimport) 
#endif 

/// 数据库类型  
enum DbType
{
	J_SQLSERVER = 1,		///< SqlServer
};

/// 本类的功能:  管理数据库
class JO_DBM_API CDbManager
{
public:
	static J_DbAccess *CreateInstance(DbType type, const j_char_t *pAddr, j_int16_t nPort, const j_char_t *pUa, const j_char_t *pPwd);
	static void ReleaseInstance(J_DbAccess **pInstance); 

protected:
	CDbManager();
	~CDbManager();
};
#endif //~__DB_MANAGER_H_