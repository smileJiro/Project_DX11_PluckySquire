#include "PartObject.h"
#include "GameInstance.h"

CPartObject::CPartObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CGameObject(_pDevice, _pContext)
{
}

CPartObject::CPartObject(const CPartObject& _Prototype)
    : CGameObject(_Prototype)
{
}

HRESULT CPartObject::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CPartObject::Initialize(void* _pArg)
{
    PARTOBJECT_DESC* pDesc = static_cast<PARTOBJECT_DESC*>(_pArg);

    m_pParentMatrices[COORDINATE_2D] = pDesc->pParentMatrices[COORDINATE_2D];
    m_pParentMatrices[COORDINATE_3D] = pDesc->pParentMatrices[COORDINATE_3D];

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    return S_OK;
}

void CPartObject::Priority_Update(_float _fTimeDelta)
{
}

void CPartObject::Update(_float _fTimeDelta)
{
    switch (m_pControllerTransform->Get_CurCoord())
    {
    case Engine::COORDINATE_2D:
        if (nullptr != m_pParentMatrices[COORDINATE_2D])
            XMStoreFloat4x4(&m_WorldMatrices[COORDINATE_2D], m_pControllerTransform->Get_WorldMatrix(COORDINATE_2D) * XMLoadFloat4x4(m_pParentMatrices[COORDINATE_2D]));
        else
            XMStoreFloat4x4(&m_WorldMatrices[COORDINATE_2D], m_pControllerTransform->Get_WorldMatrix(COORDINATE_2D));
        break;
    case Engine::COORDINATE_3D:
        if(nullptr != m_pParentMatrices[COORDINATE_3D])
            XMStoreFloat4x4(&m_WorldMatrices[COORDINATE_3D], m_pControllerTransform->Get_WorldMatrix(COORDINATE_3D) * XMLoadFloat4x4(m_pParentMatrices[COORDINATE_3D]));
        else
            XMStoreFloat4x4(&m_WorldMatrices[COORDINATE_3D], m_pControllerTransform->Get_WorldMatrix(COORDINATE_3D));
        break;
    default:
        break;
    }
}

void CPartObject::Late_Update(_float _fTimeDelta)
{
    switch (m_pControllerTransform->Get_CurCoord())
    {
    case Engine::COORDINATE_2D:
        if (nullptr != m_pParentMatrices[COORDINATE_2D])
            XMStoreFloat4x4(&m_WorldMatrices[COORDINATE_2D], m_pControllerTransform->Get_WorldMatrix(COORDINATE_2D) * XMLoadFloat4x4(m_pParentMatrices[COORDINATE_2D]));
        else
            XMStoreFloat4x4(&m_WorldMatrices[COORDINATE_2D], m_pControllerTransform->Get_WorldMatrix(COORDINATE_2D));
        break;
    case Engine::COORDINATE_3D:
        if (nullptr != m_pParentMatrices[COORDINATE_3D])
            XMStoreFloat4x4(&m_WorldMatrices[COORDINATE_3D], m_pControllerTransform->Get_WorldMatrix(COORDINATE_3D) * XMLoadFloat4x4(m_pParentMatrices[COORDINATE_3D]));
        else
            XMStoreFloat4x4(&m_WorldMatrices[COORDINATE_3D], m_pControllerTransform->Get_WorldMatrix(COORDINATE_3D));
        break;
    default:
        break;
    }
}

HRESULT CPartObject::Render()
{
    return S_OK;
}

void CPartObject::Free()
{
    for (_int i = 0; i < COORDINATE_LAST; ++i)
        m_pParentMatrices[i] = nullptr;
        
    __super::Free();
}

HRESULT CPartObject::Cleanup_DeadReferences()
{

    return S_OK;
}
