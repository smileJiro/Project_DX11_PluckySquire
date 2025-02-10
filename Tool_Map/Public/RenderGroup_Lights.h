#pragma once
#include "RenderGroup_MRT.h"

BEGIN(Map_Tool)

class CRenderGroup_Lights final : public CRenderGroup_MRT
{
private:
	CRenderGroup_Lights(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CRenderGroup_Lights() = default;

public:
	HRESULT Initialize(void* _pArg) override;
	HRESULT Render(CShader* _pRTShader, CVIBuffer_Rect* _pRTBuffer) override;

public:
	static CRenderGroup_Lights* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg);
	void Free() override;
};

END