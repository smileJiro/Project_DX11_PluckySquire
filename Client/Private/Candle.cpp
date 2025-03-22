#include "stdafx.h"
#include "Candle.h"
#include "GameInstance.h"
#include "Candle_Body.h"
#include "Candle_UI.h"
#include "Player.h"

CCandle::CCandle(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CContainerObject(_pDevice, _pContext)
{
}

CCandle::CCandle(const CCandle& _Prototype)
    :CContainerObject(_Prototype)
{
}

HRESULT CCandle::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCandle::Initialize(void* _pArg)
{
    CCandle::CANDLE_DESC* pDesc = static_cast<CCandle::CANDLE_DESC*>(_pArg);
    pDesc->eStartCoord = COORDINATE_3D;
    pDesc->isCoordChangeEnable = false;
    pDesc->iNumPartObjects = (_uint)CANDLE_PART::CANDLE_LAST;
    pDesc->iObjectGroupID = TRIGGER_OBJECT;

    /* Actor의 주인 오브젝트 포인터 */
    pDesc->eActorType = ACTOR_TYPE::STATIC;
    CActor::ACTOR_DESC ActorDesc;
    ActorDesc.pOwner = this;

    /* 충돌 감지 트리거 */
    SHAPE_SPHERE_DESC SphereDesc = {};
    SphereDesc.fRadius = 1.0f;
    SHAPE_DATA ShapeData = {};
    ShapeData.pShapeDesc = &SphereDesc;
    ShapeData.eShapeType = SHAPE_TYPE::SPHERE;
    ShapeData.eMaterial = ACTOR_MATERIAL::DEFAULT;
    ShapeData.iShapeUse = (_uint)SHAPE_USE::SHAPE_TRIGER;
    ShapeData.isTrigger = true;
    ShapeData.FilterData.MyGroup = OBJECT_GROUP::TRIGGER_OBJECT;
    ShapeData.FilterData.OtherGroupMask = OBJECT_GROUP::PLAYER;
    XMStoreFloat4x4(&ShapeData.LocalOffsetMatrix, XMMatrixTranslation(0.0f, 1.0f, 0.0f));
    ActorDesc.ShapeDatas.push_back(ShapeData);
    
    SHAPE_BOX_DESC BoxDesc = {};
    BoxDesc.vHalfExtents = {1.0f, 0.3f, 1.0f};
    SHAPE_DATA ShapeData2 = {};
    ShapeData2.pShapeDesc = &BoxDesc;
    ShapeData2.eShapeType = SHAPE_TYPE::BOX;
    ShapeData2.eMaterial = ACTOR_MATERIAL::NORESTITUTION;
    ShapeData2.iShapeUse = (_uint)SHAPE_USE::SHAPE_BODY;
    ShapeData2.isTrigger = false;
    ShapeData2.FilterData.MyGroup = OBJECT_GROUP::MAPOBJECT;
    ShapeData2.FilterData.OtherGroupMask = OBJECT_GROUP::PLAYER;
    XMStoreFloat4x4(&ShapeData2.LocalOffsetMatrix, XMMatrixTranslation(0.0f, 0.0f, 0.0f));


    ActorDesc.ShapeDatas.push_back(ShapeData2);

    /* Actor Component Finished */
    pDesc->pActorDesc = &ActorDesc;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_TargetLight()))
        return E_FAIL;

    m_eCurState = STATE_IDLE;
    return S_OK;
}

void CCandle::Priority_Update(_float _fTimeDelta)
{

    __super::Priority_Update(_fTimeDelta);
}

void CCandle::Update(_float _fTimeDelta)
{
    if (KEY_DOWN(KEY::I))
    {
        if (STATE_TURNOFF == m_eCurState)
            m_eCurState = STATE_TURNON;
        else
            m_eCurState = STATE_TURNOFF;
    }


    Action_State(_fTimeDelta);

    __super::Update(_fTimeDelta);
}

void CCandle::Late_Update(_float _fTimeDelta)
{
    State_Change();

    __super::Late_Update(_fTimeDelta);
}

HRESULT CCandle::Render()
{


    return S_OK;
}

