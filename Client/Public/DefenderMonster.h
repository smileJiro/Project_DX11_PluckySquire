#pragma once
#include "ModelObject.h"

BEGIN(Client)
class CDefenderMonster :
    public CModelObject
{
public:
	typedef struct tagDefenderMonsterDesc : public CModelObject::MODELOBJECT_DESC
	{
		_int iHP = 1;
		_int iDamage = 1;
		T_DIRECTION eTDirection = T_DIRECTION::RIGHT;
		_float fBodyRadius= 0.f;
	}DEFENDER_MONSTER_DESC;
public:
	CDefenderMonster(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CDefenderMonster(const CDefenderMonster& _Prototype);
	virtual ~CDefenderMonster() = default;
public:
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Update(_float _fTimeDelta)override;
	virtual HRESULT	Render() override;
public:
	virtual void On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;


protected:
	void Set_Direction(T_DIRECTION _eDirection);

protected:
	_int m_iHP = 1;
	_int m_iDamage = 1;
	T_DIRECTION m_eTDirection = T_DIRECTION::RIGHT;
public:
	static CDefenderMonster* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;

};

END