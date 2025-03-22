#include "stdafx.h"
#include "Blocker.h"
#include "GameInstance.h"
#include "Section_Manager.h"


CBlocker::CBlocker(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, COORDINATE _eCoord)
    :CPartObject(_pDevice, _pContext)
    , m_eBlockerCoord(_eCoord)
{
    
}

CBlocker::CBlocker(const CBlocker& _Prototype)
    :CPartObject(_Prototype)
    , m_eBlockerCoord(_Prototype.m_eBlockerCoord)
{
}

HRESULT CBlocker::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CBlocker::Initialize(void* _pArg)
{
    BLOCKER_DESC* pDesc = static_cast<BLOCKER_DESC*>(_pArg);
    // Save
    m_isFloor = pDesc->isFloor;
    m_isJumpPass = pDesc->isJumpPass;

    // Add
    pDesc->eStartCoord = m_eBlockerCoord;
    pDesc->iObjectGroupID = m_isJumpPass == true ? OBJECT_GROUP::BLOCKER_JUMPPASS : OBJECT_GROUP::BLOCKER;
    pDesc->isCoordChangeEnable = false;
    
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Component(pDesc)))
        return E_FAIL;

	return S_OK;
}

HRESULT CBlocker::Render()
{
    #ifdef _DEBUG
    for (auto& p2DCollider : m_p2DColliderComs)
    {
        p2DCollider->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));
    }
#endif // _DEBUG

    return S_OK;
}



HRESULT CBlocker::Ready_Component(BLOCKER_DESC* _pDesc)
{
    if (COORDINATE_2D == m_eBlockerCoord)
    {
        BLOCKER2D_DESC* Desc2D = static_cast<BLOCKER2D_DESC*>(_pDesc);
        /* Test 2D Collider */
        m_p2DColliderComs.resize(1);
        CCollider_AABB::COLLIDER_AABB_DESC AABBDesc = {};
        AABBDesc.pOwner = this;
        AABBDesc.vExtents = Desc2D->vColliderExtents;
        AABBDesc.vScale = Desc2D->vColliderScale;
        AABBDesc.vOffsetPosition = Desc2D->vOffsetPosition;
        AABBDesc.isBlock = true;
        AABBDesc.iCollisionGroupID = m_isJumpPass == true ? OBJECT_GROUP::BLOCKER_JUMPPASS : OBJECT_GROUP::BLOCKER;
        
        CCollider_AABB* pCollider = nullptr;
        if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
            TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &AABBDesc)))
            return E_FAIL;

    }
    else if (COORDINATE_3D == m_eBlockerCoord)
    {

    }

    return S_OK;
}

CBlocker* CBlocker::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, COORDINATE _eCoord)
{
    CBlocker* pInstance = new CBlocker(_pDevice, _pContext, _eCoord);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed Create : CBlocker");
        return nullptr;
    }

	return pInstance;
}

CGameObject* CBlocker::Clone(void* _pArg)
{
    CBlocker* pInstance = new CBlocker(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CBlocker");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBlocker::Free()
{


    __super::Free();
}

void CBlocker::On_BombSwitch(_bool _bOn)
{
    if (true != _bOn)
    {
        for (auto& pBlocker : m_p2DColliderComs)
            if (nullptr != pBlocker) pBlocker->Set_Active(false);
    }
    else
    {
    }
}
