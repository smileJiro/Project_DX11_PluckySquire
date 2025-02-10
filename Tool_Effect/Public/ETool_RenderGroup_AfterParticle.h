#pragma once
#include "RenderGroup_MRT.h"
#include "EffectTool_Defines.h"
BEGIN(EffectTool)

class CETool_RenderGroup_AfterParticle : public CRenderGroup_MRT
{
private:
	CETool_RenderGroup_AfterParticle(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CETool_RenderGroup_AfterParticle() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	virtual HRESULT Render(CShader* _pRTShader, CVIBuffer_Rect* _pRTBuffer) override;

public:
	static CETool_RenderGroup_AfterParticle* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg);
	void Free() override;
};

END