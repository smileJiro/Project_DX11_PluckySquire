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

    __super::Update(_fTimeDelta);
}

void CCollider_AABB::Late_Update(_float _fTimeDelta)
{
    if (nullptr == m_pOwner)
        return;

    Update_OwnerTransform();

    __super::Late_Update(_fTimeDelta); // Collision_Manager에 등록
//#ifdef _DEBUG
//    m_pGameInstance->Add_DebugComponent(this);
//#endif // _DEBUG
}

#ifdef _DEBUG
HRESULT CCollider_AABB::Render(_float2 _fRenderTargetSize)
{
    if (COORDINATE_2D != m_pOwner->Get_CurCoord())
        return E_FAIL;
    
    m_pEffect->SetWorld(XMMatrixIdentity());
    m_pEffect->SetView(XMMatrixIdentity());
    // 지금 플레이어의 섹션의 해상도를 가져올 방법이 없어 //

    
    m_pEffect->SetProjection(XMMatrixOrthographicLH(_fRenderTargetSize.x, _fRenderTargetSize.y, 0.0f, 1.0f));
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

_bool CCollider_AABB::Is_ContainsPoint(_float2 _vPosition)
{
    if (false == m_isActive)
        return false;

	return m_vPosition.x - m_vFinalExtents.x <= _vPosition.x && m_vPosition.x + m_vFinalExtents.x >= _vPosition.x &&
		m_vPosition.y - m_vFinalExtents.y <= _vPosition.y && m_vPosition.y + m_vFinalExtents.y >= _vPosition.y;
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
    /* 1차적으로 Other의 이전프레임 위치를 체크한다. >>> 그리고 */
    _float fWidth = m_vFinalExtents.x * 2.0f;
    _float fHeight = m_vFinalExtents.y * 2.0f;
    _float2 vLT = Get_LT();
    _float2 vRB = Get_RB();

    _float fOtherWidth = _pOther->m_vFinalExtents.x * 2.0f;
    _float fOtherHeight = _pOther->m_vFinalExtents.y * 2.0f;
    _float2 vOtherLT = _pOther->Get_LT();
    _float2 vOtherRB = _pOther->Get_RB();

    // 1. 침투 깊이를 계산한다.

    float fOverlap_X = min(vRB.x, vOtherRB.x) - max(vLT.x, vOtherLT.x);
    float fOverlap_Y = min(vLT.y, vOtherLT.y) - max(vRB.y, vOtherRB.y);

 /*   _float fOverlap_X = min(m_vPosition.x + fWidth, _pOther->m_vPosition.x + fOtherWidth) - max(m_vPosition.x, _pOther->m_vPosition.x);
    _float fOverlap_Y = min(m_vPosition.y + fHeight, _pOther->m_vPosition.y + fOtherHeight) - max(m_vPosition.y, _pOther->m_vPosition.y);*/

    if (0.0f >= fOverlap_X || 0.0f >= fOverlap_Y)
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
    vOtherFinalPos.x = XMVectorGetX(vOtherPos) + (fOverlap_X + 1.0f) * vDirection.x * fSign;
    vOtherFinalPos.y = XMVectorGetY(vOtherPos) + (fOverlap_Y + 1.0f) * vDirection.y * fSign;

    _pOther->Get_Owner()->Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, XMVectorSet(vOtherFinalPos.x, vOtherFinalPos.y, 0.0f, 1.0f));

    // 밀어낸 것을 콜라이더에 반영. 컴포넌트 업데이트 순서때문에 이걸 한번 업데이트를 여기서 돌려줘야함
    _pOther->Update_OwnerTransform();
}
void CCollider_AABB::Block_Circle(CCollider_Circle* _pOther)
{
    _float2 vLT = Get_LT(); // AABB Min
    _float2 vRB = Get_RB(); // AABB Max

    _float2 vOtherPosition = _pOther->Get_Position();
    _float fOtherFinalRadius = _pOther->Get_FinalRadius();

    // AABB 경계에서 가장 가까운 점 찾기
    _float2 vNearestPoint = {};
    vNearestPoint.x = clamp(vOtherPosition.x, vLT.x, vRB.x);
    vNearestPoint.y = clamp(vOtherPosition.y, vRB.y, vLT.y);

    _vector vDiff = XMLoadFloat2(&vOtherPosition) - XMLoadFloat2(&vNearestPoint);
    _float fDiffLen = XMVectorGetX(XMVector2Length(vDiff));
    _vector vDirection = XMVector2Normalize(vDiff);
    _float fOverLapLen = fOtherFinalRadius - fDiffLen;

    // 원의 중심이 AABB 내부에 완전히 포함된 경우 -> 밀어내는 방향 강제 지정
    if (vOtherPosition.x >= vLT.x && vOtherPosition.x <= vRB.x &&
        vOtherPosition.y >= vRB.y && vOtherPosition.y <= vLT.y)
    {
        _float2 vDistances = 
        {
            min(abs(vOtherPosition.x - vLT.x), abs(vOtherPosition.x - vRB.x)), // X축 최소 거리
            min(abs(vOtherPosition.y - vRB.y), abs(vOtherPosition.y - vLT.y))  // Y축 최소 거리
        };

        if (vDistances.x < vDistances.y)
            vDirection = XMVectorSet((vOtherPosition.x < (vLT.x + vRB.x) * 0.5f) ? -1.0f : 1.0f, 0.0f, 0.0f, 0.0f);
        else
            vDirection = XMVectorSet(0.0f, (vOtherPosition.y < (vRB.y + vLT.y) * 0.5f) ? -1.0f : 1.0f, 0.0f, 0.0f);

        fOverLapLen = fOtherFinalRadius;
    }

    _vector vOtherPos = _pOther->Get_Owner()->Get_ControllerTransform()->Get_State(CTransform::STATE_POSITION);
    _vector vOtherFinalPos = vOtherPos + fOverLapLen * vDirection;

    _pOther->Get_Owner()->Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vOtherFinalPos, 1.0f));
    _pOther->Update_OwnerTransform();
}


