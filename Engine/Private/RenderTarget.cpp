#include "RenderTarget.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"
CRenderTarget::CRenderTarget(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : m_pDevice(_pDevice)
    , m_pContext(_pContext)
{
    Safe_AddRef(_pDevice);
    Safe_AddRef(_pContext);
}

HRESULT CRenderTarget::Initialize(_uint _iWidth, _uint _iHeight, DXGI_FORMAT _ePixelFormat, const _float4& _vClearColor)
{
	m_vClearColor = _vClearColor;
	m_vSize.x = _iWidth;
	m_vSize.y = _iHeight;
	/* 구조체에 RenderTarget의 값을 할당한 뒤, RenderTarget을 생성. */
	D3D11_TEXTURE2D_DESC			TextureDesc{};

	TextureDesc.Width = m_vSize.x;
	TextureDesc.Height = m_vSize.y;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = _ePixelFormat; // RenderTarget의 용도에 따라 format이 달라진다. (데이터의 구조)

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE; // RenderTarget으로도 사용하지만 Shader의 Resource로도 사용된다.

	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture2D, nullptr, &m_pRTV)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture2D, nullptr, &m_pSRV)))
		return E_FAIL;


	return S_OK;
}

#ifdef _DEBUG

HRESULT CRenderTarget::Ready_Debug(_float _fX, _float _fY, _float _fSizeX, _float _fSizeY)
{
	_uint iNumViewports = 1;
	D3D11_VIEWPORT ViewportDesc = {};

	/* 1. 뷰포트 정보를 가져온다. */
	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	/* 2. Scaling */
	m_WorldMatrix._11 = _fSizeX;
	m_WorldMatrix._22 = _fSizeY;

	/* 3. Translation */

	m_WorldMatrix._41 = _fX - ViewportDesc.Width * 0.5f; /* 플마 width 0.5 */
	m_WorldMatrix._42 = -_fY + ViewportDesc.Height * 0.5f; /* 마플 height 0.5 */

	return S_OK;
}

HRESULT CRenderTarget::Render_Debug(CShader* _pShader, CVIBuffer_Rect* _pVIBufferRect)
{
	if (FAILED(_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(_pShader->Bind_SRV("g_Texture", m_pSRV)))
		return E_FAIL;

	_pShader->Begin(0);

	_pVIBufferRect->Render();



	return S_OK;
}
#endif // _DEBUG


HRESULT CRenderTarget::Clear()
{
	/* 선택한 색상으로 RenderTarget을 Clear하는 함수 */
	m_pContext->ClearRenderTargetView(m_pRTV, (_float*)&m_vClearColor);

	return S_OK;
}

HRESULT CRenderTarget::Bind_ShaderResource(CShader* _pShader, const _char* _pConstantName)
{
	return _pShader->Bind_SRV(_pConstantName, m_pSRV);
}

HRESULT CRenderTarget::Copy_Resource(ID3D11Texture2D* _pDest)
{
	if (nullptr == m_pTexture2D)
		return E_FAIL;

	m_pContext->CopyResource(_pDest, m_pTexture2D);

	return S_OK;
}

CRenderTarget* CRenderTarget::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _uint _iWidth, _uint _iHeight, DXGI_FORMAT _ePixelFormat, const _float4& _vClearColor)
{
	CRenderTarget* pInstance = new CRenderTarget(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_iWidth, _iHeight, _ePixelFormat, _vClearColor)))
	{
		MSG_BOX("Failed to Created : CRenderTarget");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRenderTarget::Free()
{
	Safe_Release(m_pTexture2D);
	Safe_Release(m_pRTV);
	Safe_Release(m_pSRV);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	__super::Free();

}
