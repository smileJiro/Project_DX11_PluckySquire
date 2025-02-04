#pragma once
#include "PlayerState.h"

BEGIN(Client)
class CPlayerState_Clamber :
    public CPlayerState
{
public:
    CPlayerState_Clamber(CPlayer* _pOwner);

    // CPlayerState을(를) 통해 상속됨
    void Update(_float _fTimeDelta) override;
    virtual void Enter() override;
    virtual void Exit() override;

private:
    _vector m_vClamberPosition = { 0,0,0 };
};
END