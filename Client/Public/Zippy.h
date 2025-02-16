#pragma once
#include "Monster.h"
BEGIN(Client)
class CZippy final : public CMonster
{
public:
	enum Animation2D
	{
		DASH_IN_DOWN_ELEC,
		DASH_IN_RIGHT_ELEC,
		DASH_IN_UP_ELEC,
		DASH_DOWN_ELEC,
		DASH_RIGHT_ELEC,
		DASH_UP_ELEC,
		DASH_OUT_DOWN_ELEC,
		DASH_OUT_RIGHT_ELEC,
		DASH_OUT_UP_ELEC,
		DIE_DOWN,
		DIE_RIGHT,
		DIE_UP,
		ELEC_IN_DOWN,
		ELEC_IN_RIGHT,
		ELEC_IN_UP,
		ELEC_DOWN,
		ELEC_RIGHT,
		ELEC_UP,
		ELEC_OUT_DOWN,
		ELEC_OUT_RIGHT,
		ELEC_OUT_UP,
		DASH_IN_DOWN,
		DASH_IN_RIGHT,
		DASH_IN_UP,
		DASH_DOWN,
		DASH_RIGHT,
		DASH_UP,
		DASH_OUT_DOWN,
		DASH_OUT_RIGHT,
		DASH_OUT_UP,
		HIT_DOWN,
		HIT_RIGHT,
		HIT_UP,
		IDLE_DOWN,
		IDLE_RIGHT,
		IDLE_UP,
		ANIM2D_LAST
	};

private:
	CZippy(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CZippy(const CZippy& _Prototype);
	virtual ~CZippy() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	virtual void Attack() override;
	virtual void Change_Animation() override;
	void Animation_End(COORDINATE _eCoord, _uint iAnimIdx);
	void Attack_End();

public:
	virtual void	On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void	On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void	On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;

	virtual void On_Hit(CGameObject* _pHitter, _float _fDamg) override;

	virtual _bool Has_StateAnim(MONSTER_STATE _eState = MONSTER_STATE::LAST) override;

private:
	_bool m_isElectric = { false };
	_bool m_isDash = { false };
	_float m_fSpeed = {0.f};
	_float m_fDashDistance = {0.f};
	_float3 m_vDashDirection = {};
	_float m_fAccDistance = {0.f};
	_float m_fElecTime = {0.f};
	_float m_fAccElecTime = {0.f};

private:
	virtual HRESULT					Ready_Components();
	virtual HRESULT					Ready_PartObjects();

public:
	static CZippy* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END