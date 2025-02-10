#pragma once
#include "PlayerState.h"

BEGIN(Client)
class CPlayerState_Attack :
    public CPlayerState
{
public:
    CPlayerState_Attack(CPlayer* _pOwner);

    // CPlayerState을(를) 통해 상속됨
    void Update(_float _fTimeDelta) override;
    virtual void Enter() override;
    virtual void Exit() override;
    virtual void On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx) override;
private:
	void Switch_To_AttackAnimation(_uint iComboCount);
private:
    _uint m_iComboCount = 0;
    _bool m_bCombo = false;
    _float m_f2DForwardSpeed = 0.f;
	_float m_f2DForwardStartProgress = 0.1f;
	_float m_f2DForwardEndProgress = 0.3f;

    _float m_f3DMotionCancelProgress = 0.5f;
    _float m_f2DMotionCancelProgress = 0.65f;
};

END