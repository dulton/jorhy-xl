///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx科技有限公司(版权声明) 
/// All rights reserved. 
/// 
/// @file      DbManager.cpp 
/// @brief     数据库管理模块实现
///
/// @version   1.0 (版本声明)
/// @author    Jorhy (joorhy@gmail.com) 
/// @date      2013/09/18 17:07 
///
///
/// 修订说明：最初版本
///////////////////////////////////////////////////////////////////////////  
#include "DbManager.h"
#include "SqlServerAccess.h"

#pragma comment(lib,"Debug\\SqlServerAccess.lib")

CDbManager::CDbManager()
{

}

CDbManager::~CDbManager()
{

}

J_DbAccess *CDbManager::CreateInstance(DbType type, const j_char_t *pAddr, j_int16_t nPort, const j_char_t *pUa, const j_char_t *pPwd)
{	
	J_DbAccess *pInstance = NULL;
	switch(type)
	{
	case J_SQLSERVER: 
		pInstance = new CSqlServerAccess(); 
		if(pInstance->Connect(pAddr, nPort, pUa, pPwd) != J_OK)
		{
			pInstance->Release();
			delete pInstance;
			pInstance = NULL;
		}
		break;
	}
	return pInstance;
}

void CDbManager::ReleaseInstance(J_DbAccess **pInstance)
{
	if(!pInstance || !(*pInstance))
		return;

	delete (*pInstance);
	*pInstance = NULL;
}