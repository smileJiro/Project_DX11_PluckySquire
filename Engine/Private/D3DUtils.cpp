#include "D3DUtils.h"
#include "GameInstance.h"

CD3DUtils::CD3DUtils(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :m_pDevice(_pDevice)
    ,m_pContext(_pContext)
    ,m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

HRESULT CD3DUtils::Initialize()
{
    return S_OK;
}

HRESULT CD3DUtils::Create_DSV(_uint _iWidth, _uint _iHeight, ID3D11DepthStencilView** _ppOutDSV)
{
	ID3D11Texture2D* pDepthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	/* RTV의 사이즈와 DSV의 사이즈가 일치 되어야한다. */
	TextureDesc.Width = _iWidth;
	TextureDesc.Height = _iHeight;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;

	ID3D11DepthStencilView* pDSV = nullptr;
	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &pDSV)))
		return E_FAIL;

	*_ppOutDSV = pDSV;
	Safe_AddRef(pDSV);


	Safe_Release(pDepthStencilTexture);
	Safe_Release(pDSV);

	return S_OK;
}

CD3DUtils* CD3DUtils::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CD3DUtils* pInstance = new CD3DUtils(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CD3DUtils");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CD3DUtils::Free()
{
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);

    __super::Free();
}
