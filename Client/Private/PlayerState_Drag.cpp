#include "stdafx.h"
#include "PlayerState_Drag.h"
#include "DraggableObject.h"
#include "Actor_Dynamic.h"

CPlayerState_Drag::CPlayerState_Drag(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::DRAG)
{
}

void CPlayerState_Drag::Update(_float _fTimeDelta)
{
	PLAYER_INPUT_RESULT tKeyResult = m_pOwner->Player_KeyInput();
	INTERACT_RESULT eResult = m_pOwner->Try_Interact(_fTimeDelta);
	if (INTERACT_RESULT::CHARGING == eResult)
	{
		m_pDraggableObject->Try_Interact(m_pOwner, _fTimeDelta);

		if (tKeyResult.bInputStates[PLAYER_INPUT_MOVE])
		{
			m_pOwner->Set_PlayingAnim(true);
			m_pOwner->Move(XMVector3Normalize(tKeyResult.vMoveDir) * m_fDragMoveSpeed, _fTimeDelta);
		}
		else
		{
			m_pOwner->Set_PlayingAnim(false);
		}
	}
	else
	{
		m_pOwner->Set_State(CPlayer::IDLE);
		return;
	}



}

void CPlayerState_Drag::Enter()
{
	COORDINATE eCoord = m_pOwner->Get_CurCoord();
	m_pDraggableObject = dynamic_cast<CDraggableObject*>( m_pOwner->Get_InteractableObject());
	assert(nullptr != m_pDraggableObject);
	m_fDragMoveSpeed = m_pOwner->Get_DragMoveSpeed(eCoord);
	if (COORDINATE_2D == eCoord)
	{
		
	}
	else
	{

		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_PULL_GT);
	}
}

void CPlayerState_Drag::Exit()
{

}

void CPlayerState_Drag::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{

}
