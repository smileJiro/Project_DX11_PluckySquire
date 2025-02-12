#pragma once
#include "RenderGroup_MRT.h"
#include "Camera_Tool_Defines.h"

BEGIN(Camera_Tool)

class CRenderGroup_AfterParticle : public CRenderGroup_MRT
{
private:
	CRenderGroup_AfterParticle(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CRenderGroup_AfterParticle() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	virtual HRESULT Render(CShader* _pRTShader, CVIBuffer_Rect* _pRTBuffer) override;

public:
	static CRenderGroup_AfterParticle* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg);
	void Free() override;

};

END