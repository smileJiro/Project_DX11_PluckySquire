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
    // 지금 플레이어의 섹션의 해상도를 가져올 방법이 없어 //
    
    //_float2 vRTVSize = m_pGameInstance->Get_RT_Size(m_pOwner->Get_Include_Section_Name());
    m_pEffect->SetProjection(XMMatrixOrthographicLH(RTSIZE_BOOK2D_X, RTSIZE_BOOK2D_Y, 0.0f, 1.0f));
    m_pEffect->Apply(m_pContext);
    m_pContext->IASetInputLayout(m_pInputLayout);

    m_pBatch->Begin();

    static BoundingBox BoundingBox = {};
    BoundingBox.Center = _float3(m_vPosition.x, m_vPosition.y, 0.0f);
    BoundingBox.Extents = _float3(m_vFinalExtents.x, m_vFinalExtents.y, 0.0f);
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

void CCollider_AABB::Block(CCollider* _pOther)
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

void CCollider_AABB::Block_AABB(CCollider_AABB* _pOther)
{
    _float fWidth = m_vFinalExtents.x * 2.0f;
    _float fHeight = m_vFinalExtents.y * 2.0f;
    _float2 vLT = Get_LT();
    _float2 vRB = Get_RB();

    _float fOtherWidth = _pOther->m_vFinalExtents.x * 2.0f;
    _float fOtherHeight = _pOther->m_vFinalExtents.y * 2.0f;
    _float2 vOtherLT = _pOther->Get_LT();
    _float2 vOtherRB = _pOther->Get_RB();

    // 1. 침투 깊이를 계산한다.
    _float fOverlap_X = min(m_vPosition.x + fWidth, _pOther->m_vPosition.x + fOtherWidth) - max(m_vPosition.x, _pOther->m_vPosition.x);
    _float fOverlap_Y = min(m_vPosition.y + fHeight, _pOther->m_vPosition.y + fOtherHeight) - max(m_vPosition.y, _pOther->m_vPosition.y);

    if (0.0f > fOverlap_X || 0.0f > fOverlap_Y)
        return;
    // 2. x, y 중 침투 깊이가 짧은 쪽(비율 기반)을 밀어낼 축으로 결정한다.
    _float fRatio_X = fOverlap_X / fOtherWidth;
    _float fRatio_Y = fOverlap_Y / fOtherHeight;
    _float2 vDirection = {};
    if (fRatio_X < fRatio_Y)
        vDirection = { 1.0f, 0.0f };
    else
        vDirection = { 0.0f, 1.0f };
    // 3. 두 콜라이더의 위치를 기반으로 1 또는 -1로서 밀어낼 방향을 결정한다.
    _float fSign = 1.0f;

    // 4. 위치벡터에 vDirection을 곱하여 x + y를 수행하여 누가 더 양의방향에있는지 음의방향에 있는지를 판별한다. (밀어낼 축에따라 달라지는거지.)
    _float fMyPosSum = (m_vPosition.x * vDirection.x) + (m_vPosition.y * vDirection.y);
    _float fOtherPosSum = (_pOther->m_vPosition.x * vDirection.x) + (_pOther->m_vPosition.y * vDirection.y);
    if (fMyPosSum > fOtherPosSum)
        fSign *= -1.0f;

    // 4. (침투 깊이 * 밀어낼 축 * 밀어낼 방향) 만큼 밀어낸다.
    _vector vOtherPos = _pOther->Get_Owner()->Get_ControllerTransform()->Get_State(CTransform::STATE_POSITION);
    _float2 vOtherFinalPos = {};
    vOtherFinalPos.x = XMVectorGetX(vOtherPos) + fOverlap_X * vDirection.x * fSign;
    vOtherFinalPos.y = XMVectorGetY(vOtherPos) + fOverlap_Y * vDirection.y * fSign;

    _pOther->Get_Owner()->Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, XMVectorSet(vOtherFinalPos.x, vOtherFinalPos.y, 0.0f, 1.0f));

    // 밀어낸 것을 콜라이더에 반영. 컴포넌트 업데이트 순서때문에 이걸 한번 업데이트를 여기서 돌려줘야함
    _pOther->Update_OwnerTransform();
}

void CCollider_AABB::Block_Circle(CCollider_Circle* _pOther)
{
    // 1. circle 기준 aabb 에 가장 가까운 점을 찾는다. 
    _float2 vLT = Get_LT(); // Min
    _float2 vRB = Get_RB(); // Max

    _float2 vOtherPosition = _pOther->Get_Position();
    _float fOtherFinalRadius = _pOther->Get_FinalRadius();

    // 1. Clamp를 통해 AABB에 가장 가까운 점을 찾는다.
    _float2 vNearestPoint = {};
    vNearestPoint.x = clamp(vOtherPosition.x, vLT.x, vRB.x);
    vNearestPoint.y = clamp(vOtherPosition.y, vRB.y, vLT.y);
    // 2. circle의 중점에서부터 aabb에 가장 가까운 점까지의 거리를 구한다. (밀려나야 할 축으로도 사용가능하다 뒤집으면 )
    _vector vDiff = XMLoadFloat2(&vOtherPosition) - XMLoadFloat2(&vNearestPoint);
    _float fDiffLen = XMVectorGetX(XMVector2Length(vDiff));
    _vector vDirection = XMVector2Normalize(vDiff);
    // 3. radius - 2번길이 == 밀려나야할 길이. 
    _float fOverLapLen = fOtherFinalRadius - fDiffLen;
    // 4. 3번의 길이만큼 2번의 벡터를 뒤집은 방향으로 밀어낸다.
    _vector vOtherPos = _pOther->Get_Owner()->Get_ControllerTransform()->Get_State(CTransform::STATE_POSITION);
    _vector vOtherFinalPos = {};
    vOtherFinalPos = vOtherPos + fOverLapLen * vDirection;

    _pOther->Get_Owner()->Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vOtherFinalPos, 1.0f));
    _pOther->Update_OwnerTransform();
}

void CCollider_AABB::Update_OwnerTransform()
{
    _vector vOwnerPos = m_pOwner->Get_FinalPosition();
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

    // 가로 방향 충돌 검사
    if (vLT.x > vOtherRB.x || vRB.x < vOtherLT.x)
        return false;

    // 세로 방향 충돌 검사
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
    vNearestPoint.y = clamp(vOtherPosition.y, vRB.y, vLT.y);

    // 2. 원의 중심과 가장 가까운 점 사이의 거리를 구한다.
    // 3. 그 거리와 원의 반지름을 비교한다. 
    _vector vDiff = XMLoadFloat2(&vOtherPosition) - XMLoadFloat2(&vNearestPoint);
    _float fDiffSquared = XMVectorGetX(XMVector2Dot(vDiff, vDiff));

    _bool isResult = fDiffSquared <= fOtherFinalRadius * fOtherFinalRadius;

    return isResult;
}

_float2 CCollider_AABB::Get_LT()
{
    _float2 vLT = {};
    vLT.x = m_vPosition.x - m_vFinalExtents.x;
    vLT.y = m_vPosition.y + m_vFinalExtents.y;

    return vLT;
}

_float2 CCollider_AABB::Get_RB()
{
    _float2 vRB = {};
    vRB.x = m_vPosition.x + m_vFinalExtents.x;
    vRB.y = m_vPosition.y - m_vFinalExtents.y;

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
