#pragma once
#include "RenderGroup_MRT.h"
#include "EffectTool_Defines.h"

class CRenderGroup_Blur : public CRenderGroup_MRT
{
public:
	typedef struct tagBlurDown : public CRenderGroup_MRT::RG_MRT_DESC
	{
		ID3D11DepthStencilView* pDSV1 = nullptr;
	}RGMRT_BLUR_DESC;

private:
	CRenderGroup_Blur(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CRenderGroup_Blur() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	virtual HRESULT Render(CShader* _pRTShader, CVIBuffer_Rect* _pRTBuffer) override;

private:
	ID3D11DepthStencilView* m_pDSV1 = { nullptr };

public:
	static CRenderGroup_Blur* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg);
	void Free() override;
};

