#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "BossSceneState.h"
#include "Monster.h"
#include "FSM.h"

CBossSceneState::CBossSceneState()
{
}

HRESULT CBossSceneState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;
		
	return S_OK;
}


void CBossSceneState::State_Enter()
{
	m_pOwner->Set_AnimChangeable(false);
}

void CBossSceneState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;
	if (nullptr == m_pOwner)
		return;

	m_pOwner->Get_ControllerTransform()->Set_AutoRotationYDirection(m_pTarget->Get_FinalPosition() - m_pOwner->Get_FinalPosition());
	//m_pOwner->Get_ControllerTransform()->LookAt_3D(m_pTarget->Get_FinalPosition());
	//m_pOwner->Get_ControllerTransform()->Update_AutoRotation(_fTimeDelta);
	//애니메이션 재생
	switch (m_iSceneIdx)
	{
	case FIRST:

		break;

	default:
		break;
	}

	//임시 코드
	Event_ChangeBossState(BOSS_STATE::IDLE, m_pFSM);
}

void CBossSceneState::State_Exit()
{
}

CBossSceneState* CBossSceneState::Create(void* _pArg)
{
	CBossSceneState* pInstance = new CBossSceneState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CBossSceneState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossSceneState::Free()
{
	__super::Free();
}
