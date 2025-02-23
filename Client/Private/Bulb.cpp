#include "stdafx.h"
#include "Bulb.h"

#include "GameInstance.h"
#include "Player.h"
#include "Effect_Manager.h"
#include "Effect2D_Manager.h"
#include "Section_Manager.h"

CBulb::CBulb(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CTriggerObject(_pDevice, _pContext)
{
}

CBulb::CBulb(const CBulb& _Prototype)
	: CTriggerObject(_Prototype)
	, m_pFresnelBuffer(_Prototype.m_pFresnelBuffer)
	, m_tFresnelInfo(_Prototype.m_tFresnelInfo)
{
	Safe_AddRef(m_pFresnelBuffer);
}

HRESULT CBulb::Initialize_Prototype()
{
	if (FAILED(Ready_Buffer()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBulb::Initialize(void* _pArg)
{
	CBulb::BULB_DESC* pDesc = static_cast<CBulb::BULB_DESC*>(_pArg);

	pDesc->isCoordChangeEnable = false;

	if (COORDINATE_3D == pDesc->eStartCoord)
	{
		Initialize_3D_Object(pDesc);

	}

	if (COORDINATE_2D == pDesc->eStartCoord)
		Initialize_2D_Object(pDesc);

	if (FAILED(__super::Initialize(pDesc))) {
		MSG_BOX("Bulb Initialize Falied");
		return E_FAIL;
	}

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	if (COORDINATE_3D == pDesc->eStartCoord) 
	{
		Add_Shape();
		static_cast<CActor_Dynamic*>(m_pActorCom)->Set_Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(45.f));
	
		static_cast<CActor_Dynamic*>(m_pActorCom)->Set_Gravity(false);
	}


	return S_OK;
}

void CBulb::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);
}

void CBulb::Update(_float _fTimeDelta)
{
	Default_Move(_fTimeDelta);
	Sticking(_fTimeDelta);

	__super::Update(_fTimeDelta);
}

void CBulb::Late_Update(_float _fTimeDelta)
{
	if (COORDINATE_3D == Get_CurCoord() && false == m_isFrustumCulling)
	{
		m_pGameInstance->Add_RenderObject_New(RG_3D, PR3D_GEOMETRY, this);
	}

	__super::Late_Update(_fTimeDelta);
}

HRESULT CBulb::Render()
{
	if (FAILED(Bind_ShaderResources_WVP(Get_CurCoord())))
		return E_FAIL;

	if (COORDINATE_3D == Get_CurCoord())
	{
		//{
		//	CMesh* pMesh = m_p3DModelCom->Get_Mesh(0);

		//	if (FAILED(m_p3DModelCom->Bind_Material(m_p3DShaderCom, "g_AlbedoTexture", 0, aiTextureType_DIFFUSE, 0)))
		//		return E_FAIL;
		//	if (FAILED(m_p3DModelCom->Bind_Material(m_p3DShaderCom, "g_NormalTexture", 0, aiTextureType_NORMALS, 0)))
		//		return E_FAIL;
		//	/*_float4 vColor = _float4(1.f, 1.f, 1.f, 1.f);
		//	m_p3DShaderCom->Bind_RawValue("g_vDefaultDiffuseColor", &vColor, sizeof(_float4));*/
		//	
		//	if (FAILED(m_p3DShaderCom->Begin(8)))
		//		return E_FAIL;
		//
		//	if (FAILED(pMesh->Bind_BufferDesc()))
		//		return E_FAIL;

		//	if (FAILED(pMesh->Render()))
		//		return E_FAIL;

		//}

		//{
		//	CMesh* pMesh = m_p3DModelCom->Get_Mesh(1);

		//	m_p3DShaderCom->Bind_ConstBuffer("MultiFresnels", m_pFresnelBuffer);
		//	m_p3DShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4));
		//	
		//	
		//	if (FAILED(m_p3DShaderCom->Begin((_uint)PASS_VTXMESH::FRESNEL)))
		//		return E_FAIL;

		//	if (FAILED(pMesh->Bind_BufferDesc()))
		//		return E_FAIL;

		//	if (FAILED(pMesh->Render()))
		//		return E_FAIL;
	
		//}		


		m_p3DModelCom->Render(m_p3DShaderCom, (_uint)PASS_VTXMESH::DEFAULT);
	}

	else if (COORDINATE_2D == Get_CurCoord())
	{
		m_p2DModelCom->Render(m_p2DShaderCom, (_uint)PASS_VTXPOSTEX::SPRITE2D);
		__super::Render();
	}


	return S_OK;
}

HRESULT CBulb::Initialize_3D_Object(BULB_DESC* _pDesc)
{
	_pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(360.f);
	_pDesc->tTransform3DDesc.fSpeedPerSec = 10.f;

	_pDesc->iActorType = (_uint)ACTOR_TYPE::DYNAMIC;
	_pDesc->isTrigger = true;
	_pDesc->eShapeType = (SHAPE_TYPE)SHAPE_TYPE::SPHERE;
	_pDesc->fRadius = .5f;

	_pDesc->iFillterMyGroup = OBJECT_GROUP::TRIGGER_OBJECT;
	_pDesc->iFillterOtherGroupMask = OBJECT_GROUP::PLAYER | OBJECT_GROUP::MAPOBJECT;

	_pDesc->iTriggerType = (_uint)TRIGGER_TYPE::DEFAULT_TRIGGER;

	_pDesc->vLocalPosOffset = { 0.f, 0.f, 0.f };
	_pDesc->FreezeRotation[0] = true;
	_pDesc->FreezeRotation[1] = true;
	_pDesc->FreezeRotation[2] = false;
	_pDesc->iShapeUse = SHAPE_BODY;

	return S_OK;
}

HRESULT CBulb::Initialize_2D_Object(BULB_DESC* _pDesc)
{
	_pDesc->tTransform2DDesc.fRotationPerSec = XMConvertToRadians(360.f);
	_pDesc->tTransform2DDesc.fSpeedPerSec = 10.f;

	return S_OK;
}

void CBulb::OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)
{
	if (OBJECT_GROUP::PLAYER != _Other.pActorUserData->pOwner->Get_ObjectGroupID() || 
		(_uint)SHAPE_USE::SHAPE_BODY !=_Other.pShapeUserData->iShapeUse)
		return;

	switch (_My.pShapeUserData->iShapeUse)
	{
	case BULB_SHAPE_USE::SHAPE_BODY:
	{

		Event_Get_Bulb(COORDINATE_3D);
		Event_DeleteObject(this);
		static_cast<CActor_Dynamic*>(m_pActorCom)->Set_LinearVelocity(XMVectorZero());
		m_isSticking = false;

		CEffect_Manager::GetInstance()->Active_Effect(TEXT("Bulb"), true, m_pControllerTransform->Get_WorldMatrix_Ptr());
		m_pGameInstance->Start_SFX(_wstring(L"A_sfx_pickup_lightbulb-") + to_wstring(rand() % 4), 50.f);
		m_pGameInstance->Start_SFX(_wstring(L"A_sfx_lightbulb_addv2-") + to_wstring(rand() % 8), 20.f);

	}
		break;
	case BULB_SHAPE_USE::SHAPE_STICKING:
	{
		m_pTargetWorld = _Other.pActorUserData->pOwner->Get_ControllerTransform()->Get_WorldMatrix_Ptr();
		m_isSticking = true;
	}
		break;
	}
}

