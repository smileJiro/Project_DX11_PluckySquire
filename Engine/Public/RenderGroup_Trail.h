#pragma once
#include "RenderGroup_MRT.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class ENGINE_DLL CRenderGroup_Trail : public CRenderGroup_MRT
{
public:

protected:
	CRenderGroup_Trail(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CRenderGroup_Trail() = default;

public:
	HRESULT Initialize(void* _pArg) override;
	HRESULT Render(CShader* _pRTShader, CVIBuffer_Rect* _pRTBuffer) override;

public:
	static CRenderGroup_Trail* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pDesc);
	virtual void Free() override;
};

END