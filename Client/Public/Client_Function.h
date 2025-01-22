#pragma once
/* 전역 함수들을 모아놓는 헤더. */
#include "Engine_Defines.h"
BEGIN(Engine)
class CGameObject;
class CLevel;
class CBase;
END

BEGIN(Client)
class CFSM;
class CFSM_Boss;
END

namespace Client
{
	void Event_CreateObject(_uint _iCurLevelIndex, const _tchar* _strLayerTag, Engine::CGameObject* _pGameObject);
	void Event_DeleteObject(CGameObject* _pGameObject);
	void Event_LevelChange(_int _iChangeLevelIndex, _int _iNextChangeLevelIndex = -1);
	void Event_SetActive(CBase* _pObject, _bool _isActive, _bool _isDelay = false);
	void Event_Setup_SimulationFiltering(_uint _MyGroup, _uint _OtherGroupMask);
	void Event_ChangeMonsterState(MONSTER_STATE _eState, CFSM* _pFSM);
	void Event_ChangeBossState(BOSS_STATE _eState, CFSM_Boss* _pFSM);

	std::wstring StringToWstring(const std::string& _strUTF8);
	std::string WstringToString(const std::wstring& wstr); 
	std::string MatrixToString(_float4x4 vMatrix);
	F_DIRECTION To_FDirection(_vector _vDir);
	
}