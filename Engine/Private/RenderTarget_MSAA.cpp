#include "RenderTarget_MSAA.h"
#include "GameInstance.h"

CRenderTarget_MSAA::CRenderTarget_MSAA(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CRenderTarget(_pDevice, _pContext)
	,m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CRenderTarget_MSAA::Initialize(_wstring _strName, _uint _iWidth, _uint _iHeight, DXGI_FORMAT _ePixelFormat, const _float4& _vClearColor)
{
	/* Create Resolving RenderTarget */
    if (FAILED(__super::Initialize(_strName, _iWidth, _iHeight, _ePixelFormat, _vClearColor)))
        return E_FAIL;

	/* MSAA RenderTargetView */
	if(FAILED(m_pDevice->CheckMultisampleQualityLevels(_ePixelFormat, 4, &m_iNumQualityLevels)))
		return E_FAIL;

	/* Create  RenderTarget_MSAA */
	D3D11_TEXTURE2D_DESC			TextureDesc{};

	TextureDesc.Width = (_uint)m_vSize.x;
	TextureDesc.Height = (_uint)m_vSize.y;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = m_ePixelFormat;

	TextureDesc.SampleDesc.Quality = m_iNumQualityLevels - 1;
	TextureDesc.SampleDesc.Count = 4;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE; // RenderTarget으로도 사용하지만 Shader의 Resource로도 사용된다.

	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D_MSAA)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture2D_MSAA, nullptr, &m_pRTV_MSAA)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture2D_MSAA, nullptr, &m_pSRV_MSAA)))
		return E_FAIL;

	if (FAILED(Ready_DSV_MSAA(_iWidth, _iHeight)))
		return E_FAIL;

    return S_OK;
}

HRESULT CRenderTarget_MSAA::Clear()
{
	m_pContext->ClearRenderTargetView(m_pRTV_MSAA, (_float*)&m_vClearColor);

	return S_OK;
}

HRESULT CRenderTarget_MSAA::Resolve_MSAA()
{
	/* MSAA Texture를 단일 샘플 텍스쳐로 변경 후 카피 */

	m_pContext->ResolveSubresource(m_pTexture2D, 0, m_pTexture2D_MSAA, 0, m_ePixelFormat);
	m_pContext->PSSetShaderResources(0, 1, &m_pSRV);
	return S_OK;
}

HRESULT CRenderTarget_MSAA::Ready_DSV_MSAA(_uint _iWidth, _uint _iHeight)
{
	if (nullptr == m_pDevice)
		return E_FAIL;
	/* MSAA RenderTargetView */
	if (FAILED(m_pDevice->CheckMultisampleQualityLevels(m_ePixelFormat, 4, &m_iNumQualityLevels)))
		return E_FAIL;

	ID3D11Texture2D* pDepthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	/* RTV의 사이즈와 DSV의 사이즈가 일치 되어야한다. */
	TextureDesc.Width = _iWidth;
	TextureDesc.Height = _iHeight;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	TextureDesc.SampleDesc.Quality = m_iNumQualityLevels - 1;
	TextureDesc.SampleDesc.Count = 4;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pDSV_MSAA)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);
	m_pGameInstance->Add_DSV_ToRenderer(m_strName, m_pDSV_MSAA);


	return S_OK;
}

CRenderTarget_MSAA* CRenderTarget_MSAA::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _wstring _strName, _uint _iWidth, _uint _iHeight, DXGI_FORMAT _ePixelFormat, const _float4& _vClearColor)
{
	CRenderTarget_MSAA* pInstance = new CRenderTarget_MSAA(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_strName, _iWidth, _iHeight, _ePixelFormat, _vClearColor)))
	{
		MSG_BOX("Failed to Created : CRenderTarget_MSAA");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRenderTarget_MSAA::Free()
{
	Safe_Release(m_pGameInstance);

	Safe_Release(m_pDSV_MSAA);
	Safe_Release(m_pSRV_MSAA);
	Safe_Release(m_pRTV_MSAA);
	Safe_Release(m_pTexture2D_MSAA);

	__super::Free();
}
