#pragma once
#include "Monster.h"
BEGIN(Client)
class CSpear_Soldier final : public CMonster
{
public:
	typedef struct tagSpear_SoldierDesc : public CMonster::MONSTER_DESC
	{
		_bool isC6BossMode = false;
		F_DIRECTION eDirection = F_DIRECTION::F_DIR_LAST;
	}SPEARSOLDIER_DESC;

	enum Animation
	{
		ALERT = 13,
		BLOCK_HOLD_PUT_DOWN,
		CHASE,
		BLOCK_HOLD_IMPACT,
		BLOCK_HOLD_LOOP,
		BLOCK_HOLD_UP,
		DASH_ATTACK_LOOP,
		DASH_ATTACK_RECOVERY,
		DASH_ATTACK_STARTUP,
		DEATH_01,
		DEATH_02_EDIT,
		FAST_WALK,
		HIT_BACK,
		HIT_FRONT,
		IDLE,
		SALUTE,
		SHAKE_HEAD,
		STRAFE_BWD,
		STRAFE_LEFT_01,
		STRAFE_LEFT_02,
		STRAFE_RIGHT_01,
		STRAFE_RIGHT_02,
		STRETCH_01,
		STRETCH_02,
		WALK,
		ARREST = 41,
		LAST,
	};

	enum Animation2D
	{
		ALERT_DOWN = 1,
		ALERT_RIGHT,
		ALERT_UP,
		CHASE_DOWN,
		CHASE_RIGHT,
		CHASE_UP,
		CHAT01,
		DASHATTACK_INTO_DOWN,
		DASHATTACK_INTO_RIGHT,
		DASHATTACK_INTO_UP,
		DASHATTACK_LOOP_DOWN,
		DASHATTACK_LOOP_RIGHT,
		DASHATTACK_LOOP_UP,
		DASHATTACK_OUT_DOWN,
		DASHATTACK_OUT_RIGHT,
		DEATH_DOWN,
		DEATH_RIGHT,
		DEATH_UP,
		HIT_DOWN,
		HIT_RIGHT,
		HIT_UP,
		IDLE_DOWN,
		IDLE_RIGHT,
		IDLE_UP,
		JUMP_FALL_DOWN,
		JUMP_FALL_RIGHT,
		JUMP_RISE_DOWN,
		JUMP_RISE_RIGHT,
		TALK_GENERIC_DOWN,
		TALK_GENERIC_UP,
		TALK_REDGATE,
		TALK_THEYVEGOTSWORDS,
		TALK_TROUBLESOMEPERSONS,
		WALK_DOWN,
		WALK_RIGHT,
		WALK_UP,
		ANIM2D_LAST
	};

	/*enum Animation
	{
		CROSSBOW_ALERT,
		CROSSBOW_DEATH_ALT,
		CROSSBOW_DEATH,
		CROSSBOW_HIT_FWDS,
		CROSSBOW_IDLE,
		CROSSBOW_SHOOT,
		CROSSBOW_SHOOT_IDLE,
		CROSSBOW_SHOOT_RECOVERY,
		CROSSBOW_STRAFE_BWD,
		CROSSBOW_STRAFE_FWD,
		CROSSBOW_STRAFE_LEFT,
		CROSSBOW_STRAFE_RIGHT,
		CROSSBOW_WALK,
		ALERT,
		BLOCK_HOLD_PUT_DOWN,
		CHASE,
		BLOCK_HOLD_IMPACT,
		BLOCK_HOLD_LOOP,
		BLOCK_HOLD_UP,
		DASH_ATTACK_LOOP,
		DASH_ATTACK_RECOVERY,
		DASH_ATTACK_STARTUP,
		DEATH_01,
		DEATH_02_EDIT,
		FAST_WALK,
		HIT_BACK,
		HIT_FRONT,
		IDLE,
		SALUTE,
		SHAKE_HEAD,
		STRAFE_BWD,
		STRAFE_LEFT_01,
		STRAFE_LEFT_02,
		STRAFE_RIGHT_01,
		STRAFE_RIGHT_02,
		STRETCH_01,
		STRETCH_02,
		WALK,
		BOMB_LOOP,
		BOMB_OUT,
		BOMB_THROW,
		ARREST,
		LAST,
	};*/

private:
	CSpear_Soldier(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CSpear_Soldier(const CSpear_Soldier& _Prototype);
	virtual ~CSpear_Soldier() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	virtual void OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)override;

	virtual void	On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;

	virtual void	On_Hit(CGameObject* _pHitter, _int _iDamg, _fvector _vForce)override;

public:
	virtual void Attack() override;
	virtual void Change_Animation() override;
	virtual void Animation_End(COORDINATE _eCoord, _uint iAnimIdx) override;

	virtual void Turn_Animation(_bool _isCW) override;

private:
	_bool m_isDash = { false };
	_bool m_isJump = { false };
	_float m_fDashDistance = { 0.f };
	_float m_fAccDistance = { 0.f };
	_float3 m_vDir = {};

	_bool m_isC6BossMode = { false };

private:
	virtual	HRESULT					Ready_ActorDesc(void* _pArg);
	virtual HRESULT					Ready_Components();
	virtual HRESULT					Ready_PartObjects();

public:
	static CSpear_Soldier* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END