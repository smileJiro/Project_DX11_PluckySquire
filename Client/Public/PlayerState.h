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
protected:
	CPlayer::STATE m_eStateID;
	CGameInstance* m_pGameInstance =  nullptr ;
	CPlayer* m_pOwner = nullptr;
public:
	void Free() override;
};
END