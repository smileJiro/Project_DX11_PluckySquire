#include "stdafx.h"
#include "PlayerState_TurnBook.h"
#include "SampleBook.h"

CPlayerState_TurnBook::CPlayerState_TurnBook(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::TURN_BOOK)
{
}

void CPlayerState_TurnBook::Update(_float _fTimeDelta)
{
	PLAYER_INPUT_RESULT tKeyResult = m_pOwner->Player_KeyInput_ControlBook();
	_float3 vDefaultPos = { 0,0,0 };
	 

	if (tKeyResult.bInputStates[PLAYER_INPUT_TURNBOOK_END])
	{
		Set_State(BOOK_STATE::IDLE);
	}

	if(IDLE == m_eBookState)
	{
		if (tKeyResult.bInputStates[PLAYER_INPUT_TURNBOOK_LEFT])
			Set_State(BOOK_STATE::TURN_LEFT);
		else if (tKeyResult.bInputStates[PLAYER_INPUT_TURNBOOK_RIGHT])
			Set_State(BOOK_STATE::TURN_RIGHT);
		else if (tKeyResult.bInputStates[PLAYER_INPUT_TILTBOOK_LEFT])
			Set_State(BOOK_STATE::TILT_RToL);
		else if (tKeyResult.bInputStates[PLAYER_INPUT_TILTBOOK_RIGHT])
			Set_State(BOOK_STATE::TILT_LToR);
	}
	else if (CLOSED_LEFT == m_eBookState)
	{
		if (tKeyResult.bInputStates[PLAYER_INPUT_TILTBOOK_RIGHT])
			Set_State(BOOK_STATE::TILT_LToR);
	}
	else if (CLOSED_RIGHT == m_eBookState)
	{
		if (tKeyResult.bInputStates[PLAYER_INPUT_TILTBOOK_LEFT])
			Set_State(BOOK_STATE::TILT_RToL);
	}
	else if (TURN_LEFT == m_eBookState || TURN_RIGHT == m_eBookState)
	{
		if (CSampleBook::BOOK_PAGE_ACTION::ACTION_LAST == m_pBook->Get_ActionType())
		{
			if (tKeyResult.bInputStates[PLAYER_INPUT_TURNBOOK_LEFT])
			{
				Event_Book_Main_Section_Change_Start(1, &vDefaultPos);
				Set_State(TURN_LEFT);
			}
			else if (tKeyResult.bInputStates[PLAYER_INPUT_TURNBOOK_RIGHT])
			{
				Event_Book_Main_Section_Change_Start(0, &vDefaultPos);
				Set_State(TURN_RIGHT);
			}
			else
			{
				Set_State(IDLE);
			}
		}

	}
	else if (TILT_LToR == m_eBookState)
	{
		//기울어진 상태라면?
		if (m_pBook->Is_DuringAnimation())
		{
			//왼쪽으로 되돌리기
			if (tKeyResult.bInputStates[PLAYER_INPUT_TILTBOOK_LEFT])
			{
				m_pBook->Set_PlayingAnim(true);
				m_pBook->Set_ReverseAnimation(true);
			}
			//오른쪽으로 계속 기울이기
			else if (tKeyResult.bInputStates[PLAYER_INPUT_TILTBOOK_RIGHT])
			{
				m_pBook->Set_PlayingAnim(true);
				m_pBook->Set_ReverseAnimation(false);
			}
			else
				m_pBook->Set_PlayingAnim(false);
		}
		else
		{
			if (m_pBook->Is_ReverseAnimation())
				Set_State(IDLE);
			else
				Set_State(CLOSED_RIGHT);
			return;
		}
	}
	else if (TILT_RToL == m_eBookState)
	{		//기울어진 상태라면?
		if (m_pBook->Is_DuringAnimation())
		{
			//왼쪽으로 되돌리기
			if (tKeyResult.bInputStates[PLAYER_INPUT_TILTBOOK_RIGHT])
			{
				m_pBook->Set_PlayingAnim(true);
				m_pBook->Set_ReverseAnimation(true);
			}
			//오른쪽으로 계속 기울이기
			else if (tKeyResult.bInputStates[PLAYER_INPUT_TILTBOOK_LEFT])
			{
				m_pBook->Set_PlayingAnim(true);
				m_pBook->Set_ReverseAnimation(false);
			}
			else
				m_pBook->Set_PlayingAnim(false);
		}
		else
		{
			if (m_pBook->Is_ReverseAnimation())
				Set_State(IDLE);
			else
				Set_State(CLOSED_LEFT);
			return;
		}
	}


}

void CPlayerState_TurnBook::Enter()
{
	m_pBook = dynamic_cast<CSampleBook*>( m_pOwner->Get_InteractableObject());
	if (nullptr == m_pBook)
	{
		m_pOwner->Set_State(CPlayer::IDLE);
		return;
	}
	m_bUpside = m_pBook->Get_FinalPosition().m128_f32[1] < m_pOwner->Get_FinalPosition().m128_f32[1];
	m_eBookState = BOOK_STATE::IDLE;
	m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_TURN_MID);
}

void CPlayerState_TurnBook::Exit()
{
}

void CPlayerState_TurnBook::Switch_PlayerAnimation(_bool _bLeft)
{
	m_pOwner->Switch_Animation(_bLeft ? (_uint)CPlayer::ANIM_STATE_3D::LATCH_TURN_RIGHT :(_uint)CPlayer::ANIM_STATE_3D::LATCH_TURN_LEFT);

}

void CPlayerState_TurnBook::On_StateChange(BOOK_STATE _eNewState)
{
	_float3 vDefaultPos = { 0,0,0 };
	BOOK_TRANSFORM eTransform = ConvertToTransform(_eNewState);
	switch (eTransform)
	{
	//====================왼쪽으로~===================
	case Client::CPlayerState_TurnBook::TRANSFORM_LEFT:
	{
		Switch_PlayerAnimation(!m_bUpside);

		switch (_eNewState)
		{
		case Client::CPlayerState_TurnBook::TURN_LEFT:

			break;
		case Client::CPlayerState_TurnBook::TILT_RToL:
			m_pBook->Switch_Animation(CSampleBook::BOOK_ANIMATION::CLOSE_R_TO_L);
			break;
		case Client::CPlayerState_TurnBook::CLOSED_LEFT:
		default:
			break;
		}

		break;
	}
	// ==========오른쪽으로~============================
	case Client::CPlayerState_TurnBook::TRANSFORM_RIGHT:
	{
		Switch_PlayerAnimation(m_bUpside);

		switch (_eNewState)
		{
		case Client::CPlayerState_TurnBook::TURN_RIGHT:

			break;
		case Client::CPlayerState_TurnBook::TILT_LToR:
			m_pBook->Switch_Animation(CSampleBook::BOOK_ANIMATION::CLOSE_L_TO_R);
			break;
		case Client::CPlayerState_TurnBook::CLOSED_RIGHT:
		default:
			break;
		}

		break;

		break;
	}
	//===================가운데=========================
	case Client::CPlayerState_TurnBook::TRANSFORM_CENTER:
	{
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_TURN_MID);
		m_pBook->Switch_Animation(CSampleBook::IDLE);
		break;
	}
	default:
		break;
	}

	
}

void CPlayerState_TurnBook::Set_State(BOOK_STATE _eNewState)
{
	if (m_eBookState != _eNewState)
	{
		On_StateChange(_eNewState);
		m_eBookState = _eNewState;
	}
}
