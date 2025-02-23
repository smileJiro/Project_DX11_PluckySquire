#include "Gravity.h"
#include "GameInstance.h"

CGravity::CGravity(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CComponent(_pDevice, _pContext)
{
}

CGravity::CGravity(const CGravity& _Prototype)
    :CComponent(_Prototype)
{
}

HRESULT CGravity::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CGravity::Initialize(void* _pArg)
{
    GRAVITY_DESC* pDesc = static_cast<GRAVITY_DESC*>(_pArg);
    // Save Desc
    m_pOwner = pDesc->pOwner;
    m_fGravity = pDesc->fGravity;
    m_vDirection = pDesc->vGravityDirection;
    m_fMaxGravityAcc = pDesc->fMaxGravityAcc;
    XMStoreFloat3(&m_vDirection, XMVector3Normalize(XMLoadFloat3(&m_vDirection)));

    return S_OK;
}

void CGravity::Priority_Update(_float _fTimeDelta)
{
    /* 상태 변경 */
    State_Change();

    /* 중력 적용 */
    Action_State(_fTimeDelta);
}

void CGravity::Change_State(STATE _eState)
{
    m_eCurState = _eState;
}


void CGravity::State_Change()
{
    if (m_ePreState == m_eCurState)
        return;

    switch (m_eCurState)
    {
    case Engine::CGravity::STATE_FLOOR:
        m_fGravityAcc = 0.0f;
        break;
    case Engine::CGravity::STATE_FALLDOWN:
        m_fGravityAcc = 0.0f;
        break;
    default:
        break;
    }

    m_ePreState = m_eCurState;
}

void CGravity::Action_State(_float _fTimeDelta)
{
    switch (m_eCurState)
    {
    case Engine::CGravity::STATE_FLOOR:
        Action_Floor(_fTimeDelta);
        break;
    case Engine::CGravity::STATE_FALLDOWN:
        Action_FallDown(_fTimeDelta);
        break;
    default:
        break;
    }
}

void CGravity::Action_Floor(_float _fTimeDelta)
{
}

void CGravity::Action_FallDown(_float _fTimeDelta)
{
    m_fGravityAcc += (m_fGravity + m_fGravityOffset) * _fTimeDelta;

    //if (m_fMaxGravityAcc <= m_fGravityAcc)
    //    m_fGravityAcc = m_fMaxGravityAcc;

    _vector vForce = XMLoadFloat3(&m_vDirection) * m_fGravityAcc;
    _vector vOwnerPos = m_pOwner->Get_ControllerTransform()->Get_State(CTransform::STATE_POSITION);
    vOwnerPos += vForce * _fTimeDelta;
    
    m_pOwner->Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, vOwnerPos);
}

CGravity* CGravity::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CGravity* pInstance = new CGravity(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed Create : CGravity");
        return nullptr;
    }

    return pInstance;
}

CComponent* CGravity::Clone(void* _pArg)
{
    CGravity* pInstance = new CGravity(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CGravity");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CGravity::Free()
{
    m_pOwner = nullptr;

    __super::Free();
}
