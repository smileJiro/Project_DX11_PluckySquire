#include "stdafx.h"
#include "Bomb.h"
#include "Section_Manager.h"

CBomb::CBomb(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCarriableObject(_pDevice, _pContext)
{
}


CBomb::CBomb(const CBomb& _Prototype)
	:CCarriableObject(_Prototype)
{
}

HRESULT CBomb::Initialize(void* _pArg)
{
	CARRIABLE_DESC* BombModelDesc = static_cast<CARRIABLE_DESC*>(_pArg);
	BombModelDesc->eCrriableObjId = CARRIABLE_OBJ_ID::BOMB;
	BombModelDesc->vHeadUpRoolPitchYaw3D = { 0.f,0.f,0.f };
	BombModelDesc->vHeadUpOffset3D = { 0.f,1.8f,0.f };
	BombModelDesc->isCoordChangeEnable = true;
	BombModelDesc->iModelPrototypeLevelID_3D = BombModelDesc->iCurLevelID;
	BombModelDesc->iModelPrototypeLevelID_2D = BombModelDesc->iCurLevelID;
	BombModelDesc->strModelPrototypeTag_2D = TEXT("Bomb2D");
	BombModelDesc->strModelPrototypeTag_3D = TEXT("Bomb");
	BombModelDesc->strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
	BombModelDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
	BombModelDesc->tTransform2DDesc.vInitialPosition = _float3(0.0f, 1.0f, 0.f);
	BombModelDesc->tTransform2DDesc.vInitialScaling = _float3(200.f, 200.f, 200.f);
	BombModelDesc->iRenderGroupID_3D = RG_3D;
	BombModelDesc->iPriorityID_3D = PR3D_GEOMETRY;
	BombModelDesc->tTransform3DDesc.vInitialPosition = _float3(0.0f, 1.0f, -10.f);
	BombModelDesc->tTransform3DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
	
	CActor::ACTOR_DESC ActorDesc = {};
	ActorDesc.pOwner = this;
	ActorDesc.FreezeRotation_XYZ[0] = false;
	ActorDesc.FreezeRotation_XYZ[1] = false;
	ActorDesc.FreezeRotation_XYZ[2] = false;
	ActorDesc.FreezePosition_XYZ[0] = false;
	ActorDesc.FreezePosition_XYZ[1] = false;
	ActorDesc.FreezePosition_XYZ[2] = false;

	SHAPE_SPHERE_DESC ShapeDesc = {};
	ShapeDesc.fRadius = 0.5f;
	SHAPE_DATA ShapeData;
	ShapeData.pShapeDesc = &ShapeDesc;
	ShapeData.eShapeType = SHAPE_TYPE::SPHERE;
	ShapeData.eMaterial = ACTOR_MATERIAL::DEFAULT;
	ShapeData.isTrigger = false;
	ShapeData.FilterData.MyGroup = OBJECT_GROUP::DYNAMIC_OBJECT;
	ShapeData.FilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::DYNAMIC_OBJECT | OBJECT_GROUP::PLAYER | OBJECT_GROUP::MONSTER;
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
	BombModelDesc->pActorDesc = &ActorDesc;
	BombModelDesc->eActorType = ACTOR_TYPE::DYNAMIC;
	if (FAILED(__super::Initialize(BombModelDesc)))
		return E_FAIL;


    return S_OK;
}

void CBomb::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
}

void CBomb::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT CBomb::Render()
{
	return __super::Render();
}


CBomb* CBomb::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CBomb* pInstance = new CBomb(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Bomb");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBomb::Clone(void* _pArg)
{
	CBomb* pInstance = new CBomb(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : Bomb");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBomb::Free()
{
	__super::Free();
}
