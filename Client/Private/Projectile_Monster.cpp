#include "stdafx.h"
#include "Projectile_Monster.h"
#include "ModelObject.h"
#include "Pooling_Manager.h"
#include "GameInstance.h"
#include "Section_Manager.h"

CProjectile_Monster::CProjectile_Monster(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCharacter(_pDevice, _pContext)
{
}

CProjectile_Monster::CProjectile_Monster(const CProjectile_Monster& _Prototype)
	: CCharacter(_Prototype)
{
}

HRESULT CProjectile_Monster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CProjectile_Monster::Initialize(void* _pArg)
{
    PROJECTILE_MONSTER_DESC* pDesc = static_cast<PROJECTILE_MONSTER_DESC*>(_pArg);
    pDesc->iNumPartObjects = PART_END;
    pDesc->iObjectGroupID = OBJECT_GROUP::MONSTER_PROJECTILE;
    pDesc->_isIgnoreGround = true;

    m_fLifeTime = pDesc->fLifeTime;


   /* if (FAILED(Ready_ActorDesc(pDesc)))
        return E_FAIL;*/

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    //if (FAILED(Ready_Components()))
    //    return E_FAIL;

    //if (FAILED(Ready_PartObjects()))
    //    return E_FAIL;


    //플레이어 위치 가져오기
    m_pTarget = m_pGameInstance->Get_GameObject_Ptr(m_iCurLevelID, TEXT("Layer_Player"), 0);
    if (nullptr == m_pTarget)
    {
#ifdef _DEBUG
        cout << "MONSTERI_PROJ_INIT : NO PLAYER" << endl;
#endif // _DEBUG
        return S_OK;
    }

    Safe_AddRef(m_pTarget);

	return S_OK;
}

void CProjectile_Monster::Priority_Update(_float _fTimeDelta)
{
    m_fAccTime += _fTimeDelta;

    __super::Priority_Update(_fTimeDelta);
}

void CProjectile_Monster::Update(_float _fTimeDelta)
{
	if (false == Is_Dead() && m_fLifeTime <= m_fAccTime)
    {
        m_fAccTime = 0.f;
        m_isTimeOver = true;
        Event_DeleteObject(this);
    }

    __super::Update(_fTimeDelta);
}

void CProjectile_Monster::Late_Update(_float _fTimeDelta)
{
    //첫 루프가 끝나므로 (자식 객체들이 이미 전부 lateupdate까지 돈 상태)
    if (true == m_isFirstLoop)
        m_isFirstLoop = false;
#ifdef _DEBUG
    if (COORDINATE_3D == Get_CurCoord())
        m_pGameInstance->Add_RenderObject_New(RENDERGROUP::RG_3D, PRIORITY_3D::PR3D_GEOMETRY, this);
#endif // DEBUG

	__super::Late_Update(_fTimeDelta);
}

HRESULT CProjectile_Monster::Render()
{
#ifdef _DEBUG

    if (COORDINATE_2D == Get_CurCoord())
    {
        for (_uint i = 0; i < m_p2DColliderComs.size(); ++i)
        {
            m_p2DColliderComs[i]->Render();
        }
    }
#endif // _DEBUG

    __super::Render();
    return S_OK;
}

HRESULT CProjectile_Monster::Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition)
{
    if (FAILED(__super::Change_Coordinate(_eCoordinate, _pNewPosition)))
        return E_FAIL;

    if (COORDINATE_2D == _eCoordinate)
    {
        CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this);
    }
    else
        CSection_Manager::GetInstance()->Remove_GameObject_ToCurSectionLayer(this);

    return S_OK;
}

void CProjectile_Monster::On_Hit(CGameObject* _pHitter, _float _fDamg, _fvector _vForce)
{

}

