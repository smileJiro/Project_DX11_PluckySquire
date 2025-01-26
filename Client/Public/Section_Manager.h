#pragma once
#include "Base.h"
BEGIN(Client)
class CSection;
class CSection_Manager final : public CBase
{
	DECLARE_SINGLETON(CSection_Manager)
private:
	CSection_Manager();
	virtual ~CSection_Manager() = default;

public:
	HRESULT Initialize();

public:
	HRESULT Level_Exit();	// Clear Current Level Section;
	HRESULT Level_Enter();	// Create Next Level Section;

private:
	_int m_iCurLevelID = -1;
	map<_wstring, CSection*> m_CurLevelSections;

public:
	void Free() override;
};
END
