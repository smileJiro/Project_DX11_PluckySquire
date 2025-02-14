#pragma once
/* 전역 함수들을 모아놓는 헤더. */
#include "Engine_Defines.h"
BEGIN(Engine)
class CGameObject;
class CLevel;
class CBase;
class CActor_Dynamic;
class CActorObject;
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
	void Event_ChangeBossState(BOSS_STATE _eState, CFSM* _pFSM);
	void Event_Change_Coordinate(CActorObject* _pActorObject, COORDINATE _eCoordinate, _float3* _pNewPosition = nullptr);
	void Event_Set_Kinematic(CActor_Dynamic* _pActorObject, _bool _bValue);
	void Event_Trigger_Enter(_uint _iTriggerType, _int _iTriggerID, _wstring& _szEventTag);
	void Event_Trigger_Stay(_uint _iTriggerType, _int _iTriggerID, _wstring& _szEventTag);
	void Event_Trigger_Exit(_uint _iTriggerType, _int _iTriggerID, _wstring& _szEventTag);
	void Event_Trigger_Exit_ByCollision(_uint _iTriggerType, _int _iTriggerID, _bool _isReturn = true);
	void Event_Book_Main_Section_Change_Start(_uint _iPageDirection, _float3* _fNextPosition);
	void Event_Book_Main_Section_Change_End(const _wstring& _strSectionTag);
	void Event_Book_Main_Change(_uint _iCameraType);
	void Event_SetSceneQueryFlag(CActorObject* _pActor, _uint _iShapeID, _bool _bEnable);
	void Event_SetSceneQueryFlag(CActorObject* _pActor, _uint _iShapeID, _bool _bEnable);
	void Event_Hit(CGameObject* _pHitter,CGameObject* _pVictim ,_float _fDamg);
	void Event_Get_Bulb(_uint _iCoordinate);
	void Event_Sneak_BeetleCaught(CActorObject* _pPlayer, CActorObject* _pMonster, _float3* _vPlayerPosx, _float3* _vMonsterPos);

	//void Event_CameraTrigger(_uint _iCameraTriggerType, _wstring& _szEventTag, _int _iTriggerID);
	//void Event_CameraTriggerExit(_int _iTriggerID, _bool _isReturn = true);

	//void Event_Book_Section_Change_Start(const _tchar* _strNextSectionKey, );







	std::wstring StringToWstring(const std::string& _strUTF8);
	std::string WstringToString(const std::wstring& wstr); 
	std::string MatrixToString(_float4x4 vMatrix);
	F_DIRECTION To_FDirection(_vector _vDir);
	_vector FDir_To_Vector(F_DIRECTION _eFDir);
	E_DIRECTION To_EDirection(_vector _vDir);
	_vector EDir_To_Vector(E_DIRECTION _eFDir);
	F_DIRECTION EDir_To_FDir(E_DIRECTION _eEDir);
	T_DIRECTION EDir_To_TDirection(E_DIRECTION _eEDir);
}