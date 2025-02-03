#include "Collider.h"
#include "GameInstance.h"

#include "DebugDraw.h"
_uint CCollider::s_iNextColliderID = 0;
CCollider::CCollider(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CComponent(_pDevice, _pContext)
{
}

CCollider::CCollider(const CCollider& _Prototype)
    :CComponent(_Prototype)
    ,m_iColliderID(s_iNextColliderID++)
    ,m_eType(_Prototype.m_eType)
{
#ifdef _DEBUG
    m_pInputLayout = _Prototype.m_pInputLayout;
    m_pBatch = _Prototype.m_pBatch;
    m_pEffect = _Prototype.m_pEffect;
    Safe_AddRef(m_pInputLayout);
#endif // _DEBUG
}

HRESULT CCollider::Initialize_Prototype()
{
#ifdef _DEBUG
    m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
    m_pEffect = new BasicEffect(m_pDevice);

    const void* pShaderByteCode = nullptr;
    size_t iShaderByteCodeLength = 0;

    /* m_pEffect 쉐이더의 옵션 자체를 Color 타입으로 변경해주어야 정상적인 동작을 한다. (기본 값은 Texture )*/
    m_pEffect->SetVertexColorEnabled(true);
    m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

    if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout)))
        return E_FAIL;
#endif // _DEBUG

    return S_OK;
}

HRESULT CCollider::Initialize(void* _pArg)
{
    COLLIDER_DESC* pDesc = static_cast<COLLIDER_DESC*>(_pArg);

    if (nullptr == pDesc->pOwner)
        return E_FAIL;

    // Save Desc
    m_pOwner = pDesc->pOwner;
    m_vPosition = pDesc->vPosition;
    m_vOffsetPosition = pDesc->vOffsetPosition;
    m_vScale = pDesc->vScale;
    m_isBlock = pDesc->isBlock;

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

void CCollider::Block(CCollider* _pOther)
{
}

void CCollider::Free()
{
    m_pOwner = nullptr;

#ifdef _DEBUG
    Safe_Release(m_pInputLayout);

    if (false == m_isCloned)
    {
        Safe_Delete(m_pBatch);
        Safe_Delete(m_pEffect);
    }
#endif // _DEBUG

    __super::Free();
}
