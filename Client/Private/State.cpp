#include "stdafx.h"
#include "State.h"
#include "Monster.h"
#include "FSM.h"

CState::CState()
{
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
	__super::Free();
}
