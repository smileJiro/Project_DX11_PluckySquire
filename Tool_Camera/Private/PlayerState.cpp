#include "stdafx.h"
#include "PlayerState.h"
#include "GameInstance.h"


CPlayerState::CPlayerState(CTest_Player* _pOwner, CTest_Player::STATE _eState)
	:CBase()
	, m_eStateID(_eState)
	, m_pOwner(_pOwner)
{
	m_pGameInstance = CGameInstance::GetInstance();
}

void CPlayerState::Free()
{
	__super::Free();

}
