#pragma once
#include "PlayerState.h"
BEGIN(Client)
class CSampleBook;
class CPlayerState_TurnBook :
    public CPlayerState
{
	enum TURN_STATE
	{
		IDLE = 0,
		TURN_LEFT = 1,
		TURN_RIGHT = 2,
		TILT_LEFT = 3,
		TILT_RIGHT = 4,
		TURN_STATE_LAST = 5,
	};
public:
    CPlayerState_TurnBook(CPlayer* _pOwner);

    // CPlayerState을(를) 통해 상속됨
    void Update(_float _fTimeDelta) override;
    virtual void Enter() override;
    virtual void Exit() override;

	void Switch_PlayerAnimation(_bool _bLeft);
private:
	TURN_STATE m_eTurnState = TURN_STATE::IDLE;
	_bool m_bUpside = false;
	CSampleBook* m_pBook = nullptr;
};
END
