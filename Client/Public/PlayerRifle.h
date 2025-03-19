#pragma once
#include "ModelObject.h"
BEGIN(Engine)
class CEffect_System;
END
BEGIN(Client)
class CPlayer;


class CPlayerRifle :
    public CModelObject
{
public:
	typedef struct tagPlayerRifleDesc : public CModelObject::MODELOBJECT_DESC
	{
		CPlayer* pPlayer = nullptr;
	}PLAYER_RIFLE_DESC;
private:
	CPlayerRifle(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPlayerRifle(const CPlayerRifle& _Prototype);
	virtual ~CPlayerRifle() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Update(_float _fTimeDelta) override;
	virtual void Late_Update(_float _fTimeDelta) override;

	void Shoot(_fvector _vTargetPosition);
private:
	CPlayer* m_pPlayer = nullptr;
	_float m_fShhotDelay = 0.1f;
	_float m_fShootTimeAcc = 0.f;
	_vector m_vBarrelOffset = { 0.f,0.f,0.f };
	class CPooling_Manager* m_pPoolMgr = nullptr;

	_bool	m_isEffect = { false };
	_float	m_fAccEffectDelay = 0.f;
	class CEffect_System* m_pShotEffect = { nullptr };

public:
	static CPlayerRifle* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END