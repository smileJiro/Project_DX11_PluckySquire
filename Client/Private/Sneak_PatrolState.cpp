#include "stdafx.h"
#include "Sneak_PatrolState.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Monster.h"
#include "FSM.h"

CSneak_PatrolState::CSneak_PatrolState()
{
}

HRESULT CSneak_PatrolState::Initialize(void* _pArg)
{
	SNEAKSTATEDESC* pDesc = static_cast<SNEAKSTATEDESC*>(_pArg);
	m_fAlertRange = pDesc->fAlertRange;
	m_fAttackRange = pDesc->fAttackRange;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_fSneak_PatrolOffset = 7.f;
	m_fSneak_Patrol2DOffset = m_fSneak_PatrolOffset * 100.f;
	m_iPrevDir = -1;
	m_iDir = -1;
	//m_fDelayTime = 1.f;

	Initialize_WayPoints(pDesc->eWayIndex);
	Initialize_PatrolPoints(pDesc->eWayIndex);
	Initialize_PatrolDirections(pDesc->eWayIndex);
	
	if (1 < m_PatrolWays.size())
	{
		if (1 < m_PatrolDirections.size())
		{
			m_isMoveRotate = true;
		}
		else
		{
			m_isMoveOnly = true;
		}
	}
	else
		m_isRotateOnly = true;


	return S_OK;
}

void CSneak_PatrolState::Set_Bound(_float3& _vPosition)
{
	//일단 현재 위치 기준으로 잡음
	_vector vResult=XMLoadFloat3(&_vPosition);
	_vector vOffset = XMVectorZero();
	if (COORDINATE::COORDINATE_3D == m_pOwner->Get_CurCoord())
		vOffset = XMVectorReplicate(m_fSneak_PatrolOffset);
	else if (COORDINATE::COORDINATE_2D == m_pOwner->Get_CurCoord())
		vOffset = XMVectorReplicate(m_fSneak_Patrol2DOffset);

	XMStoreFloat3(&m_tSneak_PatrolBound.vMin, vResult - vOffset);
	XMStoreFloat3(&m_tSneak_PatrolBound.vMax, vResult + vOffset);
}


void CSneak_PatrolState::State_Enter()
{
	m_fAccTime = 0.f;
	m_isToWay = false;
	m_isTurn = false;
	m_isMove = false;
	cout << "Patrol" << endl;
}

void CSneak_PatrolState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pOwner)
		return;
	//cout << "Patrol" << endl;
	if (true == m_isMove)
		m_fAccTime += _fTimeDelta;

	if (nullptr != m_pTarget)
	{
		if (m_pOwner->Get_CurCoord() == COORDINATE_LAST)
			return;
		
		if (m_pTarget->Get_CurCoord() == m_pOwner->Get_CurCoord())
		{	
			//적 발견 시 ALERT 전환
			if (Check_Target3D(true))
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

			//플레이어가 인식되지 않았을 경우 소리가 나면 경계로 전환 
			if (m_pOwner->IsTarget_In_Sneak_Detection())
			{
				m_pOwner->Stop_Rotate();
				m_pOwner->Stop_Move();
				m_pFSM->Set_Sneak_AwarePos(m_pTarget->Get_FinalPosition());
				Event_ChangeMonsterState(MONSTER_STATE::SNEAK_AWARE, m_pFSM);
				return;
			}
		}
	}

	//다음 웨이포인트 설정
	if (false == m_isTurn)
	{
		Determine_Direction();
		m_isTurn = true;
	}
	
	//이동
	Sneak_PatrolMove(_fTimeDelta, m_iDir);
}

void CSneak_PatrolState::State_Exit()
{
	m_fAccTime = 0.f;
	m_fAccDistance = 0.f;
	m_isTurn = false;
	m_isMove = false;
}

