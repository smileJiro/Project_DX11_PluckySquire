#pragma once
#include "DefenderMonster.h"
#include "ModelObject.h"
BEGIN(Engine)
class CCollider_Circle;
END
BEGIN(Client)
class CDefenderMedShip :
    public CDefenderMonster
{
public:
	typedef struct tagDefenderMedShipDesc : CDefenderMonster::DEFENDER_MONSTER_DESC
	{
		_bool bUpSide = false;
	}DEFENDER_MEDSHIP_DESC;
private:
	CDefenderMedShip(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CDefenderMedShip(const CDefenderMedShip& _Prototype);
	virtual ~CDefenderMedShip() = default;

public:
	HRESULT Initialize_Prototype(_bool _bUpSide);
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Update(_float _fTimeDelta) override;
	virtual HRESULT Render()override;

public:
	virtual void On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void On_Hit(CGameObject* _pHitter, _int _iDamg, _fvector _vForce)override;


public:
	void Set_UpSide(_bool _bUp) { m_bUpSide = _bUp; }
private:
	HRESULT	Ready_PartObjects();
private:
	_float m_fMoveSpeed = 500.f;
	_bool m_bUpSide = false;
	_float m_fVerticalMoveRange = 100.f;
	_float m_fVerticalMoveSpeed = 3.f;
	_float m_fTimeAcc = 0.f;
public:
	static CDefenderMedShip* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _bool _bUpSide);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;

};




END