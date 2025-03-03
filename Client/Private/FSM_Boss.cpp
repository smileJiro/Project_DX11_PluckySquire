#include "stdafx.h"
#include "FSM_Boss.h"
#include "Monster.h"

#include "BossSceneState.h"
#include "BossIdleState.h"
#include "BossHomingBallState.h"
#include "BossEnergyBallState.h"
#include "BossYellowBallState.h"
#include "BossWingSlamState.h"
#include "BossRockVolleyState.h"

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
	m_pOwner = pDesc->pOwner;
	m_iCurLevel = pDesc->iCurLevel;

	m_iAttackIdx = (_uint)(BOSS_STATE::ATTACK);
	return S_OK;
}

void CFSM_Boss::Update(_float _fTimeDelta)
{
	m_pCurState->State_Update(_fTimeDelta);

}

HRESULT CFSM_Boss::Add_State(_uint _iState)
{
	if (nullptr == m_pOwner)
		return E_FAIL;

	CState* pState = nullptr;
	CState::STATEDESC Desc = {};
	Desc.iCurLevel = m_iCurLevel;

	switch ((BOSS_STATE)_iState)
	{
	case Client::BOSS_STATE::SCENE:
	{
		pState = CBossSceneState::Create(&Desc);
		if (nullptr == pState)
			return E_FAIL;
		pState->Set_Owner(m_pOwner);
		pState->Set_FSM(this);
		m_States.emplace((_uint)BOSS_STATE::SCENE, pState);

		//이미 있다면 씬 인덱스만 바꿔줌
		/*auto iter = m_States.find(BOSS_STATE::SCENE);
		if (m_States.end() != iter)
			iter->second->
		else
		{
			pState = CBossSceneState::Create(&Desc);
			if (nullptr == pState)
				return E_FAIL;
			pState->Set_Owner(m_pOwner);
			pState->Set_FSM(this);
			m_States.emplace(BOSS_STATE::SCENE, pState);
		}*/
		break;
	}
	
	case Client::BOSS_STATE::IDLE:
		pState = CBossIdleState::Create(&Desc);

		if (nullptr == pState)
			return E_FAIL;
		pState->Set_Owner(m_pOwner);
		pState->Set_FSM(this);
		m_States.emplace((_uint)BOSS_STATE::IDLE, pState);
		break;

	case Client::BOSS_STATE::HOMINGBALL:
		pState = CBossHomingBallState::Create(&Desc);

		if (nullptr == pState)
			return E_FAIL;
		pState->Set_Owner(m_pOwner);
		pState->Set_FSM(this);
		m_States.emplace((_uint)BOSS_STATE::HOMINGBALL, pState);
		break;

	case Client::BOSS_STATE::ENERGYBALL:
		pState = CBossEnergyBallState::Create(&Desc);

		if (nullptr == pState)
			return E_FAIL;
		pState->Set_Owner(m_pOwner);
		pState->Set_FSM(this);
		m_States.emplace((_uint)BOSS_STATE::ENERGYBALL, pState);
		break;

	case Client::BOSS_STATE::YELLOWBALL:
		pState = CBossYellowBallState::Create(&Desc);

		if (nullptr == pState)
			return E_FAIL;
		pState->Set_Owner(m_pOwner);
		pState->Set_FSM(this);
		m_States.emplace((_uint)BOSS_STATE::YELLOWBALL, pState);
		break;

	case Client::BOSS_STATE::WINGSLAM:
		pState = CBossWingSlamState::Create(&Desc);

		if (nullptr == pState)
			return E_FAIL;
		pState->Set_Owner(m_pOwner);
		pState->Set_FSM(this);
		m_States.emplace((_uint)BOSS_STATE::WINGSLAM, pState);
		break;

	case Client::BOSS_STATE::ROCKVOLLEY:
		pState = CBossRockVolleyState::Create(&Desc);

		if (nullptr == pState)
			return E_FAIL;
		pState->Set_Owner(m_pOwner);
		pState->Set_FSM(this);
		m_States.emplace((_uint)BOSS_STATE::ROCKVOLLEY, pState);
		break;

	case Client::BOSS_STATE::LAST:
		break;
	default:
		break;
	}

	return S_OK;
}

HRESULT CFSM_Boss::Change_State(_uint _iState)
{
	if (nullptr == m_pCurState)
		return E_FAIL;
	if (nullptr == m_pOwner)
		return E_FAIL;
	if(_iState == m_iCurState)
		return S_OK;

	//몬스터가 애니메이션 전환 가능하지 않으면 상태 전환 안함
	if (false == m_pOwner->Get_AnimChangeable())
		return S_OK;

	if (BOSS_STATE::ATTACK == (BOSS_STATE)_iState)
	{
		_iState = Determine_NextAttack();
		if (0 == _iState)
			return S_OK;
	}

	m_pCurState->State_Exit();
	m_pOwner->Set_PreState(m_iCurState);

	Set_State(_iState);

	m_pOwner->Change_Animation();

	return S_OK;
}

HRESULT CFSM_Boss::Set_State(_uint _iState)
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

//현재 순차적으로 패턴 도는 중
_uint CFSM_Boss::Determine_NextAttack()
{
	//ATTACK 이후에 여러 패턴 enum으로 놨음.
	if ((_uint)BOSS_STATE::ATTACK > m_iAttackIdx)
		return 0;
	//조건 체크 후 인덱스 증가하기 때문에 ATTACK으로 설정
	if ((_uint)BOSS_STATE::LAST <= m_iAttackIdx)
		m_iAttackIdx = (_uint)BOSS_STATE::ATTACK;

	return ++m_iAttackIdx;
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
	for (auto& pState : m_States)
		Safe_Release(pState.second);

	m_States.clear();
	m_pOwner = nullptr;
	m_pCurState = nullptr;
	__super::Free();
}
