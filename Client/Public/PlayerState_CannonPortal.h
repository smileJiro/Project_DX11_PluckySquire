#pragma once
#include "PlayerState.h"
BEGIN(Client)
//PLAYER_CANNON_CURL_CANCEL,//안쓸듯?
//PLAYER_CANNON_CURL_INTO, //SPIN 시작
//PLAYER_CANNON_CURL_SPIN, //돌기
//PLAYER_CANNONCHARGE_DOWN,//웅크리기
//PLAYER_CANNONCHARGE_RIGHT,//웅크리기
//PLAYER_CANNONCHARGE_UP,//웅크리기
class CPortal_Cannon;
class CPlayerState_CannonPortal :
    public CPlayerState
{
public:
    enum SUB_STATE
    {
        SUB_INTO,
		SUB_SPIN_CHARGE,
		SUB_SPIN_CHARGE_COMPLETE,
		SUB_SHOOT_READY,
		SUB_SHOOT,
		SUB_GOAL,
		SUB_GOAL_IN_READY,
		SUB_GOAL_IN,
		SUB_LAST
    };
public:
    CPlayerState_CannonPortal(CPlayer* _pOwner);

    // CPlayerState을(를) 통해 상속됨
    void Update(_float _fTimeDelta) override;
    virtual void Enter() override;
    virtual void Exit() override;
    virtual void On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx) override;

    void Set_SubState(SUB_STATE _eState);
    _bool Is_ChargeCompleted() { return SUB_INTO != m_eSubState && SUB_SPIN_CHARGE != m_eSubState; }
private:
    CPortal_Cannon* m_pPortal = nullptr;
    CPortal_Cannon* m_pTargetPortal = nullptr;
    _vector m_vPortalPos = {};
    _vector m_vPortalPos3D = {};
	SUB_STATE m_eSubState = SUB_STATE::SUB_LAST;
    _bool m_bShootedFrame = false;
    _bool m_bGoalInFrame = false;
    E_DIRECTION m_e2DLookDir = E_DIRECTION::RIGHT;
};

END