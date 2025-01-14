#include "stdafx.h"
#include "MapObject.h"
#include "GameInstance.h"

CMapObject::CMapObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CModelObject(_pDevice, _pContext)
{
}

CMapObject::CMapObject(const CMapObject& _Prototype)
    :CModelObject(_Prototype)
{
}

HRESULT CMapObject::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CMapObject::Initialize(void* _pArg)
{
    MAPOBJ_DESC* pDesc = static_cast<MAPOBJ_DESC*>(_pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CMapObject::Priority_Update(_float _fTimeDelta)
{

    CPartObject::Priority_Update(_fTimeDelta);
}

void CMapObject::Update(_float _fTimeDelta)
{


    /* Update Parent Matrix */
    CPartObject::Update(_fTimeDelta);
}

void CMapObject::Late_Update(_float _fTimeDelta)
{





    /* Add Render Group */
    if (COORDINATE_3D == m_pControllerTransform->Get_CurCoord())
        m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
    else if (COORDINATE_2D == m_pControllerTransform->Get_CurCoord())
        m_pGameInstance->Add_RenderObject(CRenderer::RG_BOOK_2D, this);

    /* Update Parent Matrix */
    CPartObject::Late_Update(_fTimeDelta);
}

HRESULT CMapObject::Render()
{
    if (FAILED(CModelObject::Bind_ShaderResources_WVP()))
        return E_FAIL;

    switch (m_pControllerTransform->Get_CurCoord())
    {
    case Engine::COORDINATE_2D:
        CModelObject::Render_2D();
        break;
    case Engine::COORDINATE_3D:
        CModelObject::Render_3D();
        break;
    default:
        break;
    }

    return S_OK;
}

HRESULT CMapObject::Render_Shadow()
{
    return S_OK;
}

HRESULT CMapObject::Ready_Components()
{
    return S_OK;
}


CMapObject* CMapObject::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CMapObject* pInstance = new CMapObject(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CMapObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMapObject::Clone(void* _pArg)
{
    CMapObject* pInstance = new CMapObject(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CMapObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMapObject::Free()
{


    __super::Free();
}
