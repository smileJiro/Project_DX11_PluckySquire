#include "stdafx.h"
#include "Neutral_PatrolState.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Monster.h"
#include "FSM.h"

CNeutral_PatrolState::CNeutral_PatrolState()
{
}

HRESULT CNeutral_PatrolState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);
	m_fAlertRange = pDesc->fAlertRange;
	m_fAlert2DRange = pDesc->fAlert2DRange;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_fPatrolOffset = 5.f;
	m_fPatrol2DOffset = 150.f;
	m_iPrevDir = -1;
	m_iDir = -1;
	m_eDir = F_DIRECTION::F_DIR_LAST;
	m_fDelayTime = 1.f;
		
	return S_OK;
}

void CNeutral_PatrolState::Set_Bound(_float3& _vPosition)
{
	//일단 현재 위치 기준으로 잡음
	_vector vResult=XMLoadFloat3(&_vPosition);
	_vector vOffset = XMVectorZero();
	if (COORDINATE::COORDINATE_3D == m_pOwner->Get_CurCoord())
		vOffset = XMVectorReplicate(m_fPatrolOffset);
	else if (COORDINATE::COORDINATE_2D == m_pOwner->Get_CurCoord())
		vOffset = XMVectorReplicate(m_fPatrol2DOffset);

	XMStoreFloat3(&m_tPatrolBound.vMin, vResult - vOffset);
	XMStoreFloat3(&m_tPatrolBound.vMax, vResult + vOffset);

	m_isBound = true;
}


void CNeutral_PatrolState::State_Enter()
{
	m_fAccTime = 0.f;
}

void CNeutral_PatrolState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pOwner)
		return;

	if (true == m_isMove)
		m_fAccTime += _fTimeDelta;

	/*순찰 이동 로직*/
	//랜덤으로 방향 설정
	
	if (false == m_isTurn && false == m_isMove)
	{
		Determine_Direction();
	}
	
	if(true == m_isBound)
	{
		//다음 위치가 구역을 벗어나는지 체크 후 벗어나면 정지 후 반대방향으로 진행
		Check_Bound(_fTimeDelta);
	}

	//이동
	PatrolMove(_fTimeDelta, m_iDir);
}

void CNeutral_PatrolState::State_Exit()
{
	m_fAccTime = 0.f;
	m_fAccDistance = 0.f;
	m_fMoveDistance = 0.f;
	m_isTurn = false;
	m_isMove = false;
}

void CNeutral_PatrolState::PatrolMove(_float _fTimeDelta, _int _iDir)
{
	//회전중인 거도 해야할듯 일단 이동만 해봄
	if (COORDINATE_3D == m_pOwner->Get_CurCoord())
	{
		if (0 > _iDir || 7 < _iDir)
			return;

		//기본적으로 y값 상태 변화는 없다고 가정
		_vector vDir = XMVector3Normalize(Set_PatrolDirection(_iDir));

		if (true == m_isTurn && false == m_isMove)
		{
			if (m_pOwner->Rotate_To_Radians(vDir, m_pOwner->Get_ControllerTransform()->Get_RotationPerSec()))
			{
				m_isMove = true;
				
				m_pOwner->Change_Animation();
			}
			else
			{
				_bool isCW = true;
				_float fResult = XMVectorGetY(XMVector3Cross(m_pOwner->Get_ControllerTransform()->Get_State(CTransform::STATE_LOOK), vDir));
				if (fResult < 0)
					isCW = false;

				m_pOwner->Turn_Animation(isCW);
			}
		}

		if (true == m_isMove)
		{
			//m_pOwner->Get_ControllerTransform()->LookAt_3D(vDir + m_pOwner->Get_FinalPosition());
			//m_pOwner->Get_ControllerTransform()->Go_Direction(vDir, _fTimeDelta);
			//m_pOwner->Add_Force(vDir * m_pOwner->Get_ControllerTransform()->Get_SpeedPerSec()); //임시 속도
			m_pOwner->Get_ActorCom()->Set_LinearVelocity(vDir, m_pOwner->Get_ControllerTransform()->Get_SpeedPerSec()); //임시 속도
		}
	}

	else if (COORDINATE_2D == m_pOwner->Get_CurCoord())
	{
		if (0 > _iDir || 3 < _iDir)
			return;

		if (true == m_isTurn)
		{
			m_pOwner->Set_2D_Direction(m_eDir);

			m_isTurn = false;
			m_isMove = true;
		}

		if (true == m_isMove)
		{
			m_pOwner->Get_ControllerTransform()->Go_Direction(Set_PatrolDirection(_iDir), _fTimeDelta);
			
			switch (m_eDir)
			{
			case Client::F_DIRECTION::LEFT:
				m_pOwner->Get_ControllerTransform()->Go_Right(_fTimeDelta);
				break;

			case Client::F_DIRECTION::RIGHT:
				m_pOwner->Get_ControllerTransform()->Go_Right(_fTimeDelta);
				break;

			case Client::F_DIRECTION::UP:
				m_pOwner->Get_ControllerTransform()->Go_Up(_fTimeDelta);
				break;

			case Client::F_DIRECTION::DOWN:
				m_pOwner->Get_ControllerTransform()->Go_Down(_fTimeDelta);
				break;

			case Client::F_DIRECTION::F_DIR_LAST:
				return;
				break;

			default:
				break;
			}
		}
	}

	if (true == m_isMove)
	{
		m_fAccDistance += m_pOwner->Get_ControllerTransform()->Get_SpeedPerSec() * _fTimeDelta;
		if (m_fMoveDistance <= m_fAccDistance)
		{
			m_fAccDistance = 0.f;
			m_isTurn = false;
			m_isMove = false;

			Event_ChangeMonsterState(MONSTER_STATE::IDLE, m_pFSM);
		}
	}
}

