#pragma once
#include "PlayerState.h"

BEGIN(Camera_Tool)
class CPlayerState_Jump :
    public CPlayerState
{
public:
    CPlayerState_Jump(CTest_Player* _pOwner);

    // CPlayerState을(를) 통해 상속됨
    void Update(_float _fTimeDelta) override;
};

END