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
	
	if (tKeyResult.bInputStates[PLAYER_INPUT_TURNBOOK_LEFT])
	{
		if(TURN_STATE::TURN_LEFT !=  m_eTurnState)
		{
			m_eTurnState = TURN_STATE::TURN_LEFT;
			Switch_PlayerAnimation(!m_bUpside);
		}
		if(CSampleBook::BOOK_PAGE_ACTION::ACTION_LAST == m_pBook->Get_ActionType())
			Event_Book_Main_Section_Change_Start(1,&vDefaultPos);
	}
	else if (tKeyResult.bInputStates[PLAYER_INPUT_TURNBOOK_RIGHT])
	{
		if (TURN_STATE::TURN_RIGHT != m_eTurnState)
		{
			m_eTurnState = TURN_STATE::TURN_RIGHT;
			Switch_PlayerAnimation(!m_bUpside);
		}
		if (CSampleBook::BOOK_PAGE_ACTION::ACTION_LAST == m_pBook->Get_ActionType())
			Event_Book_Main_Section_Change_Start(0, &vDefaultPos);
	}
	else if (tKeyResult.bInputStates[PLAYER_INPUT_TILTBOOK_LEFT])
	{
	}
	else if (tKeyResult.bInputStates[PLAYER_INPUT_TILTBOOK_RIGHT])
	{
	}
	else if (tKeyResult.bInputStates[PLAYER_INPUT_TURNBOOK_END])
	{
		m_pOwner->Set_State(CPlayer::IDLE);
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
	m_eTurnState = TURN_STATE::IDLE;
	m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_TURN_MID);
}

void CPlayerState_TurnBook::Exit()
{
}

void CPlayerState_TurnBook::Switch_PlayerAnimation(_bool _bLeft)
{
	m_pOwner->Switch_Animation(_bLeft ? (_uint)CPlayer::ANIM_STATE_3D::LATCH_TURN_LEFT : (_uint)CPlayer::ANIM_STATE_3D::LATCH_TURN_RIGHT);

}
