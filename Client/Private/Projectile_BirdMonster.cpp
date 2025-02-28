#include "stdafx.h"
#include "Projectile_BirdMonster.h"
#include "ModelObject.h"
#include "Pooling_Manager.h"
#include "GameInstance.h"
#include "Section_Manager.h"

CProjectile_BirdMonster::CProjectile_BirdMonster(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CProjectile_Monster(_pDevice, _pContext)
{
}

CProjectile_BirdMonster::CProjectile_BirdMonster(const CProjectile_BirdMonster& _Prototype)
	: CProjectile_Monster(_Prototype)
{
}

HRESULT CProjectile_BirdMonster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CProjectile_BirdMonster::Initialize(void* _pArg)
{
    PROJECTILE_BIRDMONSTER_DESC* pDesc = static_cast<PROJECTILE_BIRDMONSTER_DESC*>(_pArg);

    pDesc->eStartCoord = COORDINATE_3D;
    pDesc->isCoordChangeEnable = false;

    pDesc->tTransform2DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    pDesc->tTransform2DDesc.fSpeedPerSec = 400.f;

    pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    pDesc->tTransform3DDesc.fSpeedPerSec = 10.f;

    pDesc->fLifeTime = 5.f;


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

void CProjectile_BirdMonster::Priority_Update(_float _fTimeDelta)
{
    __super::Priority_Update(_fTimeDelta);
}

void CProjectile_BirdMonster::Update(_float _fTimeDelta)
{
	if (COORDINATE_2D == Get_CurCoord())
    {
        if(false == m_isStop)
            m_pControllerTransform->Go_Up(_fTimeDelta);
    }

    else if (COORDINATE_3D == Get_CurCoord())
    {
        if (nullptr != m_pTarget)
        {
            if (true == m_isFirstLoop)
            {
                XMStoreFloat3(&m_vDir, m_pTarget->Get_FinalPosition() - Get_FinalPosition());
            }

            m_pControllerTransform->Go_Direction(XMVector3Normalize(XMLoadFloat3(&m_vDir)), m_pControllerTransform->Get_SpeedPerSec(), _fTimeDelta);
        }
        else
            m_pControllerTransform->Go_Straight(_fTimeDelta);

        /*_float3 vForce; XMStoreFloat3(&vForce, Get_ControllerTransform()->Get_State(CTransform::STATE_LOOK));
        m_pActorCom->Set_LinearVelocity(XMLoadFloat3(&vForce), Get_ControllerTransform()->Get_SpeedPerSec());*/
    }

    __super::Update(_fTimeDelta);
}

void CProjectile_BirdMonster::Late_Update(_float _fTimeDelta)
{

	__super::Late_Update(_fTimeDelta);
}

HRESULT CProjectile_BirdMonster::Render()
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

HRESULT CProjectile_BirdMonster::Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition)
{
    if (FAILED(__super::Change_Coordinate(_eCoordinate, _pNewPosition)))
        return E_FAIL;

    return S_OK;
}

void CProjectile_BirdMonster::Change_Animation()
{
}

void CProjectile_BirdMonster::Animation_End(COORDINATE _eCoord, _uint iAnimIdx)
{
	//if (COORDINATE_2D == _eCoord)
 //   {
 //       CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);
 //       switch (pModelObject->Get_Model(COORDINATE_2D)->Get_CurrentAnimIndex())
 //       {
 //       default:
 //           break;
 //       }
 //   }
}

void CProjectile_BirdMonster::On_Hit(CGameObject* _pHitter, _float _fDamg, _fvector _vForce)
{

}

void CProjectile_BirdMonster::OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)
{
    __super::OnTrigger_Enter(_My, _Other);

    if (OBJECT_GROUP::PLAYER & _Other.pActorUserData->iObjectGroup)
    {
        if((_uint)SHAPE_USE::SHAPE_BODY == _Other.pShapeUserData->iShapeUse)
        {
            //effect, sound

        }

    }
}

void CProjectile_BirdMonster::OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)
{
}

void CProjectile_BirdMonster::OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)
{
}

