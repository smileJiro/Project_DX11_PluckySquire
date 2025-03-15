#include "stdafx.h"
#include "PlayerState_Drag.h"
#include "DraggableObject.h"
#include "Actor_Dynamic.h"
#include "PlayerBody.h"

CPlayerState_Drag::CPlayerState_Drag(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::DRAG)
{
}

void CPlayerState_Drag::Update(_float _fTimeDelta)
{
	m_fAnimTransitionTimeAcc += _fTimeDelta;
	if (m_fAnimTransitionTimeAcc < m_fAnimTransitionTime)
		return;
	if (false == m_pDraggableObject->Is_Interacting())
	{
		m_pOwner->Set_State(CPlayer::IDLE);
		return;
	}

	COORDINATE eCoord = m_pOwner->Get_CurCoord();

	if (COORDINATE_3D == eCoord)
	{
		CActor_Dynamic* pDraggableDynamic = dynamic_cast<CActor_Dynamic*>(m_pDraggableObject->Get_ActorCom());
		CActor_Dynamic* pOwnerDynamic = dynamic_cast<CActor_Dynamic*>(m_pOwner->Get_ActorCom());
		if (pDraggableDynamic->Get_LinearVelocity().m128_f32[1] < -0.1f
			|| pOwnerDynamic->Get_LinearVelocity().m128_f32[1] < -0.1f)
		{
			m_pOwner->Set_State(CPlayer::IDLE);
			return;
		}
		m_pOwner->Stop_Rotate();

	}


	INTERACT_RESULT eResult = m_pOwner->Try_Interact(_fTimeDelta);
	//HOLDING 중. 계속 E 키 누름.
	if (INTERACT_RESULT::SUCCESS == eResult)
	{
		PLAYER_INPUT_RESULT tKeyResult = m_pOwner->Player_KeyInput();
		//E 누른 상태로 wasd키 누름
		if (tKeyResult.bInputStates[PLAYER_INPUT_MOVE])
		{
			F_DIRECTION eNewDragDirection = To_FDirection(tKeyResult.vMoveDir, eCoord);
			//드래그 방향이 바뀌면 애니메이션 바꿔야 함.
			if (eNewDragDirection != m_eOldDragDirection)
			{
				m_eOldDragDirection = eNewDragDirection;
				//물체와 나 사이의 방향과 일치하는 방향으로 이동하면 PUSH
				_bool bPushing = To_FDirection(m_vHoldOffset, eCoord) == eNewDragDirection;
				//PUSH, PULL  애니메이션 전환
				if (bPushing)
					Switch_To_PushAnimation(eCoord);
				else
					Switch_To_PullAnimation(eCoord, tKeyResult.vMoveDir);
			}
			m_pOwner->Set_PlayingAnim(true);
			
			m_pOwner->Move(XMVector3Normalize(tKeyResult.vMoveDir) * m_fDragMoveSpeed, _fTimeDelta);
			m_pDraggableObject->Move(XMVector3Normalize(tKeyResult.vMoveDir) * m_fDragMoveSpeed, _fTimeDelta);

			if (COORDINATE_3D == eCoord)
			{
				_vector vDraggablePos = m_pDraggableObject->Get_FinalPosition();
				CActor_Dynamic* pOwnerDynamic = dynamic_cast<CActor_Dynamic*>(m_pOwner->Get_ActorCom());
				_float3 vNewOwnerPos; XMStoreFloat3(&vNewOwnerPos, vDraggablePos - m_vHoldOffset);
				pOwnerDynamic->Set_GlobalPose(vNewOwnerPos);
			}


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
//Draggable한테 플레이어의 Shape를 추가하고, Draggable을 Move 시킴.
//플레이어는 그냥 Kinematic으로 만들고 SetGlobalPos로 계속 붙어있게 함.
void CPlayerState_Drag::Enter()
{
	COORDINATE eCoord = m_pOwner->Get_CurCoord();
	m_pDraggableObject = dynamic_cast<CDraggableObject*>( m_pOwner->Get_InteractableObject());
	assert(nullptr != m_pDraggableObject);
	m_fDragMoveSpeed = m_pOwner->Get_DragMoveSpeed(eCoord);
	m_vHoldOffset = ( m_pDraggableObject->Get_FinalPosition() - m_pOwner->Get_FinalPosition());
	if (COORDINATE_2D == eCoord)
	{

	}
	else
	{
		CActor_Dynamic* pDraggableDynamic = dynamic_cast<CActor_Dynamic*>( m_pDraggableObject->Get_ActorCom());
		CActor_Dynamic* pOwnerDynamic = dynamic_cast<CActor_Dynamic*>(m_pOwner->Get_ActorCom());
		m_iAdditionalShapeIndex = (_uint)pDraggableDynamic->Get_Shapes().size();
		SHAPE_DATA tShape = m_pOwner->Get_BodyShapeData();
		tShape.iShapeUse = (_uint)SHAPE_USE::SHAPE_USE_LAST;
		_matrix matLocalOffset =XMLoadFloat4x4(&tShape.LocalOffsetMatrix);
		matLocalOffset *= XMMatrixTranslationFromVector(XMVectorSetW( -m_vHoldOffset,1));
		XMStoreFloat4x4(&tShape.LocalOffsetMatrix, matLocalOffset);
		pDraggableDynamic->Add_Shape(tShape) ;
		m_pOwner->Get_Body()->Set_3DAnimationTransitionTime((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_PULL_GT, m_fAnimTransitionTime);
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_PULL_GT);
		//m_pOwner->Set_Kinematic(true);
	}
}

void CPlayerState_Drag::Exit()
{
	COORDINATE eCoord = m_pOwner->Get_CurCoord();

	if (COORDINATE_3D == eCoord)
	{
		CActor_Dynamic* pDraggableDynamic = dynamic_cast<CActor_Dynamic*>(m_pDraggableObject->Get_ActorCom());
		pDraggableDynamic->Delete_Shape(m_iAdditionalShapeIndex);
		//m_pOwner->Set_Kinematic(false);
	}
	m_pOwner->Set_PlayingAnim(true);
}

void CPlayerState_Drag::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{

}

void CPlayerState_Drag::Switch_To_PushAnimation(COORDINATE _eCoord)
{
	if (COORDINATE_3D == _eCoord)
	{
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_PUSH_GT);
	}
	else
	{

	}
}

void CPlayerState_Drag::Switch_To_PullAnimation(COORDINATE _eCoord, _vector _vMoveDir)
{

	if (COORDINATE_3D == _eCoord)
	{
		if (To_FDirection(-m_vHoldOffset, _eCoord) == To_FDirection(_vMoveDir, _eCoord))
		{
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_PULL_GT);
		}
		else
		{
			_float fCross = XMVectorGetY(XMVector3Cross(m_vHoldOffset, _vMoveDir));
			if (0 < fCross)
			{
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_PULL_RIGHT_GT);
			}
			else if (0 > fCross)
			{
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_PULL_LEFT_GT);
			}
		}


	}
	else
	{

	}
}

