#pragma once
#include "PlayerState.h"
BEGIN(Client)
class CPlayerState_SpinAttack :
    public CPlayerState
{
private:
	enum SPIN_STATE
	{
		SPIN_CHARGING,
		SPIN_SPINNING,
		SPIN_SPINEND,
		SPIN_LAST
	};
public:
    CPlayerState_SpinAttack(CPlayer* _pOwner);

    // CPlayerState을(를) 통해 상속됨
    void Update(_float _fTimeDelta) override;
    virtual void Enter() override;
    virtual void Exit() override;
    virtual void On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx) override;

private:
	void Set_SpinState(SPIN_STATE _eNewState);
	void On_RoundEnd();
private:
	COORDINATE m_eCoord = COORDINATE_3D;
	F_DIRECTION m_eFDir = F_DIRECTION::DOWN;
	SPIN_STATE m_eSpinState = SPIN_CHARGING;
	_bool m_bLaunchInput = false;
	_float m_fSpinMoveSpeed = 0;
	_float m_fLaunchableProgress3D = 0.f;
	_float m_fLaunchableProgress2D = 0.f;
    _int m_iSpinCount = 0;
    _int m_iSpinAttackLevel= 0;
};

END