void CProjectile_BirdMonster::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    __super::On_Collision2D_Enter(_pMyCollider, _pOtherCollider, _pOtherObject);

    if (OBJECT_GROUP::PLAYER & _pOtherObject->Get_ObjectGroupID() || OBJECT_GROUP::MAPOBJECT & _pOtherObject->Get_ObjectGroupID() || OBJECT_GROUP::BLOCKER & _pOtherObject->Get_ObjectGroupID())
    {
    }
}

void CProjectile_BirdMonster::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CProjectile_BirdMonster::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CProjectile_BirdMonster::Active_OnEnable()
{
    __super::Active_OnEnable();

	//if (COORDINATE_3D == Get_CurCoord())
 //       m_pActorCom->Set_ShapeEnable((_int)SHAPE_USE::SHAPE_BODY, true);
}

void CProjectile_BirdMonster::Active_OnDisable()
{
    //if (COORDINATE_3D == Get_CurCoord())
	   // m_pActorCom->Set_ShapeEnable((_int)SHAPE_USE::SHAPE_BODY, false);
    __super::Active_OnDisable();
}

HRESULT CProjectile_BirdMonster::Ready_ActorDesc(void* _pArg)
{
    CProjectile_BirdMonster::PROJECTILE_BIRDMONSTER_DESC* pDesc = static_cast<CProjectile_BirdMonster::PROJECTILE_BIRDMONSTER_DESC*>(_pArg);

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
    SHAPE_SPHERE_DESC* ShapeDesc = new SHAPE_SPHERE_DESC;
    ShapeDesc->fRadius = 0.3f;

    /* 해당 Shape의 Flag에 대한 Data 정의 */
    SHAPE_DATA* ShapeData = new SHAPE_DATA;
    ShapeData->pShapeDesc = ShapeDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
    ShapeData->eShapeType = SHAPE_TYPE::SPHERE;     // Shape의 형태.
    ShapeData->eMaterial = ACTOR_MATERIAL::DEFAULT; // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
    ShapeData->isTrigger = true;                    // Trigger 알림을 받기위한 용도라면 true
    ShapeData->iShapeUse = (_uint)SHAPE_USE::SHAPE_BODY;
    XMStoreFloat4x4(&ShapeData->LocalOffsetMatrix, XMMatrixRotationZ(XMConvertToRadians(90.f)) * XMMatrixTranslation(0.0f, 0.f, 0.0f)); // Shape의 LocalOffset을 행렬정보로 저장.

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

HRESULT CProjectile_BirdMonster::Ready_Components()
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

HRESULT CProjectile_BirdMonster::Ready_PartObjects()
{
    CModelObject::MODELOBJECT_DESC BodyDesc{};

    BodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
    BodyDesc.iCurLevelID = m_iCurLevelID;
    BodyDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

    BodyDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
    
    BodyDesc.strModelPrototypeTag_3D = TEXT("BirdMonster_Projectile");
    
    BodyDesc.iModelPrototypeLevelID_3D = m_iCurLevelID;
    
    BodyDesc.iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;

    
    BodyDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);

    BodyDesc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    BodyDesc.tTransform3DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
    BodyDesc.tTransform3DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    BodyDesc.tTransform3DDesc.fSpeedPerSec = 10.f;

    BodyDesc.iRenderGroupID_3D = RG_3D;
    BodyDesc.iPriorityID_3D = PR3D_GEOMETRY;

    m_PartObjects[PART_BODY] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &BodyDesc));
    if (nullptr == m_PartObjects[PART_BODY])
        return E_FAIL;

    return S_OK;
}

CProjectile_BirdMonster* CProjectile_BirdMonster::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CProjectile_BirdMonster* pInstance = new CProjectile_BirdMonster(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CProjectile_BirdMonster");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CProjectile_BirdMonster::Clone(void* _pArg)
{
    CProjectile_BirdMonster* pInstance = new CProjectile_BirdMonster(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CProjectile_BirdMonster");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CProjectile_BirdMonster::Free()
{

    __super::Free();
}
