#include "stdafx.h"
#include "PlayerState_ThrowObject.h"
#include "GameInstance.h"

CPlayerState_ThrowObject::CPlayerState_ThrowObject(CPlayer* _pOwner)
	: CPlayerState(_pOwner, CPlayer::THROWOBJECT)
{
}

void CPlayerState_ThrowObject::Update(_float _fTimeDelta)
{
}

void CPlayerState_ThrowObject::Enter()
{
	if (COORDINATE_3D == m_pOwner->Get_CurCoord())
	{
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_THROW_NEWRIG);
	}
	else
	{
		F_DIRECTION eFDir = To_FDirection(m_pOwner->Get_2DDirection());
		switch (eFDir)
		{
		case Client::F_DIRECTION::LEFT:
		case Client::F_DIRECTION::RIGHT:
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_THROW_OBJECT_RIGHT);
			break;
		case Client::F_DIRECTION::UP:
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_THROW_OBJECT_UP);
			break;
		case Client::F_DIRECTION::DOWN:
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_THROW_OBJECT_DOWN);
			break;
		default:
			break;
		}
	}

	m_pGameInstance->Start_SFX(_wstring(L"A_sfx_Throw-") + to_wstring(rand() % 6), 50.f);

}

void CPlayerState_ThrowObject::Exit()
{
	//m_pOwner->Set_CarryingObject(nullptr);
	//if (m_pOwner->Is_SwordHandling())
	//	m_pOwner->Equip_Part(CPlayer::PLAYER_PART_SWORD);
}

void CPlayerState_ThrowObject::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	if (COORDINATE_3D == _eCoord)
	{
		if (iAnimIdx == (_uint)CPlayer::ANIM_STATE_3D::LATCH_THROW_NEWRIG)
		{
			m_pOwner->Set_State(CPlayer::IDLE);
		}
	}
	else
	{
		if (iAnimIdx == (_uint)CPlayer::ANIM_STATE_2D::PLAYER_THROW_OBJECT_RIGHT
			|| iAnimIdx == (_uint)CPlayer::ANIM_STATE_2D::PLAYER_THROW_OBJECT_UP
			|| iAnimIdx == (_uint)CPlayer::ANIM_STATE_2D::PLAYER_THROW_OBJECT_DOWN)
		{
			m_pOwner->Set_State(CPlayer::IDLE);
		}
	}
}
