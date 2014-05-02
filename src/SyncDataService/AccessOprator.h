#ifndef __ACCESS_OPRATOR_H_
#define __ACCESS_OPRATOR_H_
#include "j_includes.h"

#import "c:\\Program Files\\Common Files\\System\\ADO\\msado15.dll" no_namespace rename("EOF", "EndOfFile")
//#import "msado15.dll" no_namespace rename("EOF", "EndOfFile")
class CAccessOprator : public J_DbAccess
{
public:
	CAccessOprator();
	~CAccessOprator();

public:
	///J_DbAccess
	virtual j_result_t Connect(const j_char_t *pAddr, j_int16_t nPort, const j_char_t *pUa, const j_char_t *pPwd);
	virtual j_result_t Init();
	virtual j_result_t Release();

public:
	j_string_t FindFilePath(const j_char_t *pEquipmentId);
	j_result_t UpdateEquipment(const j_char_t *pEquipmentId, const j_char_t *pFilePath, j_boolean_t bFlag = true);
	j_boolean_t HasDownLoaded(const j_char_t *pFileName);
	j_result_t UpdateFileInfo(const j_char_t *pEquipmentId, const j_char_t *pFileName, j_boolean_t bFlag = true);

private:  
	_ConnectionPtr m_pConn;		/// 连接对象  
	_RecordsetPtr m_pRec;				/// 记录集对象  
	_CommandPtr m_pCmd;			/// 命令对象
};

JO_DECLARE_SINGLETON(AccessOprator)

#endif //~__ACCESS_OPRATOR_H_