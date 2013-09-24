///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx科技有限公司(版权声明) 
/// All rights reserved. 
/// 
/// @file      XlHost.h 
/// @brief     XL_DVR设备模块
///
/// @version   1.0 (版本声明)
/// @author    Jorhy (joorhy@gmail.com) 
/// @date      2013/09/22 17:40 
///
///
/// 修订说明：最初版本
///////////////////////////////////////////////////////////////////////////  
#ifndef __XLHOST_H_
#define __XLHOST_H_
#include "j_includes.h"

/// 本类的功能:  XL设备接入类
class CXlHost : public J_Host
{
public:
	CXlHost();
	~CXlHost();

public:
	///J_Host
	virtual j_result_t Register(const j_socket_t &nSock);
	virtual j_result_t Release();
	virtual j_result_t SetTime();
	virtual j_result_t Dispather(const j_char_t *pData, j_int32_t nLen);
};

#endif