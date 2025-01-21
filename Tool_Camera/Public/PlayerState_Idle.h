#pragma once
#include "PlayerState.h"

BEGIN(Camera_Tool)
class CPlayerState_Idle :
    public CPlayerState
{
public:
    CPlayerState_Idle(CTest_Player* _pOwner);

    // CPlayerState을(를) 통해 상속됨
    void Update(_float _fTimeDelta) override;
    virtual void Enter() override;
    virtual void Exit() override;
};

END