#include "stdafx.h"
#include "Client_Function.h"
#include "Event_Manager.h"
#include "GameObject.h"
#include "Actor.h"
#include "Actor_Dynamic.h"

/* 함수 구현부 */
namespace Client
{
	void Event_CreateObject(_uint _iCurLevelIndex, const _tchar* _strLayerTag, Engine::CGameObject* _pGameObject)
	{
		EVENT tEvent;
		tEvent.eType = EVENT_TYPE::CREATE_OBJECT;
		tEvent.Parameters.resize(3); // NumParameters

		tEvent.Parameters[0] = (DWORD_PTR)_iCurLevelIndex;
		tEvent.Parameters[1] = (DWORD_PTR)_strLayerTag;
		tEvent.Parameters[2] = (DWORD_PTR)_pGameObject;

		CEvent_Manager::GetInstance()->AddEvent(tEvent);
	}


	void Event_DeleteObject(Engine::CGameObject* _pGameObject)
	{
		EVENT tEvent;
		tEvent.eType = EVENT_TYPE::DELETE_OBJECT;
		tEvent.Parameters.resize(1); // NumParameters

		tEvent.Parameters[0] = (DWORD_PTR)_pGameObject;

		CEvent_Manager::GetInstance()->AddEvent(tEvent);
	}

	void Event_LevelChange(_int _iChangeLevelIndex, _int _iNextChangeLevelIndex)
	{
		EVENT tEvent;
		tEvent.eType = EVENT_TYPE::LEVEL_CHANGE;
		tEvent.Parameters.resize(2); // NumParameters

		tEvent.Parameters[0] = (DWORD_PTR)_iChangeLevelIndex;
		tEvent.Parameters[1] = (DWORD_PTR)_iNextChangeLevelIndex;

		CEvent_Manager::GetInstance()->AddEvent(tEvent);
	}

	void Event_SetActive(CBase* _pObject, _bool _isActive, _bool _isDelay)
	{
		EVENT tEvent;
		tEvent.eType = EVENT_TYPE::SET_ACTIVE;
		tEvent.Parameters.resize(3); // NumParameters

		tEvent.Parameters[0] = (DWORD_PTR)_pObject;
		tEvent.Parameters[1] = (DWORD_PTR)_isActive;
		tEvent.Parameters[2] = (DWORD_PTR)_isDelay;

		CEvent_Manager::GetInstance()->AddEvent(tEvent);
	}

	void Event_Setup_SimulationFilter(Engine::CActor* _pActor, _uint _MyGroup, _uint _OtherGroupMask)
	{
		EVENT tEvent;
		tEvent.eType = EVENT_TYPE::SETUP_SIMULATION_FILTER;
		tEvent.Parameters.resize(3); // NumParameters

		tEvent.Parameters[0] = (DWORD_PTR)_pActor;
		tEvent.Parameters[1] = (DWORD_PTR)_MyGroup;
		tEvent.Parameters[2] = (DWORD_PTR)_OtherGroupMask;

		CEvent_Manager::GetInstance()->AddEvent(tEvent);
	}

	void Event_ChangeMonsterState(MONSTER_STATE _eState, CFSM* _pFSM)
	{
		EVENT tEvent;
		tEvent.eType = EVENT_TYPE::CHANGE_MONSTERSTATE;
		tEvent.Parameters.resize(2); // NumParameters

		tEvent.Parameters[0] = (DWORD_PTR)_eState;
		tEvent.Parameters[1] = (DWORD_PTR)_pFSM;

		CEvent_Manager::GetInstance()->AddEvent(tEvent);
	}

	void Event_ChangeBossState(BOSS_STATE _eState, CFSM* _pFSM)
	{
		EVENT tEvent;
		tEvent.eType = EVENT_TYPE::CHANGE_BOSSSTATE;
		tEvent.Parameters.resize(2); // NumParameters

		tEvent.Parameters[0] = (DWORD_PTR)_eState;
		tEvent.Parameters[1] = (DWORD_PTR)_pFSM;

		CEvent_Manager::GetInstance()->AddEvent(tEvent);
	}

