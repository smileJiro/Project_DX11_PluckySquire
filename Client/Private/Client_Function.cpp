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
			break;
		}
	}



}


