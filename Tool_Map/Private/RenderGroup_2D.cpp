#include "stdafx.h"
#include "RenderGroup_2D.h"
#include "GameInstance.h"
#include "Shader.h"

CRenderGroup_2D::CRenderGroup_2D(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CRenderGroup_MRT(_pDevice, _pContext)
{
}


HRESULT CRenderGroup_2D::Initialize(void* _pArg)
{
    if (nullptr == _pArg)
        return E_FAIL;


    RG_2D_DESC* pDesc = static_cast<RG_2D_DESC*>(_pArg);
    
    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(pDesc->fRenderTargetSize.x, pDesc->fRenderTargetSize.y, 0.0f, 1.0f));
    m_fRenderTarget_Size = pDesc->fRenderTargetSize;
    _wstring arrStrTag[LAST] = {
        L"Prototype_Component_Shader_VtxPosTex",
    };

    for (_uint i = 0; i < (_uint)LAST; i++)
    {
        CBase* pBase = m_pGameInstance->Clone_Prototype(
            PROTOTYPE::PROTO_COMPONENT,
            LEVEL_STATIC,
            arrStrTag[i],
            nullptr);
        if (nullptr == pBase)
            return E_FAIL;
        else
            m_arr2DShaders[i] = static_cast<CShader*>(pBase);
    }

    return __super::Initialize(_pArg);
}


HRESULT CRenderGroup_2D::Add_RenderObject(CGameObject* _pGameObject)
{
    if (COORDINATE_3D == _pGameObject->Get_CurCoord())
        return S_OK;
    return __super::Add_RenderObject(_pGameObject);
}

HRESULT CRenderGroup_2D::Render(CShader* _pRTShader, CVIBuffer_Rect* _pRTBuffer)
{
    if(FAILED(Binding_2D_View_Proj()))
        return E_FAIL;

    return __super::Render(_pRTShader,_pRTBuffer);
}

HRESULT CRenderGroup_2D::Binding_2D_View_Proj()
{
    for (_uint i = 0; i < LAST; i++)
    {
        if (nullptr == m_arr2DShaders)
        {
            MSG_BOX("RenderGroup_2D Error - Shader null");
            return E_FAIL;
        }
        if (FAILED(m_arr2DShaders[i]->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
            return E_FAIL;

        if (FAILED(m_arr2DShaders[i]->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
            return E_FAIL;
    }
    return S_OK;
}

CRenderGroup_2D* CRenderGroup_2D::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg)
{
    CRenderGroup_2D* pInstance = new CRenderGroup_2D(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Create Failed CRenderGroup_2D");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CRenderGroup_2D::Free()
{
    for (_uint i = 0; i < LAST; i++)
        Safe_Release(m_arr2DShaders[i]);
    __super::Free();
}
