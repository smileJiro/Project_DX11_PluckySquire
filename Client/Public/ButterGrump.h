#pragma once
#include "Monster.h"

BEGIN(Engine)
class CEffect_System;
END
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
		FIREBALL_SPIT_SMALL = 10,
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

	enum BOSSPART {BOSSPART_BODY, BOSSPART_LEFTEYE, BOSSPART_RIGHTEYE, BOSSPART_TONGUE, BOSSPART_SHIELD,
		BOSSPART_HOMING_EFFECT, BOSSPART_PURPLEBALL_EFFECT, BOSSPART_YELLOWBALL_EFFECT,
		BOSSPART_END};

	//enum BOSS_SHAPE_USE
	//{
	//	BOSS_SHAPE_LEFT_EYE = SHAPE_USE::SHAPE_USE_LAST,
	//	BOSS_SHAPE_RIGHT_EYE,
	//	BOSS_SHAPE_TONGUE,
	//	BOSS_SHAPE_SHIELD,
	//	BOSS_SHAPE_LAST
	//};

private:
	CButterGrump(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CButterGrump(const CButterGrump& _Prototype);
	virtual ~CButterGrump() = default;

public:
	_bool Is_Phase2()
	{
		return m_isPhase2;
	}
	_bool Is_Move()
	{
		return m_isMove;
	}
	_bool Is_OnMove()
	{
		return m_isOnMove;
	}

	void Set_OnMove(_bool _isOnMove)
	{
		m_isOnMove = _isOnMove;
	}

	_bool Is_Converse()
	{
		return m_isConverse;
	}
	_bool Is_Enforce()
	{
		return m_isEnforce;
	}

	void Set_SpawnOrb(_bool _isSpawnOrb)
	{
		m_isSpawnOrb = _isSpawnOrb;
	}

	_bool Is_Attack()
	{
		return m_isAttack;
	}
	_bool Is_AttackChained()
	{
		return m_isAttackChained;
	}
	
	_float Get_ChainDelayTime()
	{
		return m_fChainDelayTime;
	}

	void Set_CurMoveIndex(_uint _iMoveIndex)
	{
		m_iCurMoveIndex = _iMoveIndex;
	}

	_uint Get_CurMoveIndex()
	{
		return m_iCurMoveIndex;
	}
	
public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;


public:
	virtual void Change_Animation() override;
	virtual void Attack() override;
	virtual void On_Attack();
	virtual void On_Move();
	void Shield_Break();
	void Activate_Invinciblility(_bool _isActivate);
	void	Animation_End(COORDINATE _eCoord, _uint iAnimIdx);
	void Play_Intro();

	virtual void	On_Hit(CGameObject* _pHitter, _int _iDamg, _fvector _vForce)override;
	void Hit();


private:
	virtual	HRESULT					Ready_ActorDesc(void* _pArg);
	virtual HRESULT					Ready_Components();
	virtual HRESULT					Ready_PartObjects();
	HRESULT								Ready_Projectiles();

private:
	CFSM_Boss* m_pBossFSM = { nullptr };

	_bool m_isInvincible = { false };
	_bool m_isMove = { false };
	_bool m_isOnMove = { false };
	_bool m_isAlreadyMove = { false };
	_bool m_isConverse = { false };		//쉴드 패턴 전환
	_bool m_isEnforce = { false };			//패턴 강화
	_bool m_isPhase2 = { false };
	_bool m_isAttack = { false };
	_bool m_isAttackChained = { false };
	_uint m_iNumAttack = { 0 };
	_uint m_iNumAttackChain = { 0 };
	_uint m_iAttackChainCount = { 0 };
	_uint m_iCurMoveIndex = { 0 };
	_uint m_iNumConverse = { 0 };

	_float m_fMoveHPRatio_Phase1_1 = { 0.f };		//이동 패턴 쓸 체력 구간
	_float m_fMoveHPRatio_Phase1_2 = { 0.f };

	_float m_fMoveHPRatio_Phase2 = { 0.f };

	_float m_fMoveAnimationProgress[4] = {};

	//연속 패턴에서 다음 공격으로 이어지기 까지의 딜레이(애니메이션 전환 시간)
	_float m_fChainDelayTime = { 0.f };

	//패턴 파훼용 투사체가 소환되어있는지 체크
	_bool m_isSpawnOrb = { false };

	/* Effects */
	class CEffect_System* m_pHomingEffect = { nullptr };
	class CEffect_System* m_pPurpleEffect = { nullptr };
	class CEffect_System* m_pYellowEffect = { nullptr };

public:
	static CButterGrump* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END