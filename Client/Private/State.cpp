#include "stdafx.h"
#include "GameInstance.h"
#include "State.h"
#include "Monster.h"
#include "FSM.h"

CState::CState()
	: m_pGameInstance { CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
}

void CState::Set_Owner(CMonster* _pOwner)
{
	m_pOwner = _pOwner;
	Safe_AddRef(m_pOwner);
}

void CState::Free()
{
	Safe_Release(m_pOwner);
	Safe_Release(m_pFSM);
	Safe_Release(m_pGameInstance);
	__super::Free();
}
