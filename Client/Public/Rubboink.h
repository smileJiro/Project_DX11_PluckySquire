#pragma once
#include "Character.h"
#include "AnimEventReceiver.h"
BEGIN(Client)
class CRubboink final : public CCharacter, public IAnimEventReceiver
{
public:
	enum Animation
	{
		IDLE1,
		IDLE2,
		PICKED_UP_STRUGGLE,
		PICKEDUP_START,
		PLACED_DOWN,
		RUN,
		SURPRISE,
		WALK,
		AMIM_LAST
	};

	enum Animation2D
	{
		ALERT_DOWN,
		ALERT_RIGHT,
		ALERT_UP,
		IDLE_DOWN,
		IDLE_RIGHT,
		IDLE_UP,
		RUN_DOWN,
		RUN_RIGHT,
		RUN_UP,
		STRUGGLE_DOWN,
		STRUGGLE_RIGHT,
		STRUGGLE_UP,
		WALK_DOWN,
		WALK_RIGHT,
		WALK_UP,
		ANIM2D_LAST
	};

private:
	CRubboink(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CRubboink(const CRubboink& _Prototype);
	virtual ~CRubboink() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	void Animation_End(COORDINATE _eCoord, _uint iAnimIdx);

	virtual HRESULT				Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition = nullptr) override;

public:
	virtual void OnContact_Enter(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas) override;
	virtual void OnContact_Stay(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas) override;
	virtual void OnContact_Exit(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas) override;

	virtual void OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other);
	virtual void OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other);
	virtual void OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other);

	virtual void	On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void	On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void	On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;

	virtual void On_Hit(CGameObject* _pHitter, _int _iDamg, _fvector _vForce) override;

private:
	virtual	HRESULT					Ready_ActorDesc(void* _pArg);
	virtual HRESULT					Ready_Components();
	virtual HRESULT					Ready_PartObjects();

public:
	static CRubboink* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END