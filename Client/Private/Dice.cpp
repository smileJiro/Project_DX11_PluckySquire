#include "stdafx.h"
#include "Dice.h"

CDice::CDice(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCarriableObject(_pDevice, _pContext)
{
}


CDice::CDice(const CDice& _Prototype)
	:CCarriableObject(_Prototype)
{
}

HRESULT CDice::Initialize(void* _pArg)
{
	CModelObject::MODELOBJECT_DESC* DiceModelDesc = static_cast<CModelObject::MODELOBJECT_DESC*>(_pArg);
	DiceModelDesc->isCoordChangeEnable = true;
	DiceModelDesc->strModelPrototypeTag_3D = TEXT("Prototype_Component_Dice3D");
	DiceModelDesc->strModelPrototypeTag_2D = TEXT("Prototype_Component_Dice2D");
	DiceModelDesc->strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
	DiceModelDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
	DiceModelDesc->tTransform2DDesc.vInitialPosition = _float3(0.0f, 1.0f, 0.f);
	DiceModelDesc->tTransform2DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
	DiceModelDesc->tTransform3DDesc.vInitialPosition = _float3(0.0f, 1.0f, -10.f);
	DiceModelDesc->tTransform3DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
	
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
	XMStoreFloat4x4(&ShapeData.LocalOffsetMatrix, XMMatrixTranslation(0.0f, 0.f, 0.f));
	ActorDesc.ShapeDatas.push_back(ShapeData);
	ActorDesc.tFilterData.MyGroup = OBJECT_GROUP::INTERACTION_OBEJCT;
	ActorDesc.tFilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::PLAYER| OBJECT_GROUP::INTERACTION_OBEJCT;
	DiceModelDesc->pActorDesc = &ActorDesc;
	DiceModelDesc->eActorType = ACTOR_TYPE::DYNAMIC;
	if (FAILED(__super::Initialize(DiceModelDesc)))
		return E_FAIL;



    return S_OK;
}

void CDice::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
}

void CDice::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

CDice* CDice::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CDice* pInstance = new CDice(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Dice");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDice::Clone(void* _pArg)
{
	CDice* pInstance = new CDice(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : Dice");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDice::Free()
{
	__super::Free();
}
