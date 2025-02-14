#include "stdafx.h"
#include "Domino.h"
#include "Actor_Dynamic.h"

CDomino::CDomino(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CModelObject(_pDevice, _pContext)
{
}


CDomino::CDomino(const CDomino& _Prototype)
	: CModelObject(_Prototype)
{
}

HRESULT CDomino::Initialize(void* _pArg)
{

	CModelObject::MODELOBJECT_DESC* pModelDsc = static_cast<CModelObject::MODELOBJECT_DESC*>(_pArg);
	pModelDsc->isCoordChangeEnable = false;
	pModelDsc->strModelPrototypeTag_3D = TEXT("Prototype_Component_Domino");
	pModelDsc->strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
	//pModelDsc->tTransform3DDesc.vInitialPosition = _float3(3.0f, 0.651f, -10.f);
	//pModelDsc->tTransform3DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);

	CActor::ACTOR_DESC ActorDesc;
	ActorDesc.pOwner = this;
	XMStoreFloat4x4(&ActorDesc.ActorOffsetMatrix, XMMatrixTranslation(0.0f, 3.f, 0.f));
	ActorDesc.FreezeRotation_XYZ[0] = false;
	ActorDesc.FreezeRotation_XYZ[1] = false;
	ActorDesc.FreezeRotation_XYZ[2] = false;
	ActorDesc.FreezePosition_XYZ[0] = false;
	ActorDesc.FreezePosition_XYZ[1] = false;
	ActorDesc.FreezePosition_XYZ[2] = false;

	SHAPE_BOX_DESC ShapeDesc = {};
	ShapeDesc.vHalfExtents = { 0.125f,1.65f ,0.7f };
	SHAPE_DATA ShapeData;
	ShapeData.pShapeDesc = &ShapeDesc;
	ShapeData.eShapeType = SHAPE_TYPE::BOX;
	ShapeData.eMaterial = ACTOR_MATERIAL::DEFAULT;
	ShapeData.isTrigger = false;
	XMStoreFloat4x4(&ShapeData.LocalOffsetMatrix, XMMatrixTranslation(0.0f, ShapeDesc.vHalfExtents.y + 0.01f, 0.f));
	ActorDesc.ShapeDatas.push_back(ShapeData);

	SHAPE_BOX_DESC ShapeDesc2 = {};
	ShapeDesc2.vHalfExtents = { 0.3f,0.11f ,0.7f };
	ShapeData.pShapeDesc = &ShapeDesc2;
	XMStoreFloat4x4(&ShapeData.LocalOffsetMatrix, XMMatrixTranslation(0.15f, ShapeDesc2.vHalfExtents.y, 0.f));
	ActorDesc.ShapeDatas.push_back(ShapeData);

	ActorDesc.tFilterData.MyGroup = OBJECT_GROUP::INTERACTION_OBEJCT;
	ActorDesc.tFilterData.OtherGroupMask = OBJECT_GROUP::INTERACTION_OBEJCT | OBJECT_GROUP::PLAYER | OBJECT_GROUP::MAPOBJECT;
	pModelDsc->pActorDesc = &ActorDesc;
	pModelDsc->eActorType = ACTOR_TYPE::DYNAMIC;
	if (FAILED(__super::Initialize(pModelDsc)))
		return E_FAIL;

	static_cast<CActor_Dynamic*>(m_pActorCom)->Set_MassLocalPos({ 0.f,0.3f,0.f });
	m_pActorCom->Set_Mass(23.f);
    return S_OK;
}

CDomino* CDomino::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CDomino* pInstance = new CDomino(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Domino");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDomino::Clone(void* _pArg)
{
	CDomino* pInstance = new CDomino(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : Domino");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDomino::Free()
{
	__super::Free();
}
