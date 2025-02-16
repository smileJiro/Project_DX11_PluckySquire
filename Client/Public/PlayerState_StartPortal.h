#pragma once
#include "PlayerState.h"

BEGIN(Client)
class CPortal;
class CPlayerState_StartPortal :
    public CPlayerState
{
public:
    CPlayerState_StartPortal(CPlayer* _pOwner);

    // CPlayerState을(를) 통해 상속됨
    void Update(_float _fTimeDelta) override;
    virtual void Enter() override;
    virtual void Exit() override;
    virtual void On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx) override;

private:
    CPortal* m_pPortal = nullptr;
	_vector m_vTargetPos = {};
    _float m_fChargeAnimProgress = 0.5f;
	_float m_f3DJumpDistance = 3.f;
	_float m_f2DMoveSpeed = 600.f;
	_float m_f3DMoveSpeed = 10.f;
};

END