#pragma once
#include "RenderGroup_MRT.h"
#include "EffectTool_Defines.h"

/* 직접광원에 대한 연산을 처리, msaa x, 다만 광원의 포맷은 float16 */
BEGIN(EffectTool)
class CRenderGroup_DirectLights final : public CRenderGroup_MRT
{
private:
	CRenderGroup_DirectLights(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CRenderGroup_DirectLights() = default;

public:
	HRESULT Initialize(void* _pArg) override;
	HRESULT Render(CShader* _pRTShader, CVIBuffer_Rect* _pRTBuffer) override;

public:
	static CRenderGroup_DirectLights* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg);
	void Free() override;
};

END