#pragma once
#include "PlayerState.h"
BEGIN(Client)

class CPlayerState_CyberIdle :
    public CPlayerState
{
public:
    CPlayerState_CyberIdle(CPlayer* _pOwner);

    // CPlayerState을(를) 통해 상속됨
    void Update(_float _fTimeDelta) override;
    virtual void Enter() override;
    virtual void Exit() override;
    virtual void On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx) override;

private:
    _bool m_bRifleTriggered = false;
};

class CPlayerState_CyberFly :
    public CPlayerState
{
public:
    CPlayerState_CyberFly(CPlayer* _pOwner);

    // CPlayerState을(를) 통해 상속됨
    void Update(_float _fTimeDelta) override;
    virtual void Enter() override;
    virtual void Exit() override;
    virtual void On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx) override;


private:
	_bool m_bRifleTriggered = false;
    _float m_f3DCyberFlySpeed = 10.f;
	F_DIRECTION m_eFDir = F_DIRECTION::F_DIR_LAST;
};



class CPlayerState_CyberDash :
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
    _bool m_bRifleTriggered = false;
    _float m_f3DCyberDashSpeed = 20.f;
    F_DIRECTION m_eFDir = F_DIRECTION::F_DIR_LAST;
};

END