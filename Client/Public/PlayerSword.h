#pragma once
#include "ModelObject.h"
BEGIN(Client)
class CPlayerSword :
    public CModelObject
{
public:
	typedef struct tagPlayerSwordDesc : public CModelObject::MODELOBJECT_DESC
	{
		_float fThrowForce = 10.f;
		_float fHomingForce = 100.f;
		_float fRotationForce = 50.f;
		class CPlayer* pParent = nullptr;
	}PLAYER_SWORD_DESC;
private:
	CPlayerSword(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPlayerSword(const CPlayerSword& _Prototype);
	virtual ~CPlayerSword() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;

	void Throw(_fvector _vDirection);
private:
	_bool m_bFlying = false;
	_float m_fThrowForce = 10.f;
	_float m_fHomingForce = 100.f;
	_float m_fRotationForce = 50.f;
	class CPlayer* m_pPlayer = nullptr;
public:
	static CPlayerSword* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END