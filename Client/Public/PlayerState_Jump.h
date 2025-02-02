#pragma once
#include "PlayerState.h"

BEGIN(Client)
class CPlayerState_Jump :
    public CPlayerState
{
private:
    enum JUMP_STATE{UP, DOWN,LAND,LAST};
public:
    CPlayerState_Jump(CPlayer* _pOwner);

    // CPlayerState을(를) 통해 상속됨
    void Update(_float _fTimeDelta) override;
    virtual void Enter() override;
    virtual void Exit() override;
    virtual void On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)override;

private:
	void Switch_JumpAnimation(JUMP_STATE _eJMPSTATE);
private:
    _bool m_bRising = true;
    _bool m_bGrounded = false;
    _float m_fAirRunSpeed = 10.f;

    _float m_f2DHeight = 0.f;
    _float m_f2DUpForce = 0.f;
};

END