void CNeutral_PatrolState::Determine_Direction()
{
	if (COORDINATE::COORDINATE_LAST == m_pOwner->Get_CurCoord())
		return;

	while (true)
	{
		if (COORDINATE::COORDINATE_3D == m_pOwner->Get_CurCoord())
		{
			//8 방향 중 랜덤 방향 지정
			m_iDir = static_cast<_int>(floor(m_pGameInstance->Compute_Random(0.f, 8.f)));
		}

		else if (COORDINATE::COORDINATE_2D == m_pOwner->Get_CurCoord())
		{
			//4 방향 중 랜덤 방향 지정
			m_iDir = static_cast<_int>(floor(m_pGameInstance->Compute_Random(0.f, 4.f)));
		}

		if (m_iDir != m_iPrevDir || 0 > m_iPrevDir)	//직전에 갔던 방향은 가지 않음
		{
			m_iPrevDir = m_iDir;

			m_fMoveDistance = m_pGameInstance->Compute_Random(0.5f * m_pOwner->Get_ControllerTransform()->Get_SpeedPerSec(), 1.f * m_pOwner->Get_ControllerTransform()->Get_SpeedPerSec());
			
			m_isTurn = true;
			break;
		}
	}
}

_vector CNeutral_PatrolState::Set_PatrolDirection(_int _iDir)
{
	_vector vDir = {};
	if (COORDINATE_3D == m_pOwner->Get_CurCoord())
	{
		switch (_iDir)
		{
		case 0:
			vDir = XMVectorSet(0.f, 0.f, 1.f, 0.f);
			//m_pOwner->Get_ControllerTransform()->LookAt_3D(m_pOwner->Get_FinalPosition() + vDir);
			//cout << "상" << endl;
			break;
		case 1:
			vDir = XMVectorSet(1.f, 0.f, 1.f, 0.f);
			//m_pOwner->Get_ControllerTransform()->LookAt_3D(m_pOwner->Get_FinalPosition() + vDir);
			//cout << "우상" << endl;
			break;
		case 2:
			vDir = XMVectorSet(1.f, 0.f, 0.f, 0.f);
			//m_pOwner->Get_ControllerTransform()->LookAt_3D(m_pOwner->Get_FinalPosition() + vDir);
			//cout << "우" << endl;
			break;
		case 3:
			vDir = XMVectorSet(1.f, 0.f, -1.f, 0.f);
			//m_pOwner->Get_ControllerTransform()->LookAt_3D(m_pOwner->Get_FinalPosition() + vDir);
			//cout << "우하" << endl;
			break;
		case 4:
			vDir = XMVectorSet(0.f, 0.f, -1.f, 0.f);
			//m_pOwner->Get_ControllerTransform()->LookAt_3D(m_pOwner->Get_FinalPosition() + vDir);
			//cout << "하" << endl;
			break;
		case 5:
			vDir = XMVectorSet(-1.f, 0.f, -1.f, 0.f);
			//m_pOwner->Get_ControllerTransform()->LookAt_3D(m_pOwner->Get_FinalPosition() + vDir);
			//cout << "좌하" << endl;
			break;
		case 6:
			vDir = XMVectorSet(-1.f, 0.f, 0.f, 0.f);
			//m_pOwner->Get_ControllerTransform()->LookAt_3D(m_pOwner->Get_FinalPosition() + vDir);
			//cout << "좌" << endl;
			break;
		case 7:
			vDir = XMVectorSet(-1.f, 0.f, 1.f, 0.f);
			//m_pOwner->Get_ControllerTransform()->LookAt_3D(m_pOwner->Get_FinalPosition() + vDir);
			//cout << "좌상" << endl;
			break;
		default:
			break;
		}
	}
	//방향전환 시켜야함
	else if (COORDINATE_2D == m_pOwner->Get_CurCoord())
	{
		switch (_iDir)
		{
		case 0:
			vDir = XMVectorSet(-1.f, 0.f, 0.f, 0.f);
			m_eDir = F_DIRECTION::LEFT;
			//cout << "좌" << endl;
			break;
		case 1:
			vDir = XMVectorSet(1.f, 0.f, 0.f, 0.f);
			m_eDir = F_DIRECTION::RIGHT;
			//cout << "우" << endl;
			break;
		case 2:
			vDir = XMVectorSet(0.f, 1.f, 0.f, 0.f);
			m_eDir = F_DIRECTION::UP;
			//cout << "상" << endl;
			break;
		case 3:
			vDir = XMVectorSet(0.f, -1.f, 0.f, 0.f);
			m_eDir = F_DIRECTION::DOWN;
			//cout << "하" << endl;
			break;
		default:
			break;
		}
	}

	return vDir;
}