_bool CCollider_AABB::Get_CollisionPoint(CCollider* _pOther, _float2* pOutPoint)
{
    _bool bCollision = Is_Collision(_pOther);
	if (false == bCollision)
		return false;

    CCollider::TYPE eTYpe = _pOther->Get_Type();
    _float2 vLT = Get_LT();
    _float2 vRB = Get_RB();
    switch (eTYpe)
    {
    case Engine::CCollider::CIRCLE_2D:
    {
        CCollider_Circle* pCircle = static_cast<CCollider_Circle*>(_pOther);

        _float2 vOtherPosition = pCircle->Get_Position();
        _float fOtherFinalRadius = pCircle->Get_FinalRadius();

        // 1. Clamp를 통해 AABB에 가장 가까운 점을 찾는다.

        pOutPoint->x = clamp(vOtherPosition.x, vLT.x, vRB.x);
        pOutPoint->y = clamp(vOtherPosition.y, vRB.y, vLT.y);
        break;
    }
    case Engine::CCollider::AABB_2D:
    {
		CCollider_AABB* pAABB = static_cast<CCollider_AABB*>(_pOther);
        _float2 vOtherLT = pAABB->Get_LT();
        _float2 vOtherRB = pAABB->Get_RB();

        // 가로 방향 충돌 검사  
        if (vLT.x > vOtherRB.x || vRB.x < vOtherLT.x)
            return false;

        // 세로 방향 충돌 검사  
        if (vLT.y < vOtherRB.y || vRB.y > vOtherLT.y)
            return false;

        pOutPoint->x = (max(vLT.x, vOtherLT.x) + min(vRB.x, vOtherRB.x)) / 2.0f;
        pOutPoint->y = (max(vRB.y, vOtherRB.y) + min(vLT.y, vOtherLT.y)) / 2.0f;
        break;
    }
    case Engine::CCollider::TYPE_LAST:
        break;
    default:
        break;
    }


    return true ;
}

