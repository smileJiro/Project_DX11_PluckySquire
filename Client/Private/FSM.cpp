#include "stdafx.h"
#include "FSM.h"
#include "Monster.h"

#include "IdleState.h"
#include "PatrolState.h"
#include "AlertState.h"
#include "ChaseWalkState.h"
#include "MeleeAttackState.h"
#include "RangedAttackState.h"

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
	m_pOwner = pDesc->pOwner;

	return S_OK;
}

void CFSM::Update(_float _fTimeDelta)
{
	m_pCurState->State_Update(_fTimeDelta);

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

	//몬스터가 애니메이션 전환 가능하지 않으면 상태 전환 안함
	if (false == m_pOwner->Get_AnimChangeable())
		return S_OK;

	m_pCurState->State_Exit();
	m_pOwner->Set_PreState(m_iCurState);

	Set_State(_iState);

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

HRESULT CFSM::CleanUp()
{
	for (auto& Pair : m_States)
	{
		Pair.second->CleanUp();
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
