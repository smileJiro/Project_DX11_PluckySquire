#pragma once
#include "ModelObject.h"
#include "Bombable.h"

BEGIN(Client)
class CPlayerBomb :
	public CModelObject, public IBombable
{
public:
	enum ANIMATION_STATE
	{
		EXPLODE = 0,
		IDLE,
	};
private:
	explicit CPlayerBomb(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CPlayerBomb(const CPlayerBomb& _Prototype);
	virtual ~CPlayerBomb() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;

public:

	virtual void Detonate() override;
	void On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx);
	virtual void On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;

private:
	CCollider* m_pExplosionCollider = nullptr;
	set<CGameObject*> m_AttckedObjects;
	_float m_f2DKnockBackPower = 700.f;
	_int m_iAttackDamg = 2;
public:
	static CPlayerBomb* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;

};

END