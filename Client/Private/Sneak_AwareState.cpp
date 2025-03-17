#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Sneak_AwareState.h"
#include "Monster.h"
#include "FSM.h"

CSneak_AwareState::CSneak_AwareState()
{
}

HRESULT CSneak_AwareState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);
	m_fAlertRange = pDesc->fAlertRange;
	m_fChaseRange = pDesc->fChaseRange;
	m_fAttackRange = pDesc->fAttackRange;

	m_fCoolTime = 1.f;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;
		
	return S_OK;
}


void CSneak_AwareState::State_Enter()
{
	m_pOwner->Set_AnimChangeable(false);
	m_pOwner->Stop_Move();
	m_isConvert = false;
	m_fAccTime = 0.f;
	m_isRenew = false;
	cout << "Aware" << endl;
}

void CSneak_AwareState::State_Update(_float _fTimeDelta)
{	
	if (nullptr == m_pOwner)
		return;
	if(false == m_isRenew)
	{
		m_fAccTime += _fTimeDelta;
		if (m_fCoolTime <= m_fAccTime)
		{
			m_fAccTime = 0.f;
		}
	}
	if (nullptr != m_pTarget)
	{
		//소리난 위치로 회전
		_vector vDir = XMVector3Normalize(XMLoadFloat3(&m_vSneakPos) - m_pOwner->Get_FinalPosition());
		m_pOwner->Rotate_To_Radians(XMVectorSetY(vDir, 0.f), m_pOwner->Get_ControllerTransform()->Get_RotationPerSec());

		//몬스터 인식 범위 안에 들어오면 인식상태로 전환
		if (true == Check_Target3D(true))
		{
			m_pOwner->Stop_Rotate();
			m_pOwner->Stop_Move();

			_vector vDir = XMVectorSetY(m_pTarget->Get_FinalPosition() - m_pOwner->Get_FinalPosition(), 0.f);
			_float fDis = XMVectorGetX(XMVector3Length((vDir)));
			//공격 범위 안일 경우 바로 공격으로 전환
			if (fDis <= Get_CurCoordRange(MONSTER_STATE::ATTACK))
				Event_ChangeMonsterState(MONSTER_STATE::ATTACK, m_pFSM);

			return;
		}
		//if (m_isConvert == true)
		//{
		//	Event_ChangeMonsterState(MONSTER_STATE::SNEAK_INVESTIGATE, m_pFSM);
		//	return;
		//}

		//플레이어가 인식되지 않았는데 소리가 나면 위치 저장 후 경계추적으로 전환 
		if (m_pOwner->IsTarget_In_Sneak_Detection())
		{
			m_pFSM->Set_Sneak_InvestigatePos(m_pTarget->Get_FinalPosition());
			Event_ChangeMonsterState(MONSTER_STATE::SNEAK_INVESTIGATE, m_pFSM);
			//m_isConvert = true;
			return;
		}

		//인식 되지 않고 소리도 안나면 복귀상태로 전환 (지금 애니메이션 재생동안은 전환 안되니까)
		else
		{
			if (false == m_pOwner->Is_FormationMode())
				Event_ChangeMonsterState(MONSTER_STATE::SNEAK_BACK, m_pFSM);
			else
				Event_ChangeMonsterState(MONSTER_STATE::FORMATION_BACK, m_pFSM);
		}
	}
}

void CSneak_AwareState::State_Exit()
{
}

void CSneak_AwareState::Set_Sneak_AwarePos(_fvector _vPosition)
{
	XMStoreFloat3(&m_vSneakPos, _vPosition);
}

CSneak_AwareState* CSneak_AwareState::Create(void* _pArg)
{
	CSneak_AwareState* pInstance = new CSneak_AwareState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CSneak_AwareState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSneak_AwareState::Free()
{
	__super::Free();
}
