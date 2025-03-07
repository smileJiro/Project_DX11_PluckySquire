#include "stdafx.h"
#include "PlayerState_TransformIn.h"

CPlayerState_TransformIn::CPlayerState_TransformIn(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::TRANSFORM_IN)
{
}

void CPlayerState_TransformIn::Update(_float _fTimeDelta)
{
}

void CPlayerState_TransformIn::Enter()
{
	COORDINATE eCoord = m_pOwner->Get_CurCoord();
	if (COORDINATE_2D == eCoord)
	{
		m_pOwner->Transform_Out_CyberJot();
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_CYBERJOTLITE_TRANSFORM);
	}

}

void CPlayerState_TransformIn::Exit()
{
}

void CPlayerState_TransformIn::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	if (COORDINATE_2D == _eCoord && (_uint)CPlayer::ANIM_STATE_2D::PLAYER_CYBERJOTLITE_TRANSFORM == iAnimIdx)
	{
		m_pOwner->Set_State(CPlayer::IDLE);
	}
}
