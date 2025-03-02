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
	_uint iBookAnim = m_pBook->Get_CurrentAnimIndex();

	if (tKeyResult.bInputStates[PLAYER_INPUT_TURNBOOK_END])
	{
		m_pBook->Set_PlayingAnim(true);
		m_pBook->Switch_Animation(CSampleBook::IDLE);
		m_pOwner->Set_State(CPlayer::IDLE);
		return;
	}

	if(IDLE == m_eBookState)
	{
		m_fTiltIdleTimeAcc += _fTimeDelta;
		
		if (tKeyResult.bInputStates[PLAYER_INPUT_TURNBOOK_LEFT])
			Set_State(BOOK_STATE::TURN_LEFT);
		else if (tKeyResult.bInputStates[PLAYER_INPUT_TURNBOOK_RIGHT])
			Set_State(BOOK_STATE::TURN_RIGHT);
		else if(m_fTiltIdleTimeAcc >= m_fTiltIdleTime)
		{
			if (tKeyResult.bInputStates[PLAYER_INPUT_TILTBOOK_LEFT])
			{
				Set_State(BOOK_STATE::TILT_RToL);
				m_pBook->Switch_Animation(CSampleBook::BOOK_ANIMATION::CLOSE_R_TO_L);
				m_pBook->Set_ReverseAnimation(false);
			}
			else if (tKeyResult.bInputStates[PLAYER_INPUT_TILTBOOK_RIGHT])
			{
				Set_State(BOOK_STATE::TILT_LToR);
				m_pBook->Switch_Animation(CSampleBook::BOOK_ANIMATION::CLOSE_L_TO_R);
				m_pBook->Set_ReverseAnimation(false);
			}
		}
	}
	else if (CLOSED_LEFT == m_eBookState)
	{
		if (tKeyResult.bInputStates[PLAYER_INPUT_TILTBOOK_RIGHT])
		{
			Set_State(BOOK_STATE::TILT_LToR);
			//m_pBook->Set_Animation(CSampleBook::BOOK_ANIMATION::CLOSE_L_TO_R);
		}
	}
	else if (CLOSED_RIGHT == m_eBookState)
	{
		if (tKeyResult.bInputStates[PLAYER_INPUT_TILTBOOK_LEFT])
		{
			Set_State(BOOK_STATE::TILT_RToL);
			//m_pBook->Set_Animation(CSampleBook::BOOK_ANIMATION::CLOSE_R_TO_L);
		}
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
		//역방향 <-
		if (tKeyResult.bInputStates[PLAYER_INPUT_TILTBOOK_LEFT])
			Set_State(TILT_RToL);
		//정방향  ->
		else if (tKeyResult.bInputStates[PLAYER_INPUT_TILTBOOK_RIGHT])
		{
			m_pBook->Set_PlayingAnim(true);

			//반대 면이 기울어져있었으면?
			if (CSampleBook::BOOK_ANIMATION::CLOSE_R_TO_L == iBookAnim)
			{
				m_pBook->Set_ReverseAnimation(true);
				//넘기는 중에 다 펼쳐지면? 
				if (false == m_pBook->Is_DuringAnimation() 
					&&  0.2f >=m_pBook->Get_CurrentAnimationProgress())
				{
					Set_State(IDLE);
				}
			}
			// 정상 기울이기면?
			else if (CSampleBook::BOOK_ANIMATION::CLOSE_L_TO_R == iBookAnim)
			{
				m_pBook->Set_ReverseAnimation(false);
				//기울이는 중에 특정 진행도를 넘으면?
				if (m_fSlidProgress <= m_pBook->Get_CurrentAnimationProgress())
					m_pBook->SlideObjects_LToR();
				//기울이는 중에 완전히 덮히면?
				if (false == m_pBook->Is_DuringAnimation())
					Set_State(CLOSED_RIGHT);
			}
		}
		//아무 입력도 없으면?
		else
			m_pBook->Set_PlayingAnim(false);

	}
	else if (TILT_RToL == m_eBookState)
	{	//역방향 ->
		if (tKeyResult.bInputStates[PLAYER_INPUT_TILTBOOK_RIGHT])
			Set_State(TILT_LToR);
		//정방향  <-
		else if (tKeyResult.bInputStates[PLAYER_INPUT_TILTBOOK_LEFT])
		{
			m_pBook->Set_PlayingAnim(true);

			//반대 면이 기울어져있었으면?
			if (CSampleBook::BOOK_ANIMATION::CLOSE_L_TO_R == iBookAnim)
			{
				m_pBook->Set_ReverseAnimation(true);
				//넘기는 중에 다 펼쳐지면? 
				if (false == m_pBook->Is_DuringAnimation()
					&& 0.2f >= m_pBook->Get_CurrentAnimationProgress())
				{
					Set_State(IDLE);
				}
			}
			// 정상 기울이기면?
			else if (CSampleBook::BOOK_ANIMATION::CLOSE_R_TO_L == iBookAnim)
			{
				m_pBook->Set_ReverseAnimation(false);
				//기울이는 중에 특정 진행도를 넘으면?
				if (m_fSlidProgress <= m_pBook->Get_CurrentAnimationProgress())
					m_pBook->SlideObjects_RToL();
				//기울이는 중에 완전히 덮히면?
				if (false == m_pBook->Is_DuringAnimation())
					Set_State(CLOSED_LEFT);
			}
		}
		//아무 입력도 없으면?
		else
			m_pBook->Set_PlayingAnim(false);
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
	m_fTiltIdleTimeAcc = m_fTiltIdleTime;
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
			//m_pBook->Switch_Animation(CSampleBook::BOOK_ANIMATION::CLOSE_R_TO_L);
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
			//m_pBook->Switch_Animation(CSampleBook::BOOK_ANIMATION::CLOSE_L_TO_R);
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
		m_fTiltIdleTimeAcc = 0.f;
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
