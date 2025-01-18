#pragma once
#include "PlayerState.h"

BEGIN(Client)
class CPlayerState_Idle :
    public CPlayerState
{
public:
    CPlayerState_Idle(CPlayer* _pOwner);

    // CPlayerState을(를) 통해 상속됨
    void Update(_float _fTimeDelta) override;
};

END