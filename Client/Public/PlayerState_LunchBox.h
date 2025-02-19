#pragma once
#include "PlayerState.h"

BEGIN(Client)
class CLunchBox;
class CPlayerState_LunchBox :
    public CPlayerState
{
	enum LUNCHBOX_STATE
	{
		
		LUNCHBOX_STATE_IDLE,
		LUNCHBOX_STATE_CHARGE,
		LUNCHBOX_STATE_LAST
	};
public:
    CPlayerState_LunchBox(CPlayer* _pOwner);

    // CPlayerState을(를) 통해 상속됨
    void Update(_float _fTimeDelta) override;
    virtual void Enter() override;
    virtual void Exit() override;
    virtual void On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx) override;

private:
	LUNCHBOX_STATE m_eCurState = LUNCHBOX_STATE_IDLE;
    _bool m_bArrival = false;
    _vector m_vPosition;
    _vector m_vLook;
    _float m_fMoveSpeed = 10.f;
    _float m_fKnockBackPow =20.f;
    CLunchBox* m_pLunchBox = nullptr;
};

END