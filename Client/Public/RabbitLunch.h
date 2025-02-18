#pragma once
#include "CarriableObject.h"

BEGIN(Client)
class CRabbitLunch :
    public CCarriableObject
{
protected:
	explicit CRabbitLunch(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CRabbitLunch(const CRabbitLunch& _Prototype);
	virtual ~CRabbitLunch() = default;
public:
	HRESULT Initialize(void* _pArg);
	virtual void		Update(_float _fTimeDelta) override;
	virtual void		Late_Update(_float _fTimeDelta) override;
	virtual HRESULT Render() override;


public:
	static CRabbitLunch* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END