#pragma once
#include "PlayerState.h"

BEGIN(Client)
class CPlayerState_Run :
    public CPlayerState
{
public:
    CPlayerState_Run(CPlayer* _pOwner, CStateMachine* _pContext);

    // CPlayerState을(를) 통해 상속됨
    void Update(_float _fTimeDelta) override;
};

END