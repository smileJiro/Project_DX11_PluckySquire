#include "stdafx.h"
#include "RabbitLunch.h"
#include "GameInstance.h"

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
	RABBITLUNCH_DESC* pModelDesc = static_cast<RABBITLUNCH_DESC*>(_pArg);
	m_eLunchType = pModelDesc->eLunchType;

	pModelDesc->vHeadUpRoolPitchYaw3D = { 0.f,0.f,XMConvertToRadians(90.f) };

	pModelDesc->isCoordChangeEnable = true;
	pModelDesc->iModelPrototypeLevelID_3D = pModelDesc->iCurLevelID;
	pModelDesc->iModelPrototypeLevelID_2D = pModelDesc->iCurLevelID;

	CActor::ACTOR_DESC ActorDesc;
	ActorDesc.pOwner = this;
	ActorDesc.FreezeRotation_XYZ[0] = false;
	ActorDesc.FreezeRotation_XYZ[1] = false;
	ActorDesc.FreezeRotation_XYZ[2] = false;
	ActorDesc.FreezePosition_XYZ[0] = false;
	ActorDesc.FreezePosition_XYZ[1] = false;
	ActorDesc.FreezePosition_XYZ[2] = false;
	switch (pModelDesc->eCrriableObjId)
	{
	case Client::CCarriableObject::CARROT:
	{
		pModelDesc->strModelPrototypeTag_2D = TEXT("sketchspace_rabbit_carrot");
		pModelDesc->tTransform2DDesc.vInitialScaling = _float3(420.f, 156.f, 1.f);
		pModelDesc->vHeadUpOffset3D = { 0.9f,1.5f,0.f };

		SHAPE_CAPSULE_DESC ShapeDesc = {};
		ShapeDesc.fRadius = 0.35f;
		ShapeDesc.fHalfHeight = 0.4f;
		SHAPE_DATA ShapeData;
		ShapeData.pShapeDesc = &ShapeDesc;
		ShapeData.eShapeType = SHAPE_TYPE::CAPSULE;
		ShapeData.eMaterial = ACTOR_MATERIAL::DEFAULT;
		ShapeData.isTrigger = false;
		ShapeData.FilterData.MyGroup = OBJECT_GROUP::DYNAMIC_OBJECT;
		ShapeData.FilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::DYNAMIC_OBJECT | OBJECT_GROUP::PLAYER;
		XMStoreFloat4x4(&ShapeData.LocalOffsetMatrix, XMMatrixRotationZ(XMConvertToRadians(90.f)) * XMMatrixTranslation(0.0f, 1.f, 0.0f)); // Shape의 LocalOffset을 행렬정보로 저장.
		ActorDesc.ShapeDatas.push_back(ShapeData);
		break;
	}
	case Client::CCarriableObject::GRAPE:
	{
		pModelDesc->tTransform2DDesc.vInitialScaling = _float3(512.f, 512.f, 1.f);
		pModelDesc->strModelPrototypeTag_2D = TEXT("Grape_Green");
		pModelDesc->vHeadUpOffset3D = { 0.f,1.5f,0.f };

		SHAPE_SPHERE_DESC ShapeDesc = {};
		ShapeDesc.fRadius = 0.4f;
		SHAPE_DATA ShapeData;
		ShapeData.pShapeDesc = &ShapeDesc;
		ShapeData.eShapeType = SHAPE_TYPE::SPHERE;
		ShapeData.eMaterial = ACTOR_MATERIAL::DEFAULT;
		ShapeData.isTrigger = false;
		ShapeData.FilterData.MyGroup = OBJECT_GROUP::DYNAMIC_OBJECT;
		ShapeData.FilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::DYNAMIC_OBJECT | OBJECT_GROUP::PLAYER;
		ActorDesc.ShapeDatas.push_back(ShapeData);
		break;
	}
	default:
		break;
	}
	switch (m_eLunchType)
	{
	case Client::CRabbitLunch::CARROT_1:
		pModelDesc->strModelPrototypeTag_3D = TEXT("Carrots_Carrot_01");
		break;
	case Client::CRabbitLunch::CARROT_2:
		pModelDesc->strModelPrototypeTag_3D = TEXT("Carrots_Carrot_02");
		break;
	case Client::CRabbitLunch::CARROT_3:
		pModelDesc->strModelPrototypeTag_3D = TEXT("Carrots_Carrot_03");
		break;
	case Client::CRabbitLunch::GRAPE_1:
		pModelDesc->strModelPrototypeTag_3D = TEXT("Grapes_Grape_01");
		break;
	case Client::CRabbitLunch::GRAPE_2:
		pModelDesc->strModelPrototypeTag_3D = TEXT("Grapes_Grape_02");
		break;
	case Client::CRabbitLunch::GRAPE_3:
		pModelDesc->strModelPrototypeTag_3D = TEXT("Grapes_Grape_03");
		break;
	case Client::CRabbitLunch::LUNCH_TYPE_LAST:
	default:
		break;
	}

	pModelDesc->strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
	pModelDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
	pModelDesc->tTransform2DDesc.vInitialPosition = _float3(0.0f, 1.0f, 0.f);

	pModelDesc->iRenderGroupID_3D = RG_3D;
	pModelDesc->iPriorityID_3D = PR3D_GEOMETRY;
	pModelDesc->iObjectGroupID = OBJECT_GROUP::INTERACTION_OBEJCT;




	switch (m_eLunchType)
	{
	case Client::CRabbitLunch::CARROT_1:
	case Client::CRabbitLunch::CARROT_2:
	case Client::CRabbitLunch::CARROT_3:
	{


		break;
	}
	case Client::CRabbitLunch::GRAPE_1:
	case Client::CRabbitLunch::GRAPE_2:
	case Client::CRabbitLunch::GRAPE_3:
	{

		break;
	}
	case Client::CRabbitLunch::LUNCH_TYPE_LAST:
		break;
	default:
		break;
	}


	SHAPE_SPHERE_DESC ShapeDesc2 = {};
	ShapeDesc2.fRadius = 0.1f;
	SHAPE_DATA ShapeData2;
	ShapeData2.pShapeDesc = &ShapeDesc2;
	ShapeData2.eShapeType = SHAPE_TYPE::SPHERE;
	ShapeData2.eMaterial = ACTOR_MATERIAL::DEFAULT;
	ShapeData2.isTrigger = false;
	ShapeData2.FilterData.MyGroup = OBJECT_GROUP::INTERACTION_OBEJCT;
	ShapeData2.FilterData.OtherGroupMask = OBJECT_GROUP::PLAYER_TRIGGER;
	if (CARROT_1 == m_eLunchType
		|| CARROT_2 == m_eLunchType
		|| CARROT_3 == m_eLunchType)
		XMStoreFloat4x4(&ShapeData2.LocalOffsetMatrix, XMMatrixTranslation(0.0f, 0.9f, 0.f));
	else
		XMStoreFloat4x4(&ShapeData2.LocalOffsetMatrix, XMMatrixTranslation(0.0f, 0.f, 0.f));
	ActorDesc.ShapeDatas.push_back(ShapeData2);

	ActorDesc.tFilterData.MyGroup = OBJECT_GROUP::DYNAMIC_OBJECT;
	ActorDesc.tFilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::BLOCKER | OBJECT_GROUP::PLAYER| OBJECT_GROUP::PLAYER_TRIGGER;
	pModelDesc->pActorDesc = &ActorDesc;
	pModelDesc->eActorType = ACTOR_TYPE::DYNAMIC;

	if (FAILED(__super::Initialize(pModelDesc)))
		return E_FAIL;
	if (CARROT_1 == m_eLunchType
		|| CARROT_2 == m_eLunchType
		|| CARROT_3 == m_eLunchType)
		m_pActorCom->Set_MassLocalPos({0.f,0.9f,0.f});


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

_bool CRabbitLunch::Is_Carrot()
{
	return CARROT_1 == m_eLunchType
		|| CARROT_2 == m_eLunchType
		|| CARROT_3 == m_eLunchType;
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