	void Event_Change_Coordinate(CActorObject* _pActorObject, COORDINATE _eCoordinate, _float3* _pNewPosition)
	{
		EVENT tEvent;
		tEvent.eType = EVENT_TYPE::CHANGE_COORDINATE;
		tEvent.Parameters.resize(3); // NumParameters

		tEvent.Parameters[0] = (DWORD_PTR)_pActorObject;
		tEvent.Parameters[1] = (DWORD_PTR)_eCoordinate;
		_float3* pPosition = nullptr;
		if (nullptr != _pNewPosition)
			pPosition = new _float3(*_pNewPosition);

		tEvent.Parameters[2] = (DWORD_PTR)pPosition;

		CEvent_Manager::GetInstance()->AddEvent(tEvent);
	}

	void Event_Set_Kinematic(CActor_Dynamic* _pActorObject, _bool _bValue)
	{
		EVENT tEvent;
		tEvent.eType = EVENT_TYPE::SET_KINEMATIC;
		tEvent.Parameters.resize(2); // NumParameters

		tEvent.Parameters[0] = (DWORD_PTR)_pActorObject;
		tEvent.Parameters[1] = (DWORD_PTR)_bValue;
		CEvent_Manager::GetInstance()->AddEvent(tEvent);

	}

	void Event_Trigger_Enter(_uint _iTriggerType, _int _iTriggerID, _wstring& _szEventTag)
	{
		EVENT tEvent;
		tEvent.eType = EVENT_TYPE::TRIGGER_ENTER_EVENT;
		tEvent.Parameters.resize(3);

		tEvent.Parameters[0] = (DWORD_PTR)_iTriggerType;
		tEvent.Parameters[1] = (DWORD_PTR)_iTriggerID;
		tEvent.Parameters[2] = (DWORD_PTR)new _wstring(_szEventTag);

		CEvent_Manager::GetInstance()->AddEvent(tEvent);
	}

	void Event_Trigger_Stay(_uint _iTriggerType, _int _iTriggerID, _wstring& _szEventTag)
	{
		EVENT tEvent;
		tEvent.eType = EVENT_TYPE::TRIGGER_STAY_EVENT;
		tEvent.Parameters.resize(3);

		tEvent.Parameters[0] = (DWORD_PTR)_iTriggerType;
		tEvent.Parameters[1] = (DWORD_PTR)_iTriggerID;
		tEvent.Parameters[2] = (DWORD_PTR)new _wstring(_szEventTag);

		CEvent_Manager::GetInstance()->AddEvent(tEvent);
	}

	void Event_Trigger_Exit(_uint _iTriggerType, _int _iTriggerID, _wstring& _szEventTag)
	{
		EVENT tEvent;
		tEvent.eType = EVENT_TYPE::TRIGGER_EXIT_EVENT;
		tEvent.Parameters.resize(3);

		tEvent.Parameters[0] = (DWORD_PTR)_iTriggerType;
		tEvent.Parameters[1] = (DWORD_PTR)_iTriggerID;
		tEvent.Parameters[2] = (DWORD_PTR)new _wstring(_szEventTag);

		CEvent_Manager::GetInstance()->AddEvent(tEvent);
	}

	void Event_Trigger_FreezeEnter(_uint _iTriggerType, _int _iTriggerID, _wstring& _szEventTag, _float3 _vFreezeExitArm)
	{
		EVENT tEvent;
		tEvent.eType = EVENT_TYPE::TRIGGER_FREEZE_ENTER_EVENT;
		tEvent.Parameters.resize(4);

		tEvent.Parameters[0] = (DWORD_PTR)_iTriggerType;
		tEvent.Parameters[1] = (DWORD_PTR)_iTriggerID;
		tEvent.Parameters[2] = (DWORD_PTR)new _wstring(_szEventTag);
		tEvent.Parameters[3] = (DWORD_PTR)new _float3(_vFreezeExitArm);

		CEvent_Manager::GetInstance()->AddEvent(tEvent);
	}

	void Event_Trigger_Exit_ByCollision(_uint _iTriggerType, _int _iTriggerID, _bool _isReturn)
	{
		EVENT tEvent;
		tEvent.eType = EVENT_TYPE::TRIGGER_EXIT_BYCOLLISION_EVENT;
		tEvent.Parameters.resize(3);

		tEvent.Parameters[0] = (DWORD_PTR)_iTriggerType;
		tEvent.Parameters[1] = (DWORD_PTR)_iTriggerID;
		tEvent.Parameters[2] = (DWORD_PTR)_isReturn;

		CEvent_Manager::GetInstance()->AddEvent(tEvent);
	}

