#include "stdafx.h"
#include "Dice.h"
#include "Section_Manager.h"
#include "GameInstance.h"

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
	CARRIABLE_DESC* DiceModelDesc = static_cast<CARRIABLE_DESC*>(_pArg);
	DiceModelDesc->eStartCoord = COORDINATE_3D;
	DiceModelDesc->eCrriableObjId = CARRIABLE_OBJ_ID::DIEC;
	DiceModelDesc->vHeadUpRoolPitchYaw3D = { 0.f,0.f,0.f };
	DiceModelDesc->vHeadUpOffset3D = { 0.f,1.8f,0.f };
	DiceModelDesc->isCoordChangeEnable = true;
	DiceModelDesc->iModelPrototypeLevelID_3D = DiceModelDesc->iCurLevelID;
	DiceModelDesc->iModelPrototypeLevelID_2D = DiceModelDesc->iCurLevelID;
	DiceModelDesc->strModelPrototypeTag_2D = TEXT("dice_pink_03");
	DiceModelDesc->strModelPrototypeTag_3D = TEXT("dice_01");
	DiceModelDesc->strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
	DiceModelDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
	DiceModelDesc->tTransform2DDesc.vInitialPosition = _float3(0.0f, 1.0f, 0.f);
	DiceModelDesc->tTransform2DDesc.vInitialScaling = _float3(200.f, 200.f, 200.f);
	DiceModelDesc->iRenderGroupID_3D = RG_3D;
	DiceModelDesc->iPriorityID_3D = PR3D_GEOMETRY;
	//DiceModelDesc->tTransform3DDesc.vInitialPosition = _float3(0.0f, 1.0f, -10.f);
	//DiceModelDesc->tTransform3DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
	
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
	DiceModelDesc->pActorDesc = &ActorDesc;
	DiceModelDesc->eActorType = ACTOR_TYPE::DYNAMIC;
	if (FAILED(__super::Initialize(DiceModelDesc)))
		return E_FAIL;

	// 2D 충돌용 콜라이더 추가.
	m_p2DColliderComs.resize(2);

	CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};

	CircleDesc.pOwner = this;
	CircleDesc.fRadius = 0.2f;
	CircleDesc.vScale = { 1.0f, 1.0f };
	CircleDesc.vOffsetPosition = { 0.f, 0.f };
	CircleDesc.isBlock = false;
	CircleDesc.isTrigger = true;
	CircleDesc.iCollisionGroupID = OBJECT_GROUP::MAP_GIMMICK;

	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
		TEXT("Com_Body2DCollider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[1]), &CircleDesc)))
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

HRESULT CDice::Render()
{
//#ifdef _DEBUG
//	if (COORDINATE_2D == Get_CurCoord() && 2 <= m_p2DColliderComs.size() && nullptr != m_p2DColliderComs[1])
//		m_p2DColliderComs[1]->Render();
//#endif
	return __super::Render();
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
