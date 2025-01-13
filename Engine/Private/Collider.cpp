#include "Collider.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Bounding_AABB.h"

_uint CCollider::g_iNextColliderID = 0;
CCollider::CCollider(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CComponent{ _pDevice, _pContext }
{
    XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixIdentity());
}

CCollider::CCollider(const CCollider& _Prototype)
    : CComponent(_Prototype)
    , m_eType(_Prototype.m_eType)
    , m_iColliderID(g_iNextColliderID++) /* Collider ID 부여. */
    , m_OffsetMatrix(_Prototype.m_OffsetMatrix)
{
#ifdef _DEBUG
    m_pInputLayout = _Prototype.m_pInputLayout;
    m_pBatch = _Prototype.m_pBatch;
    m_pEffect = _Prototype.m_pEffect;
    Safe_AddRef(m_pInputLayout);
#endif // _DEBUG

}

HRESULT CCollider::Initialize_Prototype(TYPE _eColliderType)
{
    /* 원본 객체 생성 시 렌더링 관련 된 변수들만 초기화*/
    m_eType = _eColliderType;
   
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
    CCollider::COLLIDER_DESC* pCollDesc = static_cast<COLLIDER_DESC*>(_pArg);
    m_eUse = pCollDesc->eUse;
    /* _pArg에서 들어온 Bounding Type을 확인하여 그 타입에 맞는 바운딩 박스 클래스를 생성할 것. */
    CBounding::BOUND_DESC* pDesc = static_cast<CBounding::BOUND_DESC*>(_pArg);
    switch (m_eType)
    {
    case Engine::CCollider::SPHERE:
        m_pBounding = CBounding_Sphere::Create(m_pDevice, m_pContext, pDesc);
        break;
    case Engine::CCollider::AABB:
        m_pBounding = CBounding_AABB::Create(m_pDevice, m_pContext, pDesc);
        break;
    case Engine::CCollider::OBB:
        m_pBounding = CBounding_OBB::Create(m_pDevice, m_pContext, pDesc);
        break;
    default:
        break;
    }

    return S_OK;
}


void CCollider::Update(_fmatrix _OwnerWorldMatrix)
{
    if (false == m_isActive)
        return;
    m_pBounding->Update(XMLoadFloat4x4(&m_OffsetMatrix) * _OwnerWorldMatrix);
}

#ifdef _DEBUG

HRESULT CCollider::Render()
{
    if (false == m_isActive)
        return S_OK;
    /* dx9 처럼 W,V,P 행렬을 던져준다. */

        /* World Matrix를 Indentity로 던지는 이유 : 이미 각각의 CBounding* 가 소유한 BoundingDesc의 정점 자체를 이미 World까지 변환 할 것임. */
    m_pEffect->SetWorld(XMMatrixIdentity());
    m_pEffect->SetView(m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
    m_pEffect->SetProjection(m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));

    m_pEffect->Apply(m_pContext);
    m_pContext->IASetInputLayout(m_pInputLayout);

    m_pBatch->Begin();
    _vector vColor = {};
    switch (m_eCollision)
    {
    case Engine::CCollider::NONE:
        vColor = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
        break;
    case Engine::CCollider::COLLISION:
        vColor = XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f);
        break;
    case Engine::CCollider::COLLISION_RAY:
        vColor = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);
        break;
    case Engine::CCollider::CONTAINS:
        vColor = XMVectorSet(1.0f, 1.0f, 0.0f, 1.0f);
        break;
    default:
        break;
    }
    m_pBounding->Render(m_pBatch, vColor);

    m_pBatch->End();



    return S_OK;
}

#endif

_bool CCollider::Intersect(const CCollider* _pTargetCollider)
{
    if (false == m_isActive)
        return false;
    return m_pBounding->Intersect(_pTargetCollider->m_eType, _pTargetCollider->m_pBounding);
}

_bool CCollider::Intersect_Ray(_fvector _vRayStart, _fvector _vRayDir, _float* _pOutDst)
{
    if (false == m_isActive)
        return false;
    return m_pBounding->Intersect_Ray(_vRayStart, XMVector3Normalize(_vRayDir), _pOutDst);
}

_bool CCollider::Contains(_fvector _vTargetPos)
{
    if (false == m_isActive)
        return false;
    return m_pBounding->Contains(_vTargetPos);
}



CCollider* CCollider::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, TYPE _eColliderType)
{
    CCollider* pInstance = new CCollider(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype(_eColliderType)))
    {
        MSG_BOX("Failed to Created : CCollider");
        Safe_Release(pInstance);
    }
    return pInstance;
}
CComponent* CCollider::Clone(void* _pArg)
{
    CCollider* pInstance = new CCollider(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CCollider");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCollider::Free()
{
#ifdef _DEBUG

    Safe_Release(m_pInputLayout);

    if (false == m_isCloned)
    {
        Safe_Delete(m_pBatch);
        Safe_Delete(m_pEffect);
    }

#endif // _DEBUG

    Safe_Release(m_pBounding);


    __super::Free();
}
