#include "stdafx.h"
#include "Monster.h"
#include "GameInstance.h"
#include "FSM.h"
#include "DetectionField.h"
#include "Sneak_DetectionField.h"
#include "Section_Manager.h"
#include "Effect_Manager.h"
#include "Effect2D_Manager.h"
#include "Pooling_Manager.h"
#include "Section_2D_PlayMap.h"
#include "PlayerData_Manager.h"

CMonster::CMonster(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCharacter(_pDevice, _pContext)
{
}

CMonster::CMonster(const CMonster& _Prototype)
	: CCharacter(_Prototype)
{
}

HRESULT CMonster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster::Initialize(void* _pArg)
{
	MONSTER_DESC* pDesc = static_cast<MONSTER_DESC*>(_pArg);
	pDesc->iNumPartObjects = PART_END;

	m_fAlertRange = pDesc->fAlertRange;
	m_fChaseRange = pDesc->fChaseRange;
	m_fAttackRange = pDesc->fAttackRange;
	m_fAlert2DRange = pDesc->fAlert2DRange;
	m_fChase2DRange = pDesc->fChase2DRange;
	m_fAttack2DRange = pDesc->fAttack2DRange;
	m_fDelayTime = pDesc->fDelayTime;
	m_fCoolTime = pDesc->fCoolTime;
	m_fFOVX = pDesc->fFOVX;
	m_fFOVY = pDesc->fFOVY;
	m_eWayIndex = pDesc->eWayIndex;

	if (true == pDesc->isStay)
		m_isStay = true;
	if (true == pDesc->isSneakMode)
		m_isSneakMode = true;

	pDesc->_fStepHeightThreshold = 0.2f;
	pDesc->_fStepSlopeThreshold = 0.45f;
	//XMStoreFloat4x4(&m_matQueryShapeOffset, XMMatrixIdentity());

	// Add Desc
	pDesc->iObjectGroupID = OBJECT_GROUP::MONSTER;

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	if (COORDINATE_2D == Get_CurCoord())
	{
		Set_2DDirection(F_DIRECTION::DOWN);
	}


	if (true == m_isStay)
	{
		if (COORDINATE_3D == Get_CurCoord())
		{
			if (ACTOR_TYPE::DYNAMIC == Get_ActorType())
				static_cast<CActor_Dynamic*>(Get_ActorCom())->Set_Rotation(XMLoadFloat3(&pDesc->vLook));
			else if (ACTOR_TYPE::KINEMATIC == Get_ActorType())
				Get_ControllerTransform()->Set_Look(XMLoadFloat3(&pDesc->vLook));
		}
	}

	//플레이어 위치 가져오기
	m_pTarget = CPlayerData_Manager::GetInstance()->Get_NormalPlayer_Ptr();
	if (nullptr == m_pTarget)
	{
	#ifdef _DEBUG
		cout << "MONSTERINIT : NO PLAYER" << endl;
	#endif // _DEBUG
		return S_OK;
	}

	Safe_AddRef(m_pTarget);

	return S_OK;
}

void CMonster::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta); /* Part Object Priority_Update */
}

void CMonster::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta); /* Part Object Update */
}

void CMonster::Late_Update(_float _fTimeDelta)
{
#ifdef _DEBUG
	if (COORDINATE_3D == Get_CurCoord())
		m_pGameInstance->Add_RenderObject_New(RENDERGROUP::RG_3D, PRIORITY_3D::PR3D_GEOMETRY, this);
#endif

	__super::Late_Update(_fTimeDelta); /* Part Object Late_Update */
}

HRESULT CMonster::Render()
{

#ifdef _DEBUG
	if (COORDINATE_2D == Get_CurCoord())
	{
		for (_uint i = 0; i < m_p2DColliderComs.size(); ++i)
		{
			m_p2DColliderComs[i]->Render(SECTION_MGR->Get_Section_RenderTarget_Size(Get_Include_Section_Name()));
		}
	}
#endif // _DEBUG

	return S_OK;
}

