#include "stdafx.h"
#include "FallingRock.h"
#include "GameInstance.h"
#include "Section_Manager.h"
#include "Camera_Manager.h"

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
	
	// Add Desc
	pDesc->iObjectGroupID = OBJECT_GROUP::MONSTER_PROJECTILE;

	/* 2D */
	pDesc->Build_2D_Model(pDesc->iCurLevelID, TEXT("Prototype_Model2D_FallingRock"), TEXT("Prototype_Component_Shader_VtxPosTex"), (_uint)PASS_VTXPOSTEX::SPRITE2D, true);

	/* 3D */
	pDesc->iRenderGroupID_3D = RENDERGROUP::RG_3D;
	pDesc->iPriorityID_3D = PR3D_GEOMETRY;
	pDesc->vMaterialDefaultColor = { 1.0f, 1.0f, 1.0f , 1.0f };
	pDesc->fFrustumCullingRange = 3.0f;
	pDesc->Build_3D_Model(pDesc->iCurLevelID, TEXT("Prototype_Model3D_FallingRock"), TEXT("Prototype_Component_Shader_VtxMesh"),(_uint)PASS_VTXMESH::DEFAULT, true);

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

	m_pControllerModel->Switch_Animation(STATE_FALLDOWN);

	m_vShadowYDesc.y = XMVectorGetY(Get_ControllerTransform()->Get_State(CTransform_2D::STATE_POSITION));

	m_pActorCom->Set_MassLocalPos(_float3(0.0f,0.8f, 0.0f));
	return S_OK;
}

void CFallingRock::Priority_Update(_float _fTimeDelta)
{
	if (KEY_DOWN(KEY::G))
	{
		m_eCurState = STATE::STATE_FALLDOWN;
	}
	if (KEY_DOWN(KEY::H))
	{
		m_eCurState = STATE::STATE_BOUND_2D;
	}

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
	if (COORDINATE_2D == Get_CurCoord())
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
	default:
		break;
	}

	m_ePreState = m_eCurState;
}

void CFallingRock::State_Change_FallDown()
{
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
	XMStoreFloat3(&vForce, XMVector3Normalize(XMVectorSet(0.0f, 0.8f, -1.0f, 0.0f)) * 30.f);
	m_pActorCom->Add_Impulse(vForce);
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
	default:
		break;
	}
}

void CFallingRock::Action_FallDown(_float _fTimeDelta)
{
	/* 중력컴포넌트가 알아서 할거야. Y 체크만하자. */

	if (m_fFallDownEndY >= XMVectorGetY(Get_FinalPosition()))
		m_eCurState = STATE::STATE_BOUND_2D;

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
		if (fPosY < m_fCoordChangePosY)
		{
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
	int a = 0;
}

HRESULT CFallingRock::Ready_Components(FALLINGROCK_DESC* _pDesc)
{
	/* Com_Gravity */
	CGravity::GRAVITY_DESC GravityDesc = {};
	GravityDesc.fGravity = 9.8f * 300.f;
	GravityDesc.vGravityDirection = { 0.0f, -1.0f, 0.0f };
	GravityDesc.pOwner = this;
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Gravity"),
		TEXT("Com_Gravity"), reinterpret_cast<CComponent**>(&m_pGravityCom), &GravityDesc)))
		return E_FAIL;


	CComponent* pComponent = static_cast<CComponent*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, m_iCurLevelID, TEXT("Prototype_Model2D_FallingRockShadow"), nullptr));
	if (nullptr == pComponent)
		return E_FAIL;
	m_p2DShadowModelCom = static_cast<C2DModel*>(pComponent);


	return S_OK;
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
