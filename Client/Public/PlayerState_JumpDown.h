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
    _bool m_bGrounded = false;


    //벽 기어오르기 관련
    _float m_fWallYPosition = -1;
	_vector m_vClamberEndPosition= { 0,0,0 };
    _float m_fArmYPositionBefore = -1;
    _float m_fArmHeight = 0;
    _float m_fHeadHeight = 0;
    _float m_fArmLength = 0;
};

END