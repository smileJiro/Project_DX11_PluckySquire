#include "stdafx.h"
#include "RayShape.h"

CRayShape::CRayShape(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CContainerObject(_pDevice, _pContext)
{
}

CRayShape::CRayShape(const CRayShape& _Prototype)
    : CContainerObject(_Prototype)
{
}

HRESULT CRayShape::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CRayShape::Initialize(void* _pArg)
{
    RAYSHAPE_DESC* pDesc = static_cast<RAYSHAPE_DESC*>(_pArg);

    pDesc->eStartCoord = COORDINATE_3D;
    pDesc->isCoordChangeEnable = false;
    pDesc->iNumPartObjects = PART_LAST;

    pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(180.f);
    pDesc->tTransform3DDesc.fSpeedPerSec = 3.f;

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

void CRayShape::Priority_Update(_float _fTimeDelta)
{
    __super::Priority_Update(_fTimeDelta);
}

void CRayShape::Update(_float _fTimeDelta)
{
    __super::Update(_fTimeDelta);
}

void CRayShape::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta);
}

HRESULT CRayShape::Render()
{
    return S_OK;
}

HRESULT CRayShape::Ready_ActorDesc(void* _pArg)
{
    CRayShape::RAYSHAPE_DESC* pDesc = static_cast<CRayShape::RAYSHAPE_DESC*>(_pArg);

    //레이 세팅을 위한 트리거
    pDesc->eActorType = ACTOR_TYPE::STATIC;
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

    SHAPE_DATA* ShapeData = new SHAPE_DATA;

    /* 사용하려는 Shape의 형태를 정의 */
    switch (pDesc->eShapeType)
    {
    case SHAPE_TYPE::BOX:
    {
        SHAPE_BOX_DESC* ShapeDesc = new SHAPE_BOX_DESC;
        ShapeDesc->vHalfExtents = pDesc->vHalfExtents;

        ShapeData->pShapeDesc = ShapeDesc;
        break;
    }

    case SHAPE_TYPE::SPHERE:
    {
        SHAPE_SPHERE_DESC* ShapeDesc = new SHAPE_SPHERE_DESC;
        ShapeDesc->fRadius = pDesc->fRadius;

        ShapeData->pShapeDesc = ShapeDesc;
        break;
    }

    case SHAPE_TYPE::CAPSULE:
    {
        SHAPE_CAPSULE_DESC* ShapeDesc = new SHAPE_CAPSULE_DESC;
        ShapeDesc->fHalfHeight = pDesc->fHalfHeight;
        ShapeDesc->fRadius = pDesc->fRadius;

        ShapeData->pShapeDesc = ShapeDesc;
        break;
    }

    default:
        break;
    }

    ShapeData->eShapeType = pDesc->eShapeType;     // Shape의 형태.
    ShapeData->eMaterial = ACTOR_MATERIAL::DEFAULT; // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
    ShapeData->isTrigger = true;                    // Trigger 알림을 받기위한 용도라면 true
    ShapeData->isSceneQuery = true;           // 레이 캐스트를 위해 켬
    XMStoreFloat4x4(&ShapeData->LocalOffsetMatrix, XMMatrixRotationZ(XMConvertToRadians(pDesc->fRotAngle)) * XMMatrixTranslation(pDesc->vOffsetTrans.x, pDesc->vOffsetTrans.y, pDesc->vOffsetTrans.z)); // Shape의 LocalOffset을 행렬정보로 저장.

    /* 최종으로 결정 된 ShapeData를 PushBack */
    ActorDesc->ShapeDatas.push_back(*ShapeData);

    /* 충돌 필터에 대한 세팅 (일단 맵오브젝트로 설정해둠)*/
    ActorDesc->tFilterData.MyGroup = OBJECT_GROUP::RAY_OBJECT;
    ActorDesc->tFilterData.OtherGroupMask = OBJECT_GROUP::PLAYER | OBJECT_GROUP::PLAYER_PROJECTILE | OBJECT_GROUP::MONSTER;

    /* Actor Component Finished */
    pDesc->pActorDesc = ActorDesc;

    /* Shapedata 할당해제 */
    Safe_Delete(ShapeData);

    return S_OK;
}

HRESULT CRayShape::Ready_Components()
{
    return S_OK;
}

HRESULT CRayShape::Ready_PartObjects()
{
    return S_OK;
}

CRayShape* CRayShape::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CRayShape* pInstance = new CRayShape(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CRayShape");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CRayShape::Clone(void* _pArg)
{
    CRayShape* pInstance = new CRayShape(*this);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CRayShape");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRayShape::Free()
{
    __super::Free();
}
