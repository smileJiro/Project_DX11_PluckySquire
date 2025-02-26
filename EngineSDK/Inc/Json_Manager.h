#pragma once
#include "Base.h"

BEGIN(Engine)

class CJson_Manager : public CBase
{
private:
	CJson_Manager();
	virtual ~CJson_Manager() = default;

public:
	HRESULT Initialize(_uint _iNumLevels);
	HRESULT	Load_Json(const _tchar* _szFilePath, _Out_ json* _pOutJson);
	HRESULT Load_Json_InLevel(const _tchar* _szFilePath, const _wstring& _strKey, _uint _iLevelIndex);

	const json* Find_Json_InLevel(const _wstring& _strKey, _uint _iLevelIndex);

	void	Level_Exit(_uint _iLevelIndex);



private:
	_uint m_iNumLevels = 0;
	map<const _wstring, json*>* m_pJsons = { nullptr };
	typedef map<const _wstring, json*> JSONS;


public:
	static CJson_Manager* Create(_uint _iNumLevels);
	virtual void Free() override;
};

END