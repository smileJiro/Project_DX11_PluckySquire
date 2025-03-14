#pragma once
#include "PlayerState.h"
BEGIN(Engine)
class CActor_Dynamic;
END
BEGIN(Client)
class CPlayerState_CyberIdle :
    public CPlayerState
{
public:
    enum VELOCITY_STATE
    {
        VELOCITY_IDLE,
        VELOCITY_RUN_RIGHT,
        VELOCITY_RUN_LEFT,
        VELOCITY_RUN_UP,
        VELOCITY_RUN_DOWN,
        VELOCITY_DASH,
        VELOCITY_LAST
    };
public:
    CPlayerState_CyberIdle(CPlayer* _pOwner);

    // CPlayerState을(를) 통해 상속됨
    void Update(_float _fTimeDelta) override;
    virtual void Enter() override;
    virtual void Exit() override;
    virtual void On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx) override;

    void Set_VeloState(_fvector _vVelocity);
private:
    CActor_Dynamic* m_pDynamicActor = nullptr;
    _bool m_bRifleTriggered = false;
    _float m_f3DCyberFlySpeed = 7.f;

	VELOCITY_STATE m_eVelocityState = VELOCITY_LAST;

    _float m_fRunVelocityThreshold = 3.f;

};


class CPlayerState_CyberDash:
    public CPlayerState
{
public:
    CPlayerState_CyberDash(CPlayer* _pOwner);

    // CPlayerState을(를) 통해 상속됨
    void Update(_float _fTimeDelta) override;
    virtual void Enter() override;
    virtual void Exit() override;
    virtual void On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx) override;

private:
    CActor_Dynamic* m_pDynamicActor = nullptr;
    _float m_f3DCyberDashForce = 15.f;

    _float m_fEndDashVelocityThreshold = 5.f;
};
END