void CProjectile_Monster::OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)
{
    if (OBJECT_GROUP::PLAYER & _Other.pActorUserData->iObjectGroup)
    {
        if((_uint)SHAPE_USE::SHAPE_BODY == _Other.pShapeUserData->iShapeUse)
        {
            _vector vRepulse = 10.f * XMVector3Normalize(XMVectorSetY(_Other.pActorUserData->pOwner->Get_FinalPosition() - Get_FinalPosition(), 0.f));
            XMVectorSetY(vRepulse, -1.f);
            Event_Hit(this, static_cast<CCharacter*>(_Other.pActorUserData->pOwner), 1, vRepulse);
            //Event_KnockBack(static_cast<CCharacter*>(_My.pActorUserData->pOwner), vRepulse);
            Event_DeleteObject(this);
        }

    }

    if (OBJECT_GROUP::MAPOBJECT & _Other.pActorUserData->iObjectGroup)
        Event_DeleteObject(this);
}

void CProjectile_Monster::OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)
{
}

void CProjectile_Monster::OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)
{
}

void CProjectile_Monster::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    if (OBJECT_GROUP::PLAYER & _pOtherObject->Get_ObjectGroupID() || OBJECT_GROUP::MAPOBJECT & _pOtherObject->Get_ObjectGroupID() || OBJECT_GROUP::BLOCKER & _pOtherObject->Get_ObjectGroupID())
    {
        Event_Hit(this, static_cast<CCharacter*>(_pOtherObject), 1, XMVectorZero());
        m_isStop = true;

    }
}

void CProjectile_Monster::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CProjectile_Monster::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CProjectile_Monster::Active_OnEnable()
{
    __super::Active_OnEnable();
 
    m_isStop = false;
    m_isFirstLoop = true;
    m_isTimeOver = false;
	//if (COORDINATE_3D == Get_CurCoord())
 //       m_pActorCom->Set_ShapeEnable((_int)SHAPE_USE::SHAPE_BODY, true);
}

void CProjectile_Monster::Active_OnDisable()
{
    m_pControllerTransform->Set_WorldMatrix(XMMatrixIdentity());
    m_fAccTime = 0.f;

  /*  if(COORDINATE_2D == Get_CurCoord())
        CSection_Manager::GetInstance()->Remove_GameObject_FromSectionLayer(m_strSectionName,this);*/

    //if (COORDINATE_3D == Get_CurCoord())
	   // m_pActorCom->Set_ShapeEnable((_int)SHAPE_USE::SHAPE_BODY, false);
    __super::Active_OnDisable();
}

//HRESULT CProjectile_Monster::Ready_ActorDesc(void* _pArg)
//{
//    CProjectile_Monster::PROJECTILE_MONSTER_DESC* pDesc = static_cast<CProjectile_Monster::PROJECTILE_MONSTER_DESC*>(_pArg);
//
//    pDesc->eActorType = ACTOR_TYPE::KINEMATIC;
//    CActor::ACTOR_DESC* ActorDesc = new CActor::ACTOR_DESC;
//
//    /* Actor의 주인 오브젝트 포인터 */
//    ActorDesc->pOwner = this;
//
//    /* Actor의 회전축을 고정하는 파라미터 */
//    ActorDesc->FreezeRotation_XYZ[0] = true;
//    ActorDesc->FreezeRotation_XYZ[1] = false;
//    ActorDesc->FreezeRotation_XYZ[2] = true;
//
//    /* Actor의 이동축을 고정하는 파라미터 (이걸 고정하면 중력도 영향을 받지 않음. 아예 해당 축으로의 이동을 제한하는)*/
//    ActorDesc->FreezePosition_XYZ[0] = false;
//    ActorDesc->FreezePosition_XYZ[1] = false;
//    ActorDesc->FreezePosition_XYZ[2] = false;
//
//    /* 사용하려는 Shape의 형태를 정의 */
//    SHAPE_SPHERE_DESC* ShapeDesc = new SHAPE_SPHERE_DESC;
//    ShapeDesc->fRadius = 0.5f;
//
//    /* 해당 Shape의 Flag에 대한 Data 정의 */
//    SHAPE_DATA* ShapeData = new SHAPE_DATA;
//    ShapeData->pShapeDesc = ShapeDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
//    ShapeData->eShapeType = SHAPE_TYPE::SPHERE;     // Shape의 형태.
//    ShapeData->eMaterial = ACTOR_MATERIAL::DEFAULT; // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
//    ShapeData->isTrigger = true;                    // Trigger 알림을 받기위한 용도라면 true
//    ShapeData->iShapeUse = (_uint)SHAPE_USE::SHAPE_BODY;
//    XMStoreFloat4x4(&ShapeData->LocalOffsetMatrix, XMMatrixRotationZ(XMConvertToRadians(90.f)) * XMMatrixTranslation(0.0f, 0.f, 0.0f)); // Shape의 LocalOffset을 행렬정보로 저장.
//
//    /* 최종으로 결정 된 ShapeData를 PushBack */
//    ActorDesc->ShapeDatas.push_back(*ShapeData);
//
//    /* 충돌 필터에 대한 세팅 ()*/
//    ActorDesc->tFilterData.MyGroup = OBJECT_GROUP::MONSTER_PROJECTILE;
//    ActorDesc->tFilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::PLAYER;
//
//    /* Actor Component Finished */
//    pDesc->pActorDesc = ActorDesc;
//
//    /* Shapedata 할당해제 */
//    Safe_Delete(ShapeData);
//
//    return S_OK;
//}