void CBulb::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	if(true == _pMyCollider->Is_Trigger())
	{
		if (OBJECT_GROUP::PLAYER & _pOtherCollider->Get_CollisionGroupID())
		{
			if (BULB_2DCOLLIDER_USE::BULB == _pMyCollider->Get_ColliderUse())

			{
				Event_Get_Bulb(COORDINATE_2D);

				//Effect
				_matrix matFX = Get_ControllerTransform()->Get_WorldMatrix();
				matFX.r[0] = XMVector3Normalize(matFX.r[0]);
				matFX.r[1] = XMVector3Normalize(matFX.r[1]);
				matFX.r[2] = XMVector3Normalize(matFX.r[2]);
				CEffect2D_Manager::GetInstance()->Play_Effect(TEXT("health_pickup_small"), CSection_Manager::GetInstance()->Get_Cur_Section_Key(), matFX);
				
				Event_DeleteObject(this);
			}
			if (BULB_2DCOLLIDER_USE::BULB_STICKING == _pMyCollider->Get_ColliderUse())
			{
				m_pTargetWorld = _pOtherObject->Get_ControllerTransform()->Get_WorldMatrix_Ptr();
				m_isSticking = true;
			}
		}
	}
}

void CBulb::Add_Shape()
{
	// 충돌 감지용 구 (트리거)
	SHAPE_DATA ShapeData;

	ShapeData.eShapeType = SHAPE_TYPE::SPHERE;
	//ShapeData.iShapeUse = SHAPE_TRIGER;
	ShapeData.isTrigger = true;
	XMStoreFloat4x4(&ShapeData.LocalOffsetMatrix, XMMatrixTranslation(0.f, 0.f, 0.f)); //여기임
	SHAPE_SPHERE_DESC SphereDesc = {};
	SphereDesc.fRadius = 2.f;
	ShapeData.pShapeDesc = &SphereDesc;
	ShapeData.iShapeUse = SHAPE_STICKING;

	m_pActorCom->Add_Shape(ShapeData);
}

