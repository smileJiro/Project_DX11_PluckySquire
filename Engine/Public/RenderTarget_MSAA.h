#pragma once
#include "RenderTarget.h"
BEGIN(Engine)
class CGameInstance;
class  CRenderTarget_MSAA final : public CRenderTarget
{
protected:
	CRenderTarget_MSAA(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CRenderTarget_MSAA() = default;

public:
	HRESULT						Initialize(_wstring _strName, _uint _iWidth, _uint _iHeight, DXGI_FORMAT _ePixelFormat, const _float4& _vClearColor);
	virtual ID3D11RenderTargetView* Get_RTV() const { return m_pRTV_MSAA; }
	virtual HRESULT				Clear() override; // Clear() 함수는 직접 렌더링을 수행하고있는 RTV를 지워야한다. >>> 그래서 MSAA RTV를 지우는 용으로 override
	HRESULT						Resolve_MSAA();
private:
	CGameInstance*				m_pGameInstance = nullptr;

private: 
	/* 고해상도의 hdri 이미지가 포함된 렌더링을 하는 도화지.(float buffer) */ 
	ID3D11Texture2D*			m_pTexture2D_MSAA = nullptr;
	ID3D11RenderTargetView*		m_pRTV_MSAA = nullptr; 
	ID3D11ShaderResourceView*	m_pSRV_MSAA = nullptr; 
	ID3D11DepthStencilView*		m_pDSV_MSAA = nullptr;
	_uint						m_iNumQualityLevels = 0;
	/* 부모클래스에 정의 된 RTV는 , SRV는 MSAA RTV를 Resolving 후 복사하여 다른 셰이더에 바인드 하는 용도. */
	
private:
	HRESULT						Ready_DSV_MSAA(_uint _iWidth, _uint _iHeight);

public:
	static CRenderTarget_MSAA*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _wstring _strName, _uint _iWidth, _uint _iHeight, DXGI_FORMAT _ePixelFormat, const _float4& _vClearColor);
	void						Free() override;
};
END
