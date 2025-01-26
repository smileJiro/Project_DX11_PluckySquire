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
    // 만약 Map_2D를 동적으로 변경하고자한다면, Device, Context를 Section이 가지게 해도 될 듯 함.
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