void CSneak_PatrolState::Sneak_PatrolMove(_float _fTimeDelta, _int _iDir)
{
	if (m_PatrolWays.size() <= m_iCurWayIndex)
		return;

	if (true == m_isMoveOnly)
	{
		_vector vDir = XMLoadFloat3(&m_WayPoints[m_PatrolWays[m_iCurWayIndex]].vPosition) - m_pOwner->Get_FinalPosition();
		if (0.1f >= XMVectorGetX(XMVector3Length(vDir)))
		{
			m_pOwner->Stop_Rotate();
			m_pOwner->Stop_Move();
			m_isTurn = false;
			m_isMove = false;

			Event_ChangeMonsterState(MONSTER_STATE::SNEAK_IDLE, m_pFSM);
		}
	}


	if (true == m_isMoveOnly)
	{
		//회전
		if (true == m_isTurn && false == m_isMove)
		{
			if (m_pOwner->Rotate_To_Radians(XMLoadFloat3(&m_vDir), m_pOwner->Get_ControllerTransform()->Get_RotationPerSec()))
			{
				m_isMove = true;
				m_pOwner->Change_Animation();
			}
			else
			{
				_bool isCW = true;
				_float fResult = XMVectorGetY(XMVector3Cross(m_pOwner->Get_ControllerTransform()->Get_State(CTransform::STATE_LOOK), XMLoadFloat3(&m_vDir)));
				if (fResult < 0)
					isCW = false;

				m_pOwner->Turn_Animation(isCW);
			}
		}

		//이동
		if (true == m_isMove)
		{
			//m_pOwner->Get_ActorCom()->Set_LinearVelocity(vDir, m_pOwner->Get_ControllerTransform()->Get_SpeedPerSec());
			//웨이포인트 도달 했는지 체크 후 도달 했으면 idle로 전환

			//Determine_AvoidDirection(XMLoadFloat3(&m_PatrolWaypoints[m_iCurWayIndex]), &m_vDir);
			//static_cast<CActor_Dynamic*>(m_pOwner->Get_ActorCom())->Set_LinearVelocity(XMLoadFloat3(&m_vDir), m_pOwner->Get_ControllerTransform()->Get_SpeedPerSec());

			//if (m_pOwner->Move_To(XMLoadFloat3(&m_WayPoints[m_PatrolWays[m_iCurWayIndex]].vPosition), 0.3f))
			if (m_pOwner->Monster_MoveTo(XMLoadFloat3(&m_WayPoints[m_PatrolWays[m_iCurWayIndex]].vPosition), 0.3f))
			{
				m_pOwner->Stop_Rotate();
				m_pOwner->Stop_Move();
				m_isTurn = false;
				m_isMove = false;

				Event_ChangeMonsterState(MONSTER_STATE::SNEAK_IDLE, m_pFSM);
			}
		}
	}

	else if (true == m_isMoveRotate)
	{
		//회전
		if (true == m_isTurn && false == m_isMove)
		{
			if (m_pOwner->Rotate_To_Radians(XMLoadFloat3(&m_vDir), m_pOwner->Get_ControllerTransform()->Get_RotationPerSec()))
			{
				m_isMove = true;
				m_pOwner->Change_Animation();
			}
			else
			{
				_bool isCW = true;
				_float fResult = XMVectorGetY(XMVector3Cross(m_pOwner->Get_ControllerTransform()->Get_State(CTransform::STATE_LOOK), XMLoadFloat3(&m_vDir)));
				if (fResult < 0)
					isCW = false;

				m_pOwner->Turn_Animation(isCW);
			}
		}

		//이동
		if (true == m_isMove)
		{
			//도착 시 방향으로 회전
			if (m_pOwner->Monster_MoveTo(XMLoadFloat3(&m_WayPoints[m_PatrolWays[m_iCurWayIndex]].vPosition), 0.3f))
			{
				m_pOwner->Stop_Rotate();
				m_pOwner->Stop_Move();

				//현재 이동과 회전 같이 하는 경우 정찰 경로와 인덱스 같으므로 그냥 현재 경로 인덱스로 처리
				if (m_pOwner->Rotate_To_Radians(XMLoadFloat3(&m_PatrolDirections[m_iCurWayIndex]), m_pOwner->Get_ControllerTransform()->Get_RotationPerSec()))
				{
					m_pOwner->Stop_Rotate();
					m_pOwner->Stop_Move();
					m_isTurn = false;
					m_isMove = false;

					Event_ChangeMonsterState(MONSTER_STATE::SNEAK_IDLE, m_pFSM);
				}
			}
		}
	}

	
	else if (true == m_isRotateOnly)
	{
		//회전
		if (true == m_isTurn)
		{
			if (m_pOwner->Rotate_To_Radians(XMLoadFloat3(&m_vDir), m_pOwner->Get_ControllerTransform()->Get_RotationPerSec()))
			{
				m_pOwner->Change_Animation();
				Event_ChangeMonsterState(MONSTER_STATE::SNEAK_IDLE, m_pFSM);
			}
			else
			{
				_bool isCW = true;
				_float fResult = XMVectorGetY(XMVector3Cross(m_pOwner->Get_ControllerTransform()->Get_State(CTransform::STATE_LOOK), XMLoadFloat3(&m_vDir)));
				if (fResult < 0)
					isCW = false;

				m_pOwner->Turn_Animation(isCW);
			}
		}
	}
}

