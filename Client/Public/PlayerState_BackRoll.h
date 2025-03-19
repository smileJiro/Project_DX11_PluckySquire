#pragma once
#include "PlayerState.h"
BEGIN(Client)
class CPlayerState_BackRoll :
    public CPlayerState
{
public:

    CPlayerState_BackRoll(CPlayer* _pOwner);

    // CPlayerState을(를) 통해 상속됨
    void Update(_float _fTimeDelta) override;
    virtual void Enter() override;
    virtual void Exit() override;
    virtual void On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx) override;

private:
    _bool m_isRollLoop = false;
    _float2 m_vRollTime = { 0.75f, 0.0f };
};

END