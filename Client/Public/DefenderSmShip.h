#pragma once
#include "DefenderMonster.h"

BEGIN(Client)
class CSection_Manager;
class CDefenderSmShip :
    public CDefenderMonster
{
private:
	CDefenderSmShip(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CDefenderSmShip(const CDefenderSmShip& _Prototype);
	virtual ~CDefenderSmShip() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Update(_float _fTimeDelta) override;
public:
	virtual void On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void On_Hit(CGameObject* _pHitter, _int _iDamg, _fvector _vForce) override;

private:
	HRESULT	Ready_PartObjects();
private:
	_float m_fMoveSpeed = 500.f;

public:
	static CDefenderSmShip* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;

};

END