void CSneak_PatrolState::Determine_Direction()
{
	if (COORDINATE::COORDINATE_LAST == m_pOwner->Get_CurCoord())
		return;

	//이동 하는 경우
	if (true == m_isMoveOnly || true == m_isMoveRotate)
	{
		//다음 웨이 포인트로 넘어감.
		if (false == m_isBack)
		{
			++m_iCurWayIndex;

			if(false == m_isPatrolCycle)
			{
				if (m_PatrolWays.size() - 1 == m_iCurWayIndex)
					m_isBack = true;
			}
			else
			{
				if (m_PatrolWays.size() == m_iCurWayIndex)
					m_iCurWayIndex = 0;
			}

			//예외처리
			if (m_PatrolWays.size() - 1 < m_iCurWayIndex)
			{
				m_iCurWayIndex = (_int)m_PatrolWays.size() - 1;
				m_isBack = true;
			}
		}
		else
		{
			--m_iCurWayIndex;

			if (0 == m_iCurWayIndex)
				m_isBack = false;

			//예외처리
			if (0 > m_iCurWayIndex)
			{
				m_iCurWayIndex = 0;
				m_isBack = false;
			}
		}

		//시간 랜덤으로 지정 (양 끝 지점만 최솟값을 크게 놓음)
		if (0 == m_iCurWayIndex || m_PatrolWays.size() - 1 == m_iCurWayIndex)
			m_pFSM->Set_Sneak_StopTime(m_pGameInstance->Compute_Random(2.5f, 3.f));
		else
		{
			m_pFSM->Set_Sneak_StopTime(m_pGameInstance->Compute_Random(0.f, 3.f));
		}
		//XMStoreFloat3(&m_vDir, XMVector3Normalize(XMVectorSetY(XMLoadFloat3(&m_WayPoints[m_PatrolWays[m_iCurWayIndex]].vPosition) - m_pOwner->Get_FinalPosition(),0.f)));
		XMStoreFloat3(&m_vDir, XMVector3Normalize(XMLoadFloat3(&m_WayPoints[m_PatrolWays[m_iCurWayIndex]].vPosition) - m_pOwner->Get_FinalPosition()));
		if (m_vDir.y > 0.f)
			int a = 10;
	}
	
	//이동 없이 회전만 하는 경우
	else if (true == m_isRotateOnly)
	{
		if (1 < m_PatrolDirections.size())
		{
			if (false == m_isDirBack)
			{
				++m_iCurDirectionIndex;

				if (m_PatrolWays.size() - 1 == m_iCurDirectionIndex)
					m_isDirBack = true;

				//예외처리
				if (m_PatrolWays.size() - 1 < m_iCurDirectionIndex)
				{
					m_iCurDirectionIndex = (_int)m_PatrolWays.size() - 1;
					m_isDirBack = true;
				}
			}
			else
			{
				--m_iCurDirectionIndex;

				if (0 == m_iCurDirectionIndex)
					m_isDirBack = false;

				//예외처리
				if (0 > m_iCurDirectionIndex)
				{
					m_iCurDirectionIndex = 0;
					m_isDirBack = false;
				}
			}

			if (true == m_PatrolWays.empty())
			{
				m_pFSM->Set_Sneak_StopTime(m_pGameInstance->Compute_Random(2.5f, 3.f));
			}

			m_vDir = m_PatrolDirections[m_iCurDirectionIndex];
		}

		else if (1 == m_PatrolDirections.size())
		{
			m_iCurDirectionIndex = 0;
			m_vDir = m_PatrolDirections[m_iCurDirectionIndex];
		}
	}
}

CSneak_PatrolState* CSneak_PatrolState::Create(void* _pArg)
{
	CSneak_PatrolState* pInstance = new CSneak_PatrolState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CSneak_PatrolState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSneak_PatrolState::Free()
{
	__super::Free();
}
