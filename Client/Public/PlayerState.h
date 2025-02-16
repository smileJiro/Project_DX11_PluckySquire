#pragma once

#include "Transition.h"
#include "Player.h"

BEGIN(Engine)
class CGameInstance;

END
BEGIN(Client)
class CStateMachine;
class  CPlayerState :public CBase
{
protected:
	CPlayerState(CPlayer* _pOwner, CPlayer::STATE _eState);

	virtual ~CPlayerState() = default;
public:
	virtual void Update(_float _fTimeDelta) abstract;
	virtual void Enter() {};
	virtual void Exit() {};
	virtual void On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx) {};

	CPlayer::STATE Get_StateID() { return m_eStateID; }

public:
	virtual void OnContact_Enter(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas) {};
	virtual void OnContact_Stay(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas) {};
	virtual void OnContact_Exit(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas) {};
	virtual void OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other) {};
	virtual void OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other) {};
	virtual void OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other) {};
	virtual void On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject) {};
	virtual void On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject) {};
	virtual void On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject) {};

protected:
	CPlayer::STATE m_eStateID;
	CGameInstance* m_pGameInstance =  nullptr ;
	CPlayer* m_pOwner = nullptr;
public:
	void Free() override;
};
END