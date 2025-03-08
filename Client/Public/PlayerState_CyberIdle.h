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
        VELOCITY_RUN,
        VELOCITY_LAST
    };
public:
    CPlayerState_CyberIdle(CPlayer* _pOwner);

    // CPlayerState을(를) 통해 상속됨
    void Update(_float _fTimeDelta) override;
    virtual void Enter() override;
    virtual void Exit() override;
    virtual void On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx) override;

    void Set_VeloState(VELOCITY_STATE _eState);
    void Set_Direction(F_DIRECTION _eDir);
private:
    CActor_Dynamic* m_pDynamicActor = nullptr;
    _bool m_bRifleTriggered = false;
    _float m_f3DCyberFlySpeed = 800.f;
    _float m_f3DCyberDashForce = 15.f;
	VELOCITY_STATE m_eVelocityState = VELOCITY_IDLE;
	F_DIRECTION m_eDirection = F_DIRECTION::F_DIR_LAST;

    _float m_fRunVelocityThreshold = 3.f;
};

END