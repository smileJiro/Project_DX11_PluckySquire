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
