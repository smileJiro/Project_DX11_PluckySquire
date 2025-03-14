#include "stdafx.h"
#include "C08_Box.h"
#include "Section_Manager.h"
#include "GameInstance.h"

CC08_Box::CC08_Box(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCarriableObject(_pDevice, _pContext)
{
}


CC08_Box::CC08_Box(const CC08_Box& _Prototype)
	:CCarriableObject(_Prototype)
{
}

HRESULT CC08_Box::Initialize(void* _pArg)
{
	CARRIABLE_DESC* C08_BoxModelDesc = static_cast<CARRIABLE_DESC*>(_pArg);
	C08_BoxModelDesc->eStartCoord = COORDINATE_2D;
	C08_BoxModelDesc->eCrriableObjId = CARRIABLE_OBJ_ID::DIEC;
	C08_BoxModelDesc->vHeadUpRoolPitchYaw3D = { 0.f,0.f,0.f };
	C08_BoxModelDesc->vHeadUpOffset3D = { 0.f,1.8f,0.f };
	C08_BoxModelDesc->fHeadUpHeight2D = 80.f;
	C08_BoxModelDesc->isCoordChangeEnable = false;
	//C08_BoxModelDesc->iModelPrototypeLevelID_3D = C08_BoxModelDesc->iCurLevelID;
	C08_BoxModelDesc->iModelPrototypeLevelID_2D = C08_BoxModelDesc->iCurLevelID;
	C08_BoxModelDesc->strModelPrototypeTag_2D = TEXT("SKSP_carry_block_Sprite");
	C08_BoxModelDesc->strModelPrototypeTag_3D = TEXT("Wooden_Cube");
	C08_BoxModelDesc->strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
	C08_BoxModelDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
	//C08_BoxModelDesc->tTransform2DDesc.vInitialPosition = _float3(0.0f, 1.0f, 0.f);
	//C08_BoxModelDesc->tTransform2DDesc.vInitialScaling = _float3(200.f, 200.f, 200.f);
	C08_BoxModelDesc->iRenderGroupID_3D = RG_3D;
	C08_BoxModelDesc->iPriorityID_3D = PR3D_GEOMETRY;
	C08_BoxModelDesc->tTransform3DDesc.vInitialPosition = _float3(0.0f, 1.0f, -10.f);
	C08_BoxModelDesc->tTransform3DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);

	CActor::ACTOR_DESC ActorDesc = {};
	ActorDesc.pOwner = this;
	ActorDesc.FreezeRotation_XYZ[0] = false;
	ActorDesc.FreezeRotation_XYZ[1] = false;
	ActorDesc.FreezeRotation_XYZ[2] = false;
	ActorDesc.FreezePosition_XYZ[0] = false;
	ActorDesc.FreezePosition_XYZ[1] = false;
	ActorDesc.FreezePosition_XYZ[2] = false;

	SHAPE_BOX_DESC ShapeDesc = {};
	ShapeDesc.vHalfExtents = { 0.5f,0.5f ,0.5f };
	SHAPE_DATA ShapeData;
	ShapeData.pShapeDesc = &ShapeDesc;
	ShapeData.eShapeType = SHAPE_TYPE::BOX;
	ShapeData.eMaterial = ACTOR_MATERIAL::DEFAULT;
	ShapeData.isTrigger = false;
	ShapeData.FilterData.MyGroup = OBJECT_GROUP::DYNAMIC_OBJECT;
	ShapeData.FilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::DYNAMIC_OBJECT | OBJECT_GROUP::PLAYER;
	XMStoreFloat4x4(&ShapeData.LocalOffsetMatrix, XMMatrixTranslation(0.0f, 0.f, 0.f));
	ActorDesc.ShapeDatas.push_back(ShapeData);

	SHAPE_SPHERE_DESC ShapeDesc2 = {};
	ShapeDesc2.fRadius = 0.25f;
	SHAPE_DATA ShapeData2;
	ShapeData2.pShapeDesc = &ShapeDesc2;
	ShapeData2.eShapeType = SHAPE_TYPE::SPHERE;
	ShapeData2.eMaterial = ACTOR_MATERIAL::DEFAULT;
	ShapeData2.isTrigger = false;
	ShapeData2.FilterData.MyGroup = OBJECT_GROUP::INTERACTION_OBEJCT;
	ShapeData2.FilterData.OtherGroupMask = OBJECT_GROUP::PLAYER_TRIGGER;
	XMStoreFloat4x4(&ShapeData2.LocalOffsetMatrix, XMMatrixTranslation(0.0f, 0.f, 0.f));
	ActorDesc.ShapeDatas.push_back(ShapeData2);


	ActorDesc.tFilterData.MyGroup = OBJECT_GROUP::DYNAMIC_OBJECT;
	ActorDesc.tFilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::BLOCKER | OBJECT_GROUP::PLAYER;
	C08_BoxModelDesc->pActorDesc = &ActorDesc;
	C08_BoxModelDesc->eActorType = ACTOR_TYPE::DYNAMIC;
	if (FAILED(__super::Initialize(C08_BoxModelDesc)))
		return E_FAIL;

	// 2D 충돌용 콜라이더 추가.
	m_p2DColliderComs.resize(2);
	//static_cast<CCollider_Circle*> (m_p2DColliderComs[1])->Set_Radius();
	CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};

	CircleDesc.pOwner = this;
	CircleDesc.fRadius = 20.f;
	_float3 vScale = m_pControllerTransform->Get_Transform(COORDINATE_2D)->Get_Scale();
	CircleDesc.vScale = { 1.f / vScale.x, 1.f / vScale.y };
	CircleDesc.vOffsetPosition = { 0.f, 0.f };
	CircleDesc.isBlock = false;
	CircleDesc.isTrigger = true;
	CircleDesc.iCollisionGroupID = OBJECT_GROUP::GIMMICK_OBJECT;

	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
		TEXT("Com_Body2DCollider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[1]), &CircleDesc)))
		return E_FAIL;

	for (auto pCollider : m_p2DColliderComs)
		pCollider->Set_Active(true);

	return S_OK;
}

void CC08_Box::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
}

void CC08_Box::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT CC08_Box::Render()
{
#ifdef _DEBUG
	if (COORDINATE_2D == Get_CurCoord() && 2 <= m_p2DColliderComs.size() && nullptr != m_p2DColliderComs[1])
		m_p2DColliderComs[1]->Render();
#endif
	return __super::Render();
}

void CC08_Box::On_PickUpStart(CPlayer* _pPalyer, _fmatrix _matPlayerOffset)
{
	for (auto pCollider : m_p2DColliderComs)
		pCollider->Set_Active(false);
}

void CC08_Box::On_Land()
{
	for (auto pCollider : m_p2DColliderComs)
		pCollider->Set_Active(true);
}


CC08_Box* CC08_Box::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CC08_Box* pInstance = new CC08_Box(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : C08_Box");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CC08_Box::Clone(void* _pArg)
{
	CC08_Box* pInstance = new CC08_Box(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : C08_Box");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CC08_Box::Free()
{
	__super::Free();
}
