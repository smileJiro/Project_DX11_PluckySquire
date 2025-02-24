#include "stdafx.h"
#include "Soldier_Spear.h"
#include "Actor_Dynamic.h"
#include "3DModel.h"
#include "GameInstance.h"
#include "Section_Manager.h"

CSoldier_Spear::CSoldier_Spear(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CModelObject(_pDevice, _pContext)
{
}


CSoldier_Spear::CSoldier_Spear(const CSoldier_Spear& _Prototype)
    :CModelObject(_Prototype)
{

}

HRESULT CSoldier_Spear::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSoldier_Spear::Initialize(void* _pArg)
{
    //Part Sword
    SOLDIER_SPEAR_DESC* pDesc = static_cast<SOLDIER_SPEAR_DESC*>(_pArg);

    pDesc->isCoordChangeEnable = false;
    pDesc->eStartCoord = COORDINATE_3D;
    pDesc->iObjectGroupID = OBJECT_GROUP::MONSTER_PROJECTILE;

    m_pParent = pDesc->pParent;

    if (FAILED(Ready_ActorDesc(pDesc)))
        return E_FAIL;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    /* Actor Desc 채울 때 쓴 데이터 할당해제 */

    for (_uint i = 0; i < pDesc->pActorDesc->ShapeDatas.size(); i++)
    {
        Safe_Delete(pDesc->pActorDesc->ShapeDatas[i].pShapeDesc);
    }

    Safe_Delete(pDesc->pActorDesc);

    return S_OK;
}

void CSoldier_Spear::Update(_float _fTimeDelta)
{
    __super::Update(_fTimeDelta);
}

void CSoldier_Spear::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta);
}

HRESULT CSoldier_Spear::Render()
{
#ifdef _DEBUG
#endif
    if(Is_Active())
        return __super::Render();
    return S_OK;
}

void CSoldier_Spear::OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)
{
    if (OBJECT_GROUP::PLAYER & _Other.pActorUserData->iObjectGroup)
    {
        if ((_uint)SHAPE_USE::SHAPE_BODY == _Other.pShapeUserData->iShapeUse)
        {
            int a = 10;
        }
    }

}

void CSoldier_Spear::OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)
{
}

void CSoldier_Spear::OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)
{
}

void CSoldier_Spear::Active_OnEnable()
{
    __super::Active_OnEnable();
}

void CSoldier_Spear::Active_OnDisable()
{
    __super::Active_OnDisable();
}

HRESULT CSoldier_Spear::Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition)
{
    if (FAILED(__super::Change_Coordinate(_eCoordinate, _pNewPosition)))
        return E_FAIL;

    return S_OK;
}

void CSoldier_Spear::Attack(CGameObject* _pVictim)
{
}

_vector CSoldier_Spear::Get_LookDirection()
{
    return _vector();
}

HRESULT CSoldier_Spear::Ready_ActorDesc(void* _pArg)
{
    CSoldier_Spear::SOLDIER_SPEAR_DESC* pDesc = static_cast<CSoldier_Spear::SOLDIER_SPEAR_DESC*>(_pArg);

    pDesc->eActorType = ACTOR_TYPE::DYNAMIC;
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
    ShapeDesc->fHalfHeight = 0.5f;
    ShapeDesc->fRadius = 0.1f;

    /* 해당 Shape의 Flag에 대한 Data 정의 */
    SHAPE_DATA* ShapeData = new SHAPE_DATA;
    ShapeData->pShapeDesc = ShapeDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
    ShapeData->eShapeType = SHAPE_TYPE::CAPSULE;     // Shape의 형태.
    ShapeData->eMaterial = ACTOR_MATERIAL::DEFAULT; // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
    ShapeData->isTrigger = true;                    // Trigger 알림을 받기위한 용도라면 true
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

CSoldier_Spear* CSoldier_Spear::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CSoldier_Spear* pInstance = new CSoldier_Spear(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : Soldier_Spear");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSoldier_Spear::Clone(void* _pArg)
{
    CSoldier_Spear* pInstance = new CSoldier_Spear(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : Soldier_Spear");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSoldier_Spear::Free()
{
    m_pParent = nullptr;

    __super::Free();
}