	void Event_Book_Main_Section_Change_Start(_uint _iPageDirection, _float3* _fNextPosition)
	{
		EVENT tEvent;
		tEvent.eType = EVENT_TYPE::BOOK_MAIN_SECTION_CHANGE_ACTION_START;
		tEvent.Parameters.resize(2);

		tEvent.Parameters[0] = (DWORD_PTR)_iPageDirection;
		_float3* pPosition = nullptr;
		if (nullptr != _fNextPosition)
			pPosition = new _float3(*_fNextPosition);
		tEvent.Parameters[1] = (DWORD_PTR)pPosition;

		CEvent_Manager::GetInstance()->AddEvent(tEvent);
	}
	
	void Event_Book_Main_Section_Change_End(const _wstring& _strSectionTag)
	{
		EVENT tEvent;
		tEvent.eType = EVENT_TYPE::BOOK_MAIN_SECTION_CHANGE_ACTION_END;
		tEvent.Parameters.resize(1);

		tEvent.Parameters[0] = (DWORD_PTR)new _wstring(_strSectionTag);

		CEvent_Manager::GetInstance()->AddEvent(tEvent);
	}

	void Event_Book_Main_Change(_uint _iCameraType)
	{
		EVENT tEvent;
		tEvent.eType = EVENT_TYPE::BOOK_MAIN_CHANGE;
		tEvent.Parameters.resize(1);

		tEvent.Parameters[0] = (DWORD_PTR)_iCameraType;

		CEvent_Manager::GetInstance()->AddEvent(tEvent);
	}

	void Event_SetSceneQueryFlag(CActorObject* _pActor, _uint _iShapeID,_bool _bEnable)
	{
		EVENT tEvent;
		tEvent.eType = EVENT_TYPE::SET_SCENEQUERYFLAG;
		tEvent.Parameters.resize(3);
		tEvent.Parameters[0] = (DWORD_PTR)_pActor;
		tEvent.Parameters[1] = (DWORD_PTR)_iShapeID;
		tEvent.Parameters[2] = (DWORD_PTR)_bEnable;
		CEvent_Manager::GetInstance()->AddEvent(tEvent);
	}

	void Event_Hit(CGameObject* _pHitter, CGameObject* _pVictim, _float _fDamg)
	{
		EVENT tEvent;
		tEvent.eType = EVENT_TYPE::HIT;

		tEvent.Parameters.resize(3);
		tEvent.Parameters[0] = (DWORD_PTR)_pHitter;
		tEvent.Parameters[1] = (DWORD_PTR)_pVictim;
		tEvent.Parameters[2] = (DWORD_PTR)_fDamg;
		CEvent_Manager::GetInstance()->AddEvent(tEvent);
	}

	void Event_Get_Bulb(_uint _iCoordinate)
	{
		EVENT tEvent;
		tEvent.eType = EVENT_TYPE::GET_BULB;
		tEvent.Parameters.resize(1);
		tEvent.Parameters[0] = (DWORD_PTR)_iCoordinate;
		CEvent_Manager::GetInstance()->AddEvent(tEvent);
	}

	void Event_AddImpulse(CActorObject* _pObject, _fvector _vDirection, _float _fPower)
	{
		Event_AddImpulse(_pObject, XMVector3Normalize(_vDirection)* _fPower);
	}

	void Event_AddImpulse(CActorObject* _pObject, _fvector _vForce)
	{
		EVENT tEvent;
		tEvent.eType = EVENT_TYPE::ADDIMPULSE;

		tEvent.Parameters.resize(2);
		_float3* fForce = new _float3{ _vForce.m128_f32[0], _vForce.m128_f32[1], _vForce.m128_f32[2] };
		tEvent.Parameters[0] = (DWORD_PTR)_pObject;
		tEvent.Parameters[1] = (DWORD_PTR)fForce;
		CEvent_Manager::GetInstance()->AddEvent(tEvent);
	}