void CNeutral_PatrolState::Check_Bound(_float _fTimeDelta)
{
	_float3 vPos;
	_bool isOut = false;
	//델타타임으로 다음 위치 예상해서 막기
	XMStoreFloat3(&vPos, m_pOwner->Get_FinalPosition() + Set_PatrolDirection(m_iDir) * m_pOwner->Get_ControllerTransform()->Get_SpeedPerSec() * _fTimeDelta);
	//나갔을 때 반대방향으로
	//XMStoreFloat3(&vPos, m_pOwner->Get_FinalPosition());
	if (COORDINATE_3D == m_pOwner->Get_CurCoord())
	{
		if (m_tPatrolBound.vMin.x > vPos.x || m_tPatrolBound.vMax.x < vPos.x || m_tPatrolBound.vMin.z > vPos.z || m_tPatrolBound.vMax.z < vPos.z)
		{
			isOut = true;
		}
	}
	else if (COORDINATE_2D == m_pOwner->Get_CurCoord())
	{
		//일단 2D는 처리안해놓음
		/*if (m_tPatrolBound.vMin.x > vPos.x || m_tPatrolBound.vMax.x < vPos.x || m_tPatrolBound.vMin.y > vPos.y || m_tPatrolBound.vMax.y < vPos.y)
		{
			isOut = true;
		}*/
	}

	if (true == isOut)
	{
		m_isBack = true;
		Event_ChangeMonsterState(MONSTER_STATE::IDLE, m_pFSM);
	}
	//반대방향으로 진행해도 경계를 벗어나는 경우가 있나?
	if (true == m_isBack)
	{
		if (COORDINATE_3D == m_pOwner->Get_CurCoord())
		{
			if (4 > m_iDir)
				m_iDir += 4;
			else
				m_iDir -= 4;
		}

		else if (COORDINATE_2D == m_pOwner->Get_CurCoord())
		{
			if (2 > m_iDir)
				m_iDir += 2;
			else
				m_iDir -= 2;
		}

		m_isBack = false;
	}
}

CNeutral_PatrolState* CNeutral_PatrolState::Create(void* _pArg)
{
	CNeutral_PatrolState* pInstance = new CNeutral_PatrolState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CNeutral_PatrolState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNeutral_PatrolState::Free()
{
	__super::Free();
}
