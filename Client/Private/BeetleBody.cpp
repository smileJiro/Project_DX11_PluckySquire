#include "stdafx.h"
#include "BeetleBody.h"
#include "GameInstance.h"

CBeetleBody::CBeetleBody(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CModelObject(_pDevice, _pContext)
{
}

CBeetleBody::CBeetleBody(const CBeetleBody& _Prototype)
    :CModelObject(_Prototype)
{
}

HRESULT CBeetleBody::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CBeetleBody::Initialize(void* _pArg)
{
    BEETLEBODY_DESC* pDesc = static_cast<BEETLEBODY_DESC*>(_pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CBeetleBody::Priority_Update(_float _fTimeDelta)
{

    CPartObject::Priority_Update(_fTimeDelta);
}

void CBeetleBody::Update(_float _fTimeDelta)
{


    /* Update Parent Matrix */
    CPartObject::Update(_fTimeDelta);
}

void CBeetleBody::Late_Update(_float _fTimeDelta)
{

    /* Add Render Group */
    if(COORDINATE_3D == m_pControllerTransform->Get_CurCoord())
        m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
    else if(COORDINATE_2D == m_pControllerTransform->Get_CurCoord())
        m_pGameInstance->Add_RenderObject(CRenderer::RG_BOOK_2D, this);

    /* Update Parent Matrix */
    CPartObject::Late_Update(_fTimeDelta);
}

HRESULT CBeetleBody::Render()
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

HRESULT CBeetleBody::Render_Shadow()
{
    return S_OK;
}

HRESULT CBeetleBody::Ready_Components()
{
    return S_OK;
}


CBeetleBody* CBeetleBody::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CBeetleBody* pInstance = new CBeetleBody(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBeetleBody");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBeetleBody::Clone(void* _pArg)
{
    CBeetleBody* pInstance = new CBeetleBody(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CBeetleBody");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBeetleBody::Free()
{


    __super::Free();
}
