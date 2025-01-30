#include "Collider_AABB.h"
#include "GameObject.h"
#include "Collider_Circle.h"
#include "GameInstance.h"
#include "DebugDraw.h"

CCollider_AABB::CCollider_AABB(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CCollider(_pDevice, _pContext)
{
}

CCollider_AABB::CCollider_AABB(const CCollider_AABB& _Prototype)
    :CCollider(_Prototype)
{
}

HRESULT CCollider_AABB::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    m_eType = TYPE::AABB_2D;

    return S_OK;
}

HRESULT CCollider_AABB::Initialize(void* _pArg)
{
    COLLIDER_AABB_DESC* pDesc = static_cast<COLLIDER_AABB_DESC*>(_pArg);
    // Save Desc 
    m_vExtents = pDesc->vExtents;

    // Add Desc 

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;


    // Scale 반영
    _float3 vOwnerScale = m_pOwner->Get_FinalScale();
    XMStoreFloat2(&m_vFinalExtents, XMLoadFloat2(&m_vExtents) * XMLoadFloat2(&m_vScale) * XMVectorSet(vOwnerScale.x, vOwnerScale.y, 0.0f, 0.0f));
    return S_OK;
}

void CCollider_AABB::Update(_float _fTimeDelta)
{
    if (nullptr == m_pOwner)
        return;

    Update_OwnerTransform();
}

void CCollider_AABB::Late_Update(_float _fTimeDelta)
{
    if (nullptr == m_pOwner)
        return;

    Update_OwnerTransform();

//#ifdef _DEBUG
//    m_pGameInstance->Add_DebugComponent(this);
//#endif // _DEBUG
}

#ifdef _DEBUG
HRESULT CCollider_AABB::Render()
{
    m_pEffect->SetWorld(XMMatrixIdentity());
    m_pEffect->SetView(XMMatrixIdentity());
    _float2 vRTSize = m_pGameInstance->Get_RT_Size(TEXT("Target_Book_2D"));
    m_pEffect->SetProjection(XMMatrixOrthographicLH(vRTSize.x, vRTSize.y, 0.0f, 1.0f));
    m_pEffect->Apply(m_pContext);
    m_pContext->IASetInputLayout(m_pInputLayout);

    m_pBatch->Begin();

    static BoundingBox BoundingBox = {};
    BoundingBox.Center = _float3(m_vPosition.x, m_vPosition.y, 0.0f);
    BoundingBox.Extents = _float3(m_vExtents.x, m_vExtents.y, 0.0f);
    DX::Draw(m_pBatch, BoundingBox, XMLoadFloat4(&m_vDebugColor));

    m_pBatch->End();

    return S_OK;
}
#endif // _DEBUG



_bool CCollider_AABB::Is_Collision(CCollider* _pOther)
{
    _bool isResult = false;
    switch (_pOther->Get_Type())
    {
    case Engine::CCollider::CIRCLE_2D:
        isResult = Is_Collision_Circle(static_cast<CCollider_Circle*>(_pOther));
        break;
    case Engine::CCollider::AABB_2D:
        isResult = Is_Collision_AABB(static_cast<CCollider_AABB*>(_pOther));
        break;
    default:
        return isResult;
    }

    return isResult;
}

void CCollider_AABB::Update_OwnerTransform()
{
    _vector vOwnerPos = m_pOwner->Get_Position();
    XMStoreFloat2(&m_vPosition, XMLoadFloat2(&m_vOffsetPosition) + vOwnerPos);

    _float3 vOwnerScale = m_pOwner->Get_FinalScale();
    XMStoreFloat2(&m_vFinalExtents, XMLoadFloat2(&m_vExtents) * XMLoadFloat2(&m_vScale) * XMVectorSet(vOwnerScale.x, vOwnerScale.y, 0.0f, 0.0f));
}

_bool CCollider_AABB::Is_Collision_AABB(CCollider_AABB* _pOther)
{
    _float2 vLT = Get_LT();
    _float2 vRB = Get_RB();
    _float2 vOtherLT = _pOther->Get_LT();
    _float2 vOtherRB = _pOther->Get_RB();

    if (vLT.x > vOtherRB.x || vRB.x < vOtherLT.x)
        return false;

    if (vLT.y < vOtherRB.y || vRB.y > vOtherLT.y)
        return false;

    return true;
}

_bool CCollider_AABB::Is_Collision_Circle(CCollider_Circle* _pOther)
{
    _float2 vLT = Get_LT(); // Min
    _float2 vRB = Get_RB(); // Max

    _float2 vOtherPosition = _pOther->Get_Position();
    _float fOtherFinalRadius = _pOther->Get_FinalRadius();

    // 1. Clamp를 통해 AABB에 가장 가까운 점을 찾는다.
    _float2 vNearestPoint = {};
    vNearestPoint.x = clamp(vOtherPosition.x, vLT.x, vRB.x);
    vNearestPoint.y = clamp(vOtherPosition.y, vLT.y, vRB.y);

    // 2. 원의 중심과 가장 가까운 점 사이의 거리를 구한다.
    // 3. 그 거리와 원의 반지름을 비교한다. 
    _vector vDiff = XMLoadFloat2(&vOtherPosition) - XMLoadFloat2(&vNearestPoint);
    _float fDiffSquared = XMVectorGetX(XMVector2Dot(vDiff, vDiff));

    return fDiffSquared <= fOtherFinalRadius * fOtherFinalRadius;
}

_float2 CCollider_AABB::Get_LT()
{
    _float2 vLT = {};
    vLT.x = m_vPosition.x - m_vFinalExtents.x * 0.5f;
    vLT.y = m_vPosition.y - m_vFinalExtents.y * 0.5f;

    return vLT;
}

_float2 CCollider_AABB::Get_RB()
{
    _float2 vRB = {};
    vRB.x = m_vPosition.x + m_vFinalExtents.x * 0.5f;
    vRB.y = m_vPosition.y + m_vFinalExtents.y * 0.5f;

    return vRB;
}

CCollider_AABB* CCollider_AABB::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CCollider_AABB* pInstance = new CCollider_AABB(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed Create CCollider_AABB");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CComponent* CCollider_AABB::Clone(void* _pArg)
{
    CCollider_AABB* pInstance = new CCollider_AABB(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CCollider_AABB");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCollider_AABB::Free()
{

    __super::Free();
}
