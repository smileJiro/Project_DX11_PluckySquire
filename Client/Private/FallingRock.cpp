#include "stdafx.h"
#include "FallingRock.h"
#include "GameInstance.h"
#include "Section_Manager.h"
#include "Camera_Manager.h"
#include "Blocker.h"
#include "Effect_Manager.h"
#include "Effect2D_Manager.h"
#include "Character.h"


CFallingRock::CFallingRock(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CModelObject(_pDevice, _pContext)
{
}

CFallingRock::CFallingRock(const CFallingRock& _Prototype)
	:CModelObject(_Prototype)
{
}

HRESULT CFallingRock::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CFallingRock::Initialize(void* _pArg)
{
	FALLINGROCK_DESC* pDesc = static_cast<FALLINGROCK_DESC*>(_pArg);
	// Save Desc
	m_fFallDownEndY = pDesc->fFallDownEndY;
	m_isColBound = pDesc->isColBound;
	m_eOriginDirection = pDesc->eColBoundDirection;
	m_fBoundEndPosY = pDesc->fBoundEndPosY;
	m_isChapter4 = pDesc->m_isChapter4;
	// Add Desc
	pDesc->iObjectGroupID = OBJECT_GROUP::MONSTER_PROJECTILE;
		

	/* 2D */
	pDesc->Build_2D_Model(pDesc->iCurLevelID, TEXT("FallingRock"), TEXT("Prototype_Component_Shader_VtxPosTex"), (_uint)PASS_VTXPOSTEX::SPRITE2D, true);

	/* 3D */
	pDesc->iRenderGroupID_3D = RENDERGROUP::RG_3D;
	pDesc->iPriorityID_3D = PR3D_GEOMETRY;
	pDesc->vMaterialDefaultColor = { 1.0f, 1.0f, 1.0f , 1.0f };
	pDesc->fFrustumCullingRange = 3.0f;
	pDesc->Build_3D_Model(pDesc->iCurLevelID, TEXT("Rock_03"), TEXT("Prototype_Component_Shader_VtxMesh"), (_uint)PASS_VTXMESH::DEFAULT, true);

	pDesc->eStartCoord = COORDINATE_2D; // 여기서 세팅해줘야함.

	/* 3D Actor */
	pDesc->eActorType = ACTOR_TYPE::DYNAMIC;
	CActor::ACTOR_DESC ActorDesc;
	ActorDesc.pOwner = this;
	ActorDesc.tFilterData.MyGroup = OBJECT_GROUP::MONSTER_PROJECTILE;
	ActorDesc.tFilterData.OtherGroupMask = OBJECT_GROUP::PLAYER | OBJECT_GROUP::MAPOBJECT;
	ActorDesc.FreezeRotation_XYZ[0] = false;
	ActorDesc.FreezeRotation_XYZ[1] = false;
	ActorDesc.FreezeRotation_XYZ[2] = false;


	SHAPE_SPHERE_DESC SphereDesc{};
	SphereDesc.fRadius = 1.0f;

	SHAPE_DATA ShapeData = {};
	ShapeData.pShapeDesc = &SphereDesc;
	ShapeData.eShapeType = SHAPE_TYPE::SPHERE;
	ShapeData.iShapeUse = (_uint)SHAPE_USE::SHAPE_BODY;
	XMStoreFloat4x4(&ShapeData.LocalOffsetMatrix, XMMatrixTranslation(0.0f, 0.8f, 0.0f));
	ActorDesc.ShapeDatas.push_back(ShapeData);
	pDesc->pActorDesc = &ActorDesc;
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	if (true == m_isColBound)
	{
		Change_Bound_Direction(pDesc->eColBoundDirection);

		m_pControllerModel->Switch_Animation(STATE_BOUND_2D);
		m_eCurState = STATE::STATE_COL_BOUND;
		m_fJumpForcePerSec = 750.f;
	}
	else
	{
		m_pControllerModel->Switch_Animation(STATE_FALLDOWN);
		m_eCurState = STATE::STATE_FALLDOWN;
		m_vShadowYDesc.y = XMVectorGetY(Get_ControllerTransform()->Get_State(CTransform_2D::STATE_POSITION));
	}
	

	m_pActorCom->Set_MassLocalPos(_float3(0.0f,0.8f, 0.0f));
	return S_OK;
}

void CFallingRock::Priority_Update(_float _fTimeDelta)
{

	__super::Priority_Update(_fTimeDelta);
}

void CFallingRock::Update(_float _fTimeDelta)
{
	Action_State(_fTimeDelta);

	__super::Update(_fTimeDelta);
}

void CFallingRock::Late_Update(_float _fTimeDelta)
{
	State_Change();

	__super::Late_Update(_fTimeDelta);
}

HRESULT CFallingRock::Render()
{
	if (false == m_isColBound)
	{
		/* 2D 상태일때에는 그림자 렌더를 먼저하고, */
		_vector vPos = Get_ControllerTransform()->Get_State(CTransform_2D::STATE_POSITION);
		_float3 vScale = Get_FinalScale();
		/* 크기, y 값만, 보정해준다. */
		_float4x4 ShadowWorldMatrix = {};
		XMStoreFloat4x4(&ShadowWorldMatrix, XMMatrixScaling(400.f * vScale.x, 400.f * vScale.y, 400.f * vScale.z) * XMMatrixRotationZ(XMConvertToRadians(-7.f)) * XMMatrixTranslation(XMVectorGetX(vPos), m_vShadowYDesc.y + 70.f, XMVectorGetZ(vPos)));
		if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_Matrix("g_WorldMatrix", &ShadowWorldMatrix)))
			return E_FAIL;

		m_p2DShadowModelCom->Render(m_pShaderComs[COORDINATE_2D], m_iShaderPasses[COORDINATE_2D]);
	}

	CModelObject::Render();
	return S_OK;
}

