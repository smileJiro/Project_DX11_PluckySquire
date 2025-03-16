#include "stdafx.h"
#include "PlayerState_Mojam.h"
#include "Dialog_Manager.h"
#include "PlayerBody.h"

CPlayerState_Mojam::CPlayerState_Mojam(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::MOJAM)
{
}

void CPlayerState_Mojam::Update(_float _fTimeDelta)
{
	CPlayerBody* pPlayerBody = static_cast<CPlayerBody*>(m_pOwner->Get_PartObject(CPlayer::PLAYER_PART_BODY));
	if (nullptr != pPlayerBody)
	{
		CModel* pModel = pPlayerBody->Get_Model(COORDINATE_2D);
		if (CPlayer::ANIM_STATE_2D::PLAYER_MOJAM_MOJAM == (CPlayer::ANIM_STATE_2D)pModel->Get_CurrentAnimIndex())
		{
			_float fRatio = pModel->Get_CurrentAnimProgeress();
			
			if (0.8f < fRatio && 0 == m_iAnimActionCount) // 타이밍 맞춰서 모잼 다이얼로그 생성
			{
				_wstring strDialogueTag = TEXT("Mojam");
				CDialog_Manager::GetInstance()->Set_DialogId(strDialogueTag.c_str());
				++m_iAnimActionCount;
			}
		}

	}

}

void CPlayerState_Mojam::Enter()
{
	assert(COORDINATE_2D == m_pOwner->Get_CurCoord());
	m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_MOJAM_INTO);
	m_iAnimActionCount = 0;
}

void CPlayerState_Mojam::Exit()
{
}

void CPlayerState_Mojam::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	if (COORDINATE_2D == _eCoord)
	{
		switch (CPlayer::ANIM_STATE_2D(iAnimIdx))
		{
		case CPlayer::ANIM_STATE_2D::PLAYER_MOJAM_INTO:
		{
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_MOJAM_MOJAM);
		}
		break;
		case CPlayer::ANIM_STATE_2D::PLAYER_MOJAM_MOJAM:
		{
			m_pOwner->Set_State(CPlayer::IDLE);
		}
		break;
		}
	}
}
