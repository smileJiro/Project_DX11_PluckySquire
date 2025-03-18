#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "BossDeadState.h"
#include "Monster.h"
#include "FSM.h"

#include "Effect_Manager.h"

CBossDeadState::CBossDeadState()
{
}

HRESULT CBossDeadState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}


void CBossDeadState::State_Enter()
{
	m_pOwner->Set_AnimChangeable(false);
}

void CBossDeadState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;
	if (nullptr == m_pOwner)
		return;

	m_fAccTime += _fTimeDelta;

	_matrix matWorld = m_pOwner->Get_WorldMatrix();

	switch (m_iEffectCount)
	{
	case 0:
		if (0.f <= m_fAccTime)
		{
			matWorld.r[3] += XMVectorSet(0.f, 17.f, 0.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_4"), true, matWorld);
			matWorld.r[3] += XMVectorSet(-6.f, 0.f, 0.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_1"), true, matWorld);
			matWorld.r[3] += XMVectorSet(9.f, 0.f, 4.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_2"), true, matWorld);
			matWorld.r[3] += XMVectorSet(-3.f, 2.f, -3.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_3"), true, matWorld);
			++m_iEffectCount;
		}
		break;
	case 1 :
		if (0.55f <= m_fAccTime)
		{
			matWorld.r[3] += XMVectorSet(0.f, 11.f, 0.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_4"), true, matWorld);
			matWorld.r[3] += XMVectorSet(7.f, 5.f, 0.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_1"), true, matWorld);
			matWorld.r[3] += XMVectorSet(-5.f, -2.f, 3.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_2"), true, matWorld);
			matWorld.r[3] += XMVectorSet(-3.f, 0.f, 0.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_3"), true, matWorld);
			++m_iEffectCount;
		}
		break;
	case 2:
		if (1.1f <= m_fAccTime)
		{
			matWorld.r[3] += XMVectorSet(0.f, 13.f, 0.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_4"), true, matWorld);
			matWorld.r[3] += XMVectorSet(-8.f, 0.f, 0.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_1"), true, matWorld);
			matWorld.r[3] += XMVectorSet(13.f, -1.f, 3.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_2"), true, matWorld);
			matWorld.r[3] += XMVectorSet(0.f, 2.f, 0.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_3"), true, matWorld);
			++m_iEffectCount;
		}
		break;
	case 3:
	{
		if (1.65f <= m_fAccTime)
		{
			matWorld.r[3] += XMVectorSet(0.f, 12.f, 0.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_4"), true, matWorld);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_1"), true, matWorld);
			matWorld.r[3] += XMVectorSet(0.f, 0.f, 2.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_2"), true, matWorld);
			matWorld.r[3] += XMVectorSet(-5.f, 0.f, 1.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_3"), true, matWorld);
			++m_iEffectCount;
		}
		break;
	}
	case 4:
	{
		if (2.2f <= m_fAccTime)
		{
			matWorld.r[3] += XMVectorSet(0.f, -3.f, 0.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_Red_1"), true, matWorld);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_Red_2"), true, matWorld);

			matWorld.r[3] += XMVectorSet(0.f, 4.f, 0.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_4"), true, matWorld);
			matWorld.r[3] += XMVectorSet(-3.f, -3.f, 1.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_1"), true, matWorld);
			

			++m_iEffectCount;
		}
		break;
	}
	case 5:
	{
		if (2.75f <= m_fAccTime)
		{
			matWorld.r[3] += XMVectorSet(0.f, 5.f, 0.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_Purple_1"), true, matWorld);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_Purple_2"), true, matWorld);

			matWorld.r[3] += XMVectorSet(0.f, 1.f, 2.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_4"), true, matWorld);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_2"), true, matWorld);


			++m_iEffectCount;
		}
		break;
	}
	case 6:
	{
		if (3.3f <= m_fAccTime)
		{
			matWorld.r[3] += XMVectorSet(0.f, 8.f, 0.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_4"), true, matWorld);
			matWorld.r[3] += XMVectorSet(0.f, 1.f, 0.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_3"), true, matWorld);			
			matWorld.r[3] += XMVectorSet(0.f, 3.f, -3.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_2"), true, matWorld);
			matWorld.r[3] += XMVectorSet(4.f, 0.f, 0.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_1"), true, matWorld);

			++m_iEffectCount;
		}

		break;
	}
	case 7:
		if (3.85f <= m_fAccTime)
		{
			matWorld.r[3] += XMVectorSet(0.f, 7.f, 0.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_4"), true, matWorld);
			matWorld.r[3] += XMVectorSet(0.f, 3.f, -3.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_3"), true, matWorld);
			matWorld.r[3] += XMVectorSet(3.f, 3.f, 3.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_2"), true, matWorld);
			matWorld.r[3] += XMVectorSet(2.f, 0.f, -2.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_1"), true, matWorld);
			++m_iEffectCount;
		}
		break;
	case 8:
	{
		if (4.3f <= m_fAccTime)
		{
			matWorld.r[3] += XMVectorSet(0.f, 6.f, 0.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_4"), true, matWorld);
			matWorld.r[3] += XMVectorSet(0.f, 0.f, -3.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_3"), true, matWorld);
			matWorld.r[3] += XMVectorSet(0.f, 3.f, -2.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_2"), true, matWorld);
			matWorld.r[3] += XMVectorSet(2.f, -2.f, -1.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_1"), true, matWorld);
			++m_iEffectCount;
		}
			break;
	}
	case 9:
	{
		if (4.85f <= m_fAccTime)
		{
			matWorld.r[3] += XMVectorSet(0.f, 7.f, 0.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_4"), true, matWorld);
			matWorld.r[3] += XMVectorSet(2.f, 0.f, -3.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_3"), true, matWorld);
			matWorld.r[3] += XMVectorSet(-2.f, -1.f, 5.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_2"), true, matWorld);
			matWorld.r[3] += XMVectorSet(6.f, 2.f, 1.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_1"), true, matWorld);
			++m_iEffectCount;
		}
		break;
	}
	case 10:
	{
		if (5.4f <= m_fAccTime)
		{
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossEnd"), true, matWorld);
			++m_iEffectCount;
		}
		break;
	}
	default:
		break;
	}


	//if (true == m_pOwner->Get_AnimChangeable())
	//	Event_ChangeBossState(BOSS_STATE::IDLE, m_pFSM);
}

void CBossDeadState::State_Exit()
{
}

CBossDeadState* CBossDeadState::Create(void* _pArg)
{
	CBossDeadState* pInstance = new CBossDeadState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CBossDeadState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossDeadState::Free()
{
	__super::Free();
}