void CMonster::OnContact_Enter(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
	if (OBJECT_GROUP::PLAYER & _Other.pActorUserData->iObjectGroup 
		&&(_uint)SHAPE_USE::SHAPE_BODY == _My.pShapeUserData->iShapeUse
		&& (_uint)SHAPE_USE::SHAPE_BODY == _Other.pShapeUserData->iShapeUse)
	{
		_vector vRepulse = 10.f * XMVector3Normalize(XMVectorSetY(_Other.pActorUserData->pOwner->Get_FinalPosition() - Get_FinalPosition(), 0.f));
		Event_Hit(this, static_cast<CCharacter*>(_Other.pActorUserData->pOwner), Get_Stat().iDamg, vRepulse);
		//XMVectorSetY( vRepulse , -1.f);
		//Event_KnockBack(static_cast<CCharacter*>(_Other.pActorUserData->pOwner), vRepulse);
	}
}

void CMonster::OnContact_Stay(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
}

void CMonster::OnContact_Exit(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{

}

void CMonster::OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)
{
	//if (OBJECT_GROUP::MAPOBJECT & _Other.pActorUserData->iObjectGroup)
	//{
	//	++m_iDetect_Block_Count;
	//	//처음만 처리
	//	if(1 == m_iDetect_Block_Count)
	//		m_isDetect_Block = true;
	//	cout <<"P"<< m_iDetect_Block_Count << endl;
	//}

	//지금은 트리거가 레이용 트리거만 있으므로 따로처리안함 나중에 트리거 또 달일 생기면 처리해야함
	if (OBJECT_GROUP::MAPOBJECT & _Other.pActorUserData->iObjectGroup)
	{
		Event_SetSceneQueryFlag(_Other.pActorUserData->pOwner, _Other.pShapeUserData->iShapeIndex, true);
		//cout << _Other.pActorUserData->pOwner->Get_GameObjectInstanceID() << " Enter" << endl;
		/*if (1436 == _Other.pActorUserData->pOwner->Get_GameObjectInstanceID())
			cout << (_int)_Other.pActorUserData->pOwner->Get_ActorType() <<" : " << " Enter" << endl;*/
	}
}

void CMonster::OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)
{
	if (OBJECT_GROUP::MAPOBJECT & _Other.pActorUserData->iObjectGroup)
	{
		Event_SetSceneQueryFlag(_Other.pActorUserData->pOwner, _Other.pShapeUserData->iShapeIndex, true);
	}
}

void CMonster::OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)
{
	//if (OBJECT_GROUP::MAPOBJECT & _Other.pActorUserData->iObjectGroup)
	//{
	//	--m_iDetect_Block_Count;
	//	if(0 == m_iDetect_Block_Count)
	//		m_isDetect_Block = false;
	//	cout <<"M"<< m_iDetect_Block_Count << endl;
	//}

	if (OBJECT_GROUP::MAPOBJECT & _Other.pActorUserData->iObjectGroup)
	{
		Event_SetSceneQueryFlag(_Other.pActorUserData->pOwner, _Other.pShapeUserData->iShapeIndex, false);
		//cout << (_int)_Other.pActorUserData->pOwner->Get_ActorType() << " : " << _Other.pActorUserData->pOwner->Get_GameObjectInstanceID() << " Exit" << endl;
	}
}

void CMonster::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	OBJECT_GROUP eOtherGroupID = (OBJECT_GROUP)_pOtherCollider->Get_CollisionGroupID();
	COLLIDER2D_USE eOtherColUse = (COLLIDER2D_USE)_pOtherCollider->Get_ColliderUse();
	if (OBJECT_GROUP::PLAYER & eOtherGroupID)
	{
		if (nullptr == m_pTarget)
			m_pTarget = CPlayerData_Manager::GetInstance()->Get_CurrentPlayer_Ptr();
		Event_Hit(this, static_cast<CCharacter*>(_pOtherObject), Get_Stat().iDamg, XMVector3Normalize(m_pTarget->Get_FinalPosition() - Get_FinalPosition()), 300.f);
	}

	if (OBJECT_GROUP::FRIEND & eOtherGroupID)
	{
		if(COLLIDER2D_USE::COLLIDER2D_BODY == eOtherColUse)
			Event_Hit(this, static_cast<CCharacter*>(_pOtherObject), Get_Stat().iDamg, XMVector3Normalize(_pOtherObject->Get_FinalPosition() - Get_FinalPosition()), 300.f);
	}

	if (OBJECT_GROUP::BLOCKER & eOtherGroupID)
	{
		m_isContact_Block = true;
	}
}

