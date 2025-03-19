#include "stdafx.h"
#include "CrossBow_Arrow.h"
#include "ModelObject.h"
#include "Pooling_Manager.h"
#include "GameInstance.h"
#include "Section_Manager.h"

CCrossBow_Arrow::CCrossBow_Arrow(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CProjectile_Monster(_pDevice, _pContext)
{
}

CCrossBow_Arrow::CCrossBow_Arrow(const CCrossBow_Arrow& _Prototype)
	: CProjectile_Monster(_Prototype)
{
}

HRESULT CCrossBow_Arrow::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCrossBow_Arrow::Initialize(void* _pArg)
{
    CROSSBOW_ARROW_DESC* pDesc = static_cast<CROSSBOW_ARROW_DESC*>(_pArg);

    //투사체는 쓰는 객체가 Desc 넣어줌.
    pDesc->isCoordChangeEnable = false;

    //pDesc->tTransform2DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    //pDesc->tTransform2DDesc.fSpeedPerSec = 400.f;

    pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    pDesc->tTransform3DDesc.fSpeedPerSec = 15.f;

    pDesc->fLifeTime = 3.f;


    if (FAILED(Ready_ActorDesc(pDesc)))
        return E_FAIL;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    /* Actor Desc 채울 때 쓴 데이터 할당해제 */

    for (_uint i = 0; i < pDesc->pActorDesc->ShapeDatas.size(); i++)
    {
        Safe_Delete(pDesc->pActorDesc->ShapeDatas[i].pShapeDesc);
    }

    Safe_Delete(pDesc->pActorDesc);

	return S_OK;
}

void CCrossBow_Arrow::Priority_Update(_float _fTimeDelta)
{
    __super::Priority_Update(_fTimeDelta);
}

void CCrossBow_Arrow::Update(_float _fTimeDelta)
{
	if (COORDINATE_2D == Get_CurCoord())
    {
        if (false == m_isStop)
            m_pControllerTransform->Go_Up(_fTimeDelta);
    }

    else if (COORDINATE_3D == Get_CurCoord())
    {
        //if (nullptr != m_pTarget)
        //{
        //    if (true == m_isFirstLoop)
        //    {
        //        XMStoreFloat3(&m_vDir, m_pTarget->Get_FinalPosition() - Get_FinalPosition());
        //    }

        //    m_pControllerTransform->Go_Direction(XMVector3Normalize(XMLoadFloat3(&m_vDir)), m_pControllerTransform->Get_SpeedPerSec(), _fTimeDelta);
        //}
        //else
            m_pControllerTransform->Go_Straight(_fTimeDelta);
    }

    __super::Update(_fTimeDelta);
}

void CCrossBow_Arrow::Late_Update(_float _fTimeDelta)
{

	__super::Late_Update(_fTimeDelta);
}

HRESULT CCrossBow_Arrow::Render()
{
#ifdef _DEBUG

#endif // _DEBUG

    __super::Render();
    return S_OK;
}

HRESULT CCrossBow_Arrow::Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition)
{
    if (FAILED(__super::Change_Coordinate(_eCoordinate, _pNewPosition)))
        return E_FAIL;

    if (COORDINATE_2D == _eCoordinate)
    {
        //static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_Animation(PROJECTILE);
    }

    return S_OK;
}

void CCrossBow_Arrow::On_Hit(CGameObject* _pHitter, _float _fDamg, _fvector _vForce)
{

}

void CCrossBow_Arrow::OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)
{
    if (OBJECT_GROUP::PLAYER & _Other.pActorUserData->iObjectGroup)
    {
        if((_uint)SHAPE_USE::SHAPE_BODY == _Other.pShapeUserData->iShapeUse)
        {
            __super::OnTrigger_Enter(_My, _Other);

            m_pGameInstance->Start_SFX(_wstring(L"A_sfx_crossbow_trooper_arrow_destroyed-") + to_wstring(rand() % 4), 50.f);

        }

    }

    //if (OBJECT_GROUP::MAPOBJECT & _Other.pActorUserData->iObjectGroup)
    //    Event_DeleteObject(this);
}

void CCrossBow_Arrow::OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)
{
}

void CCrossBow_Arrow::OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)
{
}

void CCrossBow_Arrow::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    if (OBJECT_GROUP::PLAYER & _pOtherObject->Get_ObjectGroupID() || OBJECT_GROUP::MAPOBJECT & _pOtherObject->Get_ObjectGroupID() || OBJECT_GROUP::BLOCKER & _pOtherObject->Get_ObjectGroupID())
    {
        __super::On_Collision2D_Enter(_pMyCollider, _pOtherCollider, _pOtherObject);
    }
}

void CCrossBow_Arrow::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CCrossBow_Arrow::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CCrossBow_Arrow::Active_OnEnable()
{
    __super::Active_OnEnable();

	//if (COORDINATE_3D == Get_CurCoord())
 //       m_pActorCom->Set_ShapeEnable((_int)SHAPE_USE::SHAPE_BODY, true);
}

void CCrossBow_Arrow::Active_OnDisable()
{
    //if (COORDINATE_3D == Get_CurCoord())
	   // m_pActorCom->Set_ShapeEnable((_int)SHAPE_USE::SHAPE_BODY, false);
    __super::Active_OnDisable();
}

