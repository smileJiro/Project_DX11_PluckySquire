#pragma once
#include "D:\Workbench\Portfolio\CopyMojam\Client\Public\PlayerState.h"

BEGIN(Client)
class CPlayerState_PlatformJumpAttack :
    public CPlayerState
{
public:
    CPlayerState_PlatformJumpAttack(CPlayer* _pOwner);

    // CPlayerState을(를) 통해 상속됨
    void Update(_float _fTimeDelta) override;
    virtual void Enter() override;
    virtual void Exit() override;
    virtual void On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx) override;

private:
    _float m_fJumpAttackRisingForce = 1.5f;
    _bool m_bRising = true;
    _bool m_bGrounded = false;

    _float m_fAirRunSpeed2D = 300.f;
    F_DIRECTION m_eOldFDir = F_DIRECTION::F_DIR_LAST;
    _float m_f3DMotionCancelProgress = 0.258f;
    _float m_f2DMotionCancelProgress = 0.258f;
};

END