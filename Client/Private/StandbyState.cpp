#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "StandbyState.h"
#include "Monster.h"
#include "FSM.h"

CStandbyState::CStandbyState()
{
}

HRESULT CStandbyState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);
	m_fChaseRange = pDesc->fChaseRange;
	m_fChase2DRange = pDesc->fChase2DRange;
	m_fAttackRange = pDesc->fAttackRange;
	m_fAttack2DRange = pDesc->fAttack2DRange;
	m_fDelayTime = pDesc->fDelayTime;
	m_fCoolTime = pDesc->fCoolTime;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;
		
	return S_OK;
}


void CStandbyState::State_Enter()
{
	m_fAccTime = 0.f;
	m_isDelay = false;
	m_isCool = false;
}

void CStandbyState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;
	if (nullptr == m_pOwner)
		return;

	if (m_pTarget->Get_CurCoord() != m_pOwner->Get_CurCoord())
	{
		Event_ChangeMonsterState(MONSTER_STATE::IDLE, m_pFSM);
		return;
	}
	else if (COORDINATE_2D == m_pOwner->Get_CurCoord() && m_pOwner->Get_Include_Section_Name() != m_pTarget->Get_Include_Section_Name())
	{
		Event_ChangeMonsterState(MONSTER_STATE::IDLE, m_pFSM);
		return;
	}
	
	_float fDis = m_pOwner->Get_ControllerTransform()->Compute_Distance(m_pTarget->Get_FinalPosition());
	if (fDis <= Get_CurCoordRange(MONSTER_STATE::ATTACK))
	{
		if (false == m_pOwner->IsDelay() && false == m_pOwner->IsCool())
		{
			Event_ChangeMonsterState(MONSTER_STATE::ATTACK, m_pFSM);
			return;
		}
		else if (true == m_pOwner->IsDelay() || true == m_pOwner->IsCool())
		{
			//공격 못하면 가만히 있으면서 타겟 따라 회전 + 애니메이션
			if (COORDINATE::COORDINATE_3D == m_pOwner->Get_CurCoord())
			{
				_bool isTurn = true;
				_bool isCW = true;
				_vector vDir = XMVector3Normalize(m_pTarget->Get_FinalPosition() - m_pOwner->Get_FinalPosition());
				//m_pOwner->Get_ControllerTransform()->Set_AutoRotationYDirection(vDir);
				//m_pOwner->Get_ControllerTransform()->Update_AutoRotation(_fTimeDelta);

				//m_pOwner->Rotate_To(XMVectorSetY(vDir, 0.f), XMConvertToDegrees(m_pOwner->Get_ControllerTransform()->Get_RotationPerSec()));
				
				//다 돌았으면 회전 애니메이션 재생 안하도록
				if (true == m_pOwner->Rotate_To_Radians(XMVectorSetY(vDir, 0.f), m_pOwner->Get_ControllerTransform()->Get_RotationPerSec()))
				{
					m_pOwner->Stop_Rotate();
					isTurn = false;
				}

				//회전 애니메이션 넣으면 드드득 거림
				/*if (true == isTurn)
				{
					_float fResult = XMVectorGetY(XMVector3Cross(m_pOwner->Get_ControllerTransform()->Get_State(CTransform::STATE_LOOK), vDir));
					if (fResult < 0)
						isCW = false;

					m_pOwner->Turn_Animation(isCW);
				}
				else
					m_pOwner->Change_Animation();*/
			}
			else if (COORDINATE::COORDINATE_2D == m_pOwner->Get_CurCoord())
			{
				m_pOwner->Change_Dir();
			}
		}
	}
	else if (fDis <= Get_CurCoordRange(MONSTER_STATE::CHASE))
	{
		Event_ChangeMonsterState(MONSTER_STATE::CHASE, m_pFSM);
	}

	//완전히 나가면 idle 전환
	else if (fDis > Get_CurCoordRange(MONSTER_STATE::CHASE))
	{
		if(true == m_pOwner->Is_CombatMode())
		{
			Event_ChangeMonsterState(MONSTER_STATE::BACK, m_pFSM);
		}
		else
		{
			Event_ChangeMonsterState(MONSTER_STATE::IDLE, m_pFSM);
		}
	}
}

void CStandbyState::State_Exit()
{
}

CStandbyState* CStandbyState::Create(void* _pArg)
{
	CStandbyState* pInstance = new CStandbyState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CStandbyState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStandbyState::Free()
{
	__super::Free();
}
