#pragma once
#include "PlayerState.h"
BEGIN(Client)
class CBook;
class CTurnBookEffect;
class CPlayerState_TurnBook :
    public CPlayerState
{
	enum BOOK_STATE
	{
		IDLE = 0,
		TURN_LEFT ,
		TURN_RIGHT ,
		CLOSED_LEFT ,
		TILT_LToR,
		CLOSED_RIGHT ,
		TILT_RToL,
		TURN_STATE_LAST ,
	};
	enum BOOK_TRANSFORM
	{
		TRANSFORM_LEFT,
		TRANSFORM_RIGHT,
		TRANSFORM_CENTER,
		TRANSFORM_LAST
	};
public:
    CPlayerState_TurnBook(CPlayer* _pOwner);

    // CPlayerState¿ª(∏¶) ≈Î«ÿ ªÛº”µ 
    void Update(_float _fTimeDelta) override;
    virtual void Enter() override;
    virtual void Exit() override;

private:
	void Switch_PlayerAnimation(_bool _bLeft);
	void On_StateChange(BOOK_STATE _eNewState);
	void Set_State(BOOK_STATE _eNewState);
	BOOK_TRANSFORM ConvertToTransform(BOOK_STATE _eState)
	{
		switch (_eState)
		{
		case Client::CPlayerState_TurnBook::TURN_LEFT:
		case Client::CPlayerState_TurnBook::CLOSED_LEFT:
		case Client::CPlayerState_TurnBook::TILT_RToL:
			return BOOK_TRANSFORM::TRANSFORM_LEFT;
			break;
		case Client::CPlayerState_TurnBook::TURN_RIGHT:
		case Client::CPlayerState_TurnBook::TILT_LToR:
		case Client::CPlayerState_TurnBook::CLOSED_RIGHT:
			return BOOK_TRANSFORM::TRANSFORM_RIGHT;
		case Client::CPlayerState_TurnBook::IDLE:
			return BOOK_TRANSFORM::TRANSFORM_CENTER;
		default:
			return BOOK_TRANSFORM::TRANSFORM_LAST;
		}
	}
private:
	BOOK_STATE m_eBookState = BOOK_STATE::IDLE;

	_bool m_bUpside = false;
	CBook* m_pBook = nullptr;
	_float m_fTiltIdleTime = 0.5f;
	_float m_fTiltIdleTimeAcc = 0.f;
	_float m_fSlidProgress = 0.16f;

	// ¥›»˘ ªÛ≈¬ø°º≠ ø≠±‚
	//_bool				m_is
private:
	CTurnBookEffect* m_pTurnBookEffect = { nullptr };

};
END
