///////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, xx�Ƽ����޹�˾(��Ȩ����) 
/// All rights reserved. 
/// 
/// @file      XlHost.h 
/// @brief     XL_DVR�豸ģ��
///
/// @version   1.0 (�汾����)
/// @author    Jorhy (joorhy@gmail.com) 
/// @date      2013/09/22 17:40 
///
///
/// �޶�˵��������汾
///////////////////////////////////////////////////////////////////////////  
#ifndef __XLHOST_H_
#define __XLHOST_H_
#include "j_includes.h"

/// ����Ĺ���:  XL�豸������
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