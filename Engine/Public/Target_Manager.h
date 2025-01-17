#pragma once
#include "Base.h"

BEGIN(Engine)
class CRenderTarget;
class CShader;
class CVIBuffer_Rect;
class CTarget_Manager final : public CBase
{
private:
	CTarget_Manager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CTarget_Manager() = default;

public:
	HRESULT Initialize();
	HRESULT Add_RenderTarget(const _wstring& _strTargetTag, _uint _iWidth, _uint _iHeight, DXGI_FORMAT _ePixelFormat, const _float4& _vClearColor);
	HRESULT Add_MRT(const _wstring& _strMRTTag, const _wstring& _strTargetTag);
	HRESULT Begin_MRT(const _wstring& _strMRTTag, ID3D11DepthStencilView* _pDSV = nullptr, _bool isClear = true); /* 지정한 RenderTargets를 셰이더에 바인딩하고 그리기 준비를 하는 */
	HRESULT End_MRT(); /* 렌더링을 마친 후, 기존의 BackRTV를 다시 바인딩 한다. */
	HRESULT Clear_MRT(const _wstring& _strMRTTag, ID3D11DepthStencilView* _pDSV = nullptr, _bool isClear = true);
	HRESULT Bind_ShaderResource(CShader* _pShader, const _char* _pConstantName, const _wstring& _strTargetTag); /* RenderTarget을 ShaderResource로써 바인딩하는 함수. */
	HRESULT Copy_RT_Resource(const _wstring& _strTargetTag, ID3D11Texture2D* _pDest);
	HRESULT Copy_BackBuffer_RT_Resource(const _wstring& _strTargetTag);

#ifdef _DEBUG
public:
	HRESULT Ready_Debug(const _wstring& _strTargetTag, _float _fX, _float _fY, _float _fSizeX, _float _fSizeY); /* 렌더타겟을 디버그용으로 렌더하기위한 함수 */
	HRESULT Render_Debug(const _wstring& _strMRTTag, CShader* _pShader, CVIBuffer_Rect* _pVIBufferRect); /* 디버그 렌더 함수 */
#endif // _DEBUG

public:
	ID3D11ShaderResourceView*						Get_SRV(const _wstring& _strTargetTag);
	_float2											Get_RT_Size(const _wstring& _strTargetTag);
	map<const _wstring, CRenderTarget*>&			Get_RenderTargets() { return m_RenderTargets; }
	map<const _wstring, list<CRenderTarget*>>&		Get_MRTs() { return m_MRTs; }
private:
	ID3D11Device*									m_pDevice = nullptr;
	ID3D11DeviceContext*							m_pContext = nullptr;

	/* 원본 백버퍼의 정보들을 보존하기 위함. */
	ID3D11RenderTargetView*							m_pBackRTV = nullptr;
	ID3D11DepthStencilView*							m_pOriginalDSV = nullptr;
	map<const _wstring, CRenderTarget*>				m_RenderTargets;
	map<const _wstring, list<CRenderTarget*>>		m_MRTs; // 후처리 셰이딩을 위해 한번에 여러개의 RenderTarget을 바인딩해야한다. RenderTarget array를 손쉽게 구성하기위해 미리 RenderTarget을 용도별로 모아둔다.
public:
	CRenderTarget* Find_RenderTarget(const _wstring& _strTargetTag);
	list<CRenderTarget*>* Find_MRT(const _wstring _strMRTTag);

public:
	static CTarget_Manager* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;
};

END