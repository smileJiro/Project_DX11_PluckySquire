#include "stdafx.h"
#include "FSM.h"
#include "Monster.h"

#include "IdleState.h"
#include "PatrolState.h"
#include "AlertState.h"
#include "StandbyState.h"
#include "StandbyNoneAttackState.h"
#include "ChaseWalkState.h"
#include "ChaseAttackState.h"
#include "ChaseFlyState.h"
#include "MeleeAttackState.h"
#include "RangedAttackState.h"
#include "HitState.h"
#include "DeadState.h"
#include "Sneak_IdleState.h"
#include "Sneak_PatrolState.h"
#include "Sneak_BackState.h"
#include "Sneak_AlertState.h"
#include "Sneak_AwareState.h"
#include "Sneak_InvestigateState.h"
#include "Sneak_ChaseState.h"
#include "Sneak_AttackState.h"
#include "SideScroll_IdleState.h"
#include "SideScroll_PatrolState.h"
#include "SideScroll_AttackState.h"
#include "SideScroll_HitState.h"
#include "Neutral_IdleState.h"
#include "Neutral_PatrolState.h"
#include "Neutral_Patrol_JumpState.h"

CFSM::CFSM(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComponent(_pDevice, _pContext)
{
}

CFSM::CFSM(const CFSM& _Prototype)
	: CComponent(_Prototype)
{
}

//void CFSM::Set_Owner(CMonster* _pOwner)
//{
//	m_pOwner = _pOwner;
//}

void CFSM::Set_PatrolBound()
{
	if (nullptr == m_pOwner)
		return;
	//전환 시에도 불러야함
	_float3 vPosition;
	XMStoreFloat3(&vPosition, m_pOwner->Get_FinalPosition());
	static_cast<CPatrolState*>(m_States[(_uint)MONSTER_STATE::PATROL])->Set_Bound(vPosition);
}

void CFSM::Set_Sneak_StopTime(_float _fStopTime)
{
	static_cast<CSneak_IdleState*>(m_States[(_uint)MONSTER_STATE::SNEAK_IDLE])->Set_Sneak_StopTime(_fStopTime);
}

void CFSM::Set_Sneak_InvestigatePos(_fvector _vPosition)
{
	static_cast<CSneak_InvestigateState*>(m_States[(_uint)MONSTER_STATE::SNEAK_INVESTIGATE])->Set_Sneak_InvestigatePos(_vPosition);
}

void CFSM::Set_Sneak_AwarePos(_fvector _vPosition)
{
	static_cast<CSneak_AwareState*>(m_States[(_uint)MONSTER_STATE::SNEAK_AWARE])->Set_Sneak_AwarePos(_vPosition);
}

void CFSM::Set_SideScroll_PatrolBound()
{
	static_cast<CSideScroll_PatrolState*>(m_States[(_uint)MONSTER_STATE::SIDESCROLL_PATROL])->Initialize_SideScroll_PatrolBound(m_eSideScroll_Bound);
}

HRESULT CFSM::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFSM::Initialize(void* _pArg)
{
	FSMDESC* pDesc = static_cast<FSMDESC*>(_pArg);
	m_isMelee = pDesc->isMelee;
	m_fAlertRange = pDesc->fAlertRange;
	m_fChaseRange = pDesc->fChaseRange;
	m_fAttackRange = pDesc->fAttackRange;
	m_fAlert2DRange = pDesc->fAlert2DRange;
	m_fChase2DRange = pDesc->fChase2DRange;
	m_fAttack2DRange = pDesc->fAttack2DRange;
	m_fDelayTime = pDesc->fDelayTime;
	m_fCoolTime = pDesc->fCoolTime;
	m_pOwner = pDesc->pOwner;
	m_iCurLevel = pDesc->iCurLevel;
	m_eWayIndex = pDesc->eWayIndex;
	m_eSideScroll_Bound = pDesc->eSideScroll_Bound;

	return S_OK;
}

void CFSM::Update(_float _fTimeDelta)
{
	if (nullptr != m_pCurState)
	{
		m_pCurState->State_Update(_fTimeDelta);
	}
}

HRESULT CFSM::Add_State(_uint _iState)
{
	if (nullptr == m_pOwner)
		return E_FAIL;

	CState* pState = nullptr;
	CState::STATEDESC Desc;
	Desc.fAlertRange = m_fAlertRange;
	Desc.fChaseRange = m_fChaseRange;
	Desc.fAttackRange = m_fAttackRange;
	Desc.fAlert2DRange = m_fAlert2DRange;
	Desc.fChase2DRange = m_fChase2DRange;
	Desc.fAttack2DRange = m_fAttack2DRange;
	Desc.iCurLevel = m_iCurLevel;

	switch ((MONSTER_STATE)_iState)
	{
	case Client::MONSTER_STATE::IDLE:
		pState = CIdleState::Create(&Desc);
		if (nullptr == pState)
			return E_FAIL;
		pState->Set_Owner(m_pOwner);
		pState->Set_FSM(this);
		m_States.emplace((_uint)MONSTER_STATE::IDLE, pState);
		break;

	case Client::MONSTER_STATE::PATROL:
		pState = CPatrolState::Create(&Desc);
		if (nullptr == pState)
			return E_FAIL;
		pState->Set_Owner(m_pOwner);
		pState->Set_FSM(this);
		m_States.emplace((_uint)MONSTER_STATE::PATROL, pState);
		Set_PatrolBound();
		break;

	case Client::MONSTER_STATE::ALERT:
		pState = CAlertState::Create(&Desc);
		if (nullptr == pState)
			return E_FAIL;
		pState->Set_Owner(m_pOwner);
		pState->Set_FSM(this);
		m_States.emplace((_uint)MONSTER_STATE::ALERT, pState);
		break;

	case Client::MONSTER_STATE::STANDBY:
		pState = CStandbyState::Create(&Desc);
		if (nullptr == pState)
			return E_FAIL;
		pState->Set_Owner(m_pOwner);
		pState->Set_FSM(this);
		m_States.emplace((_uint)MONSTER_STATE::STANDBY, pState);
		break;

	case Client::MONSTER_STATE::CHASE:
		pState = CChaseWalkState::Create(&Desc);
		if (nullptr == pState)
			return E_FAIL;
		pState->Set_Owner(m_pOwner);
		pState->Set_FSM(this);
		m_States.emplace((_uint)MONSTER_STATE::CHASE, pState);
		break;

	case Client::MONSTER_STATE::ATTACK:
		if(true == m_isMelee)
			pState = CMeleeAttackState::Create(&Desc);
		else
			pState = CRangedAttackState::Create(&Desc);

		if (nullptr == pState)
			return E_FAIL;
		pState->Set_Owner(m_pOwner);
		pState->Set_FSM(this);
		m_States.emplace((_uint)MONSTER_STATE::ATTACK, pState);
		break;

	case Client::MONSTER_STATE::HIT:
		pState = CHitState::Create(&Desc);
		if (nullptr == pState)
			return E_FAIL;
		pState->Set_Owner(m_pOwner);
		pState->Set_FSM(this);
		m_States.emplace((_uint)MONSTER_STATE::HIT, pState);
		break;

	case Client::MONSTER_STATE::DEAD:
		pState = CDeadState::Create(&Desc);
		if (nullptr == pState)
			return E_FAIL;
		pState->Set_Owner(m_pOwner);
		pState->Set_FSM(this);
		m_States.emplace((_uint)MONSTER_STATE::DEAD, pState);
		break;

	case Client::MONSTER_STATE::SIDESCROLL_IDLE:
		pState = CSideScroll_IdleState::Create(&Desc);
		if (nullptr == pState)
			return E_FAIL;
		pState->Set_Owner(m_pOwner);
		pState->Set_FSM(this);
		m_States.emplace((_uint)MONSTER_STATE::SIDESCROLL_IDLE, pState);
		break;

	case Client::MONSTER_STATE::SIDESCROLL_PATROL:
		pState = CSideScroll_PatrolState::Create(&Desc);
		if (nullptr == pState)
			return E_FAIL;
		pState->Set_Owner(m_pOwner);
		pState->Set_FSM(this);
		m_States.emplace((_uint)MONSTER_STATE::SIDESCROLL_PATROL, pState);
		Set_SideScroll_PatrolBound();
		break;

	case Client::MONSTER_STATE::SIDESCROLL_ATTACK:
		pState = CSideScroll_AttackState::Create(&Desc);
		if (nullptr == pState)
			return E_FAIL;
		pState->Set_Owner(m_pOwner);
		pState->Set_FSM(this);
		m_States.emplace((_uint)MONSTER_STATE::SIDESCROLL_ATTACK, pState);
		break;

	case Client::MONSTER_STATE::SIDESCROLL_HIT:
		pState = CSideScroll_HitState::Create(&Desc);
		if (nullptr == pState)
			return E_FAIL;
		pState->Set_Owner(m_pOwner);
		pState->Set_FSM(this);
		m_States.emplace((_uint)MONSTER_STATE::SIDESCROLL_HIT, pState);
		break;

	case Client::MONSTER_STATE::LAST:
		break;
	default:
		break;
	}

	return S_OK;
}

HRESULT CFSM::Change_State(_uint _iState)
{
	if (nullptr == m_pCurState)
		return E_FAIL;
	if (nullptr == m_pOwner)
		return E_FAIL;

	//이미 같은 상태면 전환 안함
	if (m_iCurState == _iState)
		return S_OK;

	//몬스터가 애니메이션 전환 가능하지 않으면 상태 전환 안함
	if (false == m_pOwner->Get_AnimChangeable())
		return S_OK;

	m_pCurState->State_Exit();
	m_pOwner->Set_PreState(m_iCurState);

	Set_State(_iState);

	if(true == m_pOwner->Has_StateAnim((MONSTER_STATE)_iState))
		m_pOwner->Change_Animation();

	return S_OK;
}

HRESULT CFSM::Set_State(_uint _iState)
{
	if (nullptr == m_States[_iState])
		return E_FAIL;
	if (nullptr == m_pOwner)
		return E_FAIL;

	m_pCurState = m_States[_iState];
	m_iCurState = _iState;

	m_pCurState->State_Enter();
	m_pOwner->Set_State(_iState);

	return S_OK;
}

HRESULT CFSM::Add_SneakState()
{
	if (nullptr == m_pOwner)
		return E_FAIL;

	CState_Sneak* pState = nullptr;
	CState_Sneak::SNEAKSTATEDESC Desc;
	Desc.fAlertRange = m_fAlertRange;
	Desc.fChaseRange = m_fChaseRange;
	Desc.fAttackRange = m_fAttackRange;
	Desc.fAlert2DRange = m_fAlert2DRange;
	Desc.fChase2DRange = m_fChase2DRange;
	Desc.fAttack2DRange = m_fAttack2DRange;
	Desc.iCurLevel = m_iCurLevel;
	Desc.eWayIndex = m_eWayIndex;
	Desc.pOwner = m_pOwner;
	
	pState = CSneak_IdleState::Create(&Desc);
	if (nullptr == pState)
		return E_FAIL;
	pState->Set_Owner(m_pOwner);
	pState->Set_FSM(this);
	m_States.emplace((_uint)MONSTER_STATE::SNEAK_IDLE, pState);

	pState = CSneak_PatrolState::Create(&Desc);
	if (nullptr == pState)
		return E_FAIL;
	pState->Set_Owner(m_pOwner);
	pState->Set_FSM(this);
	m_States.emplace((_uint)MONSTER_STATE::SNEAK_PATROL, pState);

	pState = CSneak_BackState::Create(&Desc);
	if (nullptr == pState)
		return E_FAIL;
	pState->Set_Owner(m_pOwner);
	pState->Set_FSM(this);
	m_States.emplace((_uint)MONSTER_STATE::SNEAK_BACK, pState);

	pState = CSneak_AwareState::Create(&Desc);
	if (nullptr == pState)
		return E_FAIL;
	pState->Set_Owner(m_pOwner);
	pState->Set_FSM(this);
	m_States.emplace((_uint)MONSTER_STATE::SNEAK_AWARE, pState);

	pState = CSneak_InvestigateState::Create(&Desc);
	if (nullptr == pState)
		return E_FAIL;
	pState->Set_Owner(m_pOwner);
	pState->Set_FSM(this);
	m_States.emplace((_uint)MONSTER_STATE::SNEAK_INVESTIGATE, pState);

	pState = CSneak_AlertState::Create(&Desc);
	if (nullptr == pState)
		return E_FAIL;
	pState->Set_Owner(m_pOwner);
	pState->Set_FSM(this);
	m_States.emplace((_uint)MONSTER_STATE::SNEAK_ALERT, pState);

	pState = CSneak_ChaseState::Create(&Desc);
	if (nullptr == pState)
		return E_FAIL;
	pState->Set_Owner(m_pOwner);
	pState->Set_FSM(this);
	m_States.emplace((_uint)MONSTER_STATE::SNEAK_CHASE, pState);

	pState = CSneak_AttackState::Create(&Desc);
	if (nullptr == pState)
		return E_FAIL;
	pState->Set_Owner(m_pOwner);
	pState->Set_FSM(this);
	m_States.emplace((_uint)MONSTER_STATE::SNEAK_ATTACK, pState);

	return S_OK;
}

HRESULT CFSM::Add_Chase_NoneAttackState()
{
	if (nullptr == m_pOwner)
		return E_FAIL;

	CState* pState = nullptr;
	CState::STATEDESC Desc;
	Desc.fAlertRange = m_fAlertRange;
	Desc.fChaseRange = m_fChaseRange;
	Desc.fAttackRange = m_fAttackRange;
	Desc.fAlert2DRange = m_fAlert2DRange;
	Desc.fChase2DRange = m_fChase2DRange;
	Desc.fAttack2DRange = m_fAttack2DRange;
	Desc.iCurLevel = m_iCurLevel;

	pState = CIdleState::Create(&Desc);
	if (nullptr == pState)
		return E_FAIL;
	pState->Set_Owner(m_pOwner);
	pState->Set_FSM(this);
	m_States.emplace((_uint)MONSTER_STATE::IDLE, pState);

	pState = CPatrolState::Create(&Desc);
	if (nullptr == pState)
		return E_FAIL;
	pState->Set_Owner(m_pOwner);
	pState->Set_FSM(this);
	m_States.emplace((_uint)MONSTER_STATE::PATROL, pState);
	Set_PatrolBound();

	pState = CAlertState::Create(&Desc);
	if (nullptr == pState)
		return E_FAIL;
	pState->Set_Owner(m_pOwner);
	pState->Set_FSM(this);
	m_States.emplace((_uint)MONSTER_STATE::ALERT, pState);

	pState = CStandbyNoneAttackState::Create(&Desc);
	if (nullptr == pState)
		return E_FAIL;
	pState->Set_Owner(m_pOwner);
	pState->Set_FSM(this);
	m_States.emplace((_uint)MONSTER_STATE::STANDBY, pState);

	pState = CChaseAttackState::Create(&Desc);
	if (nullptr == pState)
		return E_FAIL;
	pState->Set_Owner(m_pOwner);
	pState->Set_FSM(this);
	m_States.emplace((_uint)MONSTER_STATE::CHASE, pState);

	pState = CHitState::Create(&Desc);
	if (nullptr == pState)
		return E_FAIL;
	pState->Set_Owner(m_pOwner);
	pState->Set_FSM(this);
	m_States.emplace((_uint)MONSTER_STATE::HIT, pState);

	pState = CDeadState::Create(&Desc);
	if (nullptr == pState)
		return E_FAIL;
	pState->Set_Owner(m_pOwner);
	pState->Set_FSM(this);
	m_States.emplace((_uint)MONSTER_STATE::DEAD, pState);

	return S_OK;
}

HRESULT CFSM::Add_Neutral_State(_bool _isJump)
{
	if (nullptr == m_pOwner)
		return E_FAIL;

	CState* pState = nullptr;
	CState::STATEDESC Desc;
	Desc.fAlertRange = m_fAlertRange;
	Desc.fChaseRange = m_fChaseRange;
	Desc.fAttackRange = m_fAttackRange;
	Desc.fAlert2DRange = m_fAlert2DRange;
	Desc.fChase2DRange = m_fChase2DRange;
	Desc.fAttack2DRange = m_fAttack2DRange;
	Desc.iCurLevel = m_iCurLevel;

	pState = CNeutral_IdleState::Create(&Desc);
	if (nullptr == pState)
		return E_FAIL;
	pState->Set_Owner(m_pOwner);
	pState->Set_FSM(this);
	m_States.emplace((_uint)MONSTER_STATE::IDLE, pState);

	if(false == _isJump)
	{
		pState = CNeutral_PatrolState::Create(&Desc);
		if (nullptr == pState)
			return E_FAIL;
		pState->Set_Owner(m_pOwner);
		pState->Set_FSM(this);
		m_States.emplace((_uint)MONSTER_STATE::PATROL, pState);
		Set_PatrolBound();
	}
	else
	{
		pState = CNeutral_Patrol_JumpState::Create(&Desc);
		if (nullptr == pState)
			return E_FAIL;
		pState->Set_Owner(m_pOwner);
		pState->Set_FSM(this);
		m_States.emplace((_uint)MONSTER_STATE::PATROL, pState);
		Set_PatrolBound();
	}

	pState = CHitState::Create(&Desc);
	if (nullptr == pState)
		return E_FAIL;
	pState->Set_Owner(m_pOwner);
	pState->Set_FSM(this);
	m_States.emplace((_uint)MONSTER_STATE::HIT, pState);

	pState = CDeadState::Create(&Desc);
	if (nullptr == pState)
		return E_FAIL;
	pState->Set_Owner(m_pOwner);
	pState->Set_FSM(this);
	m_States.emplace((_uint)MONSTER_STATE::DEAD, pState);

	return S_OK;
}

HRESULT CFSM::Add_FlyUnit_State()
{
	if (nullptr == m_pOwner)
		return E_FAIL;

	CState* pState = nullptr;
	CState::STATEDESC Desc;
	Desc.fAlertRange = m_fAlertRange;
	Desc.fChaseRange = m_fChaseRange;
	Desc.fAttackRange = m_fAttackRange;
	Desc.fAlert2DRange = m_fAlert2DRange;
	Desc.fChase2DRange = m_fChase2DRange;
	Desc.fAttack2DRange = m_fAttack2DRange;
	Desc.iCurLevel = m_iCurLevel;

	pState = CIdleState::Create(&Desc);
	if (nullptr == pState)
		return E_FAIL;
	pState->Set_Owner(m_pOwner);
	pState->Set_FSM(this);
	m_States.emplace((_uint)MONSTER_STATE::IDLE, pState);

	pState = CPatrolState::Create(&Desc);
	if (nullptr == pState)
		return E_FAIL;
	pState->Set_Owner(m_pOwner);
	pState->Set_FSM(this);
	m_States.emplace((_uint)MONSTER_STATE::PATROL, pState);
	Set_PatrolBound();

	pState = CAlertState::Create(&Desc);
	if (nullptr == pState)
		return E_FAIL;
	pState->Set_Owner(m_pOwner);
	pState->Set_FSM(this);
	m_States.emplace((_uint)MONSTER_STATE::ALERT, pState);

	pState = CStandbyState::Create(&Desc);
	if (nullptr == pState)
		return E_FAIL;
	pState->Set_Owner(m_pOwner);
	pState->Set_FSM(this);
	m_States.emplace((_uint)MONSTER_STATE::STANDBY, pState);

	pState = CChaseFlyState::Create(&Desc);
	if (nullptr == pState)
		return E_FAIL;
	pState->Set_Owner(m_pOwner);
	pState->Set_FSM(this);
	m_States.emplace((_uint)MONSTER_STATE::CHASE, pState);

	if (true == m_isMelee)
		pState = CMeleeAttackState::Create(&Desc);
	else
		pState = CRangedAttackState::Create(&Desc);

	if (nullptr == pState)
		return E_FAIL;
	pState->Set_Owner(m_pOwner);
	pState->Set_FSM(this);
	m_States.emplace((_uint)MONSTER_STATE::ATTACK, pState);

	pState = CHitState::Create(&Desc);
	if (nullptr == pState)
		return E_FAIL;
	pState->Set_Owner(m_pOwner);
	pState->Set_FSM(this);
	m_States.emplace((_uint)MONSTER_STATE::HIT, pState);

	pState = CDeadState::Create(&Desc);
	if (nullptr == pState)
		return E_FAIL;
	pState->Set_Owner(m_pOwner);
	pState->Set_FSM(this);
	m_States.emplace((_uint)MONSTER_STATE::DEAD, pState);

	return S_OK;
}

HRESULT CFSM::CleanUp()
{
	for (auto& Pair : m_States)
	{
		if(nullptr != Pair.second)
		{
			Pair.second->CleanUp();
		}
	}

	return S_OK;
}

CFSM* CFSM::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CFSM* pInstance = new CFSM(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CFSM");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CFSM* CFSM::Clone(void* _pArg)
{
	CFSM* pInstance = new CFSM(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CFSM");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFSM::Free()
{
	for (auto& pState : m_States)
		Safe_Release(pState.second);

	m_States.clear();
	m_pOwner = nullptr;
	m_pCurState = nullptr;
	__super::Free();
}
