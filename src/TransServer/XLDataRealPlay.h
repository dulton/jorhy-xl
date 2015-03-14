#ifndef __XLDATAREALPLAY_H_
#define __XLDATAREALPLAY_H_

#include "DataObj.h"

class CXLDataRealPlay : public CDataObj
{
public: 
	CXLDataRealPlay();
	~CXLDataRealPlay();

public:
	/// CDataObj
	virtual j_result_t ToDevCommand();
	virtual j_result_t ToClientCommand();
};

#endif //~__XLDATAREALPLAY_H_