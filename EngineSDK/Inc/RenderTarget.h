#pragma once
#include "Base.h"
/* 렌더 타겟을 보유하는 객체 */
BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CRenderTarget final : public CBase
{
public:

private:
	CRenderTarget(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CRenderTarget() = default;

public:
	HRESULT Initialize(_wstring _strName, _uint _iWidth, _uint _iHeight, DXGI_FORMAT _ePixelFormat, const _float4& _vClearColor);

#ifdef _DEBUG
public:
	HRESULT Ready_Debug(_float _fX, _float _fY, _float _fSizeX, _float _fSizeY);
	HRESULT Render_Debug(CShader* _pShader, CVIBuffer_Rect* _pVIBufferRect);
#endif // _DEBUG

public:
	HRESULT Clear();
	HRESULT Bind_ShaderResource(CShader* _pShader, const _char* _pConstantName);

	HRESULT Copy_Resource(ID3D11Texture2D* _pDest);
	ID3D11Texture2D* Get_Resource() { return m_pTexture2D; }
public:
	ID3D11RenderTargetView*		Get_RTV() const { return m_pRTV; }
	ID3D11ShaderResourceView*	Get_SRV() const { return m_pSRV; }
	_float2						Get_Size() const { return m_vSize; }
	const _wstring&				Get_Name() const { return m_strName; }
private:
	_wstring					m_strName;
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;
	ID3D11Texture2D*			m_pTexture2D = nullptr;
	ID3D11RenderTargetView*		m_pRTV = nullptr; /* 렌더 타겟으로 사용하기 위한 타입. */
	ID3D11ShaderResourceView*	m_pSRV = nullptr;
	_float4						m_vClearColor = {};
	_float2						m_vSize = {};
#ifdef _DEBUG
private:
	_float4x4					m_WorldMatrix = {}; /* 디버그용 렌더를 하기 위해 사용 할 WorldMatrix */
#endif // _DEBUG


public:
	static CRenderTarget* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _wstring _strName, _uint _iWidth, _uint _iHeight, DXGI_FORMAT _ePixelFormat, const _float4& _vClearColor);
	virtual void Free() override;
};

END