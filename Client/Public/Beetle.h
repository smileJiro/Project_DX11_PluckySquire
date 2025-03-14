#pragma once
#include "Monster.h"
BEGIN(Client)
class CBeetle final : public CMonster
{
public:
	enum Animation
	{
		ALERT,
		ATTACKSHORT,
		ATTACKRECOVERY,
		ATTACKSTRIKE,
		CONCERED,
		CONCERED_ATTACKREADY,
		DAMAGE,
		DIE,
		FLYLAND,
		FLY_LEFT,
		FLY_RIGHT,
		FLY_TAKEOFF,
		IDLE,
		RUN,
		WALK,
		CAUGHT,
		CAUGHTLOOP,
		DRAGGED,
		IDLE_STOP,
		PAUSE1,
		PAUSE2,
		STRAFE_BACK_FAST,
		STRAFE_BACK_SLOW,
		STRAFE_LEFT_FAST,
		TURN_LEFT,
		STRAFE_LEFT_SLOW,
		STRAFE_RIGHT_FAST,
		TURN_RIGHT,
		STRAFE_RIGHT_SLOW,
		WAIT1,
		WAIT2,
		LAST,
	};

private:
	CBeetle(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CBeetle(const CBeetle& _Prototype);
	virtual ~CBeetle() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	virtual void Change_Animation() override;
	virtual void Turn_Animation(_bool _isCW) override;
	void Animation_End(COORDINATE _eCoord, _uint iAnimIdx);

	virtual void Attack();

	void Switch_CombatMode();

public:
	virtual void OnContact_Enter(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas) override;
	virtual void OnContact_Stay(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas) override;
	virtual void OnContact_Exit(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas) override;

	virtual void	On_Hit(CGameObject* _pHitter, _int _iDamg, _fvector _vForce)override;

private:
	virtual	HRESULT					Ready_ActorDesc(void* _pArg);
	virtual HRESULT					Ready_Components();
	virtual HRESULT					Ready_PartObjects();

private:
	_bool m_isBackFly = { false };
	_bool m_isDash = { false };

	_float m_fBackFlyTime = {};
	_float m_fDashDistance = {};
	_float m_fAccDistance = { 0.f };
	_float3 m_vDir = {};

public:
	static CBeetle* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END