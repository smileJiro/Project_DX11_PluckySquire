#include "stdafx.h"
#include "Turret.h"
#include "GameInstance.h"
#include "Pooling_Manager.h"
#include "Bomb.h"
#include "PlayerData_Manager.h"
#include "Section_Manager.h"

CTurret::CTurret(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CModelObject(_pDevice, _pContext)
{
}

CTurret::CTurret(const CTurret& _Prototype)
    :CModelObject(_Prototype)
{
}

HRESULT CTurret::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTurret::Initialize(void* _pArg)
{
    TURRET_DESC* pDesc = static_cast<TURRET_DESC*>(_pArg);

    // Save Desc

    // Add Desc
    pDesc->Build_2D_Model(pDesc->iCurLevelID, TEXT("Excavator_Turret"), TEXT("Prototype_Component_Shader_VtxPosTex"));
    pDesc->eStartCoord = COORDINATE_2D;
    pDesc->iObjectGroupID = OBJECT_GROUP::GIMMICK_OBJECT;
    pDesc->isCoordChangeEnable = false;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    /* Set Anim End */
    Register_OnAnimEndCallBack(bind(&CTurret::On_AnimEnd, this, placeholders::_1, placeholders::_2));

    return S_OK;
}

void CTurret::Priority_Update(_float _fTimeDelta)
{

    __super::Priority_Update(_fTimeDelta);
}

void CTurret::Update(_float _fTimeDelta)
{
    Action_State(_fTimeDelta);
    __super::Update(_fTimeDelta);
}

void CTurret::Late_Update(_float _fTimeDelta)
{
    State_Change();
    __super::Late_Update(_fTimeDelta);
}

HRESULT CTurret::Render()
{
    return __super::Render();
}

void CTurret::State_Change()
{
    if (m_ePreState == m_eCurState)
        return;

    switch (m_eCurState)
    {
    case Client::CTurret::STATE_CLOSE:
        State_Change_Close();
        break;
    case Client::CTurret::STATE_LOWER:
        State_Change_Lower();
        break;
    case Client::CTurret::STATE_OPEN:
        State_Change_Open();
        break;
    case Client::CTurret::STATE_RISE:
        State_Change_Rise();
        break;
    case Client::CTurret::STATE_FIRE:
        State_Change_Fire();
        break;
    case Client::CTurret::STATE_FIRE_INTO:
        State_Change_Fire_Into();
        break;
    default:
        break;
    }

    m_ePreState = m_eCurState;
}

void CTurret::State_Change_Close()
{
    Switch_Animation(STATE::STATE_CLOSE);
}

void CTurret::State_Change_Lower()
{
    Switch_Animation(STATE::STATE_LOWER);
}

void CTurret::State_Change_Open()
{
    m_vBombCoolTime.y = 0.0f;
    Switch_Animation(STATE::STATE_OPEN);
}

void CTurret::State_Change_Rise()
{
    Switch_Animation(STATE::STATE_RISE);
}

void CTurret::State_Change_Fire()
{
    Switch_Animation(STATE::STATE_FIRE);

    CPlayer* pPlayer = CPlayerData_Manager::GetInstance()->Get_NormalPlayer_Ptr();
    if (nullptr == pPlayer)
    {
        m_eCurState = STATE_LOWER;
        State_Change();
    }
    else
    {
        _vector vPos = Get_FinalPosition();
        _float3 vPosition = {};
        XMStoreFloat3(&vPosition, vPos);
        CGameObject* pGameObject = nullptr;
        _vector vPlayerPos = pPlayer->Get_FinalPosition();
        if (COORDINATE_2D == pPlayer->Get_CurCoord())
        {
            CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_Bomb"), COORDINATE_2D, &pGameObject, &vPosition, nullptr, nullptr, &m_strSectionName);
            CBomb* pBomb = static_cast<CBomb*>(pGameObject);
            static_cast<CBomb*>(pGameObject)->Start_Parabola(vPos, vPlayerPos, 1.0f);
        }
        else
        {
            _vector v3DPos = CSection_Manager::GetInstance()->Get_WorldPosition_FromWorldPosMap(_float2(XMVectorGetX(vPos), XMVectorGetY(vPos)));
            XMStoreFloat3(&vPosition, v3DPos + XMVectorSet(0.0f, 0.7f, 0.0f, 1.0f));
            CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_Bomb"), COORDINATE_3D, &pGameObject, &vPosition);
            CBomb* pBomb = static_cast<CBomb*>(pGameObject); 
            pBomb->Set_Kinematic(false);
            pBomb->Start_Parabola_3D(vPlayerPos, XMConvertToRadians(30.f));
        }
    }



    

}

void CTurret::State_Change_Fire_Into()
{
    Switch_Animation(STATE::STATE_FIRE_INTO);
}

void CTurret::Action_State(_float _fTimeDelta)
{
    switch (m_eCurState)
    {
    case Client::CTurret::STATE_CLOSE:
        Action_State_Close(_fTimeDelta);
        break;
    case Client::CTurret::STATE_LOWER:
        Action_State_Lower(_fTimeDelta);
        break;
    case Client::CTurret::STATE_OPEN:
        Action_State_Open(_fTimeDelta);
        break;
    case Client::CTurret::STATE_RISE:
        Action_State_Rise(_fTimeDelta);
        break;
    case Client::CTurret::STATE_FIRE:
        Action_State_Fire(_fTimeDelta);
        break;
    case Client::CTurret::STATE_FIRE_INTO:
        Action_State_Fire_Into(_fTimeDelta);
        break;
    default:
        break;
    }
}

void CTurret::Action_State_Close(_float _fTimeDelta)
{
}

void CTurret::Action_State_Lower(_float _fTimeDelta)
{
}

void CTurret::Action_State_Open(_float _fTimeDelta)
{
    m_vBombCoolTime.y += _fTimeDelta;

    if (m_vBombCoolTime.x <= m_vBombCoolTime.y)
    {
        /* Fire Into 로 전환 */
        m_eCurState = STATE_FIRE_INTO;
        m_vBombCoolTime.y = 0.0f;
    }
}

void CTurret::Action_State_Rise(_float _fTimeDelta)
{
}

void CTurret::Action_State_Fire(_float _fTimeDelta)
{
    /* 플레이어 coord 확인 */

    /* 플레이어 위치 기반으로 방향 잡고 발사 */

}

void CTurret::Action_State_Fire_Into(_float _fTimeDelta)
{
}

void CTurret::On_AnimEnd(COORDINATE _eCoord, _uint _iAnimIdx)
{
    switch (m_eCurState)
    {
    case Client::CTurret::STATE_CLOSE:
        break;
    case Client::CTurret::STATE_LOWER:
        m_eCurState = STATE_CLOSE;
        break;
    case Client::CTurret::STATE_OPEN:
        break;
    case Client::CTurret::STATE_RISE:
        m_eCurState = STATE_OPEN;
        break;
    case Client::CTurret::STATE_FIRE:
        m_eCurState = STATE_OPEN;
        break;
    case Client::CTurret::STATE_FIRE_INTO:
        m_eCurState = STATE_FIRE;
        break;
    default:
        break;
    }
}

CTurret* CTurret::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CTurret* pInstance = new CTurret(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CTurret");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTurret::Clone(void* _pArg)
{
    CTurret* pInstance = new CTurret(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CTurret");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTurret::Free()
{

    __super::Free();
}
