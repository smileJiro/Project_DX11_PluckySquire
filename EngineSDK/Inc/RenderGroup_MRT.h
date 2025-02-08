#pragma once
#include "RenderGroup.h"

BEGIN(Engine)

class ENGINE_DLL CRenderGroup_MRT : public CRenderGroup
{
public:
	typedef struct tagRG_MRTDesc : public CRenderGroup::RG_DESC
	{
		_wstring				strMRTTag;
		_bool					isClear = true;

		_bool					isViewportSizeChange = false;
		_float2					vViewportSize = {};
		ID3D11DepthStencilView* pDSV = nullptr;
		
	}RG_MRT_DESC;

protected:
	CRenderGroup_MRT(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CRenderGroup_MRT() = default;

public:
	HRESULT Initialize(void* _pArg) override;
	HRESULT Render(CShader* _pRTShader, CVIBuffer_Rect* _pRTBuffer) override;

public:
	const _wstring& Get_MRTTag() const { return m_strMRTTag; }

protected:
	_wstring					m_strMRTTag;
	ID3D11DepthStencilView*		m_pDSV = nullptr; // dsv는 참 애매해 ㅅㅂ, 렌더러 등록이맞어
	_bool						m_isClear = true;

	_bool						m_isViewportSizeChange = false;
	_float2						m_vViewportSize = {};

public:
	static CRenderGroup_MRT* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pDesc);
	virtual void Free() override;
};

END