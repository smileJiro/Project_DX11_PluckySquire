#pragma once
#include "ModelObject.h"

BEGIN(Client)

class CCube final : public CModelObject
{
private:
	CCube(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCube(const CCube& _Prototype);
	virtual ~CCube() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;

	virtual HRESULT			Render_Shadow() override;

private:
	HRESULT					Ready_Components();

public:
	static CCube*			Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg) override;
	virtual void			Free() override;
};

END