_bool CCollider_AABB::Get_CollisionNormal(CCollider* _pOther, _float2* pOutNormal)
{
    _bool bCollision = Is_Collision(_pOther);
    if (false == bCollision)
        return false;

    CCollider::TYPE eTYpe = _pOther->Get_Type();
    _float2 vLT = Get_LT();
    _float2 vRB = Get_RB();
    switch (eTYpe)
    {
    case Engine::CCollider::CIRCLE_2D:
    {
        CCollider_Circle* pCircle = static_cast<CCollider_Circle*>(_pOther);

        _float2 vOtherPosition = pCircle->Get_Position();
        _float fOtherFinalRadius = pCircle->Get_FinalRadius();

        // 1. Clamp를 통해 AABB에 가장 가까운 점을 찾는다.
        _vector vClamp;
        vClamp.m128_f32[0] = clamp(vOtherPosition.x, vLT.x, vRB.x);
        vClamp.m128_f32[1] = clamp(vOtherPosition.y, vRB.y, vLT.y);
        _vector vDir = XMVector2Normalize(XMLoadFloat2(&vOtherPosition) - vClamp);
        vDir.m128_f32[2] = 0;
        vDir.m128_f32[3] = 0;
        pOutNormal->x = vDir.m128_f32[0];
        pOutNormal->y = vDir.m128_f32[1];
        break;
    }
    case Engine::CCollider::AABB_2D:
    {
        CCollider_AABB* pAABB = static_cast<CCollider_AABB*>(_pOther);
        _float2 vOtherLT = pAABB->Get_LT();
        _float2 vOtherRB = pAABB->Get_RB();

       _float fOverlap_X = min(vRB.x, vOtherRB.x) - max(vLT.x, vOtherLT.x);
       _float fOverlap_Y = min(vLT.y, vOtherLT.y) - max(vRB.y, vOtherRB.y);

	   _vector vNormal = {};
	   if (fOverlap_X < fOverlap_Y)
	   {
	        if (vLT.x < vOtherRB.x)
                vNormal = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		    else
			    vNormal = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
	   }
	   else
	   {
		   if (vLT.y < vOtherRB.y)
			   vNormal = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
		   else
	           vNormal = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	   }

        break;
    }
    case Engine::CCollider::TYPE_LAST:
        break;
    default:
        break;
    }


    return true;
}

void CCollider_AABB::Update_OwnerTransform()
{
    _vector vOwnerPos = m_pOwner->Get_FinalPosition(COORDINATE_2D);
    XMStoreFloat2(&m_vPosition, XMLoadFloat2(&m_vOffsetPosition) + vOwnerPos);

    _float3 vOwnerScale = m_pOwner->Get_FinalScale(COORDINATE_2D);
    XMStoreFloat2(&m_vFinalExtents, XMLoadFloat2(&m_vExtents) * XMLoadFloat2(&m_vScale) * XMVectorSet(vOwnerScale.x, vOwnerScale.y, 0.0f, 0.0f));
} 

_bool CCollider_AABB::Is_Collision_AABB(CCollider_AABB* _pOther)  
{  
   _float2 vLT = Get_LT();  
   _float2 vRB = Get_RB();  
   _float2 vOtherLT = _pOther->Get_LT();  
   _float2 vOtherRB = _pOther->Get_RB();  
   _float2 fCollisionPoint = {};  

   // 가로 방향 충돌 검사  
   if (vLT.x > vOtherRB.x || vRB.x < vOtherLT.x)  
       return false;  

   // 세로 방향 충돌 검사  
   if (vLT.y < vOtherRB.y || vRB.y > vOtherLT.y)  
       return false;  

   fCollisionPoint.x = (max(vLT.x, vOtherLT.x) + min(vRB.x, vOtherRB.x)) / 2.0f;  
   fCollisionPoint.y = (max(vRB.y, vOtherRB.y) + min(vLT.y, vOtherLT.y)) / 2.0f;  

   Set_CollisionPos(fCollisionPoint);
   _pOther->Set_CollisionPos(fCollisionPoint);

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
