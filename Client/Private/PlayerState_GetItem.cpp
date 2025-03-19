#include "stdafx.h"
#include "PlayerState_GetItem.h"
#include "GameInstance.h"
#include "GameInstance.h"
#include "Effect_Manager.h"
#include "ModelObject.h"
#include "Section_Manager.h"

CPlayerState_GetItem::CPlayerState_GetItem(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::GET_ITEM)
{
}

void CPlayerState_GetItem::Update(_float _fTimeDelta)
{
	if (KEY_DOWN(KEY::ENTER))
	{
		m_pOwner->Set_2DDirection(E_DIRECTION::RIGHT);
		m_pOwner->Set_State(CPlayer::IDLE);
	}
}

void CPlayerState_GetItem::Enter()
{
	COORDINATE eCoord = m_pOwner->Get_CurCoord();
	switch (eCoord)
	{
	case Engine::COORDINATE_2D:
	{
		m_pOwner->Set_2DDirection(E_DIRECTION::LEFT);
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_ITEM_RETRIEVE);
		CModelObject::MODELOBJECT_DESC tModelDesc{};
		tModelDesc.Build_2D_Model(m_pOwner->Get_CurLevelID(), TEXT("GetItem_FX"), TEXT("Prototype_Component_Shader_VtxPosTex"), (_uint)PASS_VTXPOSTEX::SPRITE2D, false);
		tModelDesc.iModelPrototypeLevelID_2D = LEVEL_STATIC;
		m_pFX = static_cast<CModelObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &tModelDesc));
		m_pGameInstance->Add_GameObject_ToLayer(m_pOwner->Get_CurLevelID(), TEXT("Layer_FX"), m_pFX);
		CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(m_pFX, SECTION_2D_PLAYMAP_EFFECT);
		break;
	}
	case Engine::COORDINATE_3D:
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_ITEM_GET_NEWRIG);
		break;

	default:
		break;
	}
}

void CPlayerState_GetItem::Exit()
{

}

void CPlayerState_GetItem::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	switch (_eCoord)
	{
	case Engine::COORDINATE_2D:
		if (iAnimIdx == (_uint)CPlayer::ANIM_STATE_2D::PLAYER_ITEM_RETRIEVE)
		{

		}
		break;
	default:
		break;
	}
}
