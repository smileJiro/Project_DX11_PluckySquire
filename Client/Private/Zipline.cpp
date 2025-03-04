#include "stdafx.h"
#include "Zipline.h"
#include "GameInstance.h"

CZipline::CZipline(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CModelObject(_pDevice, _pContext)
{
}

CZipline::CZipline(const CZipline& _Prototype)
    : CModelObject(_Prototype)
{
}

HRESULT CZipline::Initialize(void* _pArg)
{
    return E_NOTIMPL;
}

HRESULT CZipline::Render()
{
    return E_NOTIMPL;
}

CZipline* CZipline::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    return nullptr;
}

CGameObject* CZipline::Clone(void* _pArg)
{
    return nullptr;
}

void CZipline::Free()
{
}
