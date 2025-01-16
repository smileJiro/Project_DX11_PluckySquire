#include "stdafx.h"
#include "FSM.h"
#include "Monster.h"

#include "IdleState.h"
#include "ChaseWalkState.h"

CFSM::CFSM(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComponent{ _pDevice, _pContext }
{
}

CFSM::CFSM(const CFSM& _Prototype)
	: CComponent{ _Prototype }
{
}

void CFSM::Set_Owner(CMonster* _pOwner)
{
	m_pOwner = _pOwner;
}

HRESULT CFSM::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFSM::Initialize(void* _pArg)
{
	FSMDESC* pDesc = static_cast<FSMDESC*>(_pArg);
	m_fChaseRange = pDesc->fChaseRange;
	m_fAttackRange = pDesc->fAttackRange;

	return S_OK;
}

void CFSM::Update(_float _fTimeDelta)
{
	m_CurState->State_Update(_fTimeDelta);

}

HRESULT CFSM::Add_State(MONSTER_STATE _eState)
{
	CState* pState = nullptr;
	CState::STATEDESC Desc;
	Desc.fChaseRange = m_fChaseRange;
	Desc.fAttackRange = m_fAttackRange;

	switch (_eState)
	{
	case Client::MONSTER_STATE::IDLE:
		pState = CIdleState::Create(&Desc);
		if (nullptr == pState)
			return E_FAIL;
		pState->Set_Owner(m_pOwner);
		pState->Set_FSM(this);
		m_States.emplace(MONSTER_STATE::IDLE, pState);
		break;

	case Client::MONSTER_STATE::CHASE:
		pState = CChaseWalkState::Create(&Desc);
		if (nullptr == pState)
			return E_FAIL;
		pState->Set_Owner(m_pOwner);
		pState->Set_FSM(this);
		m_States.emplace(MONSTER_STATE::CHASE, pState);
		break;

	case Client::MONSTER_STATE::ATTACK:
		break;
	case Client::MONSTER_STATE::LAST:
		break;
	default:
		break;
	}

	return S_OK;
}

HRESULT CFSM::Change_State(MONSTER_STATE _eState)
{
	if (nullptr == m_CurState)
		return E_FAIL;

	m_CurState->State_Exit();

	Set_State(_eState);

	return S_OK;
}

HRESULT CFSM::Set_State(MONSTER_STATE _eState)
{
	if (nullptr == m_States[_eState])
		return E_FAIL;

	m_CurState = m_States[_eState];
	m_eCurState = _eState;

	m_CurState->State_Enter();
	m_pOwner->Set_State(_eState);

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
	__super::Free();
}
