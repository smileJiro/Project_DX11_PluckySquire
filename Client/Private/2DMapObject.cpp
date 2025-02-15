#include "stdafx.h"
#include "2DMapObject.h"
#include "GameInstance.h"
#include "Collider_AABB.h"
#include "Collider_Circle.h"

#include "Section_Manager.h"
#include "2DModel.h"


C2DMapObject::C2DMapObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CMapObject(_pDevice, _pContext)
{
}

C2DMapObject::C2DMapObject(const C2DMapObject& _Prototype)
    :CMapObject(_Prototype)
{
}

HRESULT C2DMapObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT C2DMapObject::Initialize(void* _pArg)
{
    if (nullptr == _pArg)
        return E_FAIL;

    MAPOBJ_DESC* pDesc = static_cast<MAPOBJ_DESC*>(_pArg);
    
    if (pDesc->is2DImport)
    {
        m_isSorting = pDesc->isSorting;
        m_isBackGround = pDesc->isBackGround;
        m_isCollider = pDesc->isCollider;
        m_isActive = pDesc->isActive;
    }

    return __super::Initialize(_pArg);
}

void C2DMapObject::Priority_Update(_float _fTimeDelta)
{
    __super::Priority_Update(_fTimeDelta);
}
void C2DMapObject::Update(_float _fTimeDelta)
{

    __super::Update(_fTimeDelta);
}

void C2DMapObject::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta);
}

HRESULT C2DMapObject::Render()
{
    HRESULT hr = __super::Render();
//#ifdef _DEBUG
//    if(m_pColliderCom)
//        m_pColliderCom->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));
//#endif // _DEBUG



    return hr;
}

HRESULT C2DMapObject::Render_Shadow()
{
    return S_OK;
}

HRESULT C2DMapObject::Ready_Collider(MAPOBJ_DESC* Desc, _bool _isBlock)
{
    Desc->fCollider_Offset_Pos.x *= RATIO_BOOK2D_X;
    Desc->fCollider_Offset_Pos.y *= RATIO_BOOK2D_Y;
    Desc->fCollider_Offset_Pos.y *= -1.f;


    CModel* pModel = m_pControllerModel->Get_Model(COORDINATE_2D);

    C2DModel* p2DModel = static_cast<C2DModel*>(pModel);
    const _matrix* matLocal = p2DModel->Get_CurrentSpriteTransform();
    CCollider* pCollider = nullptr;

    if (nullptr != matLocal)
    {
        _float2 fSibalOffset;
        fSibalOffset.x = (*matLocal).r[3].m128_f32[0] * RATIO_BOOK2D_X;
        fSibalOffset.y = (*matLocal).r[3].m128_f32[1] * RATIO_BOOK2D_Y;

        //_matrix matPos = XMMatrixTranslation(Desc->fCollider_Offset_Pos.x, Desc->fCollider_Offset_Pos.y, 1.f);
        //matPos *= *matLocal;

        Desc->fCollider_Offset_Pos.x += fSibalOffset.x;
        Desc->fCollider_Offset_Pos.y += fSibalOffset.y;
    }

    if (CCollider::AABB_2D == Desc->eColliderType)
    {
        Desc->fCollider_Extent.x *= RATIO_BOOK2D_X;
        Desc->fCollider_Extent.y *= RATIO_BOOK2D_Y;
        Desc->fCollider_Extent.x *= 0.5f;
        Desc->fCollider_Extent.y *= 0.5f;

        CCollider_AABB::COLLIDER_AABB_DESC AABBDesc = {};
        AABBDesc.pOwner = this;
        AABBDesc.vExtents = Desc->fCollider_Extent;
        AABBDesc.vScale = { 1.0f, 1.0f };
        AABBDesc.vOffsetPosition = Desc->fCollider_Offset_Pos;
        AABBDesc.isBlock = _isBlock;
        AABBDesc.iCollisionGroupID = Desc->iCollisionGroupID;
        
        if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
            TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&pCollider), &AABBDesc)))
            return E_FAIL;
    }
    else if (CCollider::CIRCLE_2D == Desc->eColliderType)
    {

        Desc->fCollider_Radius *= RATIO_BOOK2D_X;

        /* Test 2D Collider */
        CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};
        CircleDesc.pOwner = this;
        CircleDesc.fRadius = Desc->fCollider_Radius;
        CircleDesc.vScale = { 1.0f, 1.0f };
        CircleDesc.vOffsetPosition = Desc->fCollider_Offset_Pos;
        CircleDesc.isBlock = _isBlock;
        CircleDesc.iCollisionGroupID = Desc->iCollisionGroupID;
        if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
            TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&pCollider), &CircleDesc)))
            return E_FAIL;
    }

    return S_OK;
}

void C2DMapObject::Free()
{
    __super::Free();
}