	//void Event_CameraTrigger(_uint _iCameraTriggerType, _wstring& _szEventTag, _int _iTriggerID)
	//{
	//	EVENT tEvent;
	//	tEvent.eType = EVENT_TYPE::CAMERATRIGGER_EVENT;
	//	tEvent.Parameters.resize(3);
	//	
	//	tEvent.Parameters[0] = (DWORD_PTR)_iCameraTriggerType;
	//	tEvent.Parameters[1] = (DWORD_PTR)new _wstring(_szEventTag);
	//	tEvent.Parameters[2] = (DWORD_PTR)_iTriggerID;
	//	
	//	CEvent_Manager::GetInstance()->AddEvent(tEvent);
	//}

	//void Event_CameraTriggerExit(_int _iTriggerID, _bool _isReturn)
	//{
	//	EVENT tEvent;
	//	tEvent.eType = EVENT_TYPE::CAMERATRIGGER_EXIT_EVENT;
	//	tEvent.Parameters.resize(2);

	//	tEvent.Parameters[0] = (DWORD_PTR)_iTriggerID;
	//	tEvent.Parameters[1] = (DWORD_PTR)_isReturn;

	//	CEvent_Manager::GetInstance()->AddEvent(tEvent);
	//}

	std::wstring StringToWstring(const std::string& _str)
	{
		if (_str.empty()) return L"";

		int iWstrLen = MultiByteToWideChar(CP_UTF8, 0, _str.c_str(), -1, nullptr, 0);
		if (iWstrLen <= 0) return L"";

		std::wstring wstr(iWstrLen, 0);
		MultiByteToWideChar(CP_UTF8, 0, _str.c_str(), -1, &wstr[0], iWstrLen);

		// Null-terminated string에서 null 제거
		wstr.resize(iWstrLen - 1);
		return wstr;

		/* Ver.14 */
		// std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		//return converter.from_bytes(_strUTF8);
	}

	std::string WstringToString(const std::wstring& _strWide)
	{
		if (_strWide.empty()) return "";

		int iUTF8Len = WideCharToMultiByte(CP_UTF8, 0, _strWide.c_str(), -1, nullptr, 0, nullptr, nullptr);
		if (iUTF8Len <= 0) return "";

		std::string strUTF8(iUTF8Len, 0);
		WideCharToMultiByte(CP_UTF8, 0, _strWide.c_str(), -1, &strUTF8[0], iUTF8Len, nullptr, nullptr);

		// Null-terminated string에서 null 제거
		strUTF8.resize(iUTF8Len - 1);
		return strUTF8;

		/* Ver.14 */
		//std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		//return converter.to_bytes(_strWide);
	}

	std::string MatrixToString(_float4x4 vMatrix)
	{
		string strMatrix = to_string( vMatrix.m[0][0] ) +" " + to_string(vMatrix.m[0][1] )+ " " + to_string(vMatrix.m[0][2]) + " " + to_string(vMatrix.m[0][3]) + "\n";
		strMatrix += to_string(vMatrix.m[1][0]) + " " + to_string(vMatrix.m[1][1]) + " " + to_string(vMatrix.m[1][2]) + " " + to_string(vMatrix.m[1][3]) + "\n";
		strMatrix += to_string(vMatrix.m[2][0]) + " " + to_string(vMatrix.m[2][1]) + " " + to_string(vMatrix.m[2][2]) + " " + to_string(vMatrix.m[2][3]) + "\n";
		strMatrix += to_string(vMatrix.m[3][0]) + " " + to_string(vMatrix.m[3][1]) + " " + to_string(vMatrix.m[3][2]) + " " + to_string(vMatrix.m[3][3]) + "\n";
		return strMatrix;
	}

	F_DIRECTION To_FDirection(_vector _vDir)
	{
		//가로축이 더 클 때
		if (abs(_vDir.m128_f32[0] )> abs(_vDir.m128_f32[1]))
			return _vDir.m128_f32[0] < 0 ? F_DIRECTION::LEFT: F_DIRECTION::RIGHT;
		//세로축이 더 클 때
		else
			return _vDir.m128_f32[1] > 0 ? F_DIRECTION::UP: F_DIRECTION::DOWN;

		return F_DIRECTION::F_DIR_LAST;
	}

	_vector FDir_To_Vector(F_DIRECTION _eFDir)
	{
		switch (_eFDir)
		{
		case Client::F_DIRECTION::LEFT:
			return _vector{ -1, 0, 0 };
		case Client::F_DIRECTION::RIGHT:
			return _vector{ 1, 0, 0 };
		case Client::F_DIRECTION::UP:
			return _vector{ 0, 1, 0 };
		case Client::F_DIRECTION::DOWN:
			return _vector{ 0, -1, 0 };
		case Client::F_DIRECTION::F_DIR_LAST:
		default:
			return _vector{ 0, 0, 0 };
		}

	}

