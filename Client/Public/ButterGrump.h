#pragma once
#include "Monster.h"

BEGIN(Client)

class CFSM_Boss;

class CButterGrump final : public CMonster
{
public:
	enum Animation
	{
		DASH_DOWN,
		DASH_LEFT,
		DASH_RIGHT,
		DASH_UP,
		EXPLOSION_FALL,
		EXPLOSION_INTO,
		EXPLOSION_LOOP,
		EXPLOSION_OUT,
		FIREBALL_SPIT_BIG,
		FIREBALL_SPIT_LOOP,
		FIREBALL_SPIT_SMALL,
		IDLE,
		IDLE_CINE,
		RECEIVE_DAMAGE,
		ROAR,
		ROAR_CINE,
		TRANSITION_PHASE2,
		WINGSHIELD_BREAKOUT,
		WINGSHIELD_BREAKOUT_2,
		WINGSHIELD_INTO,
		WINGSHIELD_INTO_2,
		WINGSHIELD_LOOP,
		WINGSHIELD_LOOP_2,
		WINGSHIELD_ROCK_VOLLEY_INTO,
		WINGSHIELD_ROCK_VOLLEY_LOOP,
		WINGSHIELD_ROCK_VOLLEY_OUT,
		LB_INTRO_SH01,
		LB_INTRO_SH04,
		LB_ENGAGE_IDLE_SH02,
		LB_ENGAGE_SH02,
		FIREBALL_SPIT_LOOP_INTRO,
		FIREBALL_SPIT_LOOP_OUTRO,
		WING_SLAM_INTO,
		WING_SLAM_OUT,
		WING_SLICE_INTO,
		WING_SLICE_OUT,
		LAST,
	};

private:
	CButterGrump(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CButterGrump(const CButterGrump& _Prototype);
	virtual ~CButterGrump() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	virtual void Change_Animation() override;
	virtual void Attack(_float _fTimeDelta) override;
	void Alert_End(COORDINATE _eCoord, _uint iAnimIdx);
	void	Attack_End(COORDINATE _eCoord, _uint iAnimIdx);
	void	Intro_First_End(COORDINATE _eCoord, _uint iAnimIdx);
	void	Intro_Second_End(COORDINATE _eCoord, _uint iAnimIdx);
	void	Intro_End(COORDINATE _eCoord, _uint iAnimIdx);
	void Play_Intro();

private:
	virtual HRESULT					Ready_Components();
	virtual HRESULT					Ready_PartObjects();
	HRESULT								Ready_Projectiles();

private:
	CFSM_Boss* m_pBossFSM = { nullptr };

public:
	static CButterGrump* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END