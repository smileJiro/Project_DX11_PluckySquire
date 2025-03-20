#include "stdafx.h"
#include "PlayerState_TransformIn.h"
#include "Zip_C8.h"

CPlayerState_TransformIn::CPlayerState_TransformIn(CPlayer* _pOwner, CZip_C8* _pZip)
	:CPlayerState(_pOwner, CPlayer::TRANSFORM_IN)
	, m_pZip(_pZip)
{
	Safe_AddRef(m_pZip);
}

void CPlayerState_TransformIn::Update(_float _fTimeDelta)
{
	_float fProgress = m_pOwner->Get_AnimProgress();
	if (false == m_bAttach && m_fAttachkProgress <= fProgress)
	{
		m_bAttach = true;
		m_pZip->Set_ProgState(CZip_C8::ATTACH);
	}

}

void CPlayerState_TransformIn::Enter()
{
	COORDINATE eCoord = m_pOwner->Get_CurCoord();
	if (COORDINATE_2D == eCoord)
	{
		m_pOwner->Set_BlockPlayerInput(true);
		m_pOwner->Set_2DDirection(F_DIRECTION::RIGHT);
		m_pOwner->Set_Mode(CPlayer::PLAYER_MODE_CYBERJOT);
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_CYBERJOTLITE_TRANSFORM);
	}


}

void CPlayerState_TransformIn::Exit()
{
	m_pOwner->Set_BlockPlayerInput(false);
	Safe_Release(m_pZip);
}

void CPlayerState_TransformIn::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	if (COORDINATE_2D == _eCoord && (_uint)CPlayer::ANIM_STATE_2D::PLAYER_CYBERJOTLITE_TRANSFORM == iAnimIdx)
	{
		m_pOwner->Set_State(CPlayer::IDLE);
	}
}
