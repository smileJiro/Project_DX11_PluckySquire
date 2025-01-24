#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "BossState.h"
#include "Monster.h"
#include "FSM.h"

CBossState::CBossState()
{
}

HRESULT CBossState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;
		
	return S_OK;
}

void CBossState::Free()
{
	__super::Free();
}
