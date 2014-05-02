#ifndef __DATAOBJ_H_
#define __DATAOBJ_H_
#include "j_includes.h"

class CDataObj
{
public:
	virtual ~CDataObj() {}

	/// 格式转换函数
	virtual j_result_t ToDevCommand() = 0;
	virtual j_result_t ToClientCommand() = 0;

protected:
	/// 内部检测函数
	j_boolean_t IsCorrect() { return true; }

protected:
	/// 标准头信息
	j_int32_t m_devCommand;		///< 控制码,见CmdType定义
	j_int32_t m_cliCommand;			///< 控制码,见CmdType定义
	j_int32_t m_dataLength;				///< 数据长度,不包括头数据和尾数据		

};
#endif //~__DATAOBJ_H_