#ifndef __DATAOBJ_H_
#define __DATAOBJ_H_
#include "j_includes.h"

class CDataObj
{
public:
	virtual ~CDataObj() {}

	/// ��ʽת������
	virtual j_result_t ToDevCommand() = 0;
	virtual j_result_t ToClientCommand() = 0;

protected:
	/// �ڲ���⺯��
	j_boolean_t IsCorrect() { return true; }

protected:
	/// ��׼ͷ��Ϣ
	j_int32_t m_devCommand;		///< ������,��CmdType����
	j_int32_t m_cliCommand;			///< ������,��CmdType����
	j_int32_t m_dataLength;				///< ���ݳ���,������ͷ���ݺ�β����		

};
#endif //~__DATAOBJ_H_