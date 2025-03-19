#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Sneak_AttackState.h"
#include "Monster.h"
#include "FSM.h"

#include "Camera_Manager.h"

CSneak_AttackState::CSneak_AttackState()
{
}

HRESULT CSneak_AttackState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);
	m_fChaseRange = pDesc->fChaseRange;
	m_fChase2DRange = pDesc->fChase2DRange;
	m_fAttackRange = pDesc->fAttackRange;
	m_fAttack2DRange = pDesc->fAttack2DRange;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;
		
	return S_OK;
}


void CSneak_AttackState::State_Enter()
{
	Beetle_CutScene();
	m_pOwner->Set_AnimChangeable(false);
}

void CSneak_AttackState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;
	if (nullptr == m_pOwner)
		return;
	//cout << "Attack" << endl;
	//컷씬으로 들어가며 초기화
	//Event_ChangeMonsterState(MONSTER_STATE::STANDBY, m_pFSM);
	if(m_pOwner->Get_AnimChangeable())
	{
		if(false == m_pOwner->Is_FormationMode())
			Event_ChangeMonsterState(MONSTER_STATE::SNEAK_IDLE, m_pFSM);
		else
		{
			m_pOwner->Add_To_Formation();
			Event_ChangeMonsterState(MONSTER_STATE::FORMATION_BACK, m_pFSM);
		}
	}
}

void CSneak_AttackState::State_Exit()
{
	After_Attack();
}

void CSneak_AttackState::After_Attack()
{
	_float3 vPlayerPos = { };
	_float3 vMonsterPos = { };

	switch (m_eWayIndex)
	{
	case Client::SNEAKWAYPOINTINDEX::CHAPTER2_1:
		vPlayerPos = { -31.f, 6.56f, 22.5f };
		vMonsterPos = { -16.5f, 6.56f, 22.6f };
		break;
	case Client::SNEAKWAYPOINTINDEX::CHAPTER2_2:
		vPlayerPos = { 40.f, 0.35f, -7.f };
		vMonsterPos = { 32.15f, 0.35f, 1.66f };
		break;
	case Client::SNEAKWAYPOINTINDEX::CHAPTER2_2_2:
		vPlayerPos = { 36.6f, 0.35f, 5.5f };
		vMonsterPos = { 39.5f, 0.35f, 10.5f };
		break;
	case Client::SNEAKWAYPOINTINDEX::CHAPTER2_3:
		vPlayerPos = { 43.f, 0.35f, -0.5f };
		vMonsterPos = { 47.f, 0.35f, -0.5f };
		break;

	default:
		return;
	}
	Event_Sneak_BeetleCaught(static_cast<CActorObject*>(m_pTarget), static_cast<CActorObject*>(m_pOwner), &vPlayerPos, &vMonsterPos);
	Event_ChangeMonsterState(MONSTER_STATE::SNEAK_IDLE, m_pFSM);
}

void CSneak_AttackState::Beetle_CutScene()
{
//	CCamera_Manager::GetInstance()->Start_Changing_ArmLength_Decrease(CCamera_Manager::TARGET, 0.f, 5.f, EASE_IN_OUT);

}

CSneak_AttackState* CSneak_AttackState::Create(void* _pArg)
{
	CSneak_AttackState* pInstance = new CSneak_AttackState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CSneak_AttackState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSneak_AttackState::Free()
{
	__super::Free();
}