void CMonster::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CMonster::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	if (OBJECT_GROUP::BLOCKER & _pOtherCollider->Get_CollisionGroupID())
	{
		m_isContact_Block = false;
	}
}

void CMonster::On_Hit(CGameObject* _pHitter, _int _iDamg, _fvector _vForce)
{
	if ((_uint)MONSTER_STATE::DEAD == m_iState)
		return;

	m_tStat.iHP -= _iDamg;
#ifdef _DEBUG
	cout << m_tStat.iHP << endl;
#endif // _DEBUG

	if (0 >= m_tStat.iHP)
	{
		if (0 == m_tStat.iHP)
		{
			Set_AnimChangeable(true);
			if (0 < m_p2DColliderComs.size())
			{
				m_p2DColliderComs[0]->Set_Active(false);
			}

			Event_ChangeMonsterState(MONSTER_STATE::DEAD, m_pFSM);
		}

	}
	else
	{
		if((_uint)MONSTER_STATE::HIT != m_iState)
		{
			Set_AnimChangeable(true);
			Event_ChangeMonsterState(MONSTER_STATE::HIT, m_pFSM);
		}
		else
		{
			Set_AnimChangeable(true);
			//임시코드 (현재 상태가 HIT이면 안 불리기 때문에)
			Event_ChangeMonsterState(MONSTER_STATE::IDLE, m_pFSM);
			Event_ChangeMonsterState(MONSTER_STATE::HIT, m_pFSM);
		}
		KnockBack(_vForce);

		//Effect
		if (COORDINATE_3D == Get_CurCoord())
			//CEffect_Manager::GetInstance()->Active_Effect(TEXT("MonsterDead"), true, m_pControllerTransform->Get_WorldMatrix_Ptr());
			CEffect_Manager::GetInstance()->Active_Effect(TEXT("MonsterHit"), true, m_pControllerTransform->Get_WorldMatrix_Ptr());

		else if (COORDINATE_2D == Get_CurCoord())
		{
			_matrix matFX = Get_ControllerTransform()->Get_WorldMatrix();

			_wstring strFXTag = L"Hit_FX";
			strFXTag += to_wstring((_int)ceil(m_pGameInstance->Compute_Random(0.f, 5.f)));
			CEffect2D_Manager::GetInstance()->Play_Effect(strFXTag, Get_Include_Section_Name(), matFX);
			matFX.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f);
			switch ((_uint)ceil(m_pGameInstance->Compute_Random(0.f, 4.f)))
			{
			case 1:
				CEffect2D_Manager::GetInstance()->Play_Effect(TEXT("Hit_Words1"), Get_Include_Section_Name(), matFX);
				break;

			case 2:
				CEffect2D_Manager::GetInstance()->Play_Effect(TEXT("Hit_Words2"), Get_Include_Section_Name(), matFX);
				break;

			case 3:
				CEffect2D_Manager::GetInstance()->Play_Effect(TEXT("Hit_Words4"), Get_Include_Section_Name(), matFX);
				break;

			case 4:
				CEffect2D_Manager::GetInstance()->Play_Effect(TEXT("Hit_Words5"), Get_Include_Section_Name(), matFX);
				break;
			}
		}
	}
}

void CMonster::On_Change2DDirection(E_DIRECTION _eCurrentDir)
{
	Change_Animation();
}

void CMonster::Monster_Death()
{
	Set_AnimChangeable(true);

	if(COORDINATE_3D == Get_CurCoord())
	{
		//CEffect_Manager::GetInstance()->Active_Effect(TEXT("MonsterDead"), true, m_pControllerTransform->Get_WorldMatrix_Ptr());
		CEffect_Manager::GetInstance()->Active_EffectPosition(TEXT("MonsterDead"), true, m_pControllerTransform->Get_State(CTransform::STATE_POSITION));

		//확률로 전구 생성
			if (2 == (_int)ceil(m_pGameInstance->Compute_Random(0.f, 3.f)))
			{
				_float3 vPos; XMStoreFloat3(&vPos, Get_FinalPosition());
				CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_Bulb"), COORDINATE_3D, &vPos);
			}
	}
	else if (COORDINATE_2D == Get_CurCoord())
	{
		CEffect2D_Manager::GetInstance()->Play_Effect(TEXT("Death_Burst"), Get_Include_Section_Name(), Get_ControllerTransform()->Get_WorldMatrix());

		//확률로 전구 생성
		if (2 == (_int)ceil(m_pGameInstance->Compute_Random(0.f, 3.f)))
		{
			_float3 vPos; XMStoreFloat3(&vPos, Get_FinalPosition());
			_wstring strCurSection = Get_Include_Section_Name();
			CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_2DBulb"), COORDINATE_2D, &vPos, nullptr, nullptr, &strCurSection);
		}
	}

	Event_DeleteObject(this);
}