void CFallingRock::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	_uint iGroupID = _pOtherCollider->Get_CollisionGroupID();
	switch ((OBJECT_GROUP)iGroupID)
	{
	case Client::NONE:
		break;
	case Client::PLAYER:
	{
		CCamera_Manager::GetInstance()->Start_Shake_ByCount(CCamera_Manager::TARGET_2D, 0.1f, 0.2f, 20, CCamera::SHAKE_Y);
		_float4 vKnockBackForce = {};
		_float fKnockBackStrength = 500.f;
		if (m_eCurState == STATE_COL_BOUND)
		{
			vKnockBackForce.x = m_vColBoundDirection.x * fKnockBackStrength;
			vKnockBackForce.y = m_vColBoundDirection.y * fKnockBackStrength;
		}
		else
		{
			_vector vPos = Get_FinalPosition();
			_vector vOtherPos = _pOtherObject->Get_FinalPosition();
			_vector vKnockBackDir = vOtherPos - vPos;
			XMStoreFloat4(&vKnockBackForce, XMVector3Normalize(vKnockBackDir) * fKnockBackStrength);

		}
		Event_KnockBack(static_cast<CCharacter*>(_pOtherObject), XMVectorSet(vKnockBackForce.x, vKnockBackForce.y, 0.0f, 0.0f));
		Event_Hit(this, static_cast<CCharacter*>(_pOtherObject), 1, XMVectorZero());
		Event_DeleteObject(this); // 플레이어랑 부딪히면 사라지고, 이펙트 생성.
		//_vector vPos = Get_FinalPosition();
		//;
		CEffect2D_Manager::GetInstance()->Play_Effect(TEXT("FallingRock_Breaking"), m_strSectionName, Get_WorldMatrix());
	}
		break;
	case Client::MONSTER:
		break;
	case Client::MAPOBJECT:
		break;
	case Client::INTERACTION_OBEJCT:
		break;
	case Client::PLAYER_PROJECTILE:
		break;
	case Client::MONSTER_PROJECTILE:
		break;
	case Client::TRIGGER_OBJECT:
		break;
	case Client::RAY_OBJECT:
		break;
	case Client::PLAYER_TRIGGER:
		break;
	case Client::BLOCKER:
	{
		if (true == _pMyCollider->Is_Trigger())
			break;

		/* 1. Blocker은 항상 AABB임을 가정. */
		if (true == m_isChapter4)
		{
			Event_DeleteObject(this); // 플레이어랑 부딪히면 사라지고, 이펙트 생성.
			//_vector vPos = Get_FinalPosition();
			//;
			CEffect2D_Manager::GetInstance()->Play_Effect(TEXT("FallingRock_Breaking"), m_strSectionName, Get_WorldMatrix());
			return;
		}

		if (true == static_cast<CBlocker*>(_pOtherObject)->Is_Floor())
		{


		/* 2. 나의 Collider 중점 기준, AABB에 가장 가까운 점을 찾는다. */
			_bool isResult = false;
			_float fEpsilon = 0.01f;
			_float2 vContactVector = {};
			isResult = static_cast<CCollider_Circle*>(_pMyCollider)->Compute_NearestPoint_AABB(static_cast<CCollider_AABB*>(_pOtherCollider), nullptr, &vContactVector);
			if (true == isResult)
			{
				/* 3. 충돌지점 벡터와 중력벡터를 내적하여 그 결과를 기반으로 Floor 인지 체크. */
				_float3 vGravityDir = m_pGravityCom->Get_GravityDirection();
				_float2 vGravityDirection = _float2(vGravityDir.x, vGravityDir.y);
				_float fGdotC = XMVectorGetX(XMVector2Dot(XMLoadFloat2(&vGravityDirection), XMVector2Normalize(XMLoadFloat2(&vContactVector))));
				if (1.0f - fEpsilon <= fGdotC)
				{
					/* 결과가 1에 근접한다면 이는 floor로 봐야겠지. */
					if (m_isColBound)
					{
						m_pGravityCom->Set_GravityAcc(100.0f);
						m_eCurState = STATE::STATE_COL_BOUND;

					}
					else
					{
						m_pGravityCom->Set_GravityAcc(0.0f);

					}
				}
			}
		}
		else
		{
			if (COLBOUND_LEFT == m_eColBoundDirection)
			{
				Change_Bound_Direction(COLBOUND_RIGHT);
			}
			else
			{
				Change_Bound_Direction(COLBOUND_LEFT);
			}
			
			//Event_DeleteObject(this);
		}
		
		// 충돌시마다 감소
		m_vDecrease.y += m_vDecrease.x;
		m_pGravityCom->Set_GravityOffset(m_vDecrease.y);
	}
	break;
	default:
		break;
	}

}

