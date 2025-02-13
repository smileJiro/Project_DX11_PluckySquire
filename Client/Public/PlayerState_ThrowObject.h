#pragma once
#include "PlayerState.h"
BEGIN(Client)
class CPlayerState_ThrowObject :
    public CPlayerState
{
public:
    CPlayerState_ThrowObject(CPlayer* _pOwner);

    // CPlayerState을(를) 통해 상속됨
    void Update(_float _fTimeDelta) override;
    virtual void Enter() override;
    virtual void Exit() override;

    virtual void On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx) override;
};

END