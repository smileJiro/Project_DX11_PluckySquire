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
private:
    CPortal* m_pPortal = nullptr;
    _float m_f3DJumpDistance = 3.f;
};

END