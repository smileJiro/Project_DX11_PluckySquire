#pragma once
#include "CarriableObject.h"

BEGIN(Client)
class CBomb : public CCarriableObject
{
protected:
	explicit CBomb(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CBomb(const CBomb& _Prototype);
	virtual ~CBomb() = default;
public:
	HRESULT Initialize(void* _pArg);
	virtual void		Update(_float _fTimeDelta) override;
	virtual void		Late_Update(_float _fTimeDelta) override;
	virtual HRESULT Render() override;
	
	
public:
	static CBomb* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END