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

CPlayerState_GetItem::CPlayerState_GetItem(CPlayer* _pOwner, PLAYER_2D_ITEM_ID _eItemID)
	:CPlayerState(_pOwner, CPlayer::GET_ITEM)
	, m_eItemID (_eItemID)
{

}

void CPlayerState_GetItem::Update(_float _fTimeDelta)
{
	if (KEY_DOWN(KEY::ENTER))
	{
		m_pFX->Switch_Animation(2);


	}
	if (2 == m_pFX->Get_CurrentAnimIndex())
	{
		if( 0.5f <= m_pFX->Get_CurrentAnimationProgress())
			Event_DeleteObject(m_pItemImg);

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
		XMStoreFloat3( &tModelDesc.tTransform2DDesc.vInitialPosition, m_pOwner->Get_FinalPosition() + m_vItemOffset);
		m_pFX = static_cast<CModelObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &tModelDesc));
		m_pGameInstance->Add_GameObject_ToLayer(m_pOwner->Get_CurLevelID(), TEXT("Layer_FX"), m_pFX);
		CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(m_pFX, SECTION_2D_PLAYMAP_TRIGGER);
		m_pFX->Register_OnAnimEndCallBack(bind (& CPlayerState_GetItem::On_FXAnimEnd, this, placeholders::_1, placeholders::_2));
		m_pFX->Switch_Animation(0);

		wstring wstrItemImg = L"";
		switch (m_eItemID)
		{
		case Client::PLAYER_2D_ITEM_ID::FATHER_BODY:
			wstrItemImg = TEXT("FatherParts_Prop_Body");
			break;
		case Client::PLAYER_2D_ITEM_ID::FATHER_WING:
			wstrItemImg = TEXT("FatherParts_Prop_Wing");
			break;
		case Client::PLAYER_2D_ITEM_ID::FATER_HEAD:
			wstrItemImg = TEXT("FatherParts_Prop_Head");
			break;

		default:
			return;
		}
		tModelDesc.Build_2D_Model(m_pOwner->Get_CurLevelID(), wstrItemImg, TEXT("Prototype_Component_Shader_VtxPosTex"), (_uint)PASS_VTXPOSTEX::DEFAULT, false);
		tModelDesc.tTransform2DDesc.vInitialScaling = {300.f,300.f,300.f};
		m_pItemImg = static_cast<CModelObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &tModelDesc));
		m_pGameInstance->Add_GameObject_ToLayer(m_pOwner->Get_CurLevelID(), TEXT("Layer_FX"), m_pItemImg);
		CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(m_pItemImg, SECTION_2D_PLAYMAP_EFFECT);

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

void CPlayerState_GetItem::On_AnimEnd(COORDINATE _eCoord, _uint _iAnimIdx)
{
	switch (_eCoord)
	{
	case Engine::COORDINATE_2D:
		if (_iAnimIdx == (_uint)CPlayer::ANIM_STATE_2D::PLAYER_ITEM_RETRIEVE)
		{

		}
		break;
	default:
		break;
	}
}

void CPlayerState_GetItem::On_FXAnimEnd(COORDINATE _eCoord, _uint _iAnimIdx)
{
	if (0 == _iAnimIdx)
	{
		m_pFX->Switch_Animation(1);
	}
	else if (2 == _iAnimIdx)
	{
		Event_DeleteObject(m_pFX);

		m_pOwner->Set_2DDirection(E_DIRECTION::RIGHT);
		m_pOwner->Set_State(CPlayer::IDLE);
		return;
	}
}