void CCandle::OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)
{

    //OBJECT_GROUP eOtherGroupID = (OBJECT_GROUP)_Other.pActorUserData->iObjectGroup;
    //SHAPE_USE eOtherShapeUse = (SHAPE_USE)_Other.pShapeUserData->iShapeUse;
    //CGameObject* pGameObject = _Other.pActorUserData->pOwner;
    //switch (eOtherGroupID)
    //{
    //case Client::NONE:
    //    break;
    //case Client::PLAYER:
    //{
    //     if (SHAPE_USE::SHAPE_BODY == eOtherShapeUse)
    //    {
    //        CPlayer* pPlayer = static_cast<CPlayer*>(pGameObject);

    //        if(true == pPlayer->Is_ZetPack_Ascend())
    //            m_eCurState = STATE_TURNON;
    //    }
    //}
    //    break;
    //default:
    //    break;
    //}
}

void CCandle::OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)
{
    OBJECT_GROUP eOtherGroupID = (OBJECT_GROUP)_Other.pActorUserData->iObjectGroup;
    SHAPE_USE eOtherShapeUse = (SHAPE_USE)_Other.pShapeUserData->iShapeUse;
    CGameObject* pGameObject = _Other.pActorUserData->pOwner;
    switch (eOtherGroupID)
    {
    case Client::NONE:
        break;
    case Client::PLAYER:
    {
        if (SHAPE_USE::SHAPE_BODY == eOtherShapeUse)
        {
            CPlayer* pPlayer = static_cast<CPlayer*>(pGameObject);

            if (false == pPlayer->Is_ZetPack_Idle())
                m_eCurState = STATE_TURNON;
        }
    }
    break;
    default:
        break;
    }
}

void CCandle::OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)
{
 /*   OBJECT_GROUP eOtherGroupID = (OBJECT_GROUP)_Other.pActorUserData->iObjectGroup;
    SHAPE_USE eOtherShapeUse = (SHAPE_USE)_Other.pShapeUserData->iShapeUse;
    CGameObject* pGameObject = _Other.pActorUserData->pOwner;
    switch (eOtherGroupID)
    {
    case Client::NONE:
        break;
    case Client::PLAYER:
    {
        if (SHAPE_USE::SHAPE_BODY == eOtherShapeUse)
        {
            CPlayer* pPlayer = static_cast<CPlayer*>(pGameObject);

            if (true == pPlayer->Is_ZetPack_Ascend())
                m_eCurState = STATE_TURNON;
        }
    }
    break;
    default:
        break;
    }*/
}

void CCandle::OnOff_Candle_UI(_bool _isOnOff)
{
    if (nullptr == m_PartObjects[CANDLE_PART::CANDLE_UI])
        return;

    static_cast<CCandle_UI*>(m_PartObjects[CANDLE_PART::CANDLE_UI])->OnOff_CandleUI(_isOnOff);
}

void CCandle::State_Change()
{
    if (m_ePreState == m_eCurState)
        return;

    switch (m_eCurState)
    {
    case Client::CCandle::STATE_IDLE:
        State_Change_Idle();
        break;
    case Client::CCandle::STATE_TURNON:
        State_Change_TurnOn();
        break;
    case Client::CCandle::STATE_FLAMELOOP:
        State_Change_FlameLoop();
        break;
    case Client::CCandle::STATE_TURNOFF:
        State_Change_TurnOff();
        break;
    default:
        break;
    }

    m_ePreState = m_eCurState;
}

void CCandle::State_Change_Idle()
{
    static_cast<CCandle_Body*>(m_PartObjects[CANDLE_BODY])->State_Change_Idle();
    static_cast<CCandle_UI*>(m_PartObjects[CANDLE_UI ])->State_Change_Idle();
}

void CCandle::State_Change_TurnOn()
{
    static_cast<CCandle_Body*>(m_PartObjects[CANDLE_BODY])->State_Change_TurnOn();
    static_cast<CCandle_UI*>(m_PartObjects[CANDLE_UI])->State_Change_TurnOn();
    Event_SetActive(m_pTargetLight, true);

}

void CCandle::State_Change_FlameLoop()
{
    static_cast<CCandle_Body*>(m_PartObjects[CANDLE_BODY])->State_Change_FlameLoop();
    static_cast<CCandle_UI*>(m_PartObjects[CANDLE_UI])->State_Change_FlameLoop();
}

void CCandle::State_Change_TurnOff()
{
    static_cast<CCandle_Body*>(m_PartObjects[CANDLE_BODY])->State_Change_TurnOff();
    static_cast<CCandle_UI*>(m_PartObjects[CANDLE_UI])->State_Change_TurnOff();
    Event_SetActive(m_pTargetLight, false);
}

