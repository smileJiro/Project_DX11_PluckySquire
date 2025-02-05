#pragma once
#include "PlayerState.h"

BEGIN(Client)
class CPlayerState_Roll :
    public CPlayerState
{
public:
    CPlayerState_Roll(CPlayer* _pOwner, _fvector _vDirection);

    // CPlayerState을(를) 통해 상속됨
    void Update(_float _fTimeDelta) override;
    virtual void Enter() override;
    virtual void Exit() override;
    virtual void On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx) override;
private:
	_vector m_vDirection;
    _float m_fForwardSpeedMax = 13.f;
    _float m_fForwardSpeedMin = 0.f;
    _float m_f3DForwardStartProgress = 0.f;
    _float m_f3DForwardEndProgress = 0.52f;
    _float m_f2DForwardStartProgress = 0.f;
    _float m_f2DForwardEndProgress = 1.f;
    _float m_f3DMotionCancelProgress = 0.52f;
    _float m_f2DMotionCancelProgress = 0.8f;

};

END