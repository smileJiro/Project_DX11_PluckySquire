#pragma once
#include "Character.h"
BEGIN(Engine)
class CCollider_Circle;
class CModelObject;
END
BEGIN(Client)
enum class DEFENDER_MONSTER_ID
{
	SM_SHIP,
	TURRET,
	MED_SHIP_UP,
	MED_SHIP_DOWN,
	PERSON_CAPSULE,
	LAST
};
class CDefenderMonster :
    public CCharacter
{
public:
	typedef struct tagDefenderMonsterDesc : public CCharacter::CHARACTER_DESC
	{
		T_DIRECTION eTDirection = T_DIRECTION::RIGHT;
	}DEFENDER_MONSTER_DESC;
protected:
	CDefenderMonster(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CDefenderMonster(const CDefenderMonster& _Prototype);
	virtual ~CDefenderMonster() = default;
public:
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Update(_float _fTimeDelta) override;
public:
	virtual void On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void On_Hit(CGameObject* _pHitter, _int _iDamg, _fvector _vForce);
	virtual void On_Explode();
	virtual void On_Spawned() {};
	virtual void On_Teleport() ;
	virtual void On_LifeTimeOut();

	void Set_Direction(T_DIRECTION _eDirection);
	void Set_MoveSpeed(_float _fSpeed) { m_fMoveSpeed = _fSpeed; }
protected:

protected:
	_float m_fMoveSpeed = 100.f;
	T_DIRECTION m_eTDirection = T_DIRECTION::RIGHT;
	CCollider_Circle* m_pBodyCollider = nullptr;
	_float m_fLifeTime = 13.f;
	_float m_fLifeTimeAcc = 0.f;

	_uint m_iFXTeleportInIdx = 2;
	_uint m_iFXTeleportOutIdx = 5;
	CModelObject* m_pTeleportFX = nullptr;
	_bool m_bSpawned = false;
public:
	static CDefenderMonster* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;

};

END