#pragma once
#include "Monster.h"
BEGIN(Client)
class CGoblin final : public CMonster
{
public:
	enum Animation
	{
		ALERT,
		CHASE,
		CINE_LEAVES_BOOK_GT,
		DEATH,
		HIT,
		IDLE,
		PATROL,
		SLEEP,
		SLEEP_START,
		WAKE,
		LAST,
	};

	enum Animation2D
	{
		WAKE_2D,
		ALERT_DOWN,
		ALERT_RIGHT,
		ALERT_UP,
		CHASE_DOWN,
		CHASE_RIGHT,
		CHASE_UP,
		DEATH_DOWN,
		DEATH_RIGHT,
		DEATH_UP,
		HIT_DOWN,
		HIT_RIGHT,
		HIT_UP,
		IDLE_DOWN,
		IDLE_RIGHT,
		IDLE_UP,
		PATROL_DOWN,
		PATROL_RIGHT,
		PATROL_UP,
		SLEEP1_V02,
		SLEEP2_V02,
		SLEEP1,
		SLEEP2,
		ANIM2D_LAST
	};

private:
	CGoblin(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGoblin(const CGoblin& _Prototype);
	virtual ~CGoblin() = default;

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

public:
	virtual void OnContact_Enter(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas) override;
	virtual void OnContact_Stay(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas) override;
	virtual void OnContact_Exit(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas) override;

	virtual void OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other);
	virtual void OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other);
	virtual void OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other);

	virtual void On_Hit(CGameObject* _pHitter, _float _fDamg) override;

private:
	virtual	HRESULT					Ready_ActorDesc(void* _pArg);
	virtual HRESULT					Ready_Components();
	virtual HRESULT					Ready_PartObjects();

public:
	static CGoblin* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END