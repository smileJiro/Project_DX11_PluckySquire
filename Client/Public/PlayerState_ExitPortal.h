#pragma once
#include "PlayerState.h"

BEGIN(Client)
class CPortal;
class CPlayerState_ExitPortal :
    public CPlayerState
{
public:
    CPlayerState_ExitPortal(CPlayer* _pOwner);

    // CPlayerState을(를) 통해 상속됨
    void Update(_float _fTimeDelta) override;
    virtual void Enter() override;
    virtual void Exit() override;
    virtual void On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx) override;

    void Shoot_Player3D();
private:
    CPortal* m_pPortal = nullptr;
    NORMAL_DIRECTION m_ePortalNormal;
    _float m_f3DJumpDistance = 5.f;
	_bool m_bFirstFrame = true;
    _bool m_bSecondFrame = false;
};

END