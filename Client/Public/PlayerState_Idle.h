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
    virtual void Enter() override;
    virtual void Exit() override;
	_bool Is_Sneaking() { return m_bSneakBefore; }
    void Switch_IdleAnimation2D(F_DIRECTION _eFDir);
private:
    void Switch_IdleAnimation3D(_bool _bStealth);
private:
    _bool m_bSneakBefore = false;
    _bool m_bPlatformerMode = false;
};

END