void CMonster::Attack()
{
}

void CMonster::Move(_fvector _vForce, _float _fTimeDelta)
{
	if (COORDINATE_3D == Get_CurCoord())
	{
		CActor_Dynamic* pDynamicActor = static_cast<CActor_Dynamic*>(m_pActorCom);
		_vector vVeclocity = _vForce;

		//vVeclocity = XMVectorSetY(vVeclocity, XMVectorGetY(pDynamicActor->Get_LinearVelocity()));
		vVeclocity = XMVectorSetY(vVeclocity, 0.f);

		if (pDynamicActor->Is_Dynamic())
		{
			if (true == Is_OnGround())
				vVeclocity = StepAssist(vVeclocity, _fTimeDelta);
			pDynamicActor->Set_LinearVelocity(vVeclocity);
		}
	}
}

_bool CMonster::Monster_MoveTo(_fvector _vPosition, _float _fTimeDelta)
{
	if (COORDINATE_3D == Get_CurCoord())
	{
		CActor_Dynamic* pDynamicActor = static_cast<CActor_Dynamic*>(m_pActorCom);

		//위치로 이동하는 속도를 세팅하고 StepAssist 수행
		if (true == Move_To_3D(_vPosition))
			return true; 
		_vector vVeclocity = pDynamicActor->Get_LinearVelocity();

		//vVeclocity = XMVectorSetY(vVeclocity, XMVectorGetY(pDynamicActor->Get_LinearVelocity()));

		if (pDynamicActor->Is_Dynamic())
		{
			if (true == Is_OnGround())
			{
				vVeclocity = StepAssist(vVeclocity, _fTimeDelta);
				pDynamicActor->Set_LinearVelocity(vVeclocity);
			}
		}
	}
	return false;
}

HRESULT CMonster::Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition)
{
	if (FAILED(__super::Change_Coordinate(_eCoordinate, _pNewPosition)))
		return E_FAIL;

	if (COORDINATE_2D == Get_CurCoord())
		Set_2DDirection(F_DIRECTION::DOWN);

	Set_AnimChangeable(true);
	m_pFSM->Change_State((_uint)MONSTER_STATE::IDLE);

	return S_OK;
}

void CMonster::Change_Dir(_bool _isOnChange)
{
	//플레이어와의 각도를 구해 방향 전환 (시야각이 있을 때 기준)
	//_vector vUp = XMVectorSet(0.f, 0.f, 1.f, 0.f);
	//_vector vDir = XMVector3Normalize(m_pTarget->Get_FinalPosition() - Get_FinalPosition());
	//_vector vLook = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	//_float fAngle = XMConvertToDegrees(acosf(XMVectorGetX(XMVector3Dot(vDir, vLook))));
	//_float fResult = XMVectorGetX(XMVector3Cross(vUp, XMVector3Cross(vLook, vDir)));
	//if (0 > fResult)
	//{
	//	fAngle = 360 - fAngle;
	//}

	if (COORDINATE_2D != Get_CurCoord())
		return;
	
	_float fResult = m_pGameInstance->Get_Angle_Between_Vectors(XMVectorSet(0.f, 0.f, -1.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f), m_pTarget->Get_FinalPosition() - Get_FinalPosition());
	
	if ((315.f <= fResult && fResult < 360.f) || (45.f > fResult && 0.f <= fResult))
		Set_2DDirection(F_DIRECTION::UP, _isOnChange);
	else if (45.f <= fResult && 135.f > fResult)
		Set_2DDirection(F_DIRECTION::RIGHT, _isOnChange);
	else if (135.f <= fResult && 225.f > fResult)
		Set_2DDirection(F_DIRECTION::DOWN, _isOnChange);
	else if (225.f <= fResult && 315.f > fResult)
		Set_2DDirection(F_DIRECTION::LEFT, _isOnChange);
}

