#include "stdafx.h"
#include "DynamicCastleGate.h"
#include "ModelObject.h"
#include "Actor_Dynamic.h"
#include "GameInstance.h"

CDynamicCastleGate::CDynamicCastleGate(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CContainerObject(_pDevice, _pContext)
{
}

CDynamicCastleGate::CDynamicCastleGate(const CDynamicCastleGate& _Prototype)
	:CContainerObject(_Prototype)
{
}

HRESULT CDynamicCastleGate::Initialize(void* _pArg)
{
	CContainerObject::CONTAINEROBJ_DESC* pDesc = static_cast<CContainerObject::CONTAINEROBJ_DESC*>(_pArg);
	m_iCurLevelID = pDesc->iCurLevelID;
	pDesc->isCoordChangeEnable = false;
	pDesc->iObjectGroupID = OBJECT_GROUP::DYNAMIC_OBJECT;
	pDesc->iNumPartObjects = CASTL_PART_LAST;
	pDesc->isCoordChangeEnable = false;

	pDesc->eActorType = ACTOR_TYPE::KINEMATIC;
	CActor::ACTOR_DESC ActorDesc;
	pDesc->pActorDesc = &ActorDesc;
	ActorDesc.pOwner = this;
	ActorDesc.tFilterData.MyGroup = OBJECT_GROUP::DYNAMIC_OBJECT;
	ActorDesc.tFilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::BLOCKER | OBJECT_GROUP::PLAYER;

	XMStoreFloat4x4(&ActorDesc.ActorOffsetMatrix, XMMatrixTranslation(0.0f, 0.f, 0.f));
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
	ShapeData.FilterData.MyGroup = OBJECT_GROUP::DYNAMIC_OBJECT;
	ShapeData.FilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::DYNAMIC_OBJECT | OBJECT_GROUP::PLAYER;
	XMStoreFloat4x4(&ShapeData.LocalOffsetMatrix, XMMatrixTranslation(0.0f, ShapeDesc.vHalfExtents.y + 0.01f, 0.f));
	ActorDesc.ShapeDatas.push_back(ShapeData);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;
	if (FAILED(Ready_PartObjects()))
		return E_FAIL;
	static_cast<CActor_Dynamic*>(m_pActorCom)->Set_Rotation(_vector{ 0.f,0.f,1.f }, XMConvertToRadians(90.f));
	//static_cast<CActor_Dynamic*>(m_pActorCom)->Set_MassLocalPos({ 0.0f,0.5f,0.f });
	m_pActorCom->Set_Mass(50.f);
	return S_OK;
}

void CDynamicCastleGate::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT CDynamicCastleGate::Render()
{
	return __super::Render();
}

void CDynamicCastleGate::OnContact_Modify(const COLL_INFO& _0, const COLL_INFO& _1, CModifiableContacts& _ModifiableContacts, _bool _bIm0)
{
}

HRESULT CDynamicCastleGate::Ready_PartObjects()
{

	//VISOR
	CModelObject::MODELOBJECT_DESC tVisorDesc{};

	tVisorDesc.iCurLevelID = m_iCurLevelID;
	tVisorDesc.eStartCoord = COORDINATE_3D;
	tVisorDesc.isCoordChangeEnable = false;
	tVisorDesc.iModelPrototypeLevelID_3D = m_iCurLevelID;
	tVisorDesc.strModelPrototypeTag_3D = TEXT("Castle_Gate");
	tVisorDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
	tVisorDesc.iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;
	tVisorDesc.iRenderGroupID_3D = RG_3D;
	tVisorDesc.iPriorityID_3D = PR3D_GEOMETRY;
	tVisorDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);
	m_PartObjects[CASTL_PART_DOOR] = static_cast<CModelObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &tVisorDesc));
	if (nullptr == m_PartObjects[CASTL_PART_DOOR])
	{
		MSG_BOX("CASTL_PART_DOOR Creation Failed");
		return E_FAIL;
	}
	//m_PartObjects[CASTL_PART_DOOR]->Set_Position({ 0.f,0.225f,-0.225f });



	return S_OK;
}

CDynamicCastleGate* CDynamicCastleGate::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CDynamicCastleGate* pInstance = new CDynamicCastleGate(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : DynamicCastleGate");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDynamicCastleGate::Clone(void* _pArg)
{
	CDynamicCastleGate* pInstance = new CDynamicCastleGate(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : DynamicCastleGate");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDynamicCastleGate::Free()
{
	__super::Free();
}
