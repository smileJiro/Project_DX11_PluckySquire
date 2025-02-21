#include "stdafx.h"
#include "WordPrinter.h"
#include "GameInstance.h"

CWordPrinter::CWordPrinter(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContex)
	:m_pDevice(_pDevice)
	,m_pContext(_pContex)
	,m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}


HRESULT CWordPrinter::Initialize()
{

	m_tSampleRenderTargetDesc.Width = 100;
	m_tSampleRenderTargetDesc.Height = 100;
	m_tSampleRenderTargetDesc.MipLevels = 1;
	m_tSampleRenderTargetDesc.ArraySize = 1;
	m_tSampleRenderTargetDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // RenderTarget의 용도에 따라 format이 달라진다. (데이터의 구조)

	m_tSampleRenderTargetDesc.SampleDesc.Quality = 0;
	m_tSampleRenderTargetDesc.SampleDesc.Count = 1;

	m_tSampleRenderTargetDesc.Usage = D3D11_USAGE_DEFAULT;
	m_tSampleRenderTargetDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE; // RenderTarget으로도 사용하지만 Shader의 Resource로도 사용된다.

	m_tSampleRenderTargetDesc.CPUAccessFlags = 0;
	m_tSampleRenderTargetDesc.MiscFlags = 0;


	return S_OK;
}

ID3D11ShaderResourceView* CWordPrinter::Print_Word(const _wstring& _strWordText, _float2 _fSize)
{
	ID3D11Texture2D*		pTexture2D = nullptr;
	ID3D11RenderTargetView* pRenderTarget = nullptr;
	ID3D11ShaderResourceView* pSRV = nullptr;



#pragma region Render Target Init

	_float4	fClearColor = { 0.f,0.f,0.f,0.f };
	D3D11_TEXTURE2D_DESC	TextureDesc = m_tSampleRenderTargetDesc;

	TextureDesc.Width = (_uint)_fSize.x;
	TextureDesc.Height = (_uint)_fSize.y;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pTexture2D)))
		return nullptr;
	if (FAILED(m_pDevice->CreateRenderTargetView(pTexture2D, nullptr, &pRenderTarget)))
		return nullptr;
	if (FAILED(m_pDevice->CreateShaderResourceView(pTexture2D, nullptr, &pSRV)))
		return nullptr;

#pragma endregion


#pragma region Render Target Setting

	ID3D11ShaderResourceView* pSRVs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {
		nullptr
	};

	m_pContext->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, pSRVs);
	m_pContext->OMGetRenderTargets(1, &m_pBackRTV, &m_pOriginalDSV);

	ID3D11RenderTargetView* pRenderTargets[8] = { pRenderTarget, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

	m_pContext->OMSetRenderTargets(8, pRenderTargets, m_pOriginalDSV);

#pragma endregion


#pragma region 그리기

	m_pGameInstance->Render_Font(L"Font28", _strWordText.c_str(), { 0.f,0.f },
		XMVectorSet(0.f, 0.f, 0.f, 1.f));

#pragma endregion


#pragma region 백버퍼 원상복구, 리소스 정리

	m_pContext->OMSetRenderTargets(1, &m_pBackRTV, m_pOriginalDSV);

	Safe_Release(m_pOriginalDSV);
	Safe_Release(m_pBackRTV);
	Safe_Release(pRenderTarget);
	Safe_Release(pTexture2D);

#pragma endregion

	//반환
	return pSRV;
}

CWordPrinter* CWordPrinter::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CWordPrinter* pInstance = new CWordPrinter(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CWordPrinter");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWordPrinter::Free()
{

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);


	__super::Free();
}
