#include "stdafx.h"
#include "Map_2D.h"
#include "GameInstance.h"
#include "RenderTarget.h"


CMap_2D::CMap_2D(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:m_pDevice(_pDevice)
	,m_pContext(_pContext)
	,m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMap_2D::Initialize()
{
	// 0. Map Data Parsing
	// 1. CRenderTarget Create
	// 2. Create DSV & Register DSV To Renderer (Renderer 작업하면서 고민 필요.)
	// 3. Register_RTV_ToTargetManager();
	// 4. Resister_RenderGroup_ToRenderer();
	// 5. Texture Component Clone
	return S_OK;
}

HRESULT CMap_2D::Register_RTV_ToTargetManager()
{
	// Renderer 수정 후 작업 예정.

	return S_OK;
}

HRESULT CMap_2D::Register_RenderGroup_ToRenderer()
{
	// Renderer 수정 후 작업 예정.

	return S_OK;
}

ID3D11RenderTargetView* CMap_2D::Get_RTV_FromRenderTarget()
{
	if (nullptr == m_pRenderTarget)
		return nullptr;

	return m_pRenderTarget->Get_RTV();
}

ID3D11ShaderResourceView* CMap_2D::Get_SRV_FromRenderTarget()
{
	if (nullptr == m_pRenderTarget)
		return nullptr;

	return m_pRenderTarget->Get_SRV();
}

ID3D11ShaderResourceView* CMap_2D::Get_SRV_FromTexture(_uint _iTextureIndex)
{
	if (nullptr == m_pTextureCom)
		return nullptr;

	return m_pTextureCom->Get_SRV(_iTextureIndex);
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
	Safe_Release(m_pDSV);
	Safe_Release(m_pRenderTarget);
	Safe_Release(m_pTextureCom);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	__super::Free();
}