_bool CMonster::IsTarget_In_Detection()
{
	return m_pDetectionField->IsTarget_In_Detection();
}

_bool CMonster::IsTarget_In_Sneak_Detection()
{
	if (false == m_isSneakMode)
		return false;

	return m_pSneak_DetectionField->IsTarget_In_SneakDetection();
}

_float CMonster::Restrict_2DRangeAttack_Angle(_float _fDegrees)
{
	_float fDegrees = _fDegrees;
	switch (Get_2DDirection())
	{
	case E_DIRECTION::LEFT:
		if (fDegrees > 330.f || fDegrees <= 90.f)
			fDegrees = 330.f;
		else if (fDegrees < 210.f && fDegrees > 90.f)
			fDegrees = 210.f;
		break;
	case E_DIRECTION::RIGHT:
		if (fDegrees < 30.f || fDegrees>270.f)
			fDegrees = 30.f;
		else if (fDegrees > 150.f && fDegrees <= 270.f)
			fDegrees = 150.f;
		break;
	case E_DIRECTION::UP:
		if (fDegrees < 300.f && fDegrees>180.f)
			fDegrees = 300.f;
		else if (fDegrees > 60.f && fDegrees <= 180.f)
			fDegrees = 60.f;
		break;
	case E_DIRECTION::DOWN:
		if (fDegrees > 240.f && fDegrees< 360.f)
			fDegrees = 240.f;
		else if (fDegrees < 120.f && fDegrees >= 0.f)
			fDegrees = 120.f;
		break;
	default:
		break;
	}
	return fDegrees;
}

void CMonster::Enter_Section(const _wstring _strIncludeSectionName)
{
	__super::Enter_Section(_strIncludeSectionName);

	if (Get_CurCoord() == COORDINATE_2D)
	{
		CSection* pSection = SECTION_MGR->Find_Section(_strIncludeSectionName);
		auto pPlaySection = dynamic_cast<CSection_2D_PlayMap*>(pSection);

		if (nullptr != pPlaySection)
			pPlaySection->Add_MonsterCount();
	}

}

void CMonster::Exit_Section(const _wstring _strIncludeSectionName)
{
	__super::Exit_Section(_strIncludeSectionName);
	if (Get_CurCoord() == COORDINATE_2D)
	{
		CSection* pSection = SECTION_MGR->Find_Section(_strIncludeSectionName);
		auto pPlaySection = dynamic_cast<CSection_2D_PlayMap*>(pSection);

		if (nullptr != pPlaySection)
			pPlaySection->Reduce_MonsterCount();
	}
}

_bool CMonster::Check_InAir_Next(_float _fTimeDelta)
{
	if (COORDINATE_3D != Get_CurCoord())
		return false;

	_vector vVelocity = XMVectorSetW(static_cast<CActor_Dynamic*>(Get_ActorCom())->Get_LinearVelocity(), 0.f);

	return Check_InAir_Next(vVelocity, _fTimeDelta);
}

