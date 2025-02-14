#pragma once
#include "RenderGroup_MRT.h"
BEGIN(Client)
class CRenderGroup_WorldPos final:  public CRenderGroup_MRT
{
public:
	typedef struct tagRedderGroup_WorldPosDesc : public CRenderGroup_MRT::RG_MRT_DESC
	{
		_wstring				strSectionTag;
		_wstring				strRTTag;
	}RG_WORLDPOS_DESC;
private:
	CRenderGroup_WorldPos(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CRenderGroup_WorldPos() = default;

public:
	HRESULT Initialize(void* _pArg) override;
	HRESULT Render(CShader* _pRTShader, CVIBuffer_Rect* _pRTBuffer) override;

private :

	_wstring m_strSectionTag;
	_wstring m_strRTTag;

public:
	static CRenderGroup_WorldPos* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg);
	virtual void Free() override;
};

END