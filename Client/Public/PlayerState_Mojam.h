#pragma once
#include "PlayerState.h"
BEGIN(Client)
class CPlayerState_Mojam :
    public CPlayerState
{
public:

    CPlayerState_Mojam(CPlayer* _pOwner);

private:
    _uint m_iAnimActionCount = 0;
    // CPlayerState을(를) 통해 상속됨
    void Update(_float _fTimeDelta) override;
    virtual void Enter() override;
    virtual void Exit() override;
    virtual void On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx) override;
};

END