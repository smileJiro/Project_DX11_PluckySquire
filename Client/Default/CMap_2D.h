#pragma once
#include "Base.h"
BEGIN(Engine)
class CRenderTarget;
class CTexture;
END
BEGIN(Client)
class CMap_2D final : public CBase
{
private:
	CMap_2D(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CMap_2D() = default;

public:
	HRESULT	Initialize();
public:
	// 1. RenderTarget을 Target_Manager에 등록하는 기능.(DSV는 어떻게할지 생각해보자.)
	HRESULT Add_RenderTarget_ToTargetManager();
	// 2. Texture의 SRV를 RenderTarget의 RTV로 Copy하는 기능.
	HRESULT Add_RenderGroup_ToRenderer();

public:
	// 3. RenderTarget의 RTV를 Get
	ID3D11RenderTargetView* Get_RTV_FromRenderTarget();
	// 4. RenderTarget의 SRV를 Get
	ID3D11ShaderResourceView* Get_SRV_FromRenderTarget();
	// 5. Texture의 SRV를 Get
	ID3D11ShaderResourceView* Get_SRV_FromTexture();
	
private:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;

private:
	CRenderTarget*				m_pRenderTarget = nullptr;
	ID3D11DepthStencilView*		m_pDSV = nullptr; // Map 별로 별도의 DSV를 가진다.
	CTexture*					m_pTextureCom = nullptr; // Map Origin Texture;

public:
	static CMap_2D* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	void Free() override;
};
END
