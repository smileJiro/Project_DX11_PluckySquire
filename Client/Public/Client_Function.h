#pragma once
/* 전역 함수들을 모아놓는 헤더. */
#include "Engine_Defines.h"
BEGIN(Engine)
class CGameObject;
class CLevel;
class CBase;
END

namespace Client
{
	void Event_CreateObject(_uint _iCurLevelIndex, const _tchar* _strLayerTag, Engine::CGameObject* _pGameObject);
	void Event_DeleteObject(CGameObject* _pGameObject);
	void Event_LevelChange(_int _iChangeLevelIndex, _int _iNextChangeLevelIndex = -1);
	void Event_SetActive(CBase* _pObject, _bool _isActive, _bool _isDelay = false);

	std::wstring StringToWstring(const std::string& _strUTF8);
	std::string WstringToString(const std::wstring& wstr); 
}