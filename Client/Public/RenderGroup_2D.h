#pragma once
#include "RenderGroup_MRT.h"

BEGIN(Client)

class CRenderGroup_2D final : public CRenderGroup_MRT
{
private:
	CRenderGroup_2D(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CRenderGroup_2D() = default;

public :
	HRESULT						Add_RenderObject(CGameObject* _pGameObject) override;


public:
	static CRenderGroup_2D* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg);
	void Free() override;
};

END