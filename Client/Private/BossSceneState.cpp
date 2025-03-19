#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "BossSceneState.h"
#include "Monster.h"
#include "FSM.h"
#include "ButterGrump.h"

CBossSceneState::CBossSceneState()
{
}

HRESULT CBossSceneState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_fDelayTime = 5.f;
		
	return S_OK;
}


void CBossSceneState::State_Enter()
{
}

void CBossSceneState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;
	if (nullptr == m_pOwner)
		return;

	//if(true == m_isDelay)
	//{
	//	m_fAccTime += _fTimeDelta;
	//	if (m_fDelayTime <= m_fAccTime)
	//	{
	//		if(true == m_pOwner->Get_AnimChangeable())
	//		{
	//			static_cast<CButterGrump*>(m_pOwner)->Play_Intro(SECOND);
	//			m_fAccTime = 0.f;
	//			m_isDelay = false;
	//		}
	//	}
	//}

	if (KEY_PRESSING(KEY::CTRL))
	{
		if (KEY_DOWN(KEY::NUM9))
		{
			//애니메이션 재생
			if (true == m_pOwner->Get_AnimChangeable())
			{
				static_cast<CButterGrump*>(m_pOwner)->Play_Intro(m_iSceneIdx);
				switch (m_iSceneIdx)
				{
				case FIRST:
					m_isDelay = true;
					m_iSceneIdx = SECOND;
					break;

				default:
					break;
				}
				++m_iSceneIdx;
			}
		}
	}
	if (SCENE::LAST <= m_iSceneIdx && true == m_pOwner->Get_AnimChangeable())
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
