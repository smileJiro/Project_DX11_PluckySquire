#pragma once

#include "Transition.h"
BEGIN(Engine)
class CGameInstance;

END
BEGIN(Client)
class CStateMachine;
class CPlayer;
class  CPlayerState :public CBase
{
protected:
	CPlayerState(CPlayer* _pOwner, CStateMachine* _pContext);

	virtual ~CPlayerState() = default;
public:
	virtual void Update(_float _fTimeDelta) abstract;

protected:
	_uint m_iStateID;
	CGameInstance* m_pGameInstance =  nullptr ;
	CPlayer* m_pOwner = nullptr;
	CStateMachine* m_pStateMachine = nullptr;
public:
	void Free() override;
};
END