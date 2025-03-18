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
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead1"), true, matWorld);
			++m_iEffectCount;
		}
		break;
	case 1:
		if (0.45f <= m_fAccTime)
		{
			matWorld *= XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), 90.f);
			matWorld.r[3] += XMVectorSet(0.f, 0.f, -3.5f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead1"), true, matWorld);
			++m_iEffectCount;
		}
		break;
	case 2:
		if (0.9f <= m_fAccTime)
		{
			matWorld *= XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), 160.f);
			matWorld.r[3] += XMVectorSet(1.5f, 3.f, -1.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead1"), true, matWorld);
			++m_iEffectCount;
		}
	case 3:
	{
		if (1.35f <= m_fAccTime)
		{
			matWorld *= XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), 225.f);
			matWorld.r[3] += XMVectorSet(0.f, 2.f, -4.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead1"), true, matWorld);
			++m_iEffectCount;
		}
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
