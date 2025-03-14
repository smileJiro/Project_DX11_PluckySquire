#pragma once
#include "CarriableObject.h"

BEGIN(Client)
class CBeetle_Corpse :
    public CCarriableObject
{
protected:
	explicit CBeetle_Corpse(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CBeetle_Corpse(const CBeetle_Corpse& _Prototype);
	virtual ~CBeetle_Corpse() = default;
public:
	HRESULT Initialize(void* _pArg);
	virtual void		Update(_float _fTimeDelta) override;
	virtual void		Late_Update(_float _fTimeDelta) override;
	virtual HRESULT Render() override;
	
	
public:
	static CBeetle_Corpse* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END