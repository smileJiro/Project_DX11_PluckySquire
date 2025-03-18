#pragma once
#include "PlayerState.h"
BEGIN(Client)
class CPlayerState_Pull :
    public CPlayerState
{
public:

    CPlayerState_Pull(CPlayer* _pOwner);

    // CPlayerState을(를) 통해 상속됨
    void Update(_float _fTimeDelta) override;
    virtual void Enter() override;
    virtual void Exit() override;
    virtual void On_Hit(CGameObject* _pHitter, _int _iDamg, _fvector _vForce);
};

END