	E_DIRECTION To_EDirection(_vector _vDir)
	{
		_float fSlope = _vDir.m128_f32[1] / _vDir.m128_f32[0];
		//2 , 1/2, -1/2, -2
		if(2 >= fSlope &&  1/2 < fSlope)
		{
			if (_vDir.m128_f32[1] > 0)
				return E_DIRECTION::RIGHT_UP;
			else
				return E_DIRECTION::LEFT_DOWN;
		}
		else if ((1 / 2 >= fSlope && 0 <= fSlope)
			|| (-1 / 2 < fSlope && 0 >= fSlope))
		{
			if (_vDir.m128_f32[0] > 0)
				return E_DIRECTION::RIGHT;
			else
				return E_DIRECTION::LEFT;
		}
		else if (-1 / 2 >= fSlope && -2 < fSlope)
		{
			if (_vDir.m128_f32[0] > 0)
				return E_DIRECTION::RIGHT_DOWN;
			else
				return E_DIRECTION::LEFT_UP;
		}
		else if(-2 >= fSlope || 2 < fSlope)
		{
			if (_vDir.m128_f32[1] > 0)
				return E_DIRECTION::UP;
			else
				return E_DIRECTION::DOWN;
		}

		return E_DIRECTION::UP;
	}

	_vector EDir_To_Vector(E_DIRECTION _eFDir)
	{
		switch (_eFDir)
		{
		case Client::E_DIRECTION::LEFT:
			return _vector{ -1, 0, 0 };
		case Client::E_DIRECTION::RIGHT:
			return _vector{ 1, 0, 0 };
		case Client::E_DIRECTION::UP:
			return _vector{ 0, 1, 0 };
		case Client::E_DIRECTION::DOWN:
			return _vector{ 0, -1, 0 };
		case Client::E_DIRECTION::LEFT_UP:
			return XMVector3Normalize( _vector{ -1, 1, 0 });
		case Client::E_DIRECTION::RIGHT_UP:
			return XMVector3Normalize(_vector{ 1, 1, 0 });
		case Client::E_DIRECTION::RIGHT_DOWN:
			return XMVector3Normalize(_vector{ 1, -1, 0 });
		case Client::E_DIRECTION::LEFT_DOWN:
			return XMVector3Normalize(_vector{ -1, -1, 0 });
		case Client::E_DIRECTION::E_DIR_LAST:
		default:
			return _vector{ 0, 0, 0 };
		}
	}

	F_DIRECTION EDir_To_FDir(E_DIRECTION _eEDir)
	{
		switch (_eEDir)
		{
		case Client::E_DIRECTION::LEFT_UP:
		case Client::E_DIRECTION::LEFT:
			return F_DIRECTION::LEFT;
		case Client::E_DIRECTION::RIGHT_UP:
		case Client::E_DIRECTION::UP:
			return F_DIRECTION::UP;
		case Client::E_DIRECTION::RIGHT_DOWN:
		case Client::E_DIRECTION::RIGHT:
			return F_DIRECTION::RIGHT;
		case Client::E_DIRECTION::LEFT_DOWN:
		case Client::E_DIRECTION::DOWN:
			return F_DIRECTION::DOWN;
		default:
			return F_DIRECTION::F_DIR_LAST;
		}
	}

	T_DIRECTION EDir_To_TDirection(E_DIRECTION _eEDir)
	{
		switch (_eEDir)
		{
		case Client::E_DIRECTION::LEFT:
		case Client::E_DIRECTION::LEFT_UP:
		case Client::E_DIRECTION::LEFT_DOWN:
			return T_DIRECTION::LEFT;
		case Client::E_DIRECTION::RIGHT:
		case Client::E_DIRECTION::RIGHT_UP:
		case Client::E_DIRECTION::RIGHT_DOWN:
			return T_DIRECTION::RIGHT;
		case Client::E_DIRECTION::UP:
		case Client::E_DIRECTION::DOWN:
			return T_DIRECTION::NONE;
			break;
		default:
			return T_DIRECTION::T_DIR_LAST;
			break;
		}
	}



}