void CFallingRock::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CFallingRock::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CFallingRock::Change_Bound_Direction(COLBOUND2D _eDirection)
{
	if (m_eColBoundDirection == _eDirection)
		return;

	m_eColBoundDirection = _eDirection;
	switch (m_eColBoundDirection)
	{
	case Client::CFallingRock::COLBOUND_LEFT:
	{
		XMStoreFloat2(&m_vColBoundDirection, XMVector2Normalize(XMVectorSet(-1.0f, 1.7f, 0.0f, 0.0f)));
	}
		break;
	case Client::CFallingRock::COLBOUND_RIGHT:
	{
		XMStoreFloat2(&m_vColBoundDirection, XMVector2Normalize(XMVectorSet(1.0f, 1.7f, 0.0f, 0.0f)));
	}
		break;
	default:
		break;
	}

}

void CFallingRock::State_Change()
{
	if (m_ePreState == m_eCurState)
		return;

	switch (m_eCurState)
	{
	case Client::CFallingRock::STATE_FALLDOWN:
		State_Change_FallDown();
		break;
	case Client::CFallingRock::STATE_BOUND_2D:
		State_Change_Bound_2D();
		break;
	case Client::CFallingRock::STATE_BOUND_3D:
		State_Change_Bound_3D();
		break;
	case Client::CFallingRock::STATE_COL_BOUND:
		State_Change_ColBound_2D();
		break;
	default:
		break;
	}

	m_ePreState = m_eCurState;
}

void CFallingRock::State_Change_FallDown()
{
	m_pControllerModel->Switch_Animation(STATE_FALLDOWN);
	m_pGravityCom->Change_State(CGravity::STATE::STATE_FALLDOWN);
	m_pGravityCom->Set_Active(true);
}

void CFallingRock::State_Change_Bound_2D()
{
	_vector vPos = m_pControllerTransform->Get_State(CTransform::STATE_POSITION);
	_float fPosY = XMVectorGetY(vPos);
	/* 3. 다음 도착지 Y 계산(그림자위치) */
	_float fUpForce = m_fJumpForcePerSec * m_vJumpTime.x;
	_float fDownForce = 0.5f * m_pGravityCom->Get_Gravity() * m_vJumpTime.x * m_vJumpTime.x;
	m_vShadowYDesc.x = fPosY;
	m_vShadowYDesc.y = fPosY;
	m_vShadowYDesc.z = fPosY + (fUpForce - fDownForce);

	/* 1. 중력을 초기화 */
	m_pGravityCom->Set_GravityAcc(0.0f);
	/* 2. 점프 시작. */
	m_pControllerModel->Switch_Animation(STATE::STATE_BOUND_2D);
}

