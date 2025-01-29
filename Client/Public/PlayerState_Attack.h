#pragma once
#include "PlayerState.h"

BEGIN(Client)
class CPlayerState_Attack :
    public CPlayerState
{
public:
    CPlayerState_Attack(CPlayer* _pOwner, E_DIRECTION _eDirection);

    // CPlayerState을(를) 통해 상속됨
    void Update(_float _fTimeDelta) override;
    virtual void Enter() override;
    virtual void Exit() override;
    virtual void On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx) override;
	void Switch_To_AttackAnimation(_uint iComboCount);
private:
	E_DIRECTION m_eDirection;
    _uint m_iComboCount = 0;
    _bool m_bCombo = false;
	_float m_f3DForwardSpeed = 700.f;
	_float m_f2DForwardSpeed = 700.f;

    _float m_f3DMotionCancelProgress = 0.35f;
    _float m_f2DMotionCancelProgress = 0.8f;
    _float m_f3DForwardingProgress = 0.15f;
    _float m_f2DForwardingProgress = 0.5f;
};

END