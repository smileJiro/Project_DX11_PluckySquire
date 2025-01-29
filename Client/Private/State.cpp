#include "stdafx.h"
#include "GameInstance.h"
#include "State.h"
#include "Monster.h"
#include "FSM.h"

CState::CState()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CState::Initialize(void* _pArg)
{
	//플레이어 위치 가져오기
	m_pTarget = m_pGameInstance->Get_GameObject_Ptr(LEVEL_GAMEPLAY, TEXT("Layer_Player"), 0);
	if (nullptr == m_pTarget)
	{
	#ifdef _DEBUG
		cout << "STATE : NO PLAYER" << endl;
	#endif // _DEBUG
	}
	else
	{
		Safe_AddRef(m_pTarget);
	}

	return S_OK;
}

HRESULT CState::CleanUp()
{
	//몬스터 active false 일때 작업 필요

	if (nullptr == m_pTarget)
		return S_OK;

	if (true == m_pTarget->Is_Dead())
	{
		Safe_Release(m_pTarget);
		m_pTarget = nullptr;
	}


	return S_OK;
}

_float CState::Get_CurCoordRange(MONSTER_STATE _eState)
{
	_float fRange = { 0.f };

	switch (_eState)
	{
	case Client::MONSTER_STATE::ALERT:
		if (COORDINATE::COORDINATE_3D == m_pOwner->Get_CurCoord())
			fRange = m_fAlertRange;
		else if (COORDINATE::COORDINATE_2D == m_pOwner->Get_CurCoord())
			fRange = m_fAlert2DRange;
		break;
	case Client::MONSTER_STATE::CHASE:
		if (COORDINATE::COORDINATE_3D == m_pOwner->Get_CurCoord())
			fRange = m_fChaseRange;
		else if (COORDINATE::COORDINATE_2D == m_pOwner->Get_CurCoord())
			fRange = m_fChase2DRange;
		break;
	case Client::MONSTER_STATE::ATTACK:
		if (COORDINATE::COORDINATE_3D == m_pOwner->Get_CurCoord())
			fRange = m_fAttackRange;
		else if (COORDINATE::COORDINATE_2D == m_pOwner->Get_CurCoord())
			fRange = m_fAttack2DRange;
		break;
	case Client::MONSTER_STATE::LAST:
		break;
	default:
		break;
	}

	return fRange;
}


void CState::Set_Owner(CMonster* _pOwner)
{
	m_pOwner = _pOwner;
}

void CState::Free()
{
	Safe_Release(m_pGameInstance);

	if (nullptr != m_pTarget)
		Safe_Release(m_pTarget);
	m_pOwner = nullptr;
	__super::Free();
}