void CBulb::Sticking(_float _fTimeDelta)
{
	if (false == m_isSticking)
		return;

	_vector vTargetPos;
	memcpy(&vTargetPos, m_pTargetWorld->m[3], sizeof(_float4));
	
	if(COORDINATE_3D == Get_CurCoord())
	{
		vTargetPos = XMVectorSetY(vTargetPos, XMVectorGetY(vTargetPos) + 0.6f);
		_vector vPos = Get_ControllerTransform()->Get_State(CTransform::STATE_POSITION);

		vPos = XMVectorLerp(vPos, vTargetPos, 0.3f);

		Get_ActorCom()->Set_GlobalPose({ XMVectorGetX(vPos), XMVectorGetY(vPos), XMVectorGetZ(vPos) });
	}
	
	else if (COORDINATE_2D == Get_CurCoord())
	{
		_vector vPos = Get_FinalPosition();

		vPos = XMVectorLerp(vPos, vTargetPos, 0.3f);

		Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, vPos);
	}

}

void CBulb::Default_Move(_float _fTimeDelta)
{
	if (true == m_isSticking)
		return;

	m_fBulbTime.y += _fTimeDelta;
	_float fRatio = m_fBulbTime.y / m_fBulbTime.x;
	if (fRatio >= (1.f - EPSILON)) {
		m_eBulbState = (BULB_STATE)(m_eBulbState ^ BULB_ALL);
		m_fBulbTime.y = 0.f;
		return;
	}

	switch (m_eBulbState) {
	case BULB_UP:
	{
		if (COORDINATE_3D == Get_CurCoord())
			m_pActorCom->Set_LinearVelocity(XMVectorSet(0.f, 1.f, 0.f, 0.f), 0.35f);
		else if (COORDINATE_2D == Get_CurCoord())
			Get_ControllerTransform()->Go_Up(_fTimeDelta);
	}
		break;
	case BULB_DOWN:
	{
		if (COORDINATE_3D == Get_CurCoord())
			m_pActorCom->Set_LinearVelocity(XMVectorSet(0.f, -1.f, 0.f, 0.f), 0.35f);
		else if (COORDINATE_2D == Get_CurCoord())
			Get_ControllerTransform()->Go_Down(_fTimeDelta);
	}
		break;
	}
	



	//_vector vPos = m_pControllerTransform->Get_State(CTransform::STATE_POSITION);
	//
	//if (XMVectorGetY(vPos) < m_fMin) {	
	//	static_cast<CActor_Dynamic*>(m_pActorCom)->Add_Impulse({ 0.f, 0.6f, 0.f });
	//}
}

void CBulb::Active_OnEnable()
{
	__super::Active_OnEnable();


}

void CBulb::Active_OnDisable()
{
	m_isSticking = false;

	__super::Active_OnDisable();
}

HRESULT CBulb::Ready_Components(BULB_DESC* _pArg)
{
	if (COORDINATE_3D == Get_CurCoord())
	{
		// Shader
		if (FAILED(Add_Component(m_pGameInstance->Get_StaticLevelID(), TEXT("Prototype_Component_Shader_VtxMesh"),
			TEXT("Com_Shader_3D"), reinterpret_cast<CComponent**>(&m_p3DShaderCom))))
			return E_FAIL;

		// Model
		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("LightbulbPickup_01"),
			TEXT("Com_Model_3D"), reinterpret_cast<CComponent**>(&m_p3DModelCom))))
			return E_FAIL;
	}

	else if (COORDINATE_2D == Get_CurCoord())
	{
		// Shader
		if (FAILED(Add_Component(m_pGameInstance->Get_StaticLevelID(), TEXT("Prototype_Component_Shader_VtxPosTex"),
			TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_p2DShaderCom))))
			return E_FAIL;

		// Model
		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Model2D_Bulb"),
			TEXT("Com_Model_2D"), reinterpret_cast<CComponent**>(&m_p2DModelCom))))
			return E_FAIL;

		//2D Collider
		_float fScaleX = Get_ControllerTransform()->Get_Scale().x;
		_float fScaleY = Get_ControllerTransform()->Get_Scale().y;

		CCollider* pCollider = nullptr;

		CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};
		CircleDesc.pOwner = this;
		CircleDesc.fRadius = 20.f;
		CircleDesc.vScale = { 1.f/ fScaleX, 1.f/ fScaleY };
		CircleDesc.vOffsetPosition = { 0.f, 0.f };
		CircleDesc.isBlock = false;
		CircleDesc.isTrigger = true;
		CircleDesc.iCollisionGroupID = OBJECT_GROUP::TRIGGER_OBJECT;
		CircleDesc.iColliderUse = (_uint)BULB;
		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
			TEXT("Com_2DBulbCollider"), reinterpret_cast<CComponent**>(&pCollider), &CircleDesc)))
			return E_FAIL;

		m_p2DColliderComs.push_back(pCollider);

		pCollider = nullptr;
		CircleDesc = {};
		CircleDesc.pOwner = this;
		CircleDesc.fRadius = 100.f;
		CircleDesc.vScale = { 1.f / fScaleX, 1.f / fScaleY };
		CircleDesc.vOffsetPosition = { 0.f, 0.f };
		CircleDesc.isBlock = false;
		CircleDesc.isTrigger = true;
		CircleDesc.iCollisionGroupID = OBJECT_GROUP::TRIGGER_OBJECT;
		CircleDesc.iColliderUse = (_uint)BULB_STICKING;
		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
			TEXT("Com_2DBulbStickingCollider"), reinterpret_cast<CComponent**>(&pCollider), &CircleDesc)))
			return E_FAIL;

		m_p2DColliderComs.push_back(pCollider);
	}

	return S_OK;
}

