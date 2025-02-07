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
    virtual void On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)override;
private:

    //벽 기어오르기 관련
	_float m_fAnimRatio = 0;
    _vector m_vClamberEndPosition = { 0,0,0,1};
    _vector m_vClamberStartPosition = { 0,0,0,1 };
    _vector m_vClamberNormal= { 0,0,0,0 };

    _float m_fArmHeight = 0;
    _float m_fHeadHeight = 0;
    _float m_fArmLength = 0;

    _float m_fLerpStartProgress = 0;
    _float m_fLerpEndProgress = 1;
};
END