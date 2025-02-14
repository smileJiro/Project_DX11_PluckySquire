#include "stdafx.h"
#include "Monster.h"
#include "GameInstance.h"
#include "FSM.h"
#include "DetectionField.h"
#include "Sneak_DetectionField.h"

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
	m_fHp = pDesc->fHP;

	if (true == pDesc->isSneakMode)
		m_isSneakMode = true;

	// Add Desc
	pDesc->iCollisionGroupID = OBJECT_GROUP::MONSTER;

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	//플레이어 위치 가져오기
	m_pTarget = m_pGameInstance->Get_GameObject_Ptr(m_iCurLevelID, TEXT("Layer_Player"), 0);
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
	CGameObject::Priority_Update_Component(_fTimeDelta); /* Component Priority_Update */
	__super::Priority_Update(_fTimeDelta); /* Part Object Priority_Update */
}

void CMonster::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta); /* Part Object Update */
	m_vLookBefore = m_pControllerTransform->Get_State(CTransform::STATE_LOOK);
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


	return S_OK;
}

void CMonster::OnContact_Enter(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
	if (OBJECT_GROUP::PLAYER & _Other.pActorUserData->iObjectGroup && (_uint)SHAPE_USE::SHAPE_BODY == _My.pShapeUserData->iShapeUse)
	{
		Event_Hit(this, _Other.pActorUserData->pOwner, Get_Stat().fDamg);
		_float3 vRepulse; XMStoreFloat3(&vRepulse, 10.f * XMVector3Normalize(XMVectorSetY(_Other.pActorUserData->pOwner->Get_FinalPosition() - Get_FinalPosition(), 0.f)));
		vRepulse.y = -1.f;
		_Other.pActorUserData->pOwner->Get_ActorCom()->Add_Impulse(vRepulse);
		KnockBack(_Other.pActorUserData->pOwner);
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
}

void CMonster::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CMonster::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CMonster::On_Hit(CGameObject* _pHitter, _float _fDamg)
{
	m_tStat.fHP -= _fDamg;
	if (m_tStat.fHP < 0)
	{
		m_tStat.fHP = 0;
		//Event_DeleteObject(this);
	}
	
	Event_ChangeMonsterState(MONSTER_STATE::HIT, m_pFSM);
}

void CMonster::KnockBack(CGameObject* _pHitter)
{
	if (COORDINATE_3D == Get_CurCoord())
	{
		_float3 vRepulse;
		XMStoreFloat3(&vRepulse, -10.f * XMVector3Normalize(_pHitter->Get_FinalPosition() - Get_FinalPosition()));
		vRepulse.y = -1.f;
		Get_ActorCom()->Add_Impulse(vRepulse);
	}
}

void CMonster::Attack()
{
}

HRESULT CMonster::Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition)
{
	if (FAILED(__super::Change_Coordinate(_eCoordinate, _pNewPosition)))
		return E_FAIL;

	if (COORDINATE_2D == Get_CurCoord())
		Set_2D_Direction(F_DIRECTION::DOWN);

	Set_AnimChangeable(true);
	m_pFSM->Change_State((_uint)MONSTER_STATE::IDLE);

	return S_OK;
}

void CMonster::Change_Dir()
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
	
	_float fResult = m_pGameInstance->Get_Angle_Between_Vectors(XMVectorSet(0.f, 0.f, -1.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f), m_pTarget->Get_FinalPosition() - Get_FinalPosition());
	
	if ((315.f <= fResult && fResult < 360.f) || (45.f > fResult && 0.f <= fResult))
		Set_2D_Direction(F_DIRECTION::UP);
	else if (45.f <= fResult && 135.f > fResult)
		Set_2D_Direction(F_DIRECTION::RIGHT);
	else if (135.f <= fResult && 225.f > fResult)
		Set_2D_Direction(F_DIRECTION::DOWN);
	else if (225.f <= fResult && 315.f > fResult)
		Set_2D_Direction(F_DIRECTION::LEFT);
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

void CMonster::Set_2D_Direction(F_DIRECTION _eDir)
{
	m_e2DDirection = _eDir;
	if (F_DIRECTION::LEFT == m_e2DDirection)
	{
		_vector vRight = m_pControllerTransform->Get_State(CTransform::STATE_RIGHT);
		m_pControllerTransform->Set_State(CTransform::STATE_RIGHT, -XMVectorAbs(vRight));
	}
	else if (F_DIRECTION::RIGHT == m_e2DDirection)
	{
		_vector vRight = m_pControllerTransform->Get_State(CTransform::STATE_RIGHT);
		m_pControllerTransform->Set_State(CTransform::STATE_RIGHT, XMVectorAbs(vRight));
	}

	Change_Animation();
}

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


	m_tStat.fHP = m_fHp;

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
	m_fDelayTime = { 0.f };
	m_isCool = { false };
	m_fCoolTime = { 0.f };
	m_iAttackCount = { 0 };

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
