#pragma once
#include "RenderGroup_MRT.h"
BEGIN(Engine)
class CRenderTarget;
END
BEGIN(Camera_Tool)
/* 1. 블러 시행 레벨을 파라미터로 받아 생성한다. */
/* 2. 블러 레벨에따라 RenderTarget class를 생성한다. */
/* 3. 블러 레벨에따라 Rendering을 수행하는데, 이때 바인딩 될 RenderTarget에 대해 미리 규칙을 잡아놓는다. */

class CRenderGroup_PostProcessing final : public CRenderGroup
{
public:
	typedef struct tagPostProcessingDesc : CRenderGroup::RG_DESC
	{
		_uint iBlurLevel; /* 블러 다운샘플 및 업샘플 횟수. */
	}RG_POST_DESC;
private:
	CRenderGroup_PostProcessing(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CRenderGroup_PostProcessing() = default;

public:
	HRESULT Initialize(void* _pArg) override;
	HRESULT Render(CShader* _pRTShader, CVIBuffer_Rect* _pRTBuffer) override;

private:
	_uint m_iBlurLevel; // 블러 시행 레벨(깊이) >>> 렌더타겟 생성 규칙 m_iBurLevel * 2 - 1;

private:
	vector<CRenderTarget*> m_BlurRenderTargets;
	vector<ID3D11DepthStencilView*> m_DSVs;

private:
	HRESULT Ready_BlurRenderTarget();
public:
	static CRenderGroup_PostProcessing* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg);
	void Free() override;
};
END
