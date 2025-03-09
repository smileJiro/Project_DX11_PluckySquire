#include "Collider_Circle.h"
#include "GameObject.h"
#include "Collider_AABB.h"
#include "GameInstance.h"
#include "DebugDraw.h"

CCollider_Circle::CCollider_Circle(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CCollider(_pDevice, _pContext)
{
}

CCollider_Circle::CCollider_Circle(const CCollider_Circle& _Prototype)
    :CCollider(_Prototype)
{
}

HRESULT CCollider_Circle::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;


    m_eType = TYPE::CIRCLE_2D;
    return S_OK;
}

HRESULT CCollider_Circle::Initialize(void* _pArg)
{
    COLLIDER_CIRCLE_DESC* pDesc = static_cast<COLLIDER_CIRCLE_DESC*>(_pArg);
    // Save Desc
    m_fRadius = pDesc->fRadius;

    // Add Desc
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    // Scale 반영
    _float3 vOwnerScale = m_pOwner->Get_FinalScale(COORDINATE_2D);
    m_fFinalRadius = m_fRadius * m_vScale.x * vOwnerScale.x;

    return S_OK;
}

void CCollider_Circle::Update(_float _fTimeDelta)
{
    if (nullptr == m_pOwner)
        return;

    Update_OwnerTransform();

    __super::Update(_fTimeDelta);
}

void CCollider_Circle::Late_Update(_float _fTimeDelta)
{
    if (nullptr == m_pOwner)
        return;

    Update_OwnerTransform();

    __super::Late_Update(_fTimeDelta);
//#ifdef _DEBUG
//    m_pGameInstance->Add_DebugComponent(this);
//#endif // _DEBUG

}

#ifdef _DEBUG

HRESULT CCollider_Circle::Render(_float2 _fRenderTargetSize)
{
   /* m_pEffect->SetWorld(XMMatrixIdentity());
    m_pEffect->SetView(XMMatrixIdentity());
    m_pEffect->SetProjection(XMMatrixOrthographicLH(_fRenderTargetSize.x, _fRenderTargetSize.y, 0.0f, 1.0f));
    m_pEffect->Apply(m_pContext);
    m_pContext->IASetInputLayout(m_pInputLayout);

    m_pBatch->Begin();

    static BoundingSphere BoundingSphere = {};
    BoundingSphere.Center = _float3(m_vPosition.x, m_vPosition.y, 0.0f);
    BoundingSphere.Radius = m_fFinalRadius;
    DX::Draw(m_pBatch, BoundingSphere, XMLoadFloat4(&m_vDebugColor));

    m_pBatch->End();*/

    return S_OK;
}
#endif // _DEBUG


_bool CCollider_Circle::Is_Collision(CCollider* _pOther)
{
    if (false == m_isActive)
        return false;

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

_bool CCollider_Circle::Is_ContainsPoint(_float2 _vPosition)
{
    if (false == m_isActive)
        return false;

    _float fDistance = XMVector2Length(XMLoadFloat2(&_vPosition)-XMLoadFloat2(&m_vPosition)).m128_f32[0];

    return fDistance < m_fFinalRadius;
}


void CCollider_Circle::Update_OwnerTransform()
{
    _vector vOwnerPos = m_pOwner->Get_FinalPosition(COORDINATE_2D);
    XMStoreFloat2(&m_vPosition, XMLoadFloat2(&m_vOffsetPosition) + vOwnerPos);

    _float3 vOwnerScale = m_pOwner->Get_FinalScale();
    m_fFinalRadius = m_fRadius * m_vScale.x * vOwnerScale.x;
}

void CCollider_Circle::Block(CCollider* _pOther)
{

    switch (_pOther->Get_Type())
    {
    case Engine::CCollider::CIRCLE_2D:
        Block_Circle(static_cast<CCollider_Circle*>(_pOther));
        break;
    case Engine::CCollider::AABB_2D:
        Block_AABB(static_cast<CCollider_AABB*>(_pOther));
        break;
    default:
        break;
    }
}

void CCollider_Circle::Block_AABB(CCollider_AABB* _pOther)
{
    // 내가 서클임. 밀어내는 방향만 뒤집어
    
    // 1. circle 기준 aabb 에 가장 가까운 점을 찾는다. 
    // 1. Clamp를 통해 AABB에 가장 가까운 점을 찾는다.
    _float2 vNearestPoint = {};
    _bool isResult = Compute_NearestPoint_AABB(_pOther, &vNearestPoint);
    if (false == isResult)
        return;

    _float2 vPosition = m_vPosition;
    _float fFinalRadius = Get_FinalRadius();

    // 2. circle의 중점에서부터 aabb에 가장 가까운 점까지의 거리를 구한다. (밀려나야 할 축으로도 사용가능하다 뒤집으면 )
    _vector vDiff =  XMLoadFloat2(&vNearestPoint) - XMLoadFloat2(&vPosition);
    _float fDiffLen = XMVectorGetX(XMVector2Length(vDiff));
    _vector vDirection = XMVector2Normalize(vDiff);
    // 3. radius - 2번길이 == 밀려나야할 길이. 
    _float fOverLapLen = fFinalRadius - fDiffLen;
    // 4. 3번의 길이만큼 2번의 벡터를 뒤집은 방향으로 밀어낸다.
    _vector vOtherPos = _pOther->Get_Owner()->Get_ControllerTransform()->Get_State(CTransform::STATE_POSITION);
    _vector vOtherFinalPos = {};
    vOtherFinalPos = vOtherPos + fOverLapLen * vDirection;

    _pOther->Get_Owner()->Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vOtherFinalPos, 1.0f));

    _pOther->Update_OwnerTransform();
}

