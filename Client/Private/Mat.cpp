#include "stdafx.h"
#include "Mat.h"
#include "GameInstance.h"

CMat::CMat(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CModelObject(_pDevice, _pContext)
{
}

CMat::CMat(const CMat& _Prototype)
	:CModelObject(_Prototype)
{
}

HRESULT CMat::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMat::Initialize(void* _pArg)
{
	CMat::MODELOBJECT_DESC* pDesc = static_cast<CMat::MODELOBJECT_DESC*>(_pArg);

	pDesc->Build_3D_Model(LEVEL_CHAPTER_6, TEXT("Mat"), TEXT("Prototype_Component_Shader_VtxMesh"), (_uint)PASS_VTXMESH::DEFAULT, false);
	pDesc->eStartCoord = COORDINATE_3D;
	pDesc->fFrustumCullingRange = 3.0f;
	pDesc->iObjectGroupID = MAPOBJECT;
	pDesc->iPriorityID_3D = PR3D_GEOMETRY;
	pDesc->iRenderGroupID_3D = RG_3D;

	/* 3D Actor */
	pDesc->eActorType = ACTOR_TYPE::DYNAMIC;
	CActor::ACTOR_DESC ActorDesc;
	ActorDesc.pOwner = this;
	ActorDesc.tFilterData.MyGroup = OBJECT_GROUP::DYNAMIC_OBJECT;
	ActorDesc.tFilterData.OtherGroupMask = OBJECT_GROUP::PLAYER | OBJECT_GROUP::MAPOBJECT;
	ActorDesc.FreezeRotation_XYZ[0] = false;
	ActorDesc.FreezeRotation_XYZ[1] = false;
	ActorDesc.FreezeRotation_XYZ[2] = false;
	
	SHAPE_BOX_DESC BoxDesc{};
	BoxDesc.vHalfExtents = { 1.5f, 0.1f, 1.5f };

	SHAPE_DATA ShapeData = {};
	ShapeData.pShapeDesc = &BoxDesc;
	ShapeData.eShapeType = SHAPE_TYPE::BOX;
	ShapeData.iShapeUse = (_uint)SHAPE_USE::SHAPE_BODY;
	ShapeData.isTrigger = false;
	XMStoreFloat4x4(&ShapeData.LocalOffsetMatrix, XMMatrixTranslation(0.0f, 0.0f, 0.0f));
	ActorDesc.ShapeDatas.push_back(ShapeData);
	pDesc->pActorDesc = &ActorDesc;

	
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;


    return S_OK;
}

void CMat::Late_Update(_float _fTimeDelta)
{

	m_pControllerModel->Binding_TextureIndex_To_3D(1, aiTextureType_DIFFUSE, 0);

	__super::Late_Update(_fTimeDelta);
}

CMat* CMat::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CMat* pInstance = new CMat(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed Create : CMat");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMat::Clone(void* _pArg)
{
	CMat* pInstance = new CMat(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed Clone : CMat");
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CMat::Free()
{

	__super::Free();
}
