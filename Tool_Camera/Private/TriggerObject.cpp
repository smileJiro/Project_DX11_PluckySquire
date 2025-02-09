#include "stdafx.h"
#include "TriggerObject.h"

#include "GameInstance.h"

_int CTriggerObject::g_iNextID = -1;

CTriggerObject::CTriggerObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CActorObject(_pDevice, _pContext)
    , m_iTriggerID{ g_iNextID++ }
{
}

CTriggerObject::CTriggerObject(const CTriggerObject& _Prototype)
    : CActorObject(_Prototype)
    , m_iTriggerID{ g_iNextID++ }
{
}

HRESULT CTriggerObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTriggerObject::Initialize(void* _pArg)
{
    CTriggerObject::TRIGGEROBJECT_DESC* pDesc = static_cast<CTriggerObject::TRIGGEROBJECT_DESC*>(_pArg);

    CActor::ACTOR_DESC* ActorDesc = nullptr;

    if (COORDINATE_3D == pDesc->eStartCoord)
    {
        if (FAILED(Initialize_3D_Trigger(&ActorDesc, pDesc)))
            return E_FAIL;
    }


    if (FAILED(__super::Initialize(pDesc))) {
        MSG_BOX("Trigger Initialize Falied");
        return E_FAIL;
    }

    if (COORDINATE_2D == pDesc->eStartCoord)
    {
        if (FAILED(Initialize_2D_Trigger(pDesc)))
            return E_FAIL;
    }

    if (nullptr != ActorDesc)
    {
        ActorDesc->pOwner = nullptr;
        Safe_Delete(ActorDesc);
    }

    return S_OK;
}

HRESULT CTriggerObject::Render()
{
#ifdef _DEBUG
    if(m_pColliderCom)
        return m_pColliderCom->Render();
#endif // _DEBUG
    return S_OK;
}

HRESULT CTriggerObject::Initialize_3D_Trigger(CActor::ACTOR_DESC** _pActorDesc, TRIGGEROBJECT_DESC* _pDesc)
{
    _pDesc->isCoordChangeEnable = false;
    _pDesc->tTransform3DDesc.fSpeedPerSec = 10.f;
    _pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(180.f);

    m_iTriggerType = _pDesc->iTriggerType;
    m_szEventTag = _pDesc->szEventTag;
    m_eConditionType = _pDesc->eConditionType;
    //m_eCoordiNate = _pDesc->eCoordiNate; StartCoordinate가 있는데?

    _pDesc->eActorType = ACTOR_TYPE::STATIC;
    *_pActorDesc = new CActor::ACTOR_DESC();

    /* Actor의 주인 오브젝트 포인터 */
    (*_pActorDesc)->pOwner = this;

    // Static이라서 FreezeRotation, FreezePosition은 채우지 않음

    /* 사용하려는 Shape의 형태를 정의 */
    SHAPE_BOX_DESC ShapeBoxDesc = {};
    SHAPE_SPHERE_DESC ShapeSpereDesc = {};

    /* 해당 Shape의 Flag에 대한 Data 정의 */
    SHAPE_DATA ShapeData;

    switch (_pDesc->eShapeType) {
    case SHAPE_TYPE::BOX:
        ShapeBoxDesc.vHalfExtents = _pDesc->vHalfExtents;
        ShapeData.pShapeDesc = &ShapeBoxDesc;
        break;
    case SHAPE_TYPE::SPHERE:
        ShapeSpereDesc.fRadius = _pDesc->fRadius;
        ShapeData.pShapeDesc = &ShapeSpereDesc;
        break;
    }

    ShapeData.eShapeType = _pDesc->eShapeType;
    ShapeData.isTrigger = true;
    ShapeData.isSceneQuery = true;

    /* 최종으로 결정 된 ShapeData를 PushBack */
    (*_pActorDesc)->ShapeDatas.push_back(ShapeData);

    /* 충돌 필터에 대한 세팅 ()*/
    (*_pActorDesc)->tFilterData.MyGroup = m_iMyColliderGroup = _pDesc->iFillterMyGroup;
    (*_pActorDesc)->tFilterData.OtherGroupMask = _pDesc->iFillterOtherGroupMask;

    /* Actor Component Finished */
    _pDesc->pActorDesc = (*_pActorDesc);

    return S_OK;
}

HRESULT CTriggerObject::Initialize_2D_Trigger(TRIGGEROBJECT_DESC* _pDesc)
{
    CCollider_AABB::COLLIDER_AABB_DESC AABBDesc = {};
    AABBDesc.pOwner = this;
    AABBDesc.vExtents = { 1.f, 1.f };
    AABBDesc.vScale = { 1.f, 1.f };
    AABBDesc.vOffsetPosition = { 0.f, 0.f };
    if (FAILED(Add_Component(m_pGameInstance->Get_StaticLevelID(), TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &AABBDesc)))
        return E_FAIL;
    return S_OK;
}

void CTriggerObject::Late_Update(_float _fTimeDelta)
{
    CGameObject::Late_Update_Component(_fTimeDelta);
}

void CTriggerObject::Set_CustomData(_wstring _Key, any _pValue)
{
    m_CustomDatas[_Key] = _pValue;
}

any CTriggerObject::Get_CustomData(_wstring _Key)
{
    if (m_CustomDatas.find(_Key) != m_CustomDatas.end()) {
        return m_CustomDatas[_Key];
    }

    return nullptr;
}

void CTriggerObject::Resister_EnterHandler(function<void(_uint, _int, _wstring)> _Handler)
{
    m_EnterHandler = _Handler;
}

void CTriggerObject::Resister_StayHandler(function<void(_uint, _int, _wstring)> _Handler)
{
    m_StayHandler = _Handler;
}

void CTriggerObject::Resister_ExitHandler(function<void(_uint, _int, _wstring)> _Handler)
{
    m_ExitHandler = _Handler;
}

void CTriggerObject::Resister_ExitHandler_ByCollision(function<void(_uint, _int, const COLL_INFO&, const COLL_INFO&)> _Handler)
{
    m_CollisionExitHandler = _Handler;
}

void CTriggerObject::OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)
{
    if (!m_EnterHandler) {
        m_EnterHandler(m_iTriggerType, m_iTriggerID, m_szEventTag);
    }
}

void CTriggerObject::OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)
{
    if (!m_StayHandler) {
        m_StayHandler(m_iTriggerType, m_iTriggerID, m_szEventTag);
    }
}

void CTriggerObject::OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)
{
    if (!m_ExitHandler) {
        m_ExitHandler(m_iTriggerType, m_iTriggerID, m_szEventTag);
    }

    if (!m_CollisionExitHandler) {
        m_CollisionExitHandler(m_iTriggerType, m_iTriggerID, _My, _Other);
    }
}

void CTriggerObject::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    if (!m_EnterHandler) {
        m_EnterHandler(m_iTriggerType, m_iTriggerID, m_szEventTag);
    }
}

void CTriggerObject::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    if (!m_StayHandler) {
        m_StayHandler(m_iTriggerType, m_iTriggerID, m_szEventTag);
    }
}

void CTriggerObject::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    if (!m_ExitHandler) {
        m_ExitHandler(m_iTriggerType, m_iTriggerID, m_szEventTag);
    }
}

CTriggerObject* CTriggerObject::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CTriggerObject* pInstance = new CTriggerObject(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CTriggerObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTriggerObject::Clone(void* _pArg)
{
    CTriggerObject* pInstance = new CTriggerObject(*this);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CTriggerObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTriggerObject::Free()
{
    __super::Free();
}