void CCandle::Action_State(_float _fTimeDelta)
{
    switch (m_eCurState)
    {
    case Client::CCandle::STATE_IDLE:
        Action_State_Idle(_fTimeDelta);
        break;
    case Client::CCandle::STATE_TURNON:
        Action_State_TurnOn(_fTimeDelta);
        break;
    case Client::CCandle::STATE_FLAMELOOP:
        Action_State_FlameLoop(_fTimeDelta);
        break;
    case Client::CCandle::STATE_TURNOFF:
        Action_State_TurnOff(_fTimeDelta);
        break;
    default:
        break;
    }
}

void CCandle::Action_State_Idle(_float _fTimeDelta)
{
}

void CCandle::Action_State_TurnOn(_float _fTimeDelta)
{
}

void CCandle::Action_State_FlameLoop(_float _fTimeDelta)
{
}

void CCandle::Action_State_TurnOff(_float _fTimeDelta)
{
}

HRESULT CCandle::Ready_Components()
{
    // 캔들 클래스에서 만들어야 하는 컴포넌트는 별도로 없다. 
    return S_OK;
}

HRESULT CCandle::Ready_PartObjects(CANDLE_DESC* _pDesc)
{
    // 1. Body 
    /* Part Body */
    CCandle_Body::MODELOBJECT_DESC BodyDesc{};
    BodyDesc.iCurLevelID = m_iCurLevelID;
    BodyDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);
    BodyDesc.Build_3D_Transform(_float3(0.0f, 0.0f, 0.0f), _float3(1.0f, 1.0f, 1.0f), _float3(0.0f, 0.0f, 0.0f));
    m_PartObjects[CANDLE_PART::CANDLE_BODY] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, m_iCurLevelID, TEXT("Prototype_GameObject_Candle_Body"), &BodyDesc));
    if (nullptr == m_PartObjects[CANDLE_PART::CANDLE_BODY])
        return E_FAIL;

    // 2. UI
    CCandle_UI::CANDLE_UI_DESC UIDesc{};
    UIDesc.iCurLevelID = m_iCurLevelID;
    UIDesc.iCurLevelID = m_iCurLevelID;
    UIDesc.fX = _pDesc->vCandleUIDesc.x;
    UIDesc.fY = _pDesc->vCandleUIDesc.y;
    UIDesc.fSizeX = _pDesc->vCandleUIDesc.z;
    UIDesc.fSizeY = _pDesc->vCandleUIDesc.w;

    m_PartObjects[CANDLE_PART::CANDLE_UI] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, m_iCurLevelID, TEXT("Prototype_GameObject_Candle_UI"), &UIDesc));
    if (nullptr == m_PartObjects[CANDLE_PART::CANDLE_UI])
        return E_FAIL;

    return S_OK;
}

HRESULT CCandle::Ready_TargetLight()
{
    ///* 점광원 */
    CONST_LIGHT LightDesc = {};
    LightDesc.vPosition = _float3(0.0f, 0.0f, 0.0f);
    LightDesc.fFallOutStart = 5.2f;
    LightDesc.fFallOutEnd = 13.0f;
    LightDesc.vRadiance = _float3(9.0f, 9.0f, 9.0f);
    LightDesc.vDiffuse = _float4(1.0f, 0.371f, 0.0f, 1.0f);
    LightDesc.vAmbient = _float4(0.6f, 0.6f, 0.6f, 1.0f);
    LightDesc.vSpecular = _float4(1.0f, 0.450f, 0.0f, 1.0f);

    if (FAILED(m_pGameInstance->Add_Light_Target(LightDesc, LIGHT_TYPE::POINT, this, _float3(0.0f, 2.0f, 0.0f), &m_pTargetLight, true)))
        return E_FAIL;

    Safe_AddRef(m_pTargetLight);
    m_pTargetLight->Set_Active(false);

    return S_OK;
}

CCandle* CCandle::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CCandle* pInstance = new CCandle(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CCandle");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCandle* CCandle::Clone(void* _pArg)
{
    CCandle* pInstance = new CCandle(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CCandle");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCandle::Free()
{
    Safe_Release(m_pTargetLight);
    __super::Free();
}
