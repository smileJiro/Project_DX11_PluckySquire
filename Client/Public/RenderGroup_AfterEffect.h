#pragma once
#include "RenderGroup_MRT.h"

BEGIN(Client)

class CRenderGroup_AfterEffect final : public CRenderGroup
{
private:
	CRenderGroup_AfterEffect(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CRenderGroup_AfterEffect() = default;

public:
	HRESULT Initialize(void* _pArg) override;
	HRESULT Render(CShader* _pRTShader, CVIBuffer_Rect* _pRTBuffer) override;

public:
	static CRenderGroup_AfterEffect* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg);
	void Free() override;
};

END