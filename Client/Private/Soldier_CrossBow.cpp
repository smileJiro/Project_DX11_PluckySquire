#include "stdafx.h"
#include "Soldier_CrossBow.h"
#include "Actor_Dynamic.h"
#include "3DModel.h"
#include "GameInstance.h"
#include "Section_Manager.h"
#include "Monster.h"

CSoldier_CrossBow::CSoldier_CrossBow(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CModelObject(_pDevice, _pContext)
{
}


CSoldier_CrossBow::CSoldier_CrossBow(const CSoldier_CrossBow& _Prototype)
    :CModelObject(_Prototype)
{

}

HRESULT CSoldier_CrossBow::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSoldier_CrossBow::Initialize(void* _pArg)
{
    //Part Sword
    SOLDIER_CROSSBOW_DESC* pDesc = static_cast<SOLDIER_CROSSBOW_DESC*>(_pArg);

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

void CSoldier_CrossBow::Update(_float _fTimeDelta)
{
    __super::Update(_fTimeDelta);
}

void CSoldier_CrossBow::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta);
}

HRESULT CSoldier_CrossBow::Render()
{
#ifdef _DEBUG
#endif
    if(Is_Active())
        return __super::Render();
    return S_OK;
}

void CSoldier_CrossBow::OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)
{
    if (OBJECT_GROUP::PLAYER & _Other.pActorUserData->iObjectGroup
        && (_uint)SHAPE_USE::SHAPE_BODY == _My.pShapeUserData->iShapeUse
        && (_uint)SHAPE_USE::SHAPE_BODY == _Other.pShapeUserData->iShapeUse)
    {
        _vector vRepulse = 10.f * XMVector3Normalize(XMVectorSetY(_Other.pActorUserData->pOwner->Get_FinalPosition() - Get_FinalPosition(), 0.f));
        Event_Hit(this, static_cast<CCharacter*>(_Other.pActorUserData->pOwner), m_pParent->Get_Stat().iDamg, vRepulse);
    }

}

void CSoldier_CrossBow::OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)
{
}

void CSoldier_CrossBow::OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)
{
}

void CSoldier_CrossBow::Active_OnEnable()
{
    __super::Active_OnEnable();
}

void CSoldier_CrossBow::Active_OnDisable()
{
    __super::Active_OnDisable();
}

HRESULT CSoldier_CrossBow::Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition)
{
    if (FAILED(__super::Change_Coordinate(_eCoordinate, _pNewPosition)))
        return E_FAIL;

    return S_OK;
}

void CSoldier_CrossBow::Attack(CGameObject* _pVictim)
{
}

_vector CSoldier_CrossBow::Get_LookDirection()
{
    return _vector();
}

HRESULT CSoldier_CrossBow::Ready_ActorDesc(void* _pArg)
{
    CSoldier_CrossBow::SOLDIER_CROSSBOW_DESC* pDesc = static_cast<CSoldier_CrossBow::SOLDIER_CROSSBOW_DESC*>(_pArg);

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
    ShapeDesc->fHalfHeight = 0.2f;
    ShapeDesc->fRadius = 0.3f;

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

CSoldier_CrossBow* CSoldier_CrossBow::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CSoldier_CrossBow* pInstance = new CSoldier_CrossBow(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : Soldier_CrossBow");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSoldier_CrossBow::Clone(void* _pArg)
{
    CSoldier_CrossBow* pInstance = new CSoldier_CrossBow(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : Soldier_CrossBow");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSoldier_CrossBow::Free()
{
    m_pParent = nullptr;

    __super::Free();
}
