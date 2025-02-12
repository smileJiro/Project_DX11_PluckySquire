#pragma once
#include "RenderGroup_MRT.h"

BEGIN(AnimTool)

class CRenderGroup_Combine final : public CRenderGroup_MRT
{
private:
	CRenderGroup_Combine(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CRenderGroup_Combine() = default;

public:
	HRESULT Initialize(void* _pArg) override;
	HRESULT Render(CShader* _pRTShader, CVIBuffer_Rect* _pRTBuffer) override;
#ifdef _DEBUG
	virtual void				Update_Imgui();
#endif // _DEBUG

public:
	static CRenderGroup_Combine* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg);
	void Free() override;
};

END