#include "stdafx.h"
#include "2DMapDefaultObject.h"
#include "GameInstance.h"
#include "Collider_AABB.h"
#include "Collider_Circle.h"
#include "Collision_Manager.h"
#include "Section_Manager.h"
#include "2DModel.h"


C2DMapDefaultObject::C2DMapDefaultObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :C2DMapObject(_pDevice, _pContext)
{
}

C2DMapDefaultObject::C2DMapDefaultObject(const C2DMapDefaultObject& _Prototype)
    :C2DMapObject(_Prototype)
{
}

HRESULT C2DMapDefaultObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT C2DMapDefaultObject::Initialize(void* _pArg)
{
    if (nullptr == _pArg)
        return E_FAIL;


    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    MAPOBJ_DESC* pDesc = static_cast<MAPOBJ_DESC*>(_pArg);

    if (m_isCollider)
    {
        if (FAILED(Ready_Collider(pDesc, !m_isActive)))
            return E_FAIL;
    }

    return S_OK;
}

void C2DMapDefaultObject::Priority_Update(_float _fTimeDelta)
{
    __super::Priority_Update(_fTimeDelta);
}
void C2DMapDefaultObject::Update(_float _fTimeDelta)
{
    if (m_pColliderCom)
        CCollision_Manager::GetInstance()->Add_Collider(m_strSectionName, OBJECT_GROUP::MAPOBJECT, m_pColliderCom);

    __super::Update(_fTimeDelta);
}

void C2DMapDefaultObject::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta);
}

HRESULT C2DMapDefaultObject::Render()
{
    HRESULT hr = __super::Render();
#ifdef _DEBUG
    if (m_pColliderCom)
        m_pColliderCom->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));
#endif // _DEBUG



    return hr;
}

HRESULT C2DMapDefaultObject::Render_Shadow()
{
    return S_OK;
}


C2DMapDefaultObject* C2DMapDefaultObject::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    C2DMapDefaultObject* pInstance = new C2DMapDefaultObject(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : C2DMapDefaultObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* C2DMapDefaultObject::Clone(void* _pArg)
{
    C2DMapDefaultObject* pInstance = new C2DMapDefaultObject(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : C2DMapDefaultObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void C2DMapDefaultObject::Free()
{
    Safe_Release(m_pColliderCom);
    __super::Free();
}
