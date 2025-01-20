#pragma once
/* 전역 함수들을 모아놓는 헤더. */
#include "Engine_Defines.h"

BEGIN(Engine)
class CGameObject;
class CLevel;
class CBase;
END

namespace Map_Tool
{


	void Event_CreateObject(_uint _iCurLevelIndex, const _tchar* _strLayerTag, Engine::CGameObject* _pGameObject);
	void Event_DeleteObject(CGameObject* _pGameObject);
	void Event_LevelChange(_uint _iChangeLevelIndex, _uint _iNextChangeLevelIndex = 0);
	void Event_SetActive(CBase* _pObject, _bool _isActive, _bool _isDelay = false);

	std::wstring StringToWstring(const std::string& _strUTF8);
	std::string WstringToString(const std::wstring& _wstr); 

	pair<_wstring, _wstring> Get_FileName_From_Path(const _wstring& filePath);

	_bool Path_String_Validation_Check(const _string _strText);
	_bool Path_String_Validation_Check(const _wstring _strText);


}