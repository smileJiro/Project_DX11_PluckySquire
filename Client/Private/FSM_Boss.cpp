#include "stdafx.h"
#include "FSM_Boss.h"
#include "Monster.h"

#include "BossAttackState.h"

CFSM_Boss::CFSM_Boss(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CFSM(_pDevice, _pContext)
{
}

CFSM_Boss::CFSM_Boss(const CFSM_Boss& _Prototype)
	: CFSM(_Prototype)
{
}

HRESULT CFSM_Boss::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFSM_Boss::Initialize(void* _pArg)
{
	FSMBOSSDESC* pDesc = static_cast<FSMBOSSDESC*>(_pArg);

	return S_OK;
}

void CFSM_Boss::Update(_float _fTimeDelta)
{
	m_CurState->State_Update(_fTimeDelta);

}

HRESULT CFSM_Boss::Add_State(_uint _eState)
{
	if (nullptr == m_pOwner)
		return E_FAIL;

	CState* pState = nullptr;
	CState::STATEDESC Desc = {};

	switch ((BOSS_STATE)_eState)
	{
	case Client::BOSS_STATE::HOMINGBALL:
		pState = CBossAttackState::Create(&Desc);

		if (nullptr == pState)
			return E_FAIL;
		pState->Set_Owner(m_pOwner);
		pState->Set_FSM(this);
		m_States.emplace(BOSS_STATE::HOMINGBALL, pState);
		break;
	case Client::BOSS_STATE::LAST:
		break;
	default:
		break;
	}

	return S_OK;
}

HRESULT CFSM_Boss::Change_State(_uint _eState)
{
	if (nullptr == m_CurState)
		return E_FAIL;
	if (nullptr == m_pOwner)
		return E_FAIL;

	//몬스터가 애니메이션 전환 가능하지 않으면 상태 전환 안함
	if (false == m_pOwner->Get_AnimChangeable())
		return S_OK;

	m_CurState->State_Exit();
	m_pOwner->Set_PreState((_uint)m_eCurState);

	Set_State(_eState);

	m_pOwner->Change_Animation();

	return S_OK;
}

HRESULT CFSM_Boss::Set_State(_uint _eState)
{
	if (nullptr == m_States[_eState])
		return E_FAIL;
	if (nullptr == m_pOwner)
		return E_FAIL;

	m_CurState = m_States[_eState];
	m_eCurState = _eState;

	m_CurState->State_Enter();
	m_pOwner->Set_State((_uint)_eState);

	return S_OK;
}

HRESULT CFSM_Boss::CleanUp()
{
	for (auto& Pair : m_States)
	{
		Pair.second->CleanUp();
	}

	return S_OK;
}

CFSM_Boss* CFSM_Boss::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CFSM_Boss* pInstance = new CFSM_Boss(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CFSM_Boss");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CFSM_Boss* CFSM_Boss::Clone(void* _pArg)
{
	CFSM_Boss* pInstance = new CFSM_Boss(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CFSM_Boss");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFSM_Boss::Free()
{
	__super::Free();
}