HRESULT CBulb::Ready_Buffer()
{
	//m_tFresnelInfo.vColor = _float4(0.01f, 0.058f, 0.028f, 1.0f);
	//m_tFresnelInfo.fExp = 0.f;
	//m_tFresnelInfo.fBaseReflect = 1.2f;
	//m_tFresnelInfo.fStrength = 0.43f;

	//m_pGameInstance->CreateConstBuffer(m_tFresnelInfo, D3D11_USAGE_DYNAMIC, &m_pFresnelBuffer);

	m_tFresnelInfo.tInner.vColor = _float4(0.83f, 0.44f, 0.f, 0.49f);
	m_tFresnelInfo.tInner.fExp = 2.f;
	m_tFresnelInfo.tInner.fBaseReflect = 0.14f;
	m_tFresnelInfo.tInner.fStrength = 0.45f;

	m_tFresnelInfo.tOuter.vColor = _float4(0.85f, 0.56f, 0.17f, 0.28f);
	m_tFresnelInfo.tOuter.fExp = 0.1f;
	m_tFresnelInfo.tOuter.fBaseReflect = 3.63f;
	m_tFresnelInfo.tOuter.fStrength = 0.54f;

	m_pGameInstance->CreateConstBuffer(m_tFresnelInfo, D3D11_USAGE_DYNAMIC, &m_pFresnelBuffer);

	return S_OK;
}

HRESULT CBulb::Bind_ShaderResources_WVP(COORDINATE eCoordinate)
{
	if (COORDINATE_3D == eCoordinate)
	{
		if (FAILED(m_p3DShaderCom->Bind_Matrix("g_WorldMatrix", m_pControllerTransform->Get_WorldMatrix_Ptr())))
			return E_FAIL;

		_float4x4 ViewMat = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
		_float4x4 ProjMat = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ);

		if (FAILED(m_p3DShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
			return E_FAIL;

		if (FAILED(m_p3DShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
			return E_FAIL;
	}
	else if (COORDINATE_2D == eCoordinate)
	{
		_matrix matLocal = *m_p2DModelCom->Get_CurrentSpriteTransform();
		_matrix matRatioScalling = XMMatrixScaling((_float)RATIO_BOOK2D_X, (_float)RATIO_BOOK2D_Y, 1.f);
		matLocal *= matRatioScalling;

		_matrix matWorld = matLocal * XMLoadFloat4x4(m_pControllerTransform->Get_WorldMatrix_Ptr());

		_float4x4 matWorld4x4;
		XMStoreFloat4x4(&matWorld4x4, matWorld);
		if (FAILED(m_p2DShaderCom->Bind_Matrix("g_WorldMatrix", &matWorld4x4)))
			return E_FAIL;
	}

	return S_OK;
}

CBulb* CBulb::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CBulb* pInstance = new CBulb(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBulb");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBulb::Clone(void* _pArg)
{
	CBulb* pInstance = new CBulb(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CBulb");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBulb::Free()
{
	Safe_Release(m_p2DModelCom);
	Safe_Release(m_p3DModelCom);
	Safe_Release(m_p2DShaderCom);
	Safe_Release(m_p3DShaderCom);

	Safe_Release(m_pFresnelBuffer);

	__super::Free();
}
