#include "Collider.h"
#include "GameInstance.h"

_uint CCollider::s_iNextColliderID = 0;
CCollider::CCollider(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CComponent(_pDevice, _pContext)
{
}

CCollider::CCollider(const CCollider& _Prototype)
    :CComponent(_Prototype)
    ,m_iColliderID(s_iNextColliderID++)
{
}

HRESULT CCollider::Initialize_Prototype()
{
    
    return S_OK;
}

HRESULT CCollider::Initialize(void* _pArg)
{
    COLLIDER_DESC* pDesc = static_cast<COLLIDER_DESC*>(_pArg);

    if (nullptr == pDesc->pOwner)
        return E_FAIL;

    // Save Desc
    m_vPosition = pDesc->vPosition;
    m_vOffsetPosition = pDesc->vOffsetPosition;
    m_vScale = pDesc->vScale;

    // Add Desc 

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    return S_OK;
}

void CCollider::Update(_float _fTimeDelta)
{
}

void CCollider::Late_Update(_float _fTimeDelta)
{
}

HRESULT CCollider::Render()
{
    return S_OK;
}

void CCollider::Free()
{

    __super::Free();
}
