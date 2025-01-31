#pragma once
#include "RenderGroup.h"
#include "EffectTool_Defines.h"
BEGIN(EffectTool)

class CETool_RenderGroup_AfterEffect : public CRenderGroup
{
private:
	CETool_RenderGroup_AfterEffect(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CETool_RenderGroup_AfterEffect() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	virtual HRESULT Render(CShader* _pRTShader, CVIBuffer_Rect* _pRTBuffer) override;

public:
	static CETool_RenderGroup_AfterEffect* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg);
	void Free() override;
};

END