#pragma once
#include "RenderGroup_MRT.h"

BEGIN(Client)

class CRenderGroup_Final final : public CRenderGroup
{
private:
	CRenderGroup_Final(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CRenderGroup_Final() = default;

public:
	HRESULT Initialize(void* _pArg) override;
	HRESULT Render(CShader* _pRTShader, CVIBuffer_Rect* _pRTBuffer) override;

public:
	static CRenderGroup_Final* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg);
	void Free() override;
};

END