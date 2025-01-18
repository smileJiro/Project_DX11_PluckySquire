#include "stdafx.h"
#include "PlayerState.h"
#include "GameInstance.h"


CPlayerState::CPlayerState(CPlayer* _pOwner, CStateMachine* _pContext)
	:CBase()
	, m_pOwner(_pOwner)
	, m_pStateMachine(_pContext)
{
	m_pGameInstance = CGameInstance::GetInstance();
}

void Client::CPlayerState::Free()
{
	__super::Free();

}
