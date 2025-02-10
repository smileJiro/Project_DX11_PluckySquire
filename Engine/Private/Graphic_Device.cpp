#include "Graphic_Device.h"

CGraphic_Device::CGraphic_Device()
	: m_pDevice(nullptr)
	, m_pDeviceContext(nullptr)
{
}

HRESULT CGraphic_Device::Initialize(HWND _hWnd, _bool _isWindowed, _uint _iWinSizeX, _uint _iWinSizeY, _Inout_ ID3D11Device** _ppDevice, _Inout_ ID3D11DeviceContext** _ppContext)
{
	_uint iFlag = 0;
#ifdef _DEBUG
	iFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif
	
	
	// 장치와 dx 버전간의 호환 정보를 저장.
	D3D_FEATURE_LEVEL FeatureLV;
	
	// dx9 : 장치 초기화를 하기위한 설정을 쭈욱 하고나서 최종 장치 객체 생성.
	// dx11 : 우선적으로 장치 객체를 생성하고 장치 객체를 통해서 기타 초기화 작업 및 설정을 해나간다.

	// 그래픽 장치를 우선적으로 초기화한다.
	if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, iFlag, nullptr, 0,
		D3D11_SDK_VERSION, &m_pDevice, &FeatureLV, &m_pDeviceContext)))
		return E_FAIL;

	// SwapChain : 더블 버퍼링에 필요한 백버퍼를 생성한다.
	// Back Buffer 용 Texture2D를 생성,
	if (FAILED(Ready_SwapChain(_hWnd, _isWindowed, _iWinSizeX, _iWinSizeY)))
		return E_FAIL;

	// 위에서 생성한 Texture2D 객체를 통해 RenderTargetView Setting
	if (FAILED(Ready_BackBufferRenderTargetView()))
		return E_FAIL;

	// 깊이, 스텐실 버퍼도 함께 생성한다. 
	if (FAILED(Ready_DepthStencilView(_iWinSizeX, _iWinSizeY)))
		return E_FAIL;
	
	ID3D11RenderTargetView* pRTVs[1] = { m_pBackBufferRTV, };

	m_pDeviceContext->OMSetRenderTargets(1, pRTVs, m_pDepthStencilView);

	D3D11_VIEWPORT ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)_iWinSizeX;
	ViewPortDesc.Height = (_float)_iWinSizeY;
	ViewPortDesc.MinDepth = 0.0f;
	ViewPortDesc.MaxDepth = 1.0f;

	// 설정한 값들을 기반으로 뷰포트 세팅
	m_pDeviceContext->RSSetViewports(1, &ViewPortDesc);

	// 외부에서 파라미터로 들어온 디바이스와 컨텍스트에 값 담기. ( return)
	*_ppDevice = m_pDevice;
	*_ppContext = m_pDeviceContext;


	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);

	return S_OK;
}

HRESULT CGraphic_Device::Clear_BackBuffer_View(_float4 _vClearColor)
{
	if (nullptr == m_pDeviceContext)
	{
		return E_FAIL;
	}

	// dx9 기준 : Clear() : 백버퍼와 깊이 버퍼 모두 일괄 삭제
	// dx11 : 범위와 영역을 설정

	// RenderTargetView를 지우겠다 명시된 함수. 
	m_pDeviceContext->ClearRenderTargetView(m_pBackBufferRTV, (_float*)&_vClearColor);
	m_pDeviceContext->OMSetRenderTargets(1, &m_pBackBufferRTV, m_pDepthStencilView);


	return S_OK;
}

HRESULT CGraphic_Device::Clear_DepthStencil_View()
{
	if (nullptr == m_pDeviceContext)
	{
		return E_FAIL;
	}

	// Depth : 1.0 , Stencil : 0 으로 초기화.
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	return S_OK;
}

HRESULT CGraphic_Device::Present()
{
	if (nullptr == m_pSwapChain)
	{
		return E_FAIL;
	}

	return m_pSwapChain->Present(0, 0);
}

