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

    //pDesc->eStartCoord = COORDINATE_3D;
    pDesc->isCoordChangeEnable = false;
    pDesc->tTransform3DDesc.fSpeedPerSec = 10.f;
    pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(180.f);

    m_iTriggerType = pDesc->iTriggerType;
    m_szEventTag = pDesc->szEventTag;
    m_eConditionType = pDesc->eConditionType;
    //m_eCoordiNate = pDesc->eCoordiNate; StartCoordinate가 있는데?

    pDesc->eActorType = ACTOR_TYPE::STATIC;
    CActor::ACTOR_DESC ActorDesc;

    /* Actor의 주인 오브젝트 포인터 */
    ActorDesc.pOwner = this;

    // Static이라서 FreezeRotation, FreezePosition은 채우지 않음

    /* 사용하려는 Shape의 형태를 정의 */
    SHAPE_BOX_DESC ShapeBoxDesc = {};
    SHAPE_SPHERE_DESC ShapeSpereDesc = {};

    /* 해당 Shape의 Flag에 대한 Data 정의 */
    SHAPE_DATA ShapeData;

    switch (pDesc->eShapeType) {
    case SHAPE_TYPE::BOX:
        ShapeBoxDesc.vHalfExtents = pDesc->vHalfExtents;
        ShapeData.pShapeDesc = &ShapeBoxDesc;  
        break;
    case SHAPE_TYPE::SPHERE:
        ShapeSpereDesc.fRadius = pDesc->fRadius;
        ShapeData.pShapeDesc = &ShapeSpereDesc;
        break;
    }

    ShapeData.eShapeType = pDesc->eShapeType;
    ShapeData.isTrigger = true;
    ShapeData.isSceneQuery = true;

    /* 최종으로 결정 된 ShapeData를 PushBack */
    ActorDesc.ShapeDatas.push_back(ShapeData);

    /* 충돌 필터에 대한 세팅 ()*/
    ActorDesc.tFilterData.MyGroup = pDesc->iFillterMyGroup;
    ActorDesc.tFilterData.OtherGroupMask = pDesc->iFillterOtherGroupMask;

    /* Actor Component Finished */
    pDesc->pActorDesc = &ActorDesc;

    if (FAILED(__super::Initialize(pDesc))) {
        MSG_BOX("Trigger Initialize Falied");
        return E_FAIL;
    }

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
    if (false != m_EnterHandler) {
        m_EnterHandler(m_iTriggerType, m_iTriggerID, m_szEventTag);
    }
}

void CTriggerObject::OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)
{
    if (false != m_StayHandler) {
        m_StayHandler(m_iTriggerType, m_iTriggerID, m_szEventTag);
    }
}

void CTriggerObject::OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)
{
    if (false != m_ExitHandler) {
        m_ExitHandler(m_iTriggerType, m_iTriggerID, m_szEventTag);
    }

    if (false != m_CollisionExitHandler) {
        m_CollisionExitHandler(m_iTriggerType, m_iTriggerID, _My, _Other);
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
