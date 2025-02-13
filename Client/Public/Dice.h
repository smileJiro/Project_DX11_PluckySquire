#pragma once
#include "CarriableObject.h"

BEGIN(Client)
class CDice :
    public CCarriableObject
{
protected:
	explicit CDice(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CDice(const CDice& _Prototype);
	virtual ~CDice() = default;
public:
	HRESULT Initialize(void* _pArg);
public:
	static CDice* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END