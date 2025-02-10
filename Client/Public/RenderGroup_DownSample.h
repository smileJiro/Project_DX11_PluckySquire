#pragma once
#include "RenderGroup_MRT.h"
#include "Client_Defines.h"

BEGIN(Client)

class CRenderGroup_DownSample : public CRenderGroup_MRT
{
private:
	CRenderGroup_DownSample(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CRenderGroup_DownSample() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	virtual HRESULT Render(CShader* _pRTShader, CVIBuffer_Rect* _pRTBuffer) override;

private:
	ID3D11DepthStencilView* m_pLowerDSV = { nullptr };

public:
	static CRenderGroup_DownSample* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg);
	void Free() override;
};

END