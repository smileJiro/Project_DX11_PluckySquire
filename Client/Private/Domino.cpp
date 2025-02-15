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
	pModelDsc->strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
	pModelDsc->iObjectGroupID = OBJECT_GROUP::MAPOBJECT;

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
	ShapeDesc.vHalfExtents = { 0.18f,2.45f ,1.15f };
	SHAPE_DATA ShapeData;
	ShapeData.pShapeDesc = &ShapeDesc;
	ShapeData.eShapeType = SHAPE_TYPE::BOX;
	ShapeData.eMaterial = ACTOR_MATERIAL::DOMINO;
	ShapeData.isTrigger = false;
	XMStoreFloat4x4(&ShapeData.LocalOffsetMatrix, XMMatrixTranslation(0.0f, ShapeDesc.vHalfExtents.y + 0.01f, 0.f));
	ActorDesc.ShapeDatas.push_back(ShapeData);

	SHAPE_BOX_DESC ShapeDesc2 = {};
	ShapeDesc2.vHalfExtents = { 0.43f,0.12f ,1.15f };
	ShapeData.pShapeDesc = &ShapeDesc2;
	XMStoreFloat4x4(&ShapeData.LocalOffsetMatrix, XMMatrixTranslation(0.24f, ShapeDesc2.vHalfExtents.y + 0.01, 0.f));
	ActorDesc.ShapeDatas.push_back(ShapeData);

	ActorDesc.tFilterData.MyGroup = OBJECT_GROUP::BLOCKER;
	ActorDesc.tFilterData.OtherGroupMask = OBJECT_GROUP::BLOCKER | OBJECT_GROUP::PLAYER | OBJECT_GROUP::MAPOBJECT;
	pModelDsc->pActorDesc = &ActorDesc;
	pModelDsc->eActorType = ACTOR_TYPE::DYNAMIC;
	if (FAILED(__super::Initialize(pModelDsc)))
		return E_FAIL;

	static_cast<CActor_Dynamic*>(m_pActorCom)->Set_Rotation(_vector{0,1,0},XMConvertToRadians(180));
	static_cast<CActor_Dynamic*>(m_pActorCom)->Set_MassLocalPos({ 0.0f,0.5f,0.f });
	m_pActorCom->Set_Mass(50.f);
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
