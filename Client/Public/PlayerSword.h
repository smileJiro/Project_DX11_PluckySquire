#pragma once
#include "ModelObject.h"
BEGIN(Client)
class CPlayerSword :
    public CModelObject
{
private:
	CPlayerSword(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPlayerSword(const CPlayerSword& _Prototype);
	virtual ~CPlayerSword() = default;

public:
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;

	void Throw(_fvector _vDirection);
private:
	_bool m_bFlying = false;
	_float m_fThrowForce = 1.f;
	_float m_fHomingForce = 100.f;
public:
	static CPlayerSword* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END