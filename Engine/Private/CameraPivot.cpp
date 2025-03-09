#include "CameraPivot.h"
#include "GameInstance.h"

CCameraPivot::CCameraPivot(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CGameObject(_pDevice, _pContext)
{
}

CCameraPivot::CCameraPivot(const CCameraPivot& _Prototype)
    : CGameObject(_Prototype)
{
}

HRESULT CCameraPivot::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCameraPivot::Initialize(void* _pArg)
{
    if (nullptr == _pArg)
        return E_FAIL;

    CCameraPivot::CAMERAPIVOT_DESC* pDesc = static_cast<CCameraPivot::CAMERAPIVOT_DESC*>(_pArg);

    pDesc->eStartCoord = COORDINATE_3D;
    pDesc->isCoordChangeEnable = false;

    m_pMainTarget = pDesc->pMainTaget;
    m_pSubTarget = pDesc->pSubTarget;
    m_fRatio = pDesc->fRatio;

    m_fRatio = 0.3f;

    if(nullptr != m_pMainTarget)
        Safe_AddRef(m_pMainTarget);
    if (nullptr != m_pSubTarget)
        Safe_AddRef(m_pSubTarget);

    // Transform 먼저 생성.
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    return S_OK;
}

void CCameraPivot::Priority_Update(_float _fTimeDelta)
{
}

void CCameraPivot::Update(_float _fTimeDelta)
{
    Calculate_FinalPosition();
}

void CCameraPivot::Late_Update(_float _fTimeDelta)
{
}

HRESULT CCameraPivot::Render()
{
    return S_OK;
}

void CCameraPivot::Set_MainTarget(CGameObject* _pMainTarget)
{
    Safe_Release(m_pMainTarget);

    m_pMainTarget = _pMainTarget;

    Safe_AddRef(m_pMainTarget);

    Calculate_FinalPosition();
}

void CCameraPivot::Set_SubTarget(CGameObject* _pSubTarget)
{
    Safe_Release(m_pSubTarget);

    m_pSubTarget = _pSubTarget;

    Safe_AddRef(m_pSubTarget);

    Calculate_FinalPosition();
}

void CCameraPivot::Calculate_FinalPosition()
{
    m_fRatio = 0.7f;

    if (nullptr == m_pMainTarget || nullptr == m_pSubTarget)
        return;

    _vector vMainTargetPos = m_pMainTarget->Get_FinalPosition();
    _vector vSubTargetPos = m_pSubTarget->Get_FinalPosition();

    _vector vDir = XMVector3Normalize(vSubTargetPos - vMainTargetPos);
    _float vDistance = XMVectorGetX(XMVector3Length(vSubTargetPos - vMainTargetPos));

    _vector vPivotPos = vMainTargetPos + (vDir * (vDistance * m_fRatio));

    m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vPivotPos, 1.f));
}

CCameraPivot* CCameraPivot::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CCameraPivot* pInstance = new CCameraPivot(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CCameraPivot");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCameraPivot::Clone(void* _pArg)
{
    CCameraPivot* pInstance = new CCameraPivot(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CCameraPivot");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCameraPivot::Free()
{
    Safe_Release(m_pMainTarget);
    Safe_Release(m_pSubTarget);

    __super::Free();
}

HRESULT CCameraPivot::Cleanup_DeadReferences()
{
    if (nullptr != m_pMainTarget) {
        if (true == m_pMainTarget->Is_Dead())
        {
            Safe_Release(m_pMainTarget);
            m_pMainTarget = nullptr;
        }
    }

    if (nullptr != m_pSubTarget) {
        if (true == m_pSubTarget->Is_Dead())
        {
            Safe_Release(m_pSubTarget);
            m_pSubTarget = nullptr;
        }
    }

    return S_OK;
}
