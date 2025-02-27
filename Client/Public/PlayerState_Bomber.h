#pragma once
#include "PlayerState.h"

BEGIN(Client)
class CDetonator;
class CPlayerBomb;
class CPlayerState_Bomber :
    public CPlayerState
{
public:
    CPlayerState_Bomber(CPlayer* _pOwner);

    // CPlayerState을(를) 통해 상속됨
    void Update(_float _fTimeDelta) override;
    virtual void Enter() override;
    virtual void Exit() override;
    virtual void On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx) override;

private:
	CDetonator* m_pDetonator = nullptr;
};

END