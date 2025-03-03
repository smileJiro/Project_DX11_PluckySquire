#include "stdafx.h"
#include "Postit_Page.h"

CPostit_Page::CPostit_Page(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CModelObject(_pDevice, _pContext)
{
}

CPostit_Page::CPostit_Page(const CPostit_Page& _Prototype)
    : CModelObject(_Prototype)
{
}

HRESULT CPostit_Page::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPostit_Page::Initialize(void* _pArg)
{
    return S_OK;
}

HRESULT CPostit_Page::Render()
{
    return S_OK;
}

HRESULT CPostit_Page::Render_Shadow(_float4x4* _pViewMatrix, _float4x4* _pProjMatrix)
{
    return S_OK;
}

HRESULT CPostit_Page::Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition)
{
    return S_OK;
}

void CPostit_Page::Check_FrustumCulling()
{
}

CPostit_Page* CPostit_Page::Create()
{
    return nullptr;
}

void CPostit_Page::Free()
{
}
