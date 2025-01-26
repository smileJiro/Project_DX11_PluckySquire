#include "stdafx.h"
#include "CMap_2D.h"
#include "RenderTarget.h"

CMap_2D::CMap_2D(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:m_pDevice(_pDevice)
	,m_pContext(_pContext)
{
	Safe_AddRef(_pDevice);
	Safe_AddRef(_pContext);
}

HRESULT CMap_2D::Initialize()
{
	// 0. Map Data Parsing



	// 1. CRenderTarget Create
	// 2. Create DSV & Register DSV To Renderer (Renderer 작업하면서 고민 필요.)
	// 3. Add_RenderTarget_ToTargetManager();
	// 4. Add_RenderGroup_ToRenderer();
	// 5. Texture Component Clone


	return S_OK;
}

HRESULT CMap_2D::Add_RenderTarget_ToTargetManager()
{
	// Renderer 수정 후 작업 예정.


	return S_OK;
}

HRESULT CMap_2D::Add_RenderGroup_ToRenderer()
{
	// Renderer 수정 후 작업 예정.

	return S_OK;
}

ID3D11RenderTargetView* CMap_2D::Get_RTV_FromRenderTarget()
{
	if (nullptr == m_pRenderTarget)
		return nullptr;

	m_pRenderTarget
	return nullptr;
}

ID3D11ShaderResourceView* CMap_2D::Get_SRV_FromRenderTarget()
{
	return nullptr;
}

ID3D11ShaderResourceView* CMap_2D::Get_SRV_FromTexture()
{
	return nullptr;
}

CMap_2D* CMap_2D::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CMap_2D* pInstance = new CMap_2D(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed Create CMap_2D");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMap_2D::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	__super::Free();
}
