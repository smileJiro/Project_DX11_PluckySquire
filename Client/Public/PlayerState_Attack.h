#pragma once
#include "PlayerState.h"

BEGIN(Client)
class CPlayerState_Attack :
    public CPlayerState
{
public:
    CPlayerState_Attack(CPlayer* _pOwner);

    // CPlayerState을(를) 통해 상속됨
    void Update(_float _fTimeDelta) override;
    virtual void Enter() override;
    virtual void Exit() override;
};

END