void CCollider_Circle::Block_Circle(CCollider_Circle* _pOther)
{
    // 1. 겹치는 길이 만큼 밀어내면 끝. 
    _float OtherRadius = _pOther->m_fFinalRadius;
    _float2 vOhterPosition = _pOther->m_vPosition;

    // 2. (두 반지름의 합) - 중점간의 거리 == 밀려야할 거리. 
    _vector vDiff = XMLoadFloat2(&vOhterPosition) - XMLoadFloat2(&m_vPosition);
    _float fDiffLen = XMVectorGetX(XMVector2Length(vDiff));
    _float fOverLapLen = (OtherRadius + m_fFinalRadius) - fDiffLen;

    // 3. 중점간의 거리를 구할때, other - my =>> 정규화 하면 이게 밀려나야할 방향.
    _vector vDirection = XMVector2Normalize(vDiff);

    // 4. 밀어내기
    _vector vOtherPos = _pOther->Get_Owner()->Get_ControllerTransform()->Get_State(CTransform::STATE_POSITION);
    _vector vOtherFinalPos = {};
    vOtherFinalPos = vOtherPos + fOverLapLen * vDirection;
    _pOther->Get_Owner()->Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vOtherFinalPos, 1.0f));

    _pOther->Update_OwnerTransform();
}

_bool CCollider_Circle::Compute_NearestPoint_AABB(CCollider_AABB* _pOtherAABB, _float2* _pOutPos, _float2* _pContactVector_NotNormalize)
{

    // 1. circle 기준 aabb 에 가장 가까운 점을 찾는다. 
    _float2 vOtherLT = _pOtherAABB->Get_LT(); // Min
    _float2 vOtherRB = _pOtherAABB->Get_RB(); // Max

    _float2 vPosition = m_vPosition;
    _float fFinalRadius = Get_FinalRadius();

    // 1. Clamp를 통해 AABB에 가장 가까운 점을 찾는다.
    _float2 vNearestPoint = {};
    vNearestPoint.x = clamp(vPosition.x, vOtherLT.x, vOtherRB.x);
    vNearestPoint.y = clamp(vPosition.y, vOtherRB.y, vOtherLT.y);

    if (nullptr != _pOutPos)
        *_pOutPos = vNearestPoint;

    if (nullptr != _pContactVector_NotNormalize)
        XMStoreFloat2(_pContactVector_NotNormalize, XMLoadFloat2(&vNearestPoint) - XMLoadFloat2(&vPosition)) ;

    return true;
}

_bool CCollider_Circle::Is_Collision_Circle(CCollider_Circle* _pOther)
{
    _vector vDiff = XMLoadFloat2(&_pOther->m_vPosition) - XMLoadFloat2(&m_vPosition);
    _float fRadiusSum = m_fFinalRadius + _pOther->m_fFinalRadius;

    _float vDiffSquared = XMVectorGetX(XMVector2Dot(vDiff, vDiff));

    return vDiffSquared <= fRadiusSum * fRadiusSum;
}

_bool CCollider_Circle::Is_Collision_AABB(CCollider_AABB* _pOther)
{
    _float2 vLT = _pOther->Get_LT(); // Min
    _float2 vRB = _pOther->Get_RB(); // Max

    // 1. Clamp를 통해 AABB에 가장 가까운 점을 찾는다.
    _float2 vNearestPoint = {};
    // TODO :: 챕터 2 5번째 스페이스 탔을때 오류 뜸 
    vNearestPoint.x = clamp(m_vPosition.x, vLT.x, vRB.x); 
    vNearestPoint.y = clamp(m_vPosition.y, vRB.y, vLT.y);

    // 2. 원의 중심과 가장 가까운 점 사이의 거리를 구한다.
    _vector vDiff = XMLoadFloat2(&m_vPosition) - XMLoadFloat2(&vNearestPoint);
    _float fDiffSquared = XMVectorGetX(XMVector2Dot(vDiff, vDiff));

    // 3. 그 거리와 원의 반지름을 비교한다. 
    return fDiffSquared <= m_fFinalRadius * m_fFinalRadius;
}

CCollider_Circle* CCollider_Circle::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CCollider_Circle* pInstance = new CCollider_Circle(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed Create Collider_Circle");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CComponent* CCollider_Circle::Clone(void* _pArg)
{
    CCollider_Circle* pInstance = new CCollider_Circle(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CCollider_Circle");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCollider_Circle::Free()
{

    __super::Free();
}