//HRESULT CProjectile_Monster::Ready_Components()
//{
//    /* 2D Collider */
//    m_p2DColliderComs.resize(1);
//
//    CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};
//    CircleDesc.pOwner = this;
//    CircleDesc.fRadius = 30.f;
//    CircleDesc.vScale = { 1.0f, 1.0f };
//    CircleDesc.vOffsetPosition = { 0.f, 0.f };
//    CircleDesc.isBlock = false;
//    CircleDesc.iCollisionGroupID = OBJECT_GROUP::MONSTER_PROJECTILE;
//    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
//        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &CircleDesc)))
//        return E_FAIL;
//
//    return S_OK;
//}

//HRESULT CProjectile_Monster::Ready_PartObjects()
//{
//    CModelObject::MODELOBJECT_DESC BodyDesc{};
//
//    BodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
//    BodyDesc.iCurLevelID = m_iCurLevelID;
//    BodyDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();
//
//    BodyDesc.strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
//    BodyDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
//    BodyDesc.strModelPrototypeTag_2D = TEXT("barferbug_projectile");
//    BodyDesc.strModelPrototypeTag_3D = TEXT("S_FX_CMN_Sphere_01");
//    BodyDesc.iModelPrototypeLevelID_2D = m_iCurLevelID;
//    BodyDesc.iModelPrototypeLevelID_3D = m_iCurLevelID;
//    BodyDesc.iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;
//    BodyDesc.iShaderPass_3D = (_uint)PASS_VTXMESH::TEST_PROJECTILE;
//
//    BodyDesc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
//    BodyDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);
//
//    BodyDesc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
//    BodyDesc.tTransform3DDesc.vInitialScaling = _float3(0.5f, 0.5f, 0.5f);
//    BodyDesc.tTransform3DDesc.fRotationPerSec = XMConvertToRadians(90.f);
//    BodyDesc.tTransform3DDesc.fSpeedPerSec = 10.f;
//
//    BodyDesc.tTransform2DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
//    BodyDesc.tTransform2DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
//    BodyDesc.tTransform2DDesc.fRotationPerSec = XMConvertToRadians(90.f);
//    BodyDesc.tTransform2DDesc.fSpeedPerSec = 10.f;
//
//    BodyDesc.iRenderGroupID_3D = RG_3D;
//    BodyDesc.iPriorityID_3D = PR3D_GEOMETRY;
//
//    m_PartObjects[PART_BODY] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &BodyDesc));
//    if (nullptr == m_PartObjects[PART_BODY])
//        return E_FAIL;
//
//    return S_OK;
//}

void CProjectile_Monster::Free()
{
    Safe_Release(m_pTarget);
    __super::Free();
}