void CFallingRock::State_Change_Bound_3D()
{
	/* 0. 내가 속한 SectionManager의 WorldPosMap에서 좌표가져오기 */
	_vector vPos = Get_FinalPosition();
	_float4 v3DWorldPos = {};
	XMStoreFloat4(&v3DWorldPos, CSection_Manager::GetInstance()->Get_WorldPosition_FromWorldPosMap(m_strSectionName, _float2(XMVectorGetX(vPos), XMVectorGetY(vPos))));

	NORMAL_DIRECTION eNorDir = (NORMAL_DIRECTION)((_int)roundf(v3DWorldPos.w)); /* 추후 노말을 기준으로 힘의 방향을 결정해도 돼*/
	/* 1. 중력컴포넌트 끄기 >>> 액터쓸거니까 */
	Event_SetActive(m_pGravityCom, false);

	/* 2. Change Coordinate */
	Change_Coordinate(COORDINATE_3D, (_float3*)&v3DWorldPos);

	/* 2. Actor에 힘을 줘야함. */
	_float3 vForce = {};
	XMStoreFloat3(&vForce, XMVector3Normalize(XMVectorSet(0.0f, 0.8f, -1.0f, 0.0f)) * m_fForce3D);
	m_pActorCom->Add_Impulse(vForce);

	// Effect
	CEffect_Manager::GetInstance()->Active_EffectPosition(TEXT("RockOut"), true, XMLoadFloat4(&v3DWorldPos));

}

void CFallingRock::State_Change_ColBound_2D()
{
	/* 1. 중력을 초기화 */
	m_pGravityCom->Change_State(CGravity::STATE_FALLDOWN);
	m_pGravityCom->Set_GravityAcc(0.0f);
	/* 2. 점프 시작. */
	m_pControllerModel->Switch_Animation(STATE::STATE_BOUND_2D);
	m_vLifeTimeAcc.y = 0.0f;
	
	Change_Bound_Direction(m_eOriginDirection);
	m_vDecrease.y = 0.0f;
	m_pGravityCom->Set_GravityOffset(0.0f);
}

void CFallingRock::Action_State(_float _fTimeDelta)
{
	switch (m_eCurState)
	{
	case Client::CFallingRock::STATE_FALLDOWN:
		Action_FallDown(_fTimeDelta);
		break;
	case Client::CFallingRock::STATE_BOUND_2D:
		Action_Bound_2D(_fTimeDelta);
		break;
	case Client::CFallingRock::STATE_BOUND_3D:
		Action_Bound_3D(_fTimeDelta);
		break;
	case Client::CFallingRock::STATE_COL_BOUND:
		Action_ColBound_2D(_fTimeDelta);
		break;
	default:
		break;
	}
}

void CFallingRock::Action_FallDown(_float _fTimeDelta)
{
	/* 중력컴포넌트가 알아서 할거야. Y 체크만하자. */

	if (m_fFallDownEndY >= XMVectorGetY(Get_FinalPosition()))
	{
		m_eCurState = STATE::STATE_BOUND_2D;
		CCamera_Manager::GetInstance()->Start_Shake_ByCount(CCamera_Manager::TARGET_2D, 0.1f, 0.2f, 20, CCamera::SHAKE_Y);
	}

	_vector vPos = m_pControllerTransform->Get_State(CTransform::STATE_POSITION);
	m_vShadowYDesc.y = XMVectorGetY(vPos);
}

void CFallingRock::Action_Bound_2D(_float _fTimeDelta)
{
	_vector vPos = m_pControllerTransform->Get_State(CTransform::STATE_POSITION);
	
	m_vJumpTime.y += _fTimeDelta;
	if (m_vJumpTime.x <= m_vJumpTime.y)
	{
		_float fPosY = XMVectorGetY(vPos);
		if (fPosY < m_fBoundEndPosY)
		{
			if(false == m_isChapter4)
				m_eCurState = STATE_BOUND_3D;
		}
		/* 3. 다음 도착지 Y 계산(그림자위치) */
		_float fUpForce = m_fJumpForcePerSec * m_vJumpTime.x;
		_float fDownForce = 0.5f * m_pGravityCom->Get_Gravity() * m_vJumpTime.x * m_vJumpTime.x;
		m_vShadowYDesc.x = fPosY;
		m_vShadowYDesc.y = fPosY;
		m_vShadowYDesc.z = fPosY + (fUpForce - fDownForce);

		/* 1. 중력 초기화 */
		m_pGravityCom->Set_GravityAcc(0.0f);
		/* 2. 다시 점프 */
		m_vJumpTime.y = 0.0f;

		/* 3. 카메라 셰이크 */
		CCamera_Manager::GetInstance()->Start_Shake_ByCount(CCamera_Manager::TARGET_2D, 0.1f, 0.01f, 30, CCamera::SHAKE_XY);
	}
	else
	{
		_float fPosY = XMVectorGetY(vPos) + m_fJumpForcePerSec * _fTimeDelta;
		Set_Position(XMVectorSetY(vPos, fPosY));
		m_vShadowYDesc.y = LERP(m_vShadowYDesc.x, m_vShadowYDesc.z, m_vJumpTime.y / m_vJumpTime.x);
		
	}

	
}