_bool CMonster::Check_InAir_Next(_fvector _vForce, _float _fTimeDelta)
{
	if (COORDINATE_3D != Get_CurCoord())
		return false;

	//다음 예상위치에서 아래로 스윕해서 공중인지 체크
	_float3 vOrigin;
	_float3 vRayDir = { 0.f,-1.f,0.f };

	_vector vDir = XMVectorSetW(_vForce, 0.f);
	//다음 예상위치 + 몸 크기 의 위치에서 스윕해서 자연스럽게 보이도록
	XMStoreFloat3(&vOrigin, Get_FinalPosition() + vDir * _fTimeDelta + XMVector3Normalize(vDir) * m_fHalfBodySize);
	//XMStoreFloat3(&vOrigin, Get_FinalPosition() + XMVector3Normalize(vDir) * m_fHalfBodySize);

	PxGeometryHolder pxGeomHolder = m_pActorCom->Get_Shapes()[(_uint)SHAPE_USE::SHAPE_BODY]->getGeometry().any();
	PxGeometryType::Enum eGeomType = pxGeomHolder.getType();
	if (PxGeometryType::eCAPSULE == eGeomType)
	{
		PxCapsuleGeometry& pxCapsule = pxGeomHolder.capsule();
		//pxCapsule.radius *= 0.5f;
		//vOrigin.y += (m_fStepHeightThreshold + pxCapsule.halfHeight + pxCapsule.radius);
	}
	else if (PxGeometryType::eBOX == eGeomType)
	{
		PxBoxGeometry& pxBox = pxGeomHolder.box();
		//vOrigin.y += (m_fStepHeightThreshold + pxBox.halfExtents.y);
	}
	else if (PxGeometryType::eSPHERE == eGeomType)
	{
		PxSphereGeometry& pxSphere = pxGeomHolder.sphere();
		//vOrigin.y += (m_fStepHeightThreshold + pxSphere.radius);
	}

	//임계값보다 큰 길이로 스윕
	if (false == m_pGameInstance->SingleSweep_GroupFilter(&pxGeomHolder.any(), m_matQueryShapeOffset, vOrigin, vRayDir, m_fStepHeightThreshold + 0.1f, OBJECT_GROUP::MONSTER | OBJECT_GROUP::MONSTER_PROJECTILE))
	{
		return true;
	}

	return false;
}

_bool CMonster::Check_Block(_float _fTimeDelta)
{
	if (COORDINATE_3D == Get_CurCoord())
	{
		_vector vVelocity = XMVectorSetW(static_cast<CActor_Dynamic*>(Get_ActorCom())->Get_LinearVelocity(), 0.f);

		return Check_Block(vVelocity, _fTimeDelta);
	}
	else if (COORDINATE_2D == Get_CurCoord())
	{
		return m_isContact_Block;
	}
	return false;
}

_bool CMonster::Check_Block(_fvector _vForce, _float _fTimeDelta)
{
	if (COORDINATE_3D != Get_CurCoord())
		return false;

	//현재 위치에서 앞으로 레이를 쏴서 장애물이 앞에 있는지 체크
	_float3 vOrigin;
	XMStoreFloat3(&vOrigin, Get_FinalPosition());
	_float3 vRayDir;
	XMStoreFloat3(&vRayDir, XMVector3Normalize(XMVectorSetW(_vForce, 0.f)));

	vOrigin.y += 0.5f * m_fHalfBodySize;

	_float fDistance = Get_ControllerTransform()->Get_SpeedPerSec() * _fTimeDelta + m_fHalfBodySize;

	return m_pGameInstance->RayCast_Nearest_GroupFilter(vOrigin, vRayDir, fDistance, OBJECT_GROUP::MONSTER | OBJECT_GROUP::MONSTER_PROJECTILE);
}

//void CMonster::Set_2D_Direction(F_DIRECTION _eDir)
//{
//	if (_eDir == m_e2DDirection)
//		return;
//
//	m_e2DDirection = _eDir;
//	if (F_DIRECTION::LEFT == m_e2DDirection)
//	{
//		_vector vRight = m_pControllerTransform->Get_State(CTransform::STATE_RIGHT);
//		m_pControllerTransform->Set_State(CTransform::STATE_RIGHT, -XMVectorAbs(vRight));
//	}
//	else if (F_DIRECTION::RIGHT == m_e2DDirection)
//	{
//		_vector vRight = m_pControllerTransform->Get_State(CTransform::STATE_RIGHT);
//		m_pControllerTransform->Set_State(CTransform::STATE_RIGHT, XMVectorAbs(vRight));
//	}
//
//	Change_Animation();
//}

HRESULT CMonster::Cleanup_DeadReferences()
{
	if (FAILED(__super::Cleanup_DeadReferences()))
		return E_FAIL;

	if (nullptr == m_pTarget)
	{
#ifdef _DEBUG
		cout << "MONSTER : NO PLAYER" << endl;
#endif // _DEBUG
		return S_OK;
	}

	if (true == m_pTarget->Is_Dead())
	{
		Safe_Release(m_pTarget);
		m_pTarget = nullptr;
		m_pFSM->CleanUp();
	}

	return S_OK;
}

