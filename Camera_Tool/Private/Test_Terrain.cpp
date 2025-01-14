#include "stdafx.h"
#include "Test_Terrain.h"

CTest_Terrain::CTest_Terrain(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CModelObject(_pDevice, _pContext)
{
}

CTest_Terrain::CTest_Terrain(const CTest_Terrain& _Prototype)
    :CModelObject(_Prototype)
{
}

HRESULT CTest_Terrain::Initialize_Prototype()
{
    return E_NOTIMPL;
}

HRESULT CTest_Terrain::Initialize(void* _pArg)
{
    return E_NOTIMPL;
}

void CTest_Terrain::Priority_Update(_float _fTimeDelta)
{
}

void CTest_Terrain::Update(_float _fTimeDelta)
{
}

void CTest_Terrain::Late_Update(_float _fTimeDelta)
{
}

HRESULT CTest_Terrain::Render()
{
    return E_NOTIMPL;
}

HRESULT CTest_Terrain::Render_Shadow()
{
    return E_NOTIMPL;
}

HRESULT CTest_Terrain::Ready_Components()
{
    return E_NOTIMPL;
}

CTest_Terrain* CTest_Terrain::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    return nullptr;
}

CGameObject* CTest_Terrain::Clone(void* _pArg)
{
    return nullptr;
}

void CTest_Terrain::Free()
{
}
