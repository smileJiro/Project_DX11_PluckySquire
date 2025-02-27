#pragma once
#include "PlayerState.h"
BEGIN(Client)
class CPlayerState_JumpDown :
    public CPlayerState
{
public:
    CPlayerState_JumpDown(CPlayer* _pOwner);

    // CPlayerState을(를) 통해 상속됨
    void Update(_float _fTimeDelta) override;
    virtual void Enter() override;
    virtual void Exit() override;
    virtual void On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)override;

private:
    void Switch_To_JumpDownAnimation();

	_bool Try_Clamber();
private:
    F_DIRECTION m_eOldFDir = F_DIRECTION::F_DIR_LAST;
    _bool m_bPlatformerMode = false;
    _bool m_bGrounded = false;
    _float m_fAirRunSpeed = 1000.f;
    _float m_fAirRunSpeed2D = 300.f;
    _float m_fAirRotateSpeed = 100.f;
    //벽 기어오르기 관련
    _float m_fWallYPosition = -1;
	_vector m_vClamberEndPosition= { 0,0,0,1 };

    _float m_fArmYPositionBefore = -1;
    _float m_fArmHeight = 0;
    _float m_fHeadHeight = 0;
    _float m_fArmLength = 0;
};

END