void CMonster::Active_OnEnable()
{
	// 1. PxActor 활성화 (활성화 시점에는 먼저 켜고)
	CActorObject::Active_OnEnable();

	//m_pControllerTransform->Set_WorldMatrix(XMMatrixIdentity());
	m_tStat.iHP = m_tStat.iMaxHP;

	// 2. 몬스터 할거 하고
//	m_pTarget = m_pGameInstance->Get_GameObject_Ptr(LEVEL_CHAPTER_2, TEXT("Layer_Player"), 0);
//	if (nullptr == m_pTarget)
//	{
//#ifdef _DEBUG
//		cout << "MONSTERINIT : NO PLAYER" << endl;
//#endif // _DEBUG
//		return;
//	}
//
//	Safe_AddRef(m_pTarget);


}

void CMonster::Active_OnDisable()
{
	// 1. 몬스터 할거 하고
	m_fAccTime = { 0.f };
	m_isDelay = { false };
	m_isCool = { false };
	m_iAttackCount = { 0 };

	Event_ChangeMonsterState(MONSTER_STATE::IDLE, m_pFSM);
	

	// 2. PxActor 비활성화 
	CActorObject::Active_OnDisable();
}

HRESULT CMonster::Ready_ActorDesc(void* _pArg)
{
	//MONSTER_DESC* pDesc = static_cast<MONSTER_DESC*>(_pArg);

	//pDesc->eActorType = ACTOR_TYPE::KINEMATIC;
	//CActor::ACTOR_DESC* ActorDesc = new CActor::ACTOR_DESC;

	///* Actor의 주인 오브젝트 포인터 */
	//ActorDesc->pOwner = this;

	///* Actor의 회전축을 고정하는 파라미터 */
	//ActorDesc->FreezeRotation_XYZ[0] = true;
	//ActorDesc->FreezeRotation_XYZ[1] = false;
	//ActorDesc->FreezeRotation_XYZ[2] = true;

	///* Actor의 이동축을 고정하는 파라미터 (이걸 고정하면 중력도 영향을 받지 않음. 아예 해당 축으로의 이동을 제한하는)*/
	//ActorDesc->FreezePosition_XYZ[0] = false;
	//ActorDesc->FreezePosition_XYZ[1] = false;
	//ActorDesc->FreezePosition_XYZ[2] = false;

	///* 사용하려는 Shape의 형태를 정의 */
	//SHAPE_CAPSULE_DESC* ShapeDesc = new SHAPE_CAPSULE_DESC;
	//ShapeDesc->fHalfHeight = 0.5f;
	//ShapeDesc->fRadius = 0.5f;

	///* 해당 Shape의 Flag에 대한 Data 정의 */
	//SHAPE_DATA* ShapeData = new SHAPE_DATA;
	//ShapeData->pShapeDesc = ShapeDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
	//ShapeData->eShapeType = SHAPE_TYPE::CAPSULE;     // Shape의 형태.
	//ShapeData->eMaterial = ACTOR_MATERIAL::DEFAULT; // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
	//ShapeData->isTrigger = false;                    // Trigger 알림을 받기위한 용도라면 true
	//XMStoreFloat4x4(&ShapeData->LocalOffsetMatrix, XMMatrixRotationZ(XMConvertToRadians(90.f)) * XMMatrixTranslation(0.0f, 0.5f, 0.0f)); // Shape의 LocalOffset을 행렬정보로 저장.

	///* 최종으로 결정 된 ShapeData를 PushBack */
	//ActorDesc->ShapeDatas.push_back(*ShapeData);

	///* 충돌 필터에 대한 세팅 ()*/
	//ActorDesc->tFilterData.MyGroup = OBJECT_GROUP::MONSTER;
	//ActorDesc->tFilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::PLAYER | OBJECT_GROUP::PLAYER_PROJECTILE | OBJECT_GROUP::MONSTER;

	///* Actor Component Finished */
	//pDesc->pActorDesc = ActorDesc;

	return S_OK;
}

void CMonster::Free()
{
	Safe_Release(m_pTarget);

	Safe_Release(m_pFSM);
	Safe_Release(m_pDetectionField);
	Safe_Release(m_pSneak_DetectionField);
#ifdef _DEBUG
	Safe_Release(m_pDraw);
#endif // _DEBUG

	__super::Free();
}