HRESULT CCrossBow_Arrow::Ready_ActorDesc(void* _pArg)
{
    CCrossBow_Arrow::CROSSBOW_ARROW_DESC* pDesc = static_cast<CCrossBow_Arrow::CROSSBOW_ARROW_DESC*>(_pArg);

    pDesc->eActorType = ACTOR_TYPE::KINEMATIC;
    CActor::ACTOR_DESC* ActorDesc = new CActor::ACTOR_DESC;

    /* Actor의 주인 오브젝트 포인터 */
    ActorDesc->pOwner = this;

    /* Actor의 회전축을 고정하는 파라미터 */
    ActorDesc->FreezeRotation_XYZ[0] = true;
    ActorDesc->FreezeRotation_XYZ[1] = false;
    ActorDesc->FreezeRotation_XYZ[2] = true;

    /* Actor의 이동축을 고정하는 파라미터 (이걸 고정하면 중력도 영향을 받지 않음. 아예 해당 축으로의 이동을 제한하는)*/
    ActorDesc->FreezePosition_XYZ[0] = false;
    ActorDesc->FreezePosition_XYZ[1] = false;
    ActorDesc->FreezePosition_XYZ[2] = false;

    /* 사용하려는 Shape의 형태를 정의 */
    SHAPE_CAPSULE_DESC* ShapeDesc = new SHAPE_CAPSULE_DESC;
    ShapeDesc->fHalfHeight = 0.3f;
    ShapeDesc->fRadius = 0.1f;

    /* 해당 Shape의 Flag에 대한 Data 정의 */
    SHAPE_DATA* ShapeData = new SHAPE_DATA;
    ShapeData->pShapeDesc = ShapeDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
    ShapeData->eShapeType = SHAPE_TYPE::CAPSULE;     // Shape의 형태.
    ShapeData->eMaterial = ACTOR_MATERIAL::DEFAULT; // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
    ShapeData->isTrigger = true;                    // Trigger 알림을 받기위한 용도라면 true
    ShapeData->iShapeUse = (_uint)SHAPE_USE::SHAPE_BODY;
    XMStoreFloat4x4(&ShapeData->LocalOffsetMatrix, XMMatrixRotationY(XMConvertToRadians(90.f)) * XMMatrixTranslation(0.0f, 0.f, 0.0f)); // Shape의 LocalOffset을 행렬정보로 저장.

    /* 최종으로 결정 된 ShapeData를 PushBack */
    ActorDesc->ShapeDatas.push_back(*ShapeData);

    /* 충돌 필터에 대한 세팅 ()*/
    ActorDesc->tFilterData.MyGroup = OBJECT_GROUP::MONSTER_PROJECTILE;
    ActorDesc->tFilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::PLAYER;

    /* Actor Component Finished */
    pDesc->pActorDesc = ActorDesc;

    /* Shapedata 할당해제 */
    Safe_Delete(ShapeData);

    return S_OK;
}

HRESULT CCrossBow_Arrow::Ready_Components()
{
    /* 2D Collider */
    //m_p2DColliderComs.resize(1);

    //CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};
    //CircleDesc.pOwner = this;
    //CircleDesc.fRadius = 30.f;
    //CircleDesc.vScale = { 1.0f, 1.0f };
    //CircleDesc.vOffsetPosition = { 0.f, 0.f };
    //CircleDesc.isBlock = false;
    //CircleDesc.iCollisionGroupID = OBJECT_GROUP::MONSTER_PROJECTILE;
    //if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
    //    TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &CircleDesc)))
    //    return E_FAIL;

    return S_OK;
}

HRESULT CCrossBow_Arrow::Ready_PartObjects()
{
    CModelObject::MODELOBJECT_DESC BodyDesc{};

    BodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
    BodyDesc.iCurLevelID = m_iCurLevelID;
    BodyDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

    //BodyDesc.strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
    BodyDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
    //BodyDesc.strModelPrototypeTag_2D = TEXT("barferbug_projectile");
    BodyDesc.strModelPrototypeTag_3D = TEXT("Crossbow_Arrow");
    //BodyDesc.iModelPrototypeLevelID_2D = m_iCurLevelID;
    BodyDesc.iModelPrototypeLevelID_3D = LEVEL_STATIC;
    //BodyDesc.iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;
    BodyDesc.iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;

    //BodyDesc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
    BodyDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);

    BodyDesc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    BodyDesc.tTransform3DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
    BodyDesc.tTransform3DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    BodyDesc.tTransform3DDesc.fSpeedPerSec = 12.f;

    //BodyDesc.tTransform2DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    //BodyDesc.tTransform2DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
    //BodyDesc.tTransform2DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    //BodyDesc.tTransform2DDesc.fSpeedPerSec = 10.f;

    BodyDesc.iRenderGroupID_3D = RG_3D;
    BodyDesc.iPriorityID_3D = PR3D_GEOMETRY;

    m_PartObjects[PART_BODY] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &BodyDesc));
    if (nullptr == m_PartObjects[PART_BODY])
        return E_FAIL;

    return S_OK;
}

CCrossBow_Arrow* CCrossBow_Arrow::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CCrossBow_Arrow* pInstance = new CCrossBow_Arrow(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CCrossBow_Arrow");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCrossBow_Arrow::Clone(void* _pArg)
{
    CCrossBow_Arrow* pInstance = new CCrossBow_Arrow(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CCrossBow_Arrow");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCrossBow_Arrow::Free()
{

    __super::Free();
}
