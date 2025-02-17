#pragma once
#include "PlayerState.h"

BEGIN(Client)
class CPortal;
class CPlayerState_JumpToPortal :
    public CPlayerState
{
public:
    CPlayerState_JumpToPortal(CPlayer* _pOwner);

    // CPlayerState을(를) 통해 상속됨
    void Update(_float _fTimeDelta) override;
    virtual void Enter() override;
    virtual void Exit() override;
    virtual void On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx) override;

    virtual void OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)override;
private:
    CPortal* m_pPortal = nullptr;
    _bool m_bPortaled = false;
};

END