void CFallingRock::Action_Bound_3D(_float _fTimeDelta)
{
	if (true == m_isDead)
		return;

	m_fDeadTime.y += _fTimeDelta;
	if (m_fDeadTime.x <= m_fDeadTime.y)
	{
		m_fDeadTime.y = 0.0f;
		Event_DeleteObject(this); /* 풀링객체면 알아서 Layer에서 Delete 하지않고, Active만 False 처리한다. */
		m_eCurState = STATE::STATE_FALLDOWN;
		State_Change();
	}
}

void CFallingRock::Action_ColBound_2D(_float _fTimeDelta)
{
	m_vLifeTimeAcc.y += _fTimeDelta;
	
	if (m_vLifeTimeAcc.x <= m_vLifeTimeAcc.y)
	{
		m_vLifeTimeAcc.y = 0.0f;
		m_eCurState = STATE::STATE_LAST;
		Event_DeleteObject(this);
		State_Change();
	}
	else
	{
		_vector vPos = m_pControllerTransform->Get_State(CTransform::STATE_POSITION);
		vPos += XMLoadFloat2(&m_vColBoundDirection) * m_fJumpForcePerSec * _fTimeDelta;
		Set_Position(vPos);
	}

}

HRESULT CFallingRock::Ready_Components(FALLINGROCK_DESC* _pDesc)
{
	/* Com_Gravity */
	CGravity::GRAVITY_DESC GravityDesc = {};
	if(false == m_isColBound)
		GravityDesc.fGravity = 9.8f * 300.f;
	else
		GravityDesc.fGravity = 9.8f * 170.f;

	GravityDesc.vGravityDirection = { 0.0f, -1.0f, 0.0f };
	GravityDesc.pOwner = this;
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Gravity"),
		TEXT("Com_Gravity"), reinterpret_cast<CComponent**>(&m_pGravityCom), &GravityDesc)))
		return E_FAIL;

	/* 발밑 그림자 전용 Model2D */
	if (false == m_isColBound)
	{
		CComponent* pComponent = static_cast<CComponent*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, m_iCurLevelID, TEXT("Prototype_Model2D_FallingRockShadow"), nullptr));
		if (nullptr == pComponent)
			return E_FAIL;
		m_p2DShadowModelCom = static_cast<C2DModel*>(pComponent);
	}

	m_p2DColliderComs.resize(1);
	/* Test 2D Collider */
	CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};
	CircleDesc.pOwner = this;
	CircleDesc.fRadius = 20.f;
	CircleDesc.vScale = { 1.0f, 1.0f };
	CircleDesc.vOffsetPosition = { 0.f, CircleDesc.fRadius * 0.5f };
	if(false == m_isColBound && false == m_isChapter4)
	{
		CircleDesc.isBlock = true;
		CircleDesc.iCollisionGroupID = OBJECT_GROUP::FALLINGROCK_BASIC;
	}
	else
	{
		CircleDesc.isBlock = false;
		CircleDesc.iCollisionGroupID = OBJECT_GROUP::MONSTER_PROJECTILE;
	}
	CircleDesc.isTrigger = false;
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
		TEXT("Com_Body2DCollider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &CircleDesc)))
		return E_FAIL;


	return S_OK;
}

void CFallingRock::Active_OnEnable()
{
	if (false == m_isColBound)
	{
		_vector vPos = m_pControllerTransform->Get_State(CTransform::STATE_POSITION);
		m_vShadowYDesc.y = XMVectorGetY(vPos);
		m_eCurState = STATE::STATE_FALLDOWN;

		__super::Active_OnEnable();
	}
	else
	{
		m_eCurState = STATE::STATE_COL_BOUND;
		State_Change_ColBound_2D();
	}



}

void CFallingRock::Active_OnDisable()
{
	if (STATE::STATE_BOUND_3D == m_eCurState || STATE::STATE_BOUND_2D == m_eCurState)
	{
		m_fDeadTime.y = 0.0f;
		Event_DeleteObject(this); /* 풀링객체면 알아서 Layer에서 Delete 하지않고, Active만 False 처리한다. */
		m_eCurState = STATE::STATE_FALLDOWN;
		State_Change();

	}


	__super::Active_OnDisable();
}

CFallingRock* CFallingRock::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CFallingRock* pInstance = new CFallingRock(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed Create : CFallingRock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFallingRock::Clone(void* _pArg)
{
	CFallingRock* pInstance = new CFallingRock(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed Clone : CFallingRock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFallingRock::Free()
{
	Safe_Release(m_p2DShadowModelCom);
	__super::Free();
}
