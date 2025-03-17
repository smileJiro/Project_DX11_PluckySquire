#pragma once
#include "PlayerState.h"

BEGIN(Client)
class CPlayerState_JumpUp :
    public CPlayerState
{

public:
    CPlayerState_JumpUp(CPlayer* _pOwner);

    // CPlayerState을(를) 통해 상속됨
    void Update(_float _fTimeDelta) override;
    virtual void Enter() override;
    virtual void Exit() override;
    virtual void On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)override;

	void Switch_JumpAnimation();
private:
private:
    F_DIRECTION m_eOldFDir = F_DIRECTION::F_DIR_LAST;
    _bool m_bPlatformerMode = false;
    _float m_fAirRunSpeed = 0.f;
    _float m_fAirRunSpeed2D = 0.f;
	_float m_fAirRotateSpeed = 0.f ;
};

END