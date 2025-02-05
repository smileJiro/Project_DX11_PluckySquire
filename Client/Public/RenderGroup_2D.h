#pragma once
#include "RenderGroup_MRT.h"

BEGIN(Client)

class CRenderGroup_2D final : public CRenderGroup_MRT
{
public:
	typedef struct tagRG_2DTDesc : public CRenderGroup_MRT::RG_MRT_DESC
	{
		_float2 fRenderTargetSize;
	}RG_2D_DESC;
public :
	enum SHADER_2D_TYPE
	{
		SHADER_POS,
		LAST
	};

private:
	CRenderGroup_2D(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CRenderGroup_2D() = default;

public :
	HRESULT						Initialize(void* _pArg) override;
	HRESULT						Add_RenderObject(CGameObject* _pGameObject) override;
	HRESULT						Render(CShader* _pRTShader, CVIBuffer_Rect* _pRTBuffer) override;

	HRESULT						Binding_2D_View_Proj();


public:
	_float4x4					m_ViewMatrix;
	_float4x4					m_ProjMatrix;
	class CShader*				m_arr2DShaders[LAST] = {};

	static CRenderGroup_2D* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg);
	
	void Free() override;
};

END