HRESULT CGraphic_Device::Ready_SwapChain(HWND _hWnd, _bool _isWindowed, _uint _iWinCX, _uint _iWinCY)
{
	IDXGIDevice* pDevice = nullptr;
	m_pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDevice);

	IDXGIAdapter* pAdapter = nullptr;
	pDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pAdapter);

	IDXGIFactory* pFactory = nullptr;
	pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pFactory);

	DXGI_SWAP_CHAIN_DESC SwapChain;
	ZeroMemory(&SwapChain, sizeof(DXGI_SWAP_CHAIN_DESC));

	/* 백버퍼 == 텍스쳐 */
	/*텍스처(백버퍼 == ID3D11Texture2D)를 생성하는 행위*/
	SwapChain.BufferDesc.Width = _iWinCX;	/* 가로 픽셀 수 */
	SwapChain.BufferDesc.Height = _iWinCY;	/* 세로 픽셀 수 */

	/* float4(1.f, 1.f, 1.f, 1.f) */
	/* float4(1.f, 0.f, 0.f, 1.f) */
	SwapChain.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // DXGI_FORMAT_B8G8R8A8_UNORM; /*DXGI_FORMAT_R16G16B16A16_FLOAT*/ /* 만든 픽셀하나의 데이터 정보 : 32BIT픽셀생성하되 부호가 없는 정규화된 수를 저장할께 */
	SwapChain.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChain.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	/* 스케치북에 사과를 그릴꺼야. */
	/* RENDER_TARGET : 그림을 당하는 대상. 스케치북 */
	SwapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChain.BufferCount = 2;

	/*스왑하는 형태 : 모니터 주사율에 따라 조절해도 됨. */
	SwapChain.BufferDesc.RefreshRate.Numerator = 60;
	SwapChain.BufferDesc.RefreshRate.Denominator = 1;

	/* 멀티샘플링 : 안티얼라이징 (계단현상방지) */
	/* 나나중에 후처리 렌더링 : 멀티샘플링 지원(x) */
	SwapChain.SampleDesc.Quality = 0;
	SwapChain.SampleDesc.Count = 1;

	SwapChain.OutputWindow = _hWnd;
	SwapChain.Windowed = _isWindowed;
	SwapChain.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	SwapChain.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	if (FAILED(pFactory->CreateSwapChain(m_pDevice, &SwapChain, &m_pSwapChain)))
		return E_FAIL;


	Safe_Release(pFactory);
	Safe_Release(pAdapter);
	Safe_Release(pDevice);

	return S_OK;
}

HRESULT CGraphic_Device::Ready_BackBufferRenderTargetView()
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	// BackBuffer 생성하기 위한. BaseData Type Object
	ID3D11Texture2D* pBackBufferTexture = nullptr;
	if (FAILED(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBufferTexture)))
		return E_FAIL;

	/* 실제 렌더타겟용도로 사용할 수 있는 텍스쳐 타입(ID3D11RenderTargetView)의 객체를 생성ㅎ나다. */
	if (FAILED(m_pDevice->CreateRenderTargetView(pBackBufferTexture, nullptr, &m_pBackBufferRTV)))
		return E_FAIL;

	Safe_Release(pBackBufferTexture);

	return S_OK;
}

HRESULT CGraphic_Device::Ready_DepthStencilView(_uint _iWinCX, _uint _iWinCY)
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	ID3D11Texture2D* pDepthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	/* 깊이 버퍼의 픽셀으 ㄴ백버퍼의 픽셀과 갯수가 동일해야만 깊이 텍스트가 가능해진다. */
	/* 픽셀의 수가 다르면 아에 렌덜잉을 못함. */
	TextureDesc.Width = _iWinCX;
	TextureDesc.Height = _iWinCY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	/* 동적? 정적?  */
	TextureDesc.Usage = D3D11_USAGE_DEFAULT /* 정적 */;
	/* 추후에 어떤 용도로 바인딩 될 수 있는 View타입의 텍스쳐를 만들기위한 Texture2D입니까? */
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL
		/*| D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE*/;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;

	/* RenderTargetView */
	/* ShaderResourceView */
	/* DepthStencilView */

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pDepthStencilView)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);

	return S_OK;
}

CGraphic_Device* CGraphic_Device::Create(HWND hWnd, _bool isWindowed, _uint iWinSizeX, _uint iWinSizeY, ID3D11Device** ppDevice, ID3D11DeviceContext** ppDeviceContextOut)
{
	CGraphic_Device* pInstance = new CGraphic_Device();

	if (FAILED(pInstance->Initialize(hWnd, isWindowed, iWinSizeX, iWinSizeY, ppDevice, ppDeviceContextOut)))
	{
		MSG_BOX("Failed to Created : CGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGraphic_Device::Free()
{
	Safe_Release(m_pSwapChain);
	Safe_Release(m_pDepthStencilView);
	Safe_Release(m_pBackBufferRTV);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDepthTexture);


	//#if defined(DEBUG) || defined(_DEBUG)
	//	ID3D11Debug* d3dDebug;
	//	HRESULT hr = m_pDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&d3dDebug));
	//	if (SUCCEEDED(hr))
	//	{
	//		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
	//		OutputDebugStringW(L"                                                                    D3D11 Live Object ref Count Checker \r ");
	//		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
	//
	//		hr = d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	//
	//		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
	//		OutputDebugStringW(L"                                                                    D3D11 Live Object ref Count Checker END \r ");
	//		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
	//	}
	//	if (d3dDebug != nullptr)            d3dDebug->Release();
	//#endif


	Safe_Release(m_pDevice);
}
