#include "stdafx.h"
#include "FSM_Boss.h"
#include "Monster.h"

#include "BossSceneState.h"
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

HRESULT CFSM_Boss::Add_State(BOSS_STATE _eState)
{
	if (nullptr == m_pOwner)
		return E_FAIL;

	CState* pState = nullptr;
	CState::STATEDESC Desc = {};

	switch (_eState)
	{
	case Client::BOSS_STATE::SCENE:
	{
		pState = CBossSceneState::Create(&Desc);
		if (nullptr == pState)
			return E_FAIL;
		pState->Set_Owner(m_pOwner);
		pState->Set_FSM(this);
		m_BossStates.emplace(BOSS_STATE::SCENE, pState);

		//이미 있다면 씬 인덱스만 바꿔줌
		/*auto iter = m_BossStates.find(BOSS_STATE::SCENE);
		if (m_BossStates.end() != iter)
			iter->second->
		else
		{
			pState = CBossSceneState::Create(&Desc);
			if (nullptr == pState)
				return E_FAIL;
			pState->Set_Owner(m_pOwner);
			pState->Set_FSM(this);
			m_BossStates.emplace(BOSS_STATE::SCENE, pState);
		}*/
		break;
	}

	case Client::BOSS_STATE::HOMINGBALL:
		pState = CBossAttackState::Create(&Desc);

		if (nullptr == pState)
			return E_FAIL;
		pState->Set_Owner(m_pOwner);
		pState->Set_FSM(this);
		m_BossStates.emplace(BOSS_STATE::HOMINGBALL, pState);
		break;

	case Client::BOSS_STATE::LAST:
		break;
	default:
		break;
	}

	return S_OK;
}

HRESULT CFSM_Boss::Change_State(BOSS_STATE _eState)
{
	if (nullptr == m_CurState)
		return E_FAIL;
	if (nullptr == m_pOwner)
		return E_FAIL;
	if(_eState == m_eCurBossState)
		return S_OK;

	//몬스터가 애니메이션 전환 가능하지 않으면 상태 전환 안함
	if (false == m_pOwner->Get_AnimChangeable())
		return S_OK;

	m_CurState->State_Exit();
	m_pOwner->Set_PreState((_uint)m_eCurBossState);

	Set_State(_eState);

	m_pOwner->Change_Animation();

	return S_OK;
}

HRESULT CFSM_Boss::Set_State(BOSS_STATE _eState)
{
	if (nullptr == m_BossStates[_eState])
		return E_FAIL;
	if (nullptr == m_pOwner)
		return E_FAIL;

	m_CurState = m_BossStates[_eState];
	m_eCurBossState = _eState;

	m_CurState->State_Enter();
	m_pOwner->Set_State((_uint)_eState);

	return S_OK;
}

HRESULT CFSM_Boss::CleanUp()
{
	for (auto& Pair : m_BossStates)
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
	for (auto& pState : m_BossStates)
		Safe_Release(pState.second);

	m_BossStates.clear();
	__super::Free();
}
