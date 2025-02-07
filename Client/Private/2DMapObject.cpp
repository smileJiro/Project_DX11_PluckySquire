#include "stdafx.h"
#include "2DMapObject.h"
#include "GameInstance.h"


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




    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    return S_OK;
}

void C2DMapObject::Priority_Update(_float _fTimeDelta)
{
    CPartObject::Priority_Update(_fTimeDelta);
}
void C2DMapObject::Update(_float _fTimeDelta)
{
    CModelObject::Update(_fTimeDelta);
}

void C2DMapObject::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta);
}

HRESULT C2DMapObject::Render()
{
    return __super::Render();
}

HRESULT C2DMapObject::Render_Shadow()
{
    return S_OK;
}


C2DMapObject* C2DMapObject::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    C2DMapObject* pInstance = new C2DMapObject(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : C2DMapObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* C2DMapObject::Clone(void* _pArg)
{
    C2DMapObject* pInstance = new C2DMapObject(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : C2DMapObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void C2DMapObject::Free()
{
    __super::Free();
}
