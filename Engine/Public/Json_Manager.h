#pragma once
#include "Base.h"

BEGIN(Engine)

class CJson_Manager : public CBase
{
private:
	CJson_Manager();
	virtual ~CJson_Manager() = default;

public:
	HRESULT	Load_Json(const _tchar* _szFilePath, _Out_ json* _pOutJson);

public:
	static CJson_Manager* Create();
	virtual void Free() override;
};

END