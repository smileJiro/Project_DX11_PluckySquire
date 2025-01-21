#pragma once
#include "Engine_Defines.h"
BEGIN(Engine)
class CGameObject;
class CLevel;
class CBase;
END


namespace AnimTool
{
	void Event_CreateObject(_uint _iCurLevelIndex, const _tchar* _strLayerTag, Engine::CGameObject* _pGameObject);
	void Event_DeleteObject(CGameObject* _pGameObject);
	void Event_LevelChange(_int _iChangeLevelIndex, _int _iNextChangeLevelIndex = -1);
	void Event_SetActive(CBase* _pObject, _bool _isActive, _bool _isDelay = false);


	std::wstring StringToWstring(const std::string& _strUTF8);
	std::string WstringToString(const std::wstring& wstr);
	std::string MatrixToString(_float4x4 vMatrix);
	std::wstring OpenFileDialog(LPCWSTR szFilter);
	std::wstring OpenDirectoryDialog();
}