#include "stdafx.h"
#include "RabbitLunch.h"

CRabbitLunch::CRabbitLunch(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CCarriableObject(_pDevice, _pContext)
{
}

CRabbitLunch::CRabbitLunch(const CRabbitLunch& _Prototype)
	:CCarriableObject(_Prototype)
{
}

HRESULT CRabbitLunch::Initialize(void* _pArg)
{
	CARRIABLE_DESC* pModelDesc = static_cast<CARRIABLE_DESC*>(_pArg);

	pModelDesc->isCoordChangeEnable = true;
	pModelDesc->iModelPrototypeLevelID_3D = pModelDesc->iCurLevelID;
	pModelDesc->iModelPrototypeLevelID_2D = pModelDesc->iCurLevelID;

	pModelDesc->strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
	pModelDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
	pModelDesc->tTransform2DDesc.vInitialPosition = _float3(0.0f, 1.0f, 0.f);
	pModelDesc->tTransform2DDesc.vInitialScaling = _float3(200.f, 200.f, 200.f);
	pModelDesc->iRenderGroupID_3D = RG_3D;
	pModelDesc->iPriorityID_3D = PR3D_GEOMETRY;
	pModelDesc->iObjectGroupID = OBJECT_GROUP::INTERACTION_OBEJCT;

	CActor::ACTOR_DESC ActorDesc;
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
	ActorDesc.tFilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::BLOCKER | OBJECT_GROUP::PLAYER| OBJECT_GROUP::PLAYER_TRIGGER;
	pModelDesc->pActorDesc = &ActorDesc;
	pModelDesc->eActorType = ACTOR_TYPE::DYNAMIC;

	if (FAILED(__super::Initialize(pModelDesc)))
		return E_FAIL;
	return S_OK;
}

void CRabbitLunch::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
}

void CRabbitLunch::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT CRabbitLunch::Render()
{
	return __super::Render();
}

CRabbitLunch* CRabbitLunch::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CRabbitLunch* pInstance = new CRabbitLunch(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : RabbitLunch");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRabbitLunch::Clone(void* _pArg)
{
	CRabbitLunch* pInstance = new CRabbitLunch(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : RabbitLunch");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRabbitLunch::Free()
{
	__super::Free();
}
