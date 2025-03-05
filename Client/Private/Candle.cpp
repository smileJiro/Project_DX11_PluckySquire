#include "stdafx.h"
#include "Candle.h"
#include "GameInstance.h"
#include "Candle_Body.h"

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
    CCandle::CONTAINEROBJ_DESC* pDesc = static_cast<CCandle::CONTAINEROBJ_DESC*>(_pArg);
    pDesc->eStartCoord = COORDINATE_3D;
    pDesc->isCoordChangeEnable = false;
    pDesc->iNumPartObjects = (_uint)CANDLE_PART::CANDLE_LAST;
    pDesc->iObjectGroupID = TRIGGER_OBJECT;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    return S_OK;
}

void CCandle::Priority_Update(_float _fTimeDelta)
{

    __super::Priority_Update(_fTimeDelta);
}

void CCandle::Update(_float _fTimeDelta)
{
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
}

void CCandle::State_Change_TurnOn()
{
    static_cast<CCandle_Body*>(m_PartObjects[CANDLE_BODY])->State_Change_TurnOn();
}

void CCandle::State_Change_FlameLoop()
{
    static_cast<CCandle_Body*>(m_PartObjects[CANDLE_BODY])->State_Change_FlameLoop();
}

void CCandle::State_Change_TurnOff()
{
    static_cast<CCandle_Body*>(m_PartObjects[CANDLE_BODY])->State_Change_TurnOff();
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

HRESULT CCandle::Ready_PartObjects()
{
    // 1. Body 
    /* Part Body */
    CCandle_Body::MODELOBJECT_DESC BodyDesc{};
    BodyDesc.iCurLevelID = m_iCurLevelID;
    BodyDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);
    BodyDesc.Build_3D_Transform(_float3(0.0f, 0.0f, 0.0f), _float3(1.0f, 1.0f, 1.0f), _float3(0.0f, 0.0f, 0.0f));
    m_PartObjects[CANDLE_PART::CANDLE_BODY] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, m_iCurLevelID, TEXT("Prototype_GameObject_Candle_Body"), &BodyDesc));
    if (nullptr == m_PartObjects[PART_BODY])
        return E_FAIL;

    // 2. UI


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

    __super::Free();
}
