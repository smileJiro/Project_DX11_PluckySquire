#pragma once
#include "PlayerState.h"

BEGIN(Client)
class CPlayerState_Run :
    public CPlayerState
{
public:
    CPlayerState_Run(CPlayer* _pOwner);

    // CPlayerState을(를) 통해 상속됨
    void Update(_float _fTimeDelta) override;
    virtual void Enter() override;
    virtual void Exit() override;

	void Switch_RunAnimation2D(F_DIRECTION _eFDir);

private:
	_float m_fSpeed = 7.f;
};

END