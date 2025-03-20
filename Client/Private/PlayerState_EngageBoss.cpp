#include "stdafx.h"
#include "PlayerState_EngageBoss.h"
#include "GameInstance.h"
#include "Effect_Manager.h"
#include "ModelObject.h"
#include "ZetPack.h"

CPlayerState_EngageBoss::CPlayerState_EngageBoss(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::ENGAGE_BOSS)
{
}

void CPlayerState_EngageBoss::Update(_float _fTimeDelta)
{
}

void CPlayerState_EngageBoss::Enter()
{
	//m_pOwner->Set_Mode(CPlayer::PLAYER_MODE_CYBERJOT);
	m_pOwner->Set_Kinematic(true);
	m_pOwner->Equip_Part(CPlayer::PLAYER_PART_RIFLE);
	m_pOwner->Equip_Part(CPlayer::PLAYER_PART_VISOR);
	m_pOwner->Equip_Part(CPlayer::PLAYER_PART_ZETPACK);
	m_pOwner->Equip_Part(CPlayer::PLAYER_PART_CYBERCURSOR);

	m_pOwner->Set_Position(XMVectorSet(-10.6962425f, 6.06746035f, 11.9308004f, 1.f));

	if (nullptr != m_pOwner->Get_ZetPack())
		m_pOwner->Get_ZetPack()->Switch_State(CZetPack::STATE_CYBER);
	m_pOwner->Set_Position(_vector{  -11.0362425f ,5.76746035f , 11.9308004f });
	m_pOwner->Set_Kinematic(true);
	m_pOwner->LookDirectionXZ_Kinematic(_vector{ 0.f,0.f,-1.f });
	m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::CYBERJOT_CINE_DO9_LB_ENGAGE_SH01);

}

void CPlayerState_EngageBoss::Exit()
{
}

void CPlayerState_EngageBoss::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	if ((_uint)CPlayer::ANIM_STATE_3D::CYBERJOT_CINE_DO9_LB_ENGAGE_SH01 == iAnimIdx)
	{
		m_pGameInstance->Get_GameObject_Ptr(m_pOwner->Get_CurLevelID(), TEXT("Layer_Boss"), 0)->Set_Position(_vector{0.f , 30.f, -10.f});
		m_pOwner->Set_Position(_vector{ 0.f , 30.f, -75.f });
		m_pOwner->LookDirectionXZ_Kinematic(_vector{ 0.f,0.f,1.f });
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::CYBERJOT_CINE_DO9_LB_ENGAGE_SH02);
	}
	else if ((_uint)CPlayer::ANIM_STATE_3D::CYBERJOT_CINE_DO9_LB_ENGAGE_SH02 == iAnimIdx)
	{
	//	m_pOwner->Set_State(CPlayer::CYBER_FLY);
		int a = 0;
	}
}
