#pragma once
#include "Base.h"
BEGIN(Engine)
class CRenderTarget;
class CTexture;
class CGameInstance;
END
BEGIN(Client)
class CMap_2D final : public CBase
{
private:
	CMap_2D(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CMap_2D() = default;

public:
	HRESULT	Initialize();

private: /* 외부 요청을 통해 Register할지, 생성시점에 무조건 Register 할지, 그냥 Section채로 그릴지 Renderer 작업하면서 생각 필요. */
	// 1. RenderTarget을 Target_Manager에 등록하는 기능.(DSV는 어떻게할지 생각해보자.) (x)
	HRESULT Register_RTV_ToTargetManager();
	// 2. Texture의 SRV를 RenderTarget의 RTV로 Copy하는 기능. (x)
	HRESULT Register_RenderGroup_ToRenderer();

public:
	// 3. RenderTarget의 RTV를 Get (o)
	ID3D11RenderTargetView* Get_RTV_FromRenderTarget();
	// 4. RenderTarget의 SRV를 Get (o)
	ID3D11ShaderResourceView* Get_SRV_FromRenderTarget();
	// 5. Texture의 SRV를 Get (o)
	ID3D11ShaderResourceView* Get_SRV_FromTexture(_uint _iTextureIndex);
	
private:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;
	CGameInstance*				m_pGameInstance = nullptr;

private:
	CRenderTarget*				m_pRenderTarget = nullptr;
	ID3D11DepthStencilView*		m_pDSV = nullptr;				// Map 별로 별도의 DSV를 가진다.
	CTexture*					m_pTextureCom = nullptr;		// Map Origin Texture;

public:
	static CMap_2D* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	void Free() override;
};
END
