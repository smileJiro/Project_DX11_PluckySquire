#include "stdafx.h"
#include "GameInstance.h"
#include "State.h"
#include "Monster.h"
#include "FSM.h"

CState::CState()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CState::Initialize(void* _pArg)
{
	m_iCurLevel = static_cast<STATEDESC*>(_pArg)->iCurLevel;
	//플레이어 위치 가져오기
	m_pTarget = m_pGameInstance->Get_GameObject_Ptr(m_iCurLevel, TEXT("Layer_Player"), 0);
	if (nullptr == m_pTarget)
	{
	#ifdef _DEBUG
		cout << "STATE : NO PLAYER" << endl;
	#endif // _DEBUG
	}
	else
	{
		Safe_AddRef(m_pTarget);
	}

	return S_OK;
}

HRESULT CState::CleanUp()
{
	//몬스터 active false 일때 작업 필요

	if (nullptr == m_pTarget)
		return S_OK;

	if (true == m_pTarget->Is_Dead())
	{
		Safe_Release(m_pTarget);
		m_pTarget = nullptr;
	}


	return S_OK;
}

_float CState::Get_CurCoordRange(MONSTER_STATE _eState)
{
	_float fRange = { 0.f };

	switch (_eState)
	{
	case Client::MONSTER_STATE::ALERT:
		if (COORDINATE::COORDINATE_3D == m_pOwner->Get_CurCoord())
			fRange = m_fAlertRange;
		else if (COORDINATE::COORDINATE_2D == m_pOwner->Get_CurCoord())
			fRange = m_fAlert2DRange;
		break;
	case Client::MONSTER_STATE::CHASE:
		if (COORDINATE::COORDINATE_3D == m_pOwner->Get_CurCoord())
			fRange = m_fChaseRange;
		else if (COORDINATE::COORDINATE_2D == m_pOwner->Get_CurCoord())
			fRange = m_fChase2DRange;
		break;
	case Client::MONSTER_STATE::ATTACK:
		if (COORDINATE::COORDINATE_3D == m_pOwner->Get_CurCoord())
			fRange = m_fAttackRange;
		else if (COORDINATE::COORDINATE_2D == m_pOwner->Get_CurCoord())
			fRange = m_fAttack2DRange;
		break;
	case Client::MONSTER_STATE::LAST:
		break;
	default:
		break;
	}

	return fRange;
}

_bool CState::Check_Target3D(_bool _isSneak)
{
	if (COORDINATE_2D == m_pOwner->Get_CurCoord())
		return false;

	MONSTER_STATE eState = MONSTER_STATE::ALERT;
	if (true == _isSneak)
		eState = MONSTER_STATE::SNEAK_ALERT;

	if (m_pOwner->IsTarget_In_Detection())
	{
		//------테스트
		_float3 vPos; XMStoreFloat3(&vPos, XMVectorSetY(m_pOwner->Get_FinalPosition(), m_pOwner->Get_FinalPosition().m128_f32[1] + m_pOwner->Get_RayOffset().y));
		_vector vTargetDir = m_pTarget->Get_FinalPosition() - XMLoadFloat3(&vPos);
		_float3 vDir; XMStoreFloat3(&vDir, XMVector3Normalize(XMVectorSetY(vTargetDir, 0.f)));
		_float3 vOutPos;
		CActorObject* pActor = nullptr;

		if (m_pGameInstance->RayCast_Nearest(vPos, vDir, Get_CurCoordRange(MONSTER_STATE::ALERT), &vOutPos, &pActor))
		{
			if (OBJECT_GROUP::MAPOBJECT & static_cast<ACTOR_USERDATA*>(pActor->Get_ActorCom()->Get_RigidActor()->userData)->iObjectGroup)
			{
				//플레이어가 레이 오브젝트보다 가까우면 인식(임시로 맵오브젝트 써봄)
				if (2 == m_pGameInstance->Compare_VectorLength(vTargetDir, XMLoadFloat3(&vOutPos) - m_pOwner->Get_FinalPosition()))
				{
					Event_ChangeMonsterState(eState, m_pFSM);
					return true;
				}
			}
		}
		//레이 충돌 안했을 때(장애물이 없었을 때)
		else
		{
			Event_ChangeMonsterState(eState, m_pFSM);
			return true;
		}
		//---------
	}

	return false;
}

void CState::Set_Sneak_InvestigatePos(_fvector _vPosition)
{
	XMStoreFloat3(&m_vSneakPos, _vPosition);
}


void CState::Set_Owner(CMonster* _pOwner)
{
	m_pOwner = _pOwner;
}

void CState::Free()
{
	Safe_Release(m_pGameInstance);

	if (nullptr != m_pTarget)
		Safe_Release(m_pTarget);
	m_pOwner = nullptr;
	__super::Free();
}
