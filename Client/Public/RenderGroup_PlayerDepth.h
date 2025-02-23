#pragma once
#include "RenderGroup_MRT.h"

BEGIN(Engine)

class CRenderGroup_PlayerDepth : public CRenderGroup_MRT
{
public:
	typedef struct tagRG_PlayerDepthDesc : public CRenderGroup_MRT::RG_MRT_DESC
	{
	}RG_PLAYERDEPTH_DESC;
protected:
	CRenderGroup_PlayerDepth(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CRenderGroup_PlayerDepth() = default;

public:
	HRESULT Initialize(void* _pArg) override;
	HRESULT Render(CShader* _pRTShader, CVIBuffer_Rect* _pRTBuffer) override;


public:
	static CRenderGroup_PlayerDepth* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pDesc);
	virtual void Free() override;
};

END