#include "stdafx.h"
#include "NPC_Store.h"
#include "ModelObject.h"
#include "GameInstance.h"

CNPC_Store::CNPC_Store(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CNPC(_pDevice, _pContext)
{
}

CNPC_Store::CNPC_Store(const CNPC_Store& _Prototype)
	:CNPC(_Prototype)
{
}

HRESULT CNPC_Store::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNPC_Store::Initialize(void* _pArg)
{
	NPC_DESC* pDesc = static_cast<NPC_DESC*>(_pArg);
	pDesc->eStartCoord = COORDINATE_2D;
	pDesc->isCoordChangeEnable = false;
	pDesc->iNumPartObjects = PART_END;
	pDesc->tTransform2DDesc.fRotationPerSec = XMConvertToRadians(90.f);
	pDesc->tTransform2DDesc.fSpeedPerSec = 3.f;
	pDesc->eActorType = ACTOR_TYPE::DYNAMIC;

	//CActor::ACTOR_DESC ActorDesc;
	//
	///* Actor의 주인 오브젝트 포인터 */
	//ActorDesc.pOwner = this;
	//
	///* Actor의 회전축을 고정하는 파라미터 */
	//ActorDesc.FreezeRotation_XYZ[0] = false;
	//ActorDesc.FreezeRotation_XYZ[1] = false;
	//ActorDesc.FreezeRotation_XYZ[2] = false;
	//
	///* Actor의 이동축을 고정하는 파라미터 (이걸 고정하면 중력도 영향을 받지 않음. 아예 해당 축으로의 이동을 제한하는)*/
	//ActorDesc.FreezePosition_XYZ[0] = false;
	//ActorDesc.FreezePosition_XYZ[1] = false;
	//ActorDesc.FreezePosition_XYZ[2] = false;
	//
	///* 사용하려는 Shape의 형태를 정의 */
	//SHAPE_CAPSULE_DESC ShapeDesc = {};
	//ShapeDesc.fHalfHeight = 0.25f;
	//ShapeDesc.fRadius = 0.25f;
	//
	///* 해당 Shape의 Flag에 대한 Data 정의 */
	//SHAPE_DATA ShapeData;
	//ShapeData.pShapeDesc = &ShapeDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
	//ShapeData.eShapeType = SHAPE_TYPE::CAPSULE;     // Shape의 형태.
	//ShapeData.eMaterial = ACTOR_MATERIAL::DEFAULT; // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
	//ShapeData.isTrigger = false;                    // Trigger 알림을 받기위한 용도라면 true
	//XMStoreFloat4x4(&ShapeData.LocalOffsetMatrix, XMMatrixRotationZ(XMConvertToRadians(90.f)) * XMMatrixTranslation(0.0f, 0.5f, 0.0f)); // Shape의 LocalOffset을 행렬정보로 저장.
	//
	///* 최종으로 결정 된 ShapeData를 PushBack */
	//ActorDesc.ShapeDatas.push_back(ShapeData);
	//
	///* 만약, Shape을 여러개 사용하고싶다면, 아래와 같이 별도의 Shape에 대한 정보를 추가하여 push_back() */
	//ShapeData.eShapeType = SHAPE_TYPE::SPHERE;
	//ShapeData.isTrigger = true;                     // Trigger로 사용하겠다.
	//XMStoreFloat4x4(&ShapeData.LocalOffsetMatrix, XMMatrixTranslation(0, 0.5, 0)); //여기임
	//SHAPE_SPHERE_DESC SphereDesc = {};
	//SphereDesc.fRadius = 1.f;
	//ShapeData.pShapeDesc = &SphereDesc;
	//ActorDesc.ShapeDatas.push_back(ShapeData);
	//
	///* 충돌 필터에 대한 세팅 ()*/
	//ActorDesc.tFilterData.MyGroup = OBJECT_GROUP::PLAYER;
	//ActorDesc.tFilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::MONSTER | OBJECT_GROUP::INTERACTION_OBEJCT | OBJECT_GROUP::MONSTER_PROJECTILE;
	//
	//pDesc->pActorDesc = &ActorDesc;
	//
	//
	//__super::Initialize(pDesc);
	//Ready_PartObjects();



	return S_OK;
}

void CNPC_Store::Priority_Update(_float _fTimeDelta)
{
}

void CNPC_Store::Update(_float _fTimeDelta)
{
}

void CNPC_Store::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT CNPC_Store::Render()
{
	return S_OK;
}

void CNPC_Store::OnContact_Enter(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
}

void CNPC_Store::OnContact_Stay(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
}

void CNPC_Store::OnContact_Exit(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
}

HRESULT CNPC_Store::Ready_ActorDesc(void* _pArg)
{
	return S_OK;
}

HRESULT CNPC_Store::Ready_Components()
{
	return S_OK;
}

HRESULT CNPC_Store::Ready_PartObjects()
{

	CModelObject::MODELOBJECT_DESC pDesc {};

	pDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
	pDesc.iCurLevelID = m_iCurLevelID;
	pDesc.isCoordChangeEnable = false;
	pDesc.iModelPrototypeLevelID_2D = m_iCurLevelID;
	pDesc.strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
	pDesc.strModelPrototypeTag_2D = TEXT("NPC_Shop");
	
	//pDesc.strModelPrototypeTag_2D = TEXT("NPC_Store");
	pDesc.iModelPrototypeLevelID_2D = m_iCurLevelID;
	pDesc.iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE_ANIM;
	pDesc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
	
	pDesc.tTransform2DDesc.vInitialPosition = _float3(0.f, 0.f, 0.f);
	pDesc.tTransform2DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
	pDesc.tTransform2DDesc.fRotationPerSec = XMConvertToRadians(90.f);
	pDesc.tTransform2DDesc.fSpeedPerSec = 10.f;


	m_PartObjects[PART_BODY] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &pDesc));

	if (nullptr == m_PartObjects[PART_BODY])
		return E_FAIL;

	return S_OK;

}

CNPC_Store* CNPC_Store::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CNPC_Store* pInstance = new CNPC_Store(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CPick_Bulb Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CNPC_Store::Clone(void* _pArg)
{
	CNPC_Store* pInstance = new CNPC_Store(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CNPC_Store Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CNPC_Store::Free()
{
	__super::Free();
}

HRESULT CNPC_Store::Cleanup_DeadReferences()
{
	return S_OK;
}
