#include "stdafx.h"
#include "Section_2D.h"
#include "GameInstance.h"

#include "Map_2D.h"

CSection_2D::CSection_2D(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CSection(_pDevice, _pContext)
{
}

HRESULT CSection_2D::Initialize()
{
    if (FAILED(__super::Initialize()))
        return E_FAIL;

    if (FAILED(Ready_Map_2D()))
        return E_FAIL;


    return S_OK;
}

HRESULT CSection_2D::Register_RTV_ToTargetManager()
{
    /* 현재 미구현, Rendering 관련해서는 좀만 더 고민해볼예정. */
    return S_OK;
}

HRESULT CSection_2D::Register_RenderGroup_ToRenderer()
{
    /* 현재 미구현, Rendering 관련해서는 좀만 더 고민해볼예정. */
    return S_OK;
}

ID3D11RenderTargetView* CSection_2D::Get_RTV_FromRenderTarget()
{
    if (nullptr == m_pMap)
        return nullptr;

    return m_pMap->Get_RTV_FromRenderTarget();
}

ID3D11ShaderResourceView* CSection_2D::Get_SRV_FromRenderTarget()
{
    if (nullptr == m_pMap)
        return nullptr;

    return m_pMap->Get_SRV_FromRenderTarget();
}

ID3D11ShaderResourceView* CSection_2D::Get_SRV_FromTexture(_uint _iTextureIndex)
{
    if (nullptr == m_pMap)
        return nullptr;

    return m_pMap->Get_SRV_FromTexture(_iTextureIndex);
}

HRESULT CSection_2D::Ready_Map_2D()
{
    m_pMap = CMap_2D::Create(m_pDevice, m_pContext);
    if (nullptr == m_pMap)
        return E_FAIL;

    return S_OK;
}

CSection_2D* CSection_2D::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CSection_2D* pInstance = new CSection_2D(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed Create CSection_2D");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSection_2D::Free()
{
    Safe_Release(m_pMap);

    __super::Free();
}
