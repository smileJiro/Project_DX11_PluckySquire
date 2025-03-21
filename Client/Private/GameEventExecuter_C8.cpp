#include "stdafx.h"
#include "GameEventExecuter_C8.h"
#include "Trigger_Manager.h"
#include "Dialog_Manager.h"
#include "Friend_Controller.h"

#include "Camera_Manager.h"
#include "Camera_Target.h"
#include "Camera_2D.h"
#include "Section_2D_PlayMap.h"
#include "Section_Manager.h"
#include "Player.h"
#include "GameInstance.h"
#include "Beetle.h"
#include "Blocker.h"
#include "Big_Laser.h"
#include "PrintFloorWord.h"
#include "Friend_Pip.h"
#include "Spear_Soldier.h"
#include "Beetle.h"
#include "Friend_Violet.h"
#include "Friend_Thrash.h"

#include "Camera_CutScene.h"
#include "Effect2D_Manager.h"
#include "2DMapActionObject.h"
#include "Room_Door.h"
#include "Book.h"
#include "PlayerData_Manager.h"

#include "PlayerBody.h"
#include "DraggableObject.h"
#include "UI_Manager.h"
#include "Gear.h"
#include "Portal.h"

#include "Dialog_Manager.h"
#include "Npc_Humgrump.h"
#include "Pooling_Manager.h"
#include "Bomb.h"

#include "Logo_ColorObject.h"
#include "Formation_Manager.h"
#include "Event_Manager.h"
#include "Zip_C8.h"
#include "Dialog_Manager.h"

#include "ButterGrump.h"

CGameEventExecuter_C8::CGameEventExecuter_C8(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CGameEventExecuter(_pDevice, _pContext)
{
}


HRESULT CGameEventExecuter_C8::Initialize(void* _pArg)
{
	m_iCurLevelID = LEVEL_CHAPTER_8;
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	// 초기 설정 
	switch ((CTrigger_Manager::EVENT_EXECUTER_ACTION_TYPE)m_iEventExcuterAction)
	{
	case Client::CTrigger_Manager::C02P0910_LIGHTNING_BOLT_SPAWN:
		break;
	default:
		break;
	}

	return S_OK;
}

void CGameEventExecuter_C8::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);

}

void CGameEventExecuter_C8::Update(_float _fTimeDelta)
{
	if (!Is_Dead())
	{
		switch ((CTrigger_Manager::EVENT_EXECUTER_ACTION_TYPE)m_iEventExcuterAction)
		{
		case Client::CTrigger_Manager::CHAPTER8_LASER_STAGE:
			Chapter8_Laser_Stage(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER8_LASER_STAGE_2:
			Chapter8_Laser_Stage_2(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER8_FRIEND_APPEAR_VIOLET:
			Chapter8_Friend_Appear_Violet(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER8_FRIEND_APPEAR_THRASH:
			Chapter8_Friend_Appear_Thrash(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER8_INTRO:
			Chapter8_Intro(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER8_3D_OUT_01:
			Chapter8_3D_Out_01(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER8_3D_OUT_02:
			Chapter8_3D_Out_02(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER8_MAP_INTRO:
			Chapter8_Map_Intro(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER8_INTRO_POSTIT_SEQUENCE:
			Chapter8_Intro_Postit_Sequence(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER8_OUTRO_POSTIT_SEQUENCE:
			Chapter8_Outro_Postit_Sequence(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER8_SWORD:
			Chapter8_Sword(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER8_STOP_STAMP:
			Chapter8_Stop_Stamp(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER8_BOMB_STAMP:
			Chapter8_Bomb_Stamp(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER8_TILTING_GLOVE:
			Chapter8_Tilting_Glove(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::START_TRAIN:
			Start_Train(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER8_MEET_HUMGRUMP:
			Chapter8_Meet_Humgrump(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER8_POSTIT_ARM_CHANGING:
			Chapter8_Postit_Arm_Changing(_fTimeDelta);
			break;		
		case Client::CTrigger_Manager::CHAPTER8_BOOKDROP:
			Chapter8_BookDrop(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER8_BOOKFREEZING_OFF:
			Chapter8_BookFreezing_Off(_fTimeDelta);
			break;		
		case Client::CTrigger_Manager::CHAPTER8_2D_IN:
			Chapter8_2D_In(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER8_BOSS_INTRO:
			Chapter8_Boss_Intro(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER8_GOING_TO_BOSS:
			Chapter8_Going_To_Boss(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER8_TRANSFORMZIP:
			Chapter8_TransformZip(_fTimeDelta);
			break;
		default:
			break;
		}
	}

}

void CGameEventExecuter_C8::Late_Update(_float _fTimeDelta)
{
}

void CGameEventExecuter_C8::Chapter8_Laser_Stage(_float _fTimeDelta)
{
	m_fTimer += _fTimeDelta;


	// INDEX : 0 -> 레이저, 1-> 다리, 2->블로커
	enum LASER_STAGE {
		LASER,
		BRIDGE,
		BLOCKER,
		PIP,
		LAST
	};


	CPlayer* pPlayer = Get_Player();
	if (pPlayer == nullptr)
		return;

	if (Is_Dead())
		return;


#pragma region 죽었을때 처리 
	if (pPlayer->Get_CurrentStateID() == CPlayer::DIE)
	{
		if (false == m_isPlayerFirstDead)
		{
			m_isPlayerFirstDead = true;
			pPlayer->Set_BlockPlayerInput(true);
			Init_Step();
		}
		else if (Step_Check(STEP_0))
		{
			Next_Step_Over(1.f);
		}
		else if (Step_Check(STEP_1))
		{
			if (Is_Start())
				CCamera_Manager::GetInstance()->Start_FadeOut();
			else
				Next_Step_Over(1.5f);
		}
		else if (Step_Check(STEP_2))
		{
			if (Is_Start())
			{

				for (_uint i = 0; i < LAST; i++)
				{
					// 원복시켜놓자. 
					// 다리는 지우지말까?
					if (i != BRIDGE)
						Event_DeleteObject(m_TargetObjects[i]);
					else
						m_TargetObjects[BRIDGE]->Set_Position({ 902.f,-107.f });
					m_TargetObjects[i] = nullptr;
				}


				_vector vPos = XMVectorSet(-808.1f, 192.f, 0.f, 0.f);
				pPlayer->Set_Position(vPos);
				pPlayer->Revive();
				CCamera_Manager::GetInstance()->Start_FadeIn();


				CTriggerObject::TRIGGEROBJECT_DESC Desc = {};
				Desc.vHalfExtents = { 100.f, 120.0f, 0.f };
				Desc.iTriggerType = (_uint)TRIGGER_TYPE::EVENT_TRIGGER;
				Desc.szEventTag = TEXT("Chapter8_Laser_Stage");
				Desc.eConditionType = CTriggerObject::TRIGGER_ENTER;
				Desc.isReusable = false;
				Desc.eStartCoord = COORDINATE_2D;
				Desc.tTransform2DDesc.vInitialPosition = { -806.f,-180.f, 0.f };

				CSection* pBookSection = CSection_Manager::GetInstance()->Find_Section(TEXT("Chapter8_P1112"));
				CTrigger_Manager::GetInstance()->Create_TriggerObject(LEVEL_STATIC, LEVEL_CHAPTER_8, &Desc, pBookSection);

				pPlayer->Set_BlockPlayerInput(false);
				GameEvent_End();
			}
		}
	}
#pragma endregion
	else
	{

		// 스텝 1. 레이저 생성, 맵오브젝트들 배치.
		if (Step_Check(STEP_0))
		{
			Next_Step_Over(0.5f);
		}
		else if (Step_Check(STEP_1))
		{
			if (Is_Start())
			{

				m_TargetObjects.resize(LAST);

				// 플레이어 인풋 막기.
				pPlayer->Set_BlockPlayerInput(true);
				CGameObject* pGameObject = nullptr;

				//레이저 생성
				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_iCurLevelID, TEXT("Prototype_GameObject_Big_Laser"), m_iCurLevelID, L"Layer_MapGimmik", &pGameObject)))
					return;
				else
					m_TargetObjects[LASER] = pGameObject;
				if (FAILED(CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(SECTION_MGR->Get_Cur_Section_Key(), pGameObject)))
					return;

				// 다리 찾기.
				CSection_2D* pSection = static_cast<CSection_2D*>(SECTION_MGR->Find_Section(SECTION_MGR->Get_Cur_Section_Key()));
				auto pLayer = pSection->Get_Section_Layer(SECTION_PLAYMAP_2D_RENDERGROUP::SECTION_2D_PLAYMAP_BACKGROUND);
				const auto& Objects = pLayer->Get_GameObjects();

				for (auto iter = Objects.begin(); iter != Objects.end(); )
				{
					auto pActionObj = dynamic_cast<C2DMapActionObject*>(*iter);

					if (nullptr != pActionObj)
					{
						//if (C2DMapActionObject::ACTIVE_TYPE_DYNAMIC_BACKGROUND == pActionObj->Get_ActionType())
						//{
						//	// 어차피 포탈이 없는 맵이고, 다리에 대한 소팅이 필요하기때문에 한단 위로 올려준다.
						//	SECTION_MGR->Change_GameObject_LayerIndex(pActionObj->Get_Include_Section_Name(), pActionObj, SECTION_2D_PLAYMAP_PORTAL);
						//	continue;
						//}
						//else 
						if (C2DMapActionObject::ACTIVETYPE_MOVING_BRIDGE == pActionObj->Get_ActionType())
							// 다리 찾아서 세팅.
							m_TargetObjects[BRIDGE] = pActionObj;
					}
					iter++;
				}
				// 다리 속도 설정.
				m_TargetObjects[BRIDGE]->Get_ControllerTransform()->Set_SpeedPerSec(450.f);

				// 다리 막는 블로커 세팅. 
				CBlocker::BLOCKER2D_DESC Desc = {};
				Desc.iCurLevelID = (LEVEL_ID)CSection_Manager::GetInstance()->Get_SectionLeveID();
				Desc.vColliderExtents = { 1.f, 1.f };
				Desc.vColliderScale = { 30.f, 132.6f };
				Desc.Build_2D_Transform({ 238.f, -180.f });
				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC,
					TEXT("Prototype_GameObject_Blocker2D"), Desc.iCurLevelID, L"Layer_Blocker", &pGameObject, &Desc)))
				{
					assert(nullptr);
					return;
				}
				else
				{
					SECTION_MGR->Add_GameObject_ToCurSectionLayer(pGameObject, SECTION_2D_PLAYMAP_TRIGGER);
					m_TargetObjects[BLOCKER] = pGameObject;
				}

				//핍 생성 .
				_wstring  strFriendTag = L"Pip";
				CFriend_Pip::FRIEND_DESC PipDesc{};
				PipDesc.Build_2D_Transform(_float2(600.00f, 145.00f), _float2(1.0f, 1.0f), 400.f);
				PipDesc.iCurLevelID = m_iCurLevelID;
				PipDesc.eStartState = CFriend::FRIEND_LAST;
				PipDesc.eStartDirection = CFriend::DIR_LEFT;
				PipDesc.iModelTagLevelID = LEVEL_STATIC;
				PipDesc.iNumDialoguesIndices = 0;
				PipDesc.strFightLayerTag = TEXT("Layer_Monster");



				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Friend_Pip"), m_iCurLevelID, L"Layer_Friend", &pGameObject, &PipDesc)))
					return;

				if (FAILED(CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(SECTION_MGR->Get_Cur_Section_Key(), pGameObject)))
					return;
				m_TargetObjects[PIP] = pGameObject;
				m_TargetObjects[PIP]->Set_Active(false);


				//CFriend_Controller::GetInstance()->Register_Friend(strFriendTag, static_cast<CFriend*>(pGameObject));
				// 그냥 생성하자마자 바로 조금 옆으로움직여주자.
				static_cast<CBig_Laser*>(m_TargetObjects[LASER])->Move_Start(300.f, 200.f);

			}

			Next_Step(false == static_cast<CBig_Laser*>(m_TargetObjects[LASER])->Is_Move());
		}
		// 다 옆으로 움직였으면, 켜자.
		else if (Step_Check(STEP_2))
		{
			if (Is_Start())
			{
				static_cast<CModelObject*>(m_TargetObjects[LASER])->Switch_Animation(CBig_Laser::LASER_START);
				START_SFX(L"A_sfx_laser_igniting", 60.f, false);
				START_SFX(L"A_sfx_laser_shooting_loop", 60.f, true);
			}
			Next_Step_Over(1.5f);
		}
		// 움직이기 시작한다. 플레이어 무빙을 해제한다.
		else if (Step_Check(STEP_3))
		{
			if (Is_Start())
			{
				START_BGM(L"LCD_MUS_C09_P1718_LASER_FULL", 20.f);
				static_cast<CBig_Laser*>(m_TargetObjects[LASER])->Set_Beam_Collider(true);
				static_cast<CBig_Laser*>(m_TargetObjects[LASER])->Move_Start(1800.f, 100.f);
				pPlayer->Set_BlockPlayerInput(false);
			}
			Next_Step_Over(4.5f);
		}
		// 매혹적
		else if (Step_Check(STEP_4))
		{
			if (Is_Start())
			{
				auto pLayer = m_pGameInstance->Find_Layer(m_iCurLevelID, L"Layer_FloorWord");
				if (nullptr != pLayer)
				{
					auto GameObjects = pLayer->Get_GameObjects();
					if (false == GameObjects.empty())
					{
						for (auto pWord : GameObjects)
						{
							auto pFloorWord = dynamic_cast<CPrintFloorWord*>(pWord);
							if (nullptr != pFloorWord)
							{
								if (pPlayer->Get_Include_Section_Name() == pFloorWord->Get_Include_Section_Name())
									pFloorWord->Start_FloorWord();
							}
						}
					}
				}
			}

			Next_Step_Over(3.4f);
		}
		// 찍찍이가 구해주는거. 
		else if (Step_Check(STEP_5))
		{
			if (Is_Start())
			{
				m_TargetObjects[PIP]->Set_Active(true);
				static_cast<CFriend*>(m_TargetObjects[PIP])->Change_AnyState(CFriend_Pip::PIP_C09_JUMPUP, false, CFriend::DIR_LEFT);
			}
			Next_Step_Over(1.5f);
		}
		else if (Step_Check(STEP_6))
		{
			if (Is_Start())
			{
				static_cast<CFriend*>(m_TargetObjects[PIP])->Change_AnyState(CFriend_Pip::PIP_BUTTONPUSH, false, CFriend::DIR_LEFT);
			}
			if (Next_Step_Over(1.f))
			{
			};
		}
		// 다리 움직임.
		else if (Step_Check(STEP_7))
		{
			_vector vTargetPos = m_TargetObjects[BRIDGE]->Get_FinalPosition();
			vTargetPos = XMVectorSetX(vTargetPos, 452.0f);

			if (Is_Start())
			{
				START_SFX(L"A_sfx_platform_appear", 50.f, false);
				START_SFX(L"A_sfx_platform_extend", 50.f, true);

			}
			_vector vDir = XMVectorSetW(XMVector2Normalize(vTargetPos - m_TargetObjects[BRIDGE]->Get_FinalPosition()), 0.f);
			m_TargetObjects[BRIDGE]->Get_ControllerTransform()->Go_Direction(vDir, _fTimeDelta);
			//static_cast<CModelObject*>(m_TargetObjects[BRIDGE])->Get_ControllerTransform()->MoveTo(vTargetPos,_fTimeDelta);


			if (Next_Step(m_TargetObjects[BRIDGE]->Get_ControllerTransform()->Compute_Distance(vTargetPos) < 10.f))
			{
				START_SFX(L"A_sfx_metal_pistons", 50.f, false);
				STOP_SFX(L"A_sfx_platform_extend");
			}
		}
		else if (Step_Check(STEP_8))
		{
			if (Is_Start())
			{
				Event_DeleteObject(m_TargetObjects[BLOCKER]);
				m_TargetObjects[BLOCKER] = nullptr;
			}

			if (1.5f < m_fTimer && true == m_TargetObjects[PIP]->Is_Active() && false == m_isPlag)
			{
				m_TargetObjects[PIP]->Set_Active(false);
				m_isPlag = true;
			}

			Next_Step(false == static_cast<CBig_Laser*>(m_TargetObjects[LASER])->Is_Move());
		}
		else
		{
			GameEvent_End();
		}

	}



}

void CGameEventExecuter_C8::Chapter8_Laser_Stage_2(_float _fTimeDelta)
{
	m_fTimer += _fTimeDelta;

	CPlayer* pPlayer = Get_Player();

	if (Step_Check(STEP_0))
	{

		if (Is_Start())
		{
			pPlayer->Set_BlockPlayerInput(true);


			auto pFriend = m_pGameInstance->Get_GameObject_Ptr(m_iCurLevelID, L"Layer_Friend", 0);
			assert(pFriend);


			m_TargetObjects.push_back(pFriend);
			pFriend->Set_Active(true);
			pFriend->Set_Position(XMVectorSet(851.f, 130.f, 0.f, 0.f));
			static_cast<CFriend_Pip*>(pFriend)->Change_CurState(CFriend::FRIEND_IDLE);
			_vector vPlayerPos = pPlayer->Get_FinalPosition();
			static_cast<CFriend_Pip*>(pFriend)->Move_Position(_float2(XMVectorGetX(vPlayerPos), 96.0f), CFriend::DIR_DOWN);
		}

		Next_Step_Over(1.f);
	}
	else if (Step_Check(STEP_1))
	{
		if (Is_Start())
		{
			START_BGM(L"LCD_MUS_C09_P1718_REUNITEDWITHPIP_Stem_Base", 20.f);
			static_cast<CFriend*>(m_TargetObjects[0])->Change_AnyState(CFriend_Pip::PIP_EXCITED_DOWN, false, CFriend::DIR_DOWN);
		}

		Next_Step_Over(1.f);
	}
	else if (Step_Check(STEP_2))
	{
		if (Is_Start())
		{
			CDialog_Manager::GetInstance()->Set_NPC(m_TargetObjects[0]);
			CDialog_Manager::GetInstance()->Set_DialogId(L"Chapter8_Laser_Stage_2");
		}
		else
			if (Next_Step(!CDialog_Manager::GetInstance()->Get_DisPlayDialogue()))
				pPlayer->Set_BlockPlayerInput(true);
	}
	else
	{
		CFriend_Controller::GetInstance()->Register_Friend(TEXT("Pip"), (CFriend*)m_TargetObjects[0]);
		CFriend_Controller::GetInstance()->Register_Friend_ToTrainList(TEXT("Pip"));

		CFriend_Controller::GetInstance()->Start_Train();

		pPlayer->Set_BlockPlayerInput(false);

		GameEvent_End();
	}

}

void CGameEventExecuter_C8::Chapter8_Friend_Appear_Violet(_float _fTimeDelta)
{
	enum APPEAR_VIOLET_STAGE {
		PIP,
		VIOLET,
		HAT,
		LAST
	};
	m_fTimer += _fTimeDelta;

	CPlayer* pPlayer = Get_Player();
	if (Step_Check(STEP_0))
	{
		if (Is_Start())
		{
			//
			START_SFX(L"A_sfx_platform_extend", 40.f, true);
			m_TargetObjects.resize(LAST);

			pPlayer->Set_BlockPlayerInput(true);
			CFriend_Controller::GetInstance()->End_Train();

			// 뱌올렛 생성
			_wstring  strFriendTag = L"Violet";

			CGameObject* pGameObject = nullptr;

			CFriend_Violet::FRIEND_DESC VioletDesc{};
			VioletDesc.Build_2D_Transform(_float2(-265.00, 30.00f), _float2(1.0f, 1.0f), 400.f);
			VioletDesc.iCurLevelID = m_iCurLevelID;
			VioletDesc.eStartState = CFriend::FRIEND_LAST;
			VioletDesc.eStartDirection = CFriend::DIR_RIGHT;
			VioletDesc.iModelTagLevelID = LEVEL_STATIC;
			VioletDesc.iNumDialoguesIndices = 0;
			VioletDesc.strFightLayerTag = TEXT("Layer_Monster");

			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC,
				TEXT("Prototype_GameObject_Friend_Violet"), m_iCurLevelID,
				L"Layer_Friend", &pGameObject, &VioletDesc)))
				return;

			if (FAILED(CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(
				L"Chapter8_P1718", pGameObject)))
				return;
			m_TargetObjects[VIOLET] = pGameObject;



			CModelObject::MODELOBJECT_DESC Desc = {};

			Desc.Build_2D_Model(m_iCurLevelID, L"Violet_Hat", L"Prototype_Component_Shader_VtxPosTex");
			Desc.iModelPrototypeLevelID_2D = LEVEL_STATIC;
			Desc.Build_2D_Transform({ -272.f,20.f });
			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC,
				TEXT("Prototype_GameObject_ModelObject"), m_iCurLevelID,
				L"Layer_Friend", &pGameObject, &Desc)))
				return;

			if (FAILED(CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(
				L"Chapter8_P1718", pGameObject)))
				return;
			m_TargetObjects[HAT] = pGameObject;



			static_cast<CFriend*>(m_TargetObjects[VIOLET])->Change_AnyState(CFriend_Violet::VIOLET_C09_LYINGDESPONDENT, true, CFriend::DIR_RIGHT);


			m_TargetObjects[PIP] = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Pip"));
		}
		Next_Step_Over(1.f);
	}
	else if (Step_Check(STEP_1))
	{
		if (Is_Start())
		{
			if (m_TargetObjects[PIP] != nullptr)
			{
				static_cast<CFriend_Pip*>(m_TargetObjects[PIP])->Move_Position(_float2(-185.f, -8.f), CFriend::DIR_LEFT);
			}
			pPlayer->Set_2DDirection(F_DIRECTION::LEFT);
			AUTOMOVE_COMMAND AutoMove{};
			AutoMove.eType = AUTOMOVE_TYPE::MOVE_TO;
			AutoMove.fPostDelayTime = 0.0f;
			AutoMove.fPreDelayTime = 0.0f;
			AutoMove.fMoveSpeedMag = 2.f;
			AutoMove.iAnimIndex = (_uint)CPlayer::ANIM_STATE_2D::PLAYER_RUN_RIGHT;
			AutoMove.vTarget = XMVectorSet(-175.f, 33.f, 0.f, 0.f);

			AUTOMOVE_COMMAND AutoMove2{};
			AutoMove2.eType = AUTOMOVE_TYPE::LOOK_DIRECTION;
			AutoMove2.fPostDelayTime = 0.0f;
			AutoMove2.fPreDelayTime = 0.0f;
			AutoMove2.vTarget = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
			AutoMove2.iAnimIndex = (_uint)CPlayer::ANIM_STATE_2D::PLAYER_IDLE_RIGHT;

			pPlayer->Add_AutoMoveCommand(AutoMove);
			pPlayer->Add_AutoMoveCommand(AutoMove2);
			pPlayer->Start_AutoMove(true);
		}

		Next_Step_Over(2.5f);
	}
	else if (Step_Check(STEP_2))
	{
		if (Is_Start())
		{
			START_SFX(L"A_sfx_violet_wake_up", 40.f, false);
			static_cast<CFriend*>(m_TargetObjects[VIOLET])->Change_AnyState(CFriend_Violet::VIOLET_C09_LYINGTOSITTINGUP, false, CFriend::DIR_RIGHT);
		}

		Next_Step_Over(3.5f);
	}
	else if (Step_Check(STEP_3))
	{
		if (Is_Start())
		{
			CDialog_Manager::GetInstance()->Set_NPC(m_TargetObjects[VIOLET]);
			CDialog_Manager::GetInstance()->Set_NPC(m_TargetObjects[PIP]);
			CDialog_Manager::GetInstance()->Set_DialogId(L"Chapter8_Friend_Appear_Violet_01");
		}
		else
			if (Next_Step(!CDialog_Manager::GetInstance()->Get_DisPlayDialogue()))
				pPlayer->Set_BlockPlayerInput(true);
	}
	else if (Step_Check(STEP_4))
	{
		if (Is_Start())
		{
			START_SFX(L"A_sfx_violet_jumping_off_bed", 40.f, false);
			static_cast<CFriend*>(m_TargetObjects[VIOLET])->Change_AnyState(CFriend_Violet::VIOLET_C09_JUMPINGOFFBED, false, CFriend::DIR_RIGHT);
		}

		if (nullptr != m_TargetObjects[HAT])
		{
			_float fProgress = static_cast<CContainerObject*>(m_TargetObjects[VIOLET])->Get_Part_AnimProgress(CContainerObject::PART_BODY);

				if (0.663f < fProgress)
				{
					Event_DeleteObject(m_TargetObjects[HAT]);
					m_TargetObjects[HAT] = nullptr;
				}
		}

		Next_Step_Over(8.f);
	}
	else if (Step_Check(STEP_5))
	{
		if (Is_Start())
		{
			CDialog_Manager::GetInstance()->Set_NPC(m_TargetObjects[VIOLET]);
			CDialog_Manager::GetInstance()->Set_NPC(m_TargetObjects[PIP]);
			CDialog_Manager::GetInstance()->Set_DialogId(L"Chapter8_Friend_Appear_Violet_02");
		}
		else
			if (Next_Step(!CDialog_Manager::GetInstance()->Get_DisPlayDialogue()))
				pPlayer->Set_BlockPlayerInput(true);
		if (KEY_DOWN(KEY::Q))
		{
			static_cast<CFriend*>(m_TargetObjects[VIOLET])->Change_AnyState(CFriend_Violet::VIOLET_C09_JUMPINGOFFBED, false, CFriend::DIR_RIGHT);
		}
	}
	else
	{
		CFriend_Controller::GetInstance()->Register_Friend(TEXT("Violet"), (CFriend*)m_TargetObjects[VIOLET]);
		CFriend_Controller::GetInstance()->Register_Friend_ToTrainList(TEXT("Violet"));

		CFriend_Controller::GetInstance()->Start_Train();
		pPlayer->Set_BlockPlayerInput(false);
		GameEvent_End();
	}
}

void CGameEventExecuter_C8::Chapter8_Friend_Appear_Thrash(_float _fTimeDelta)
{
	enum APPEAR_VIOLET_STAGE {
		PIP,
		VIOLET,
		THRASH,
		LAST
	};
	m_fTimer += _fTimeDelta;

	CPlayer* pPlayer = Get_Player();
	if (Step_Check(STEP_0))
	{
		if (Is_Start())
		{
			pPlayer->Set_BlockPlayerInput(true);
			CFriend_Controller::GetInstance()->End_Train();

			m_TargetObjects.resize(LAST);

			// 쓰래시 생성 생성
			_wstring  strFriendTag = L"Thrash";

			CGameObject* pGameObject = nullptr;

			CFriend_Thrash::FRIEND_DESC ThrashDesc{};
			ThrashDesc.Build_2D_Transform(_float2(351.0f, -21.0f), _float2(1.0f, 1.0f), 400.f);
			ThrashDesc.iCurLevelID = m_iCurLevelID;
			ThrashDesc.eStartState = CFriend::FRIEND_LAST;
			ThrashDesc.eStartDirection = CFriend::DIR_LEFT;
			ThrashDesc.iModelTagLevelID = LEVEL_STATIC;
			ThrashDesc.iNumDialoguesIndices = 0;
			ThrashDesc.strFightLayerTag = TEXT("Layer_Monster");

			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC,
				TEXT("Prototype_GameObject_Friend_Thrash"), m_iCurLevelID,
				L"Layer_Friend", &pGameObject, &ThrashDesc)))
				return;

			if (FAILED(CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(
				L"Chapter8_P1718", pGameObject)))
				return;

			m_TargetObjects[THRASH] = pGameObject;
			m_TargetObjects[PIP] = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Pip"));
			m_TargetObjects[VIOLET] = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Violet"));

			static_cast<CFriend*>(m_TargetObjects[THRASH])->Change_AnyState(CFriend_Thrash::THRASH_C09_TAPPING, true, CFriend::DIR_UP);

		}
		Next_Step_Over(0.8f);
	}
	else if (Step_Check(STEP_1))
	{
		if (Is_Start())
		{
			if (m_TargetObjects[PIP] != nullptr)
			{
				static_cast<CFriend*>(m_TargetObjects[PIP])->Move_Position(_float2(235.0f, -71.0f), CFriend::DIR_RIGHT);
			}

			if (m_TargetObjects[VIOLET] != nullptr)
			{
				static_cast<CFriend*>(m_TargetObjects[VIOLET])->Move_Position(_float2(235.0f, 61.0f), CFriend::DIR_RIGHT);
			}
			START_BGM(L"LCD_MUS_C09_THRASHREUNITED_P5152_LOOP_Stem_Base", 20.f);

			AUTOMOVE_COMMAND AutoMove{};
			AutoMove.eType = AUTOMOVE_TYPE::MOVE_TO;
			AutoMove.fPostDelayTime = 0.0f;
			AutoMove.fPreDelayTime = 0.0f;
			AutoMove.fMoveSpeedMag = 2.f;
			AutoMove.iAnimIndex = (_uint)CPlayer::ANIM_STATE_2D::PLAYER_RUN_RIGHT;
			AutoMove.vTarget = XMVectorSet(275.0f, -17.0f, 0.f, 0.f);

			AUTOMOVE_COMMAND AutoMove2{};
			AutoMove2.eType = AUTOMOVE_TYPE::LOOK_DIRECTION;
			AutoMove2.fPostDelayTime = 0.0f;
			AutoMove2.fPreDelayTime = 0.0f;
			AutoMove2.vTarget = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
			AutoMove2.iAnimIndex = (_uint)CPlayer::ANIM_STATE_2D::PLAYER_IDLE_RIGHT;

			pPlayer->Add_AutoMoveCommand(AutoMove);
			pPlayer->Add_AutoMoveCommand(AutoMove2);
			pPlayer->Start_AutoMove(true);
		}

		Next_Step_Over(2.5f);
	}
	else if (Step_Check(STEP_2))
	{
		if (Is_Start())
		{
			CDialog_Manager::GetInstance()->Set_NPC(m_TargetObjects[VIOLET]);
			CDialog_Manager::GetInstance()->Set_NPC(m_TargetObjects[PIP]);
			CDialog_Manager::GetInstance()->Set_NPC(m_TargetObjects[THRASH]);
			CDialog_Manager::GetInstance()->Set_DialogId(L"Chapter8_Friend_Appear_Thrash_01");
		}
		else
			if (Next_Step(!CDialog_Manager::GetInstance()->Get_DisPlayDialogue()))
				pPlayer->Set_BlockPlayerInput(true);
	}
	else if (Step_Check(STEP_3))
	{
		if (Is_Start())
		{
			START_SFX(L"A_sfx_thrash_jumping_off_pipe", 40.f, false);
			STOP_SFX(L"A_sfx_thrash_tapping_pipe");

			static_cast<CFriend*>(m_TargetObjects[THRASH])->Change_AnyState(CFriend_Thrash::THRASH_C09_JUMPINGOFFPIPE, false, CFriend::DIR_RIGHT);
		}

		Next_Step_Over(4.5f);
	}
	else if (Step_Check(STEP_4))
	{
		if (Is_Start())
		{
			CDialog_Manager::GetInstance()->Set_NPC(m_TargetObjects[VIOLET]);
			CDialog_Manager::GetInstance()->Set_NPC(m_TargetObjects[PIP]);
			CDialog_Manager::GetInstance()->Set_NPC(m_TargetObjects[THRASH]);
			CDialog_Manager::GetInstance()->Set_DialogId(L"Chapter8_Friend_Appear_Thrash_02");
		}
		else
			if (Next_Step(!CDialog_Manager::GetInstance()->Get_DisPlayDialogue()))
				pPlayer->Set_BlockPlayerInput(true);
	}
	else if (Step_Check(STEP_5))
	{
		if (Is_Start())
		{
			pPlayer->Set_State(CPlayer::MOJAM);
			static_cast<CFriend*>(m_TargetObjects[VIOLET])->Change_CurState(CFriend::FRIEND_MOJAM);
			static_cast<CFriend*>(m_TargetObjects[PIP])->Change_CurState(CFriend::FRIEND_MOJAM);
			static_cast<CFriend*>(m_TargetObjects[THRASH])->Change_CurState(CFriend::FRIEND_MOJAM);
		}
		// 모잼?
		else if (!CDialog_Manager::GetInstance()->Get_DisPlayDialogue() && CPlayer::STATE::IDLE == pPlayer->Get_CurrentStateID())
		{
			Next_Step(true);
		}
	}
	else
	{
		CFriend_Controller::GetInstance()->Register_Friend(TEXT("Thrash"), (CFriend*)m_TargetObjects[THRASH]);
		CFriend_Controller::GetInstance()->Register_Friend_ToTrainList(TEXT("Thrash"));

		CFriend_Controller::GetInstance()->Start_Train();
		pPlayer->Set_BlockPlayerInput(false);
		GameEvent_End();
	}
}

void CGameEventExecuter_C8::Chapter8_Intro(_float _fTimeDelta)
{
	m_fTimer += _fTimeDelta;
	if (Step_Check(STEP_0))
	{
		if (Is_Start())
		{
			CCamera_Manager::GetInstance()->Change_CameraType(CCamera_Manager::CUTSCENE);
			CCamera_Manager::GetInstance()->Set_NextCutSceneData(TEXT("Chapter8_Intro"));
		}

		Next_Step_Over(2.8f);
	}
	else if (Step_Check(STEP_1))
	{
		if (Is_Start())
		{
			CRoom_Door* pDoor = static_cast<CRoom_Door*>(m_pGameInstance->Get_GameObject_Ptr(LEVEL_CHAPTER_8, TEXT("Layer_RoomDoor"), 0));
			pDoor->Start_Turn_DoorKnob(true);
		}
		Next_Step_Over(0.5f);
	}
	else if (Step_Check(STEP_2))
	{
		CRoom_Door* pDoor = static_cast<CRoom_Door*>(m_pGameInstance->Get_GameObject_Ptr(LEVEL_CHAPTER_8, TEXT("Layer_RoomDoor"), 0));

		if (Is_Start())
		{
			pDoor->Start_Turn_Door(true);
		}

		if (false == pDoor->Is_Turn_DoorKnob()) {
			pDoor->Start_Turn_DoorKnob(true);
			Next_Step(true);
		}
	}
	else if (Step_Check(STEP_3)) {

		CCamera_CutScene* pCamera = static_cast<CCamera_CutScene*>(CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::CUTSCENE));

		if (true == pCamera->Is_Finish_CutScene()) {
			Next_Step(true);
		}
	}
	else if (Step_Check(STEP_4)) {

		if (m_fTimer >= 3.f) {
			CCamera_Manager::GetInstance()->Change_CameraType(CCamera_Manager::TARGET_2D, true, 0.5f);
			Uimgr->Set_PlayNarration(TEXT("Chapter8_P0102_Narration_01"));
			GameEvent_End();
		}
	}
}

void CGameEventExecuter_C8::Chapter8_Map_Intro(_float _fTimeDelta)
{
	m_fTimer += _fTimeDelta;

	if (Step_Check(STEP_0))
	{
		CPlayer* pPlayer = Get_Player();

		if (Is_Start()) {
			// 1. Player 움직임 막기
			pPlayer->Set_BlockPlayerInput(true);

			Next_Step(true);

		}

	}
	else if (Step_Check(STEP_1)) {
		// 3. CutScene 재생
		CCamera_Manager::GetInstance()->Set_NextCutSceneData(TEXT("Chapter8_Map_Intro"));
		CCamera_Manager::GetInstance()->Change_CameraType(CCamera_Manager::CUTSCENE, true, 1.8f);
		Next_Step(true);
	}
	else if (Step_Check(STEP_2)) {
		CCamera_CutScene* pCamera = static_cast<CCamera_CutScene*>(CCamera_Manager::GetInstance()->Get_CurrentCamera());
		if (true == pCamera->Is_Finish_CutScene()) {
			Next_Step(true);
		}
	}
	else if (Step_Check(STEP_3)) {
		if (Is_Start()) {
			CCamera_Manager::GetInstance()->Start_FadeOut(0.8f);
		}

		// 4. FadeOut이 끝난 후 CutScene Camera -> Target Camera로 전환
		if (m_fTimer > 0.8f) {
			static_cast<CCamera_CutScene*>(CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::CUTSCENE))->Set_Pause_After_CutScene(false);
			Next_Step(true);
		}
	}
	// 5. 전환 후 Target Camera로(다음 프레임) FadeIn 시작 + 기존 CutScene Camera를 다시 밝게 만들기
	else if (Step_Check(STEP_4)) {
		if (Is_Start()) {
			CCamera_Manager::GetInstance()->Start_FadeIn(0.7f);
			CCamera_Manager::GetInstance()->Set_FadeRatio(CCamera_Manager::CUTSCENE, 1.f, true);

			CPlayer* pPlayer = Get_Player();

			// 6. Player 움직임 풀기
			pPlayer->Set_BlockPlayerInput(false);

			GameEvent_End();
		}
	}
}

void CGameEventExecuter_C8::Chapter8_Intro_Postit_Sequence(_float _fTimeDelta)
{
	m_fTimer += _fTimeDelta;

	function fCamerafunc = []()
		{
			// <-으로 돌리기. 1초동안
			CCamera_Manager::GetInstance()->Start_Turn_AxisY(CCamera_Manager::TARGET, 1.f, XMConvertToRadians(-40.f), XMConvertToRadians(-25.f));
			// 암 3.f 줄이기, 1초동안
			CCamera_Manager::GetInstance()->Start_Changing_ArmLength_Decrease(CCamera_Manager::TARGET, 1.f,
				3.f, EASE_IN_OUT);
			// 타겟 오프셋 y -2.f
			CCamera_Manager::GetInstance()->Start_Changing_AtOffset(CCamera_Manager::TARGET,
				1.f,
				XMVectorSet(0.f, -2.f, 0.f, 0.f),
				EASE_IN_OUT);
		};
	if (true == Postit_Process(L"Chapter8_SKSP_Postit", L"Chapter8_Intro_Postit_Sequence", 1.f, CPostit_Page::POSTIT_PAGE_POSTION_TYPE_A, true, fCamerafunc))
	{
		GameEvent_End();
	}
}

void CGameEventExecuter_C8::Chapter8_Outro_Postit_Sequence(_float _fTimeDelta)
{
	m_fTimer += _fTimeDelta;
	auto pBook = Get_Book();
	CPlayer* pPlayer = Get_Player();

	if (Step_Check(STEP_0))
	{
		function fCamerafunc = []()
			{
				// <-으로 돌리기. 1초동안
				CCamera_Manager::GetInstance()->Start_Turn_AxisY(CCamera_Manager::TARGET, 1.f, XMConvertToRadians(-40.f), XMConvertToRadians(-25.f));
				// 암 3.f 줄이기, 1초동안
				CCamera_Manager::GetInstance()->Start_Changing_ArmLength_Decrease(CCamera_Manager::TARGET, 1.f,
					3.f, EASE_IN_OUT);
				// 타겟 오프셋 y -2.f
				CCamera_Manager::GetInstance()->Start_Changing_AtOffset(CCamera_Manager::TARGET,
					1.f,
					XMVectorSet(0.f, -2.f, 0.f, 0.f),
					EASE_IN_OUT);
			};
		Postit_Process_Start(L"Chapter8_SKSP_Postit", 1.f, true, fCamerafunc);
	}
	else if (Step_Check(STEP_1))
	{
		Postit_Process_PageAppear();
	}
	else if (Step_Check(STEP_2))
	{
		Postit_Process_PageTalk(L"Chapter8_Outro_Postit_Sequence_01");
	}
	else if (Step_Check(STEP_3))
	{
		//// 암 타겟오프셋 x3 y2 z-3 이동
		if (Is_Start())
		{
			m_tReturnArmData = CCamera_Manager::GetInstance()->Save_ArmData();
			CCamera_Manager::GetInstance()->Start_Turn_AxisY(CCamera_Manager::TARGET, 1.f, 
				XMConvertToRadians(40.f), XMConvertToRadians(25.f));
			CCamera_Manager::GetInstance()->Start_Changing_AtOffset(CCamera_Manager::TARGET,
				1.f,
				XMVectorSet(20.f, 0.f, -5.f, 0.f),
				EASE_IN_OUT);
			CCamera_Manager::GetInstance()->Start_Changing_ArmLength_Increase(CCamera_Manager::TARGET, 1.f,
				20.f, EASE_IN_OUT);
		}
		// 카메라 무브
		Next_Step_Over(1.f);
	}
	else if (Step_Check(STEP_4))
	{
		Postit_Process_PageTalk(L"Chapter8_Outro_Postit_Sequence_02");
	}
	else if (Step_Check(STEP_5))
	{
		if (Is_Start())
		{
			//CCamera_Manager::GetInstance()->Change_CameraTarget(pPlayer);
			CCamera_Manager::GetInstance()->Load_SavedArmData(m_tReturnArmData,1.f);
		}
		// 카메라 무브
		Next_Step_Over(1.f);
	}
	else if (Step_Check(STEP_6))
	{
		Postit_Process_PageTalk(L"Chapter8_Outro_Postit_Sequence_03");
	}
	else if (Step_Check(STEP_7))
	{
		Postit_Process_End(1.f);
	}
	else
		GameEvent_End();
}

void CGameEventExecuter_C8::Chapter8_3D_Out_01(_float _fTimeDelta)
{
	m_fTimer += _fTimeDelta;
	CPlayer* pPlayer = Get_Player();
	CFriend* pThrash = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Thrash"));
	CFriend* pPip = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Pip"));
	CFriend* pViolet = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Violet"));
	if (Step_Check(STEP_0))
	{
		if (Is_Start())
		{
			CFriend_Controller::GetInstance()->End_Train();
			pPlayer->Set_BlockPlayerInput(true);
		
		}
		Next_Step_Over(0.5f);

	}
	else if (Step_Check(STEP_1))
	{
		if (Is_Start())
		{
			pPlayer->Set_2DDirection(F_DIRECTION::UP);
			CFriend_Controller::GetInstance()->End_Train();
			_vector vPlayerPos = pPlayer->Get_FinalPosition() + XMVectorSet(0.0f, 30.f, 0.0f, 0.0f);
			_vector vThrashPos = pThrash->Get_FinalPosition() + XMVectorSet(-60.0f, 30.f, 0.0f, 0.0f);
			_vector vVioletPos = pViolet->Get_FinalPosition() + XMVectorSet(60.0f, 30.f, 0.0f, 0.0f);
			_vector vPipPos = pPip->Get_FinalPosition() + XMVectorSet(-120.0f, 30.f, 0.0f, 0.0f);
			AUTOMOVE_COMMAND AutoMove{};
			AutoMove.eType = AUTOMOVE_TYPE::MOVE_TO;
			AutoMove.fPostDelayTime = 0.0f;
			AutoMove.fPreDelayTime = 0.0f;
			AutoMove.iAnimIndex = (_uint)CPlayer::ANIM_STATE_2D::PLAYER_RUN_UP;
			AutoMove.vTarget = vPlayerPos;

			AUTOMOVE_COMMAND AutoMove2{};
			AutoMove2.eType = AUTOMOVE_TYPE::LOOK_DIRECTION;
			AutoMove2.fPostDelayTime = 0.0f;
			AutoMove2.fPreDelayTime = 0.0f;
			AutoMove2.fMoveSpeedMag = 2.f;
			AutoMove2.vTarget = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
			AutoMove2.iAnimIndex = (_uint)CPlayer::ANIM_STATE_2D::PLAYER_IDLE_UP;

			pPlayer->Add_AutoMoveCommand(AutoMove);
			pPlayer->Add_AutoMoveCommand(AutoMove2);
			pPlayer->Start_AutoMove(true);
			pThrash->Move_Position(_float2(XMVectorGetX(vThrashPos), XMVectorGetY(vThrashPos)), CFriend::DIR_UP);
			pViolet->Move_Position(_float2(XMVectorGetX(vVioletPos), XMVectorGetY(vVioletPos)), CFriend::DIR_UP);
			pPip->Move_Position(_float2(XMVectorGetX(vPipPos), XMVectorGetY(vPipPos)), CFriend::DIR_UP);

		}
		Next_Step_Over(0.5f);
	}	
	else if (Step_Check(STEP_2))
	{
		if (Is_Start())
		{
			CDialog_Manager::GetInstance()->Set_DialogId(L"Chapter8_3D_Out_01_01");
		}
		else
			if (Next_Step(!CDialog_Manager::GetInstance()->Get_DisPlayDialogue()))
				pPlayer->Set_BlockPlayerInput(true);
	}
	else if (Step_Check(STEP_3)) {
		if (Is_Start())
		{
			auto pLayer = m_pGameInstance->Find_Layer(m_iCurLevelID, L"Layer_FloorWord");
			if (nullptr != pLayer)
			{
				auto GameObjects = pLayer->Get_GameObjects();
				if (false == GameObjects.empty())
				{
					for (auto pWord : GameObjects)
					{
						auto pFloorWord = dynamic_cast<CPrintFloorWord*>(pWord);
						if (nullptr != pFloorWord)
						{
							if (pPlayer->Get_Include_Section_Name() == pFloorWord->Get_Include_Section_Name())
								pFloorWord->Start_FloorWord();
						}
					}
				}
			}
		}
		Next_Step_Over(6.f);
	}
	else if (Step_Check(STEP_4)) {
		if (Is_Start())
		{
			CDialog_Manager::GetInstance()->Set_DialogId(L"Chapter8_3D_Out_01_02");
		}
		else
			if (Next_Step(!CDialog_Manager::GetInstance()->Get_DisPlayDialogue()))
				pPlayer->Set_BlockPlayerInput(true);
	}
	// 5. 전환 후 Target Camera로(다음 프레임) FadeIn 시작 + 기존 CutScene Camera를 다시 밝게 만들기
	else 
	{
		// 6. Player 움직임 풀기
		pPlayer->Set_BlockPlayerInput(false);
		CFriend_Controller::GetInstance()->Start_Train();

		GameEvent_End();
	}
}

void CGameEventExecuter_C8::Chapter8_3D_Out_02(_float _fTimeDelta)
{
	m_fTimer += _fTimeDelta;
	CPlayer* pPlayer = Get_Player();
	CFriend* pThrash = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Thrash"));
	CFriend* pPip = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Pip"));
	CFriend* pViolet = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Violet"));

//#ifdef _DEBUG
	if (nullptr == pViolet)
	{
		if (false == Is_Dead())
		{
			if (Change_PlayMap(0.f))
			{
				Get_Book()->Set_Freezing(true);
				GameEvent_End();
			}
		}
		return;
	}
//#endif // _DEBUG


	if (Step_Check(STEP_0))
	{
		if (Is_Start())
		{
			CFriend_Controller::GetInstance()->End_Train();
			pPlayer->Set_BlockPlayerInput(true);

		}
		Next_Step_Over(0.5f);

	}
	else if (Step_Check(STEP_1))
	{
		if (Is_Start())
		{
			CSection* pSection = SECTION_MGR->Find_Section(L"Chapter8_P1920");

			if (nullptr != pSection)
				static_cast<CSection_2D_PlayMap*>(pSection)->Set_PortalActive(true);

			pPlayer->Set_2DDirection(F_DIRECTION::UP);
			CFriend_Controller::GetInstance()->End_Train();
			_vector vPlayerPos = XMVectorSet(-1004.0, 50.f, 0.0f, 0.0f);
			_vector vThrashPos = vPlayerPos + XMVectorSet(-40.0f, -90.f, 0.0f, 0.0f);
			_vector vVioletPos = vPlayerPos + XMVectorSet(40.0f, -90.f, 0.0f, 0.0f);
			_vector vPipPos = vPlayerPos + XMVectorSet(0.0f, -130.f, 0.0f, 0.0f);
			AUTOMOVE_COMMAND AutoMove{};
			AutoMove.eType = AUTOMOVE_TYPE::MOVE_TO;
			AutoMove.fPostDelayTime = 0.0f;
			AutoMove.fPreDelayTime = 0.0f;
			AutoMove.iAnimIndex = (_uint)CPlayer::ANIM_STATE_2D::PLAYER_RUN_UP;
			AutoMove.vTarget = vPlayerPos;

			AUTOMOVE_COMMAND AutoMove2{};
			AutoMove2.eType = AUTOMOVE_TYPE::LOOK_DIRECTION;
			AutoMove2.fPostDelayTime = 0.0f;
			AutoMove2.fPreDelayTime = 0.0f;
			AutoMove2.fMoveSpeedMag = 2.f;
			AutoMove2.vTarget = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
			AutoMove2.iAnimIndex = (_uint)CPlayer::ANIM_STATE_2D::PLAYER_IDLE_UP;
			pPlayer->Add_AutoMoveCommand(AutoMove);
			pPlayer->Add_AutoMoveCommand(AutoMove2);
			pPlayer->Start_AutoMove(true);
			pThrash->Move_Position(_float2(XMVectorGetX(vThrashPos), XMVectorGetY(vThrashPos)), CFriend::DIR_UP);
			pViolet->Move_Position(_float2(XMVectorGetX(vVioletPos), XMVectorGetY(vVioletPos)), CFriend::DIR_UP);
			pPip->Move_Position(_float2(XMVectorGetX(vPipPos), XMVectorGetY(vPipPos)), CFriend::DIR_UP);
		}

		Next_Step_Over(Change_PlayMap(0.f));
	}
	else if (Step_Check(STEP_2))
	{
		if (Is_Start())
		{
			pPlayer->Set_2DDirection(F_DIRECTION::DOWN);

			CDialog_Manager::GetInstance()->Set_DialogId(L"Chapter8_3D_Out_02");
		}
		else
			if (Next_Step(!CDialog_Manager::GetInstance()->Get_DisPlayDialogue()))
				pPlayer->Set_BlockPlayerInput(true);
	}	
	else if (Step_Check(STEP_3))
	{
		if (Is_Start())
		{
			pPlayer->Set_BlockPlayerInput(false);
		}
		else
			if (Next_Step(COORDINATE_3D == pPlayer->Get_CurCoord()))
			{
				int a = 1;
				//TODO :: 애님메시 셰이더 변경 		
			}
	}
	else
	{
		// 6. Player 움직임 풀기
		//CFriend_Controller::GetInstance()->Start_Train();

		GameEvent_End();
	}
}

void CGameEventExecuter_C8::Chapter8_Sword(_float _fTimeDelta)
{
	m_fTimer += _fTimeDelta;

	function fCamerafunc = []()
		{
			CCamera_Manager::GetInstance()->Start_Turn_AxisY(CCamera_Manager::TARGET, 1.f, XMConvertToRadians(60.f), (_uint)EASE_IN_OUT);

			//CCamera_Manager::GetInstance()->Start_Changing_ArmLength_Increase(CCamera_Manager::TARGET, 1.f,
			//	3.f, EASE_IN_OUT);
			CCamera_Manager::GetInstance()->Start_Changing_AtOffset(CCamera_Manager::TARGET,
				1.f,
				XMVectorSet(0.f, -2.5f, 0.f, 0.f),
				EASE_IN_OUT);
		};
	if (true == Postit_Process(L"Chapter8_SKSP_Postit", L"Chapter8_Sword", 1.f, CPostit_Page::POSTIT_PAGE_POSTION_TYPE_A, false, fCamerafunc))
	{
		Get_Player()->Set_Mode(CPlayer::PLAYER_MODE_SWORD);


		/* beetle 전환 */
		auto pLayer = m_pGameInstance->Find_Layer(m_iCurLevelID, TEXT("Layer_Sneak_Beetle"));

		if (nullptr != pLayer)
		{
			const auto& Objects = pLayer->Get_GameObjects();

			for_each(Objects.begin(), Objects.end(), [](CGameObject* pGameObject) {
				auto pObject = dynamic_cast<CBeetle*>(pGameObject);

				if (nullptr != pObject)
				{
					pObject->Switch_CombatMode();
				}
				});
		}
		/**/

		GameEvent_End();
	}
}

void CGameEventExecuter_C8::Chapter8_Stop_Stamp(_float _fTimeDelta)
{
	m_fTimer += _fTimeDelta;

	function fCamerafunc = []()
		{
			// <-으로 돌리기. 1초동안
			CCamera_Manager::GetInstance()->Start_Turn_AxisY(CCamera_Manager::TARGET, 1.f, XMConvertToRadians(-70.f), (_uint)EASE_IN_OUT);
			CCamera_Manager::GetInstance()->Start_Changing_ArmLength_Increase(CCamera_Manager::TARGET, 1.f,
				3.f, EASE_IN_OUT);
			CCamera_Manager::GetInstance()->Start_Changing_AtOffset(CCamera_Manager::TARGET,
				1.f,
				XMVectorSet(0.f, -2.f, 0.f, 0.f),
				EASE_IN_OUT);
		};
	if (true == Postit_Process(L"Chapter8_SKSP_Postit", L"Chapter8_Stop_Stamp", 1.f, CPostit_Page::POSTIT_PAGE_POSTION_TYPE_A, false, fCamerafunc))
	{
		GameEvent_End();
	}
}

void CGameEventExecuter_C8::Chapter8_Bomb_Stamp(_float _fTimeDelta)
{
	m_fTimer += _fTimeDelta;

	function fCamerafunc = []()
		{
			CCamera_Manager::GetInstance()->Start_Turn_AxisY(CCamera_Manager::TARGET, 1.f, XMConvertToRadians(-60.f), (_uint)EASE_IN_OUT);
			CCamera_Manager::GetInstance()->Start_Changing_ArmLength_Increase(CCamera_Manager::TARGET, 1.f,
				3.f, EASE_IN_OUT);
			CCamera_Manager::GetInstance()->Start_Changing_AtOffset(CCamera_Manager::TARGET,
				1.f,
				XMVectorSet(0.f, -2.f, 0.f, 0.f),
				EASE_IN_OUT);
		};
	if (true == Postit_Process(L"Chapter8_SKSP_Postit", L"Chapter8_Bomb_Stamp", 1.f, CPostit_Page::POSTIT_PAGE_POSTION_TYPE_A, false, fCamerafunc))
	{
		GameEvent_End();
	}
}

void CGameEventExecuter_C8::Chapter8_Tilting_Glove(_float _fTimeDelta)
{
	m_fTimer += _fTimeDelta;

	function fCamerafunc = []()
		{
			CCamera_Manager::GetInstance()->Start_Changing_ArmLength_Increase(CCamera_Manager::TARGET, 1.f,
				1.f, EASE_IN_OUT);
			CCamera_Manager::GetInstance()->Start_Changing_AtOffset(CCamera_Manager::TARGET,
				1.f,
				XMVectorSet(0.f, -2.5f, 0.f, 0.f),
				EASE_IN_OUT);

			CCamera_Manager::GetInstance()->Start_Turn_AxisRight(CCamera_Manager::TARGET, 1.f, XMConvertToRadians(30.f), (_uint)EASE_IN_OUT);
		};
	if (true == Postit_Process(L"Chapter8_SKSP_Postit", L"Chapter8_Tilting_Glove", 1.f, CPostit_Page::POSTIT_PAGE_POSTION_TYPE_A, false, fCamerafunc))
	{

		// 포스트잇 전부 끝나고, 북 가져와서 모두 얼린다.
		auto pBook = Get_Book();
		if (nullptr != pBook)
		{
			auto pLayer = m_pGameInstance->Find_Layer(m_iCurLevelID, L"Layer_Gear");
			

			if (nullptr != pLayer)
			{
				auto GameObjects = pLayer->Get_GameObjects();

				if (false == GameObjects.empty())
				{
					for_each(GameObjects.begin(), GameObjects.end(), [](CGameObject* pGameObject) {
						static_cast<CContainerObject*>(pGameObject)->Set_PartAnimPlaying(CGear::GEAR_PART_GEAR,false);
						static_cast<CContainerObject*>(pGameObject)->Set_PartAnimPlaying(CGear::GEAR_PART_TEETH,false);
						});
				}
			}

			auto pPortal =static_cast<CSection_2D_PlayMap*>(SECTION_MGR->Find_Section(SECTION_MGR->Get_Cur_Section_Key()))->Get_Portal(0);
			static_cast<CContainerObject*>(pPortal)->Set_Active(false);
			
			pBook->Set_Freezing(true);
		

			// 그다음 아웃트로 책벌레 이벤트를 생성한다. 
			CTriggerObject::TRIGGEROBJECT_DESC Desc = {};
			Desc.vHalfExtents = { 1.0999999046325684f,
									3.799999952316284f,
									3.0999999046325684f };
			Desc.iTriggerType = (_uint)TRIGGER_TYPE::EVENT_TRIGGER;
			Desc.szEventTag = TEXT("Chapter8_Outro_Postit_Sequence");
			Desc.eConditionType = CTriggerObject::TRIGGER_ENTER;
			Desc.isReusable = false; 
			Desc.eStartCoord = COORDINATE_3D;
			Desc.tTransform3DDesc.vInitialPosition = { -115.94000244140625f,
													64.43000030517578f,
													23.530000686645508f };

			CTrigger_Manager::GetInstance()->Create_TriggerObject(LEVEL_STATIC, (LEVEL_ID)m_iCurLevelID, &Desc);
		}
		GameEvent_End();
	}
}

void CGameEventExecuter_C8::Chapter8_Meet_Humgrump(_float _fTimeDelta)
{
	m_fTimer += _fTimeDelta;

	if (Step_Check(STEP_0))
	{
		if (Is_Start()) {

#pragma region Friends
			// Debug
			_wstring strFriendTag = L"Thrash_Debug";
			{ /* Friend_Thrash */
				CFriend_Thrash::FRIEND_DESC Desc{};
				Desc.Build_2D_Transform(_float2(-580.f, 100.f), _float2(1.0f, 1.0f), 400.f);
				Desc.iCurLevelID = LEVEL_CHAPTER_2;
				Desc.eStartState = CFriend::FRIEND_IDLE;
				Desc.eStartDirection = CFriend::DIR_LEFT;
				Desc.iModelTagLevelID = LEVEL_STATIC;
				Desc.iNumDialoguesIndices = 0;
				Desc.strFightLayerTag = TEXT("Layer_Monster");

				CGameObject* pGameObject = nullptr;
				m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Friend_Thrash"), LEVEL_CHAPTER_8, TEXT("Layer_Friends"), &pGameObject, &Desc);

				CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter8_P2324"), pGameObject);

				CFriend_Controller::GetInstance()->Register_Friend(strFriendTag, static_cast<CFriend*>(pGameObject));
				CFriend_Controller::GetInstance()->Register_Friend_ToTrainList(strFriendTag);
			} /* Friend_Thrash */

			{ /* Friend_Violet */
				strFriendTag = L"Violet_Debug";
				CFriend_Violet::FRIEND_DESC Desc{};
				Desc.Build_2D_Transform(_float2(-730.f, 100.f), _float2(1.0f, 1.0f), 400.f);
				Desc.iCurLevelID = LEVEL_CHAPTER_2;
				Desc.eStartState = CFriend::FRIEND_IDLE;
				Desc.eStartDirection = CFriend::DIR_RIGHT;
				Desc.iModelTagLevelID = LEVEL_STATIC;
				Desc.iNumDialoguesIndices = 0;
				Desc.strFightLayerTag = TEXT("Layer_Monster");

				CGameObject* pGameObject = nullptr;
				m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Friend_Violet"), LEVEL_CHAPTER_8, TEXT("Layer_Friends"), &pGameObject, &Desc);

				CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter8_P2324"), pGameObject);

				CFriend_Controller::GetInstance()->Register_Friend(strFriendTag, static_cast<CFriend*>(pGameObject));
				CFriend_Controller::GetInstance()->Register_Friend_ToTrainList(strFriendTag);
			} /* Friend_Violet */

			{ /* Friend_Pip */
				strFriendTag = L"Pip_Debug";
				CFriend_Pip::FRIEND_DESC Desc{};
				Desc.Build_2D_Transform(_float2(-990.f, 20.f), _float2(1.0f, 1.0f), 400.f);
				Desc.iCurLevelID = LEVEL_CHAPTER_2;
				Desc.eStartState = CFriend::FRIEND_IDLE;
				Desc.eStartDirection = CFriend::DIR_DOWN;
				Desc.iModelTagLevelID = LEVEL_STATIC;
				Desc.iNumDialoguesIndices = 0;
				Desc.strFightLayerTag = TEXT("Layer_Monster");

				CGameObject* pGameObject = nullptr;
				m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Friend_Pip"), LEVEL_CHAPTER_8, TEXT("Layer_Friends"), &pGameObject, &Desc);

				CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter8_P2324"), pGameObject);

				CFriend_Controller::GetInstance()->Register_Friend(strFriendTag, static_cast<CFriend*>(pGameObject));
				CFriend_Controller::GetInstance()->Register_Friend_ToTrainList(strFriendTag);
			} /* Friend_Pip */
#pragma endregion

			// 1. Player 자동 이동
			CPlayer* pPlayer = Get_Player();

			AUTOMOVE_COMMAND tCommand = {};
			tCommand.eType = AUTOMOVE_TYPE::MOVE_TO;
			tCommand.iAnimIndex = (_uint)CPlayer::ANIM_STATE_2D::PLAYER_RUN_SWORD_UP;
			tCommand.vTarget = XMVectorSet(0.f, -287.33f, 0.0f, 1.f);
			tCommand.fPreDelayTime = 0.7f;
			tCommand.fPostDelayTime = 0.f;

			pPlayer->Add_AutoMoveCommand(tCommand);

			tCommand.eType = AUTOMOVE_TYPE::LOOK_DIRECTION;
			tCommand.iAnimIndex = (_uint)CPlayer::ANIM_STATE_2D::PLAYER_IDLE_SWORD_UP;
			tCommand.vTarget = XMVectorSet(0.f, 0.f, 0.0f, 1.f);
			tCommand.fPreDelayTime = 0.0f;
			tCommand.fPostDelayTime = 1.f;

			pPlayer->Add_AutoMoveCommand(tCommand);

			pPlayer->Start_AutoMove(true);

			// Debug
			CFriend* pThrash = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Thrash_Debug"));
			CFriend* pViolet = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Violet_Debug"));
			CFriend* pPip = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Pip_Debug"));

			pThrash->Move_Position(_float2(-50.f, -322.33f), CFriend::DIR_UP);
			pViolet->Move_Position(_float2(50.f, -322.33f), CFriend::DIR_UP);
			pPip->Move_Position(_float2(0.f, -357.33f), CFriend::DIR_UP);

			pThrash->Get_ControllerTransform()->Set_SpeedPerSec(200.f);
			pViolet->Get_ControllerTransform()->Set_SpeedPerSec(200.f);
			pPip->Get_ControllerTransform()->Set_SpeedPerSec(200.f);

		}

		// 2. 중앙으로 타겟 카메라 변경
		if (m_fTimer > 0.8f) {
			static _float4x4 matCenter = {};
			_vector vCenter = XMVectorSet(0.f, -106.36f, 0.f, 1.f);

			memcpy(&m_TargetWorldMatrix.m[3], &vCenter, sizeof(_float4));

			CCamera_Manager::GetInstance()->Change_CameraTarget(&m_TargetWorldMatrix, 0.5f);

			Next_Step(true);
		}
	}
	else if (Step_Check(STEP_1)) {

		CPlayer* pPlayer = Get_Player();

		if (false == pPlayer->Is_AutoMoving()) {
			// 3. Player Input Lock
			pPlayer->Set_BlockPlayerInput(true);

			// 4. Dialogue 재생
			CDialog_Manager::GetInstance()->Set_DialogId(L"Chapter8_Meet_Humgrump_1");

			CLayer* pLayer = m_pGameInstance->Find_Layer(m_iCurLevelID, TEXT("Layer_NPC"));

			list<CGameObject*> pObjects = pLayer->Get_GameObjects();

			// 4. TargetObjects Humgrump와 찍찍이 지정
			for (auto& pObject : pObjects) {
				CModelObject* pModel = dynamic_cast<CModelObject*>(pObject);

				if (nullptr == pModel)
					continue;

				if (TEXT("Humgrump") == pModel->Get_ModelPrototypeTag(COORDINATE_2D)) {
					CDialog_Manager::GetInstance()->Set_NPC(pModel);
					m_TargetObjects.push_back(pObject);
				}
			}

			CFriend* pPip = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Pip_Debug"));
			CDialog_Manager::GetInstance()->Set_NPC(pPip);

			Next_Step(true);
		}
	}
	else if (Step_Check(STEP_2)) {

		if (false == CDialog_Manager::GetInstance()->Get_DisPlayDialogue()) {

			if (Is_Start()) {
				static_cast<CModelObject*>(m_TargetObjects[0])->Switch_Animation(CNpc_Humgrump::CHAPTER8_TURN);
			}
		}

		if (false == static_cast<CModelObject*>(m_TargetObjects[0])->Is_DuringAnimation()) {
			// 5. Dialogue 재생
			CDialog_Manager::GetInstance()->Set_DialogId(L"Chapter8_Meet_Humgrump_2");
			CDialog_Manager::GetInstance()->Set_NPC(m_TargetObjects[0]);

			// 6. 험그럼프 Talk로 변경
			static_cast<CModelObject*>(m_TargetObjects[0])->Switch_Animation(CNpc_Humgrump::CHAPTER8_TALK_HAPPY);

			// 찍찍이로 말하는 대상 설정, 지금은 Player로 임시 설정함
			CFriend* pPip = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Pip_Debug"));
			CDialog_Manager::GetInstance()->Set_NPC(pPip);
			Next_Step(true);
		}
	}
	else if (Step_Check(STEP_3)) {
		if (1 == CDialog_Manager::GetInstance()->Get_CurrentLineIndex()) {

			if (Is_Start()) {
				// 7. 찍찍이 Time (찍찍이 Animation 변경해야 함)
				static_cast<CModelObject*>(m_TargetObjects[0])->Switch_Animation(CNpc_Humgrump::CHAPTER8_IDLE);
				//static_cast<CModelObject*>(m_TargetObjects[1])->Switch_Animation(CNpc_MoonBeard::CHAPTER6_GIVE_IT_AREST);
			}
		}

		if (3 == CDialog_Manager::GetInstance()->Get_CurrentLineIndex()) {
			// 7. 험그럼프 Time (찍찍이 Animation 변경해야 함)
			static_cast<CModelObject*>(m_TargetObjects[0])->Switch_Animation(CNpc_Humgrump::CHAPTER8_TALK_HAPPY);
			//static_cast<CModelObject*>(m_TargetObjects[1])->Switch_Animation(CNpc_MoonBeard::CHAPTER6_GIVE_IT_AREST);

			Next_Step(true);
		}
	}
	else if (Step_Check(STEP_4)) {
		if (5 == CDialog_Manager::GetInstance()->Get_CurrentLineIndex()) {

			if (Is_Start()) {
				// 7. 찍찍이 Time (찍찍이 Animation 변경해야 함)
				static_cast<CModelObject*>(m_TargetObjects[0])->Switch_Animation(CNpc_Humgrump::CHAPTER8_IDLE);
				//static_cast<CModelObject*>(m_TargetObjects[1])->Switch_Animation(CNpc_MoonBeard::CHAPTER6_GIVE_IT_AREST);
			}
		}

		if (6 == CDialog_Manager::GetInstance()->Get_CurrentLineIndex()) {
			// 7. 험그럼프 Time (찍찍이 Animation 변경해야 함)
			static_cast<CModelObject*>(m_TargetObjects[0])->Switch_Animation(CNpc_Humgrump::CHAPTER8_TALK_HAPPY);
			//static_cast<CModelObject*>(m_TargetObjects[1])->Switch_Animation(CNpc_MoonBeard::CHAPTER6_GIVE_IT_AREST);

			Next_Step(true);
		}
	}
	else if (Step_Check(STEP_5)) {
		if (false == CDialog_Manager::GetInstance()->Get_DisPlayDialogue()) {

			if (Is_Start()) {
				static_cast<CModelObject*>(m_TargetObjects[0])->Switch_Animation(CNpc_Humgrump::CHAPTER8_TRANSFORM);
			}
		}

		if (false == static_cast<CModelObject*>(m_TargetObjects[0])->Is_DuringAnimation()) {
			// 8. Dialogue 재생
			CDialog_Manager::GetInstance()->Set_DialogId(L"Chapter8_Meet_Humgrump_3");
			CDialog_Manager::GetInstance()->Set_NPC(m_TargetObjects[0]);

			// 8. 험그럼프 Talk로 변경
			static_cast<CModelObject*>(m_TargetObjects[0])->Switch_Animation(CNpc_Humgrump::CHAPTER8_TRANSFORM_TALK);

			Next_Step(true);
		}
	}
	else if (Step_Check(STEP_6)) {
		if (false == CDialog_Manager::GetInstance()->Get_DisPlayDialogue()) {

			if (Is_Start()) {
				// 9. Bubble로 변신, 카메라 줌
				static_cast<CModelObject*>(m_TargetObjects[0])->Switch_Animation(CNpc_Humgrump::CHAPTER8_BUBBLE_BRUST);

				CCamera_Manager::GetInstance()->Set_ResetData(CCamera_Manager::TARGET_2D);
				CCamera_Manager::GetInstance()->Start_Changing_ArmLength_Decrease(CCamera_Manager::TARGET_2D, 1.3f, 1.5f, EASE_IN_OUT);
				CCamera_Manager::GetInstance()->Start_Shake_ByTime(CCamera_Manager::TARGET_2D, 1.3f, 0.02f);
			}
		}

		if (false == static_cast<CModelObject*>(m_TargetObjects[0])->Is_DuringAnimation()) {
			// 9. Dialogue 재생
			CDialog_Manager::GetInstance()->Set_DialogId(L"Chapter8_Bubble_Humgrump");
			CDialog_Manager::GetInstance()->Set_NPC(m_TargetObjects[0]);

			// 9. 험그럼프 Talk로 변경
			static_cast<CModelObject*>(m_TargetObjects[0])->Switch_Animation(CNpc_Humgrump::CHAPTER8_BUBBLE_BRUST_TALK);

			Next_Step(true);
		}
	}
	else if (Step_Check(STEP_7)) {
		if (false == CDialog_Manager::GetInstance()->Get_DisPlayDialogue()) {

			if (Is_Start()) {
				static_cast<CModelObject*>(m_TargetObjects[0])->Switch_Animation(CNpc_Humgrump::CHAPTER8_FLY_AWAY);
			}
		}

		if (false == static_cast<CModelObject*>(m_TargetObjects[0])->Is_DuringAnimation()) {
			// 10. 다 날아갔으면 Active 끄고 Camera Zoom 더 멀리
			m_TargetObjects[0]->Set_Active(false);
			CCamera_Manager::GetInstance()->Start_Changing_ArmLength(CCamera_Manager::TARGET_2D, 0.5f, 14.4f, EASE_IN_OUT);
			_vector vCenter = XMVectorSet(0.f, -37.36f, 0.f, 1.f);

			memcpy(&m_TargetWorldMatrix.m[3], &vCenter, sizeof(_float4));

			// 11. Player와 친구들 이동
			// Player
			CPlayer* pPlayer = Get_Player();

			AUTOMOVE_COMMAND tCommand = {};
			tCommand.eType = AUTOMOVE_TYPE::MOVE_TO;
			tCommand.iAnimIndex = (_uint)CPlayer::ANIM_STATE_2D::PLAYER_RUN_SWORD_UP;
			tCommand.vTarget = XMVectorSet(0.f, -60.f, 0.0f, 1.f);
			tCommand.fPreDelayTime = 0.3f;
			tCommand.fPostDelayTime = 0.f;

			pPlayer->Add_AutoMoveCommand(tCommand);

			tCommand.eType = AUTOMOVE_TYPE::LOOK_DIRECTION;
			tCommand.iAnimIndex = (_uint)CPlayer::ANIM_STATE_2D::PLAYER_IDLE_SWORD_UP;
			tCommand.vTarget = XMVectorSet(0.f, 0.f, 0.0f, 1.f);
			tCommand.fPreDelayTime = 0.0f;
			tCommand.fPostDelayTime = 1.f;

			pPlayer->Add_AutoMoveCommand(tCommand);

			pPlayer->Start_AutoMove(true);


			// Debug
			CFriend* pThrash = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Thrash_Debug"));
			CFriend* pViolet = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Violet_Debug"));
			CFriend* pPip = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Pip_Debug"));

			pThrash->Move_Position(_float2(-50.f, -95.f), CFriend::DIR_UP);
			pViolet->Move_Position(_float2(50.f, -95.f), CFriend::DIR_UP);
			pPip->Move_Position(_float2(0.f, -130.f), CFriend::DIR_UP);

			Next_Step(true);
		}
	}
	else if (Step_Check(STEP_8)) {

		CPlayer* pPlayer = Get_Player();

		// Humgrump 고치 시작
		if (false == pPlayer->Is_AutoMoving()) {
			// 1. 고치 Intro
			static_cast<CModelObject*>(m_TargetObjects[0])->Switch_Animation(CNpc_Humgrump::CHAPTER8_PUPA_INTRO_0);
			m_TargetObjects[0]->Set_Active(true);
			m_TargetObjects[0]->Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.0f, -160.f, 0.03f, 1.f));
			m_iSubStep++;

			// 배경 어둡게
			Ready_Action(SECTION_MGR->Get_Cur_Section_Key(), SECTION_2D_PLAYMAP_BACKGROUND, C2DMapActionObject::ACTIVE_TYPE_DYNAMIC_BACKGROUND, 0.f);

			Next_Step(true);
		}
	}
	else if (Step_Check(STEP_9)) {

		// 2. 고치가 튕길 때 Player와 친구들 뒤로 날아가기
		if (m_fTimer >= 0.1f && 0 == m_iSubStep) {

			// Player
			CPlayer* pPlayer = Get_Player();

			AUTOMOVE_COMMAND tCommand = {};
			tCommand.eType = AUTOMOVE_TYPE::MOVE_TO;
			tCommand.iAnimIndex = (_uint)CPlayer::ANIM_STATE_2D::PLAYER_KNOCKEDBACK_LOOP;
			tCommand.vTarget = XMVectorSet(52.f, -250.00f, 0.0f, 1.f);
			tCommand.fPreDelayTime = 0.3f;
			tCommand.fPostDelayTime = 0.f;

			pPlayer->Add_AutoMoveCommand(tCommand);

			tCommand.eType = AUTOMOVE_TYPE::LOOK_DIRECTION;
			tCommand.iAnimIndex = (_uint)CPlayer::ANIM_STATE_2D::PLAYER_KNOCKEDBACK_FLOOR;
			tCommand.vTarget = XMVectorSet(0.f, 0.f, 0.0f, 1.f);
			tCommand.fPreDelayTime = 0.0f;
			tCommand.fPostDelayTime = 1.f;

			pPlayer->Add_AutoMoveCommand(tCommand);
			pPlayer->Start_AutoMove(true);


			// Debug
			// Thrash
			CFriend* pThrash = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Thrash_Debug"));
			CFriend* pViolet = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Violet_Debug"));
			CFriend* pPip = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Pip_Debug"));

			tCommand.eType = AUTOMOVE_TYPE::MOVE_TO;
			tCommand.iAnimIndex = CFriend_Thrash::ANIM::THRASH_KNOCKEDBACK_LOOP;
			tCommand.vTarget = XMVectorSet(-160.f, -200.f, 0.0f, 1.f);
			tCommand.fPreDelayTime = 0.3f;
			tCommand.fPostDelayTime = 0.f;

			pThrash->Add_AutoMoveCommand(tCommand);

			tCommand.eType = AUTOMOVE_TYPE::LOOK_DIRECTION;
			tCommand.iAnimIndex = CFriend_Thrash::ANIM::THRASH_IDLE_FLOOR;
			tCommand.vTarget = XMVectorSet(0.f, 0.f, 0.0f, 1.f);
			tCommand.fPreDelayTime = 0.0f;
			tCommand.fPostDelayTime = 0.f;

			pThrash->Add_AutoMoveCommand(tCommand);
			pThrash->Start_AutoMove(true);

			// Violet
			tCommand.eType = AUTOMOVE_TYPE::MOVE_TO;
			tCommand.iAnimIndex = CFriend_Violet::ANIM::VIOLET_KNOCKEDBACK_LOOP;
			tCommand.vTarget = XMVectorSet(160.f, -200.f, 0.0f, 1.f);
			tCommand.fPreDelayTime = 0.3f;
			tCommand.fPostDelayTime = 0.f;

			pViolet->Add_AutoMoveCommand(tCommand);

			tCommand.eType = AUTOMOVE_TYPE::LOOK_DIRECTION;
			tCommand.iAnimIndex = CFriend_Violet::ANIM::VIOLET_IDLE_FLOOR;
			tCommand.vTarget = XMVectorSet(0.f, 0.f, 0.0f, 1.f);
			tCommand.fPreDelayTime = 0.0f;
			tCommand.fPostDelayTime = 0.f;

			pViolet->Add_AutoMoveCommand(tCommand);
			pViolet->Start_AutoMove(true);

			// Pip
			tCommand.eType = AUTOMOVE_TYPE::MOVE_TO;
			tCommand.iAnimIndex = CFriend_Pip::ANIM::PIP_KNOCKEDBACK_LOOP;
			tCommand.vTarget = XMVectorSet(-52.f, -250.00f, 0.0f, 1.f);
			tCommand.fPreDelayTime = 0.3f;
			tCommand.fPostDelayTime = 0.f;

			pPip->Add_AutoMoveCommand(tCommand);

			tCommand.eType = AUTOMOVE_TYPE::LOOK_DIRECTION;
			tCommand.iAnimIndex = CFriend_Pip::ANIM::PIP_IDLE_UP;
			tCommand.vTarget = XMVectorSet(0.f, 0.f, 0.0f, 1.f);
			tCommand.fPreDelayTime = 0.0f;
			tCommand.fPostDelayTime = 0.f;

			pPip->Add_AutoMoveCommand(tCommand);
			pPip->Start_AutoMove(true);

			m_iSubStep++;
		}

		// auto move가 끝났다면으로 변경
		if (1 == m_iSubStep) {
			// 3. 고치 Intro 끝났다면 Idle noface로 변경
			if (false == static_cast<CModelObject*>(m_TargetObjects[0])->Is_DuringAnimation()) {
				static_cast<CModelObject*>(m_TargetObjects[0])->Switch_Animation(CNpc_Humgrump::CHAPTER8_PUPA_IDLE_NOFACE);

				Next_Step(true);
			}
		}
	}
	else if (Step_Check(STEP_10)) {
		if (m_fTimer >= 0.8f) {
			if (Is_Start()) {
				// 바이올렛 대사 1
				CDialog_Manager::GetInstance()->Set_DialogId(L"Chapter8_Disgusting");

				// 바이올렛으로 말하는 대상 설정, 지금은 Player로 임시 설정함
				CFriend* pViolet = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Violet_Debug"));
				CDialog_Manager::GetInstance()->Set_NPC(pViolet);

				Next_Step(true);
			}
		}
	}
	else if (Step_Check(STEP_11)) {
		if (false == CDialog_Manager::GetInstance()->Get_DisPlayDialogue() && 0 == m_iSubStep) {
			static_cast<CModelObject*>(m_TargetObjects[0])->Switch_Animation(CNpc_Humgrump::CHAPTER8_PUPA_INTRO_1);

			m_iSubStep++;
		}

		if (false == static_cast<CModelObject*>(m_TargetObjects[0])->Is_DuringAnimation() && 1 == m_iSubStep) {
			static_cast<CModelObject*>(m_TargetObjects[0])->Switch_Animation(CNpc_Humgrump::CHAPTER8_PUPA_IDLE);

			Next_Step(true);
		}
	}
	else if (Step_Check(STEP_12)) {
		if (m_fTimer >= 0.4f) {
			if (Is_Start()) {
				// 5. 험그럼프 대사 1
				CDialog_Manager::GetInstance()->Set_DialogId(L"Chapter8_Humgrump_Pupa_1");
				CDialog_Manager::GetInstance()->Set_NPC(m_TargetObjects[0]);
				static_cast<CModelObject*>(m_TargetObjects[0])->Switch_Animation(CNpc_Humgrump::CHAPTER8_PUPA_TALK_0);

			}
		}

		if (m_fTimer >= 0.5f && false == CDialog_Manager::GetInstance()->Get_DisPlayDialogue() && 0 == m_iSubStep) {
			static_cast<CModelObject*>(m_TargetObjects[0])->Switch_Animation(CNpc_Humgrump::CHAPTER8_PUPA_GRUNT_0);
			m_iSubStep++;
		}

		if (false == static_cast<CModelObject*>(m_TargetObjects[0])->Is_DuringAnimation() && 1 == m_iSubStep) {
			static_cast<CModelObject*>(m_TargetObjects[0])->Switch_Animation(CNpc_Humgrump::CHAPTER8_PUPA_GRUNT_OUT);
			m_iSubStep++;
		}

		if (false == static_cast<CModelObject*>(m_TargetObjects[0])->Is_DuringAnimation() && 2 == m_iSubStep) {
			// 6. 험그럼프 대사 2
			CDialog_Manager::GetInstance()->Set_DialogId(L"Chapter8_Humgrump_Pupa_2");
			CDialog_Manager::GetInstance()->Set_NPC(m_TargetObjects[0]);
			static_cast<CModelObject*>(m_TargetObjects[0])->Switch_Animation(CNpc_Humgrump::CHAPTER8_PUPA_TALK_1);

			Next_Step(true);
		}
	}
	else if (Step_Check(STEP_13)) {
		if (false == CDialog_Manager::GetInstance()->Get_DisPlayDialogue()) {
			// 7. 커지고 폭발
			if (Is_Start()) {
				static_cast<CModelObject*>(m_TargetObjects[0])->Switch_Animation(CNpc_Humgrump::CHAPTER8_PUPA_GROW);

				// Debug
				CFriend* pThrash = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Thrash_Debug"));
				CFriend* pViolet = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Violet_Debug"));
				CFriend* pPip = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Pip_Debug"));

				pThrash->Get_ControllerTransform()->Set_SpeedPerSec(400.f);
				pViolet->Get_ControllerTransform()->Set_SpeedPerSec(400.f);
				pPip->Get_ControllerTransform()->Set_SpeedPerSec(400.f);

				m_iSubStep++;
			}
		}

		if (1 == m_iSubStep) {

			if (false == static_cast<CModelObject*>(m_TargetObjects[0])->Is_DuringAnimation()) {
				// 검은색 FadeOut 시작
				CCamera_Manager::GetInstance()->Start_FadeOut(0.6f);
				Next_Step(true);
			}
		}
	}
	else if (Step_Check(STEP_14))
	{
		if (m_fTimer >= 0.7f) {

			if (Is_Start()) 
			{

				CCamera* pCamera = CCamera_Manager::GetInstance()->Get_CurrentCamera();
				CCamera_2D* pCam = static_cast<CCamera_2D*>(pCamera);

				pCam->Set_TrackingTime(0.1f);
				auto pBook = Get_Book();

				Event_DeleteObject(pBook);
				pBook = nullptr;
				Uimgr->Remove_Book();

				CGameObject* pGameObject = nullptr;
				CBook::BOOK_DESC Desc = {};
				Desc.iCurLevelID = m_iCurLevelID;
				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Book"),
					m_iCurLevelID, L"Layer_Book", &pGameObject, &Desc)))
					return;

				pBook = static_cast<CBook*>(pGameObject);
				//pBook->Set_Freezing(true);

				Uimgr->Set_Book(pBook);
				SECTION_MGR->Remove_Section(L"Chapter8_SKSP_01");
				SECTION_MGR->Remove_Section(L"Chapter8_SKSP_02");
				SECTION_MGR->Remove_Section(L"Chapter8_SKSP_03");
				SECTION_MGR->Remove_Section(L"Chapter8_SKSP_04");
				SECTION_MGR->Remove_Section(L"Chapter8_SKSP_05");
				SECTION_MGR->Remove_Section(L"Chapter8_SKSP_06");
				SECTION_MGR->Remove_Section(L"Chapter8_SKSP_07");
				SECTION_MGR->Remove_Section(L"Chapter8_SKSP_08");
				SECTION_MGR->Remove_Section(L"Chapter8_SKSP_09");
				SECTION_MGR->Remove_Section(L"Chapter8_SKSP_10");

				Event_ChangeMapObject(m_iCurLevelID, L"Chapter_Boss.mchc", L"Layer_MapObject");
				
				//TODO ::보스 생성 위치
				CGameObject* pBoss = nullptr;
				CButterGrump::MONSTER_DESC Boss_Desc;
				Boss_Desc.iCurLevelID = m_iCurLevelID;
				Boss_Desc.eStartCoord = COORDINATE_3D;
				Boss_Desc.tTransform3DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
				Boss_Desc.tTransform3DDesc.vInitialPosition = _float3(0.53f, 50.f, 30.0f);

				m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHAPTER_8, TEXT("Prototype_GameObject_ButterGrump"),
					m_iCurLevelID, TEXT("Layer_Boss"), &pBoss, &Boss_Desc);

				// 찌릿이 만들기
				CZip_C8::MODELOBJECT_DESC tZipDesc{};
				tZipDesc.iCurLevelID = m_iCurLevelID;
				tZipDesc.Build_2D_Transform(_float2(-248.0, -111.0), _float2(1.0f, 1.0f));
				m_pGameInstance->Add_GameObject_ToLayer(m_iCurLevelID, TEXT("Prototype_GameObject_ZipC8"), m_iCurLevelID, TEXT("Layer_Zip"), &pGameObject, &tZipDesc);
				CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter8_SKSP_11"), pGameObject, SECTION_2D_PLAYMAP_OBJECT);


				// 2D 험그럼프 없애기
				Event_DeleteObject(m_TargetObjects[0]);
				m_TargetObjects[0] = nullptr;

				// 포탈 활성화
				static_cast<CSection_2D_PlayMap*>(SECTION_MGR->Find_Section(SECTION_MGR->Get_Cur_Section_Key()))->Set_PortalActive(true);
			}
		}

		if (m_fTimer >= 1.3f) {
			// 검은색 FadeIn 시작
			CCamera_Manager::GetInstance()->Start_FadeIn(0.6f);
			CCamera_Manager::GetInstance()->Start_FadeOut(0.6f);
			CCamera* pCamera = CCamera_Manager::GetInstance()->Get_CurrentCamera();
			CCamera_2D* pCam = static_cast<CCamera_2D*>(pCamera);

			pCam->Set_TrackingTime(0.3f);
			Next_Step(true);
		}
	}
	else if (Step_Check(STEP_15)) {
		if (m_fTimer >= 1.5f) {
			if (Is_Start()) {
				CCamera_Manager::GetInstance()->Start_Changing_ArmLength_Decrease(CCamera_Manager::TARGET_2D, 0.7f, 1.5f, EASE_IN_OUT);
			}
		}

		if (m_fTimer >= 2.5f && 0 == m_iSubStep) {
			CDialog_Manager::GetInstance()->Set_DialogId(L"Chapter8_Going_To_Humgrump");

			// Debug
			CFriend* pThrash = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Thrash_Debug"));
			CFriend* pViolet = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Violet_Debug"));
			CFriend* pPip = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Pip_Debug"));

			CDialog_Manager::GetInstance()->Set_NPC(pViolet);
			CDialog_Manager::GetInstance()->Set_NPC(pPip);
			CDialog_Manager::GetInstance()->Set_NPC(pThrash);

			m_iSubStep++;
		}

		if (1 == m_iSubStep) {
			if (false == CDialog_Manager::GetInstance()->Get_DisPlayDialogue()) {
				CCamera_Manager::GetInstance()->Change_CameraTarget(Get_Player(), 0.7f);
				CPlayer* pPlayer = Get_Player();

				pPlayer->Set_BlockPlayerInput(false);

				// Monster 제거
				CLayer* pLayer = m_pGameInstance->Find_Layer(m_iCurLevelID, TEXT("Layer_Monster"));

				for (auto& Monster : pLayer->Get_GameObjects()) {
					Event_DeleteObject(Monster);
				}

				pLayer = m_pGameInstance->Find_Layer(m_iCurLevelID, TEXT("Layer_Sneak_Beetle"));

				for (auto& Beetle : pLayer->Get_GameObjects()) {
					Event_DeleteObject(Beetle);
				}

				pLayer = m_pGameInstance->Find_Layer(m_iCurLevelID, TEXT("Layer_Sneak_Soldier"));

				for (auto& Soldier : pLayer->Get_GameObjects()) {
					Event_DeleteObject(Soldier);
				}

				// 이전 Trigger 제거
				pLayer = m_pGameInstance->Find_Layer(m_iCurLevelID, TEXT("Layer_TriggerObject"));

				for (auto& TriggerObject : pLayer->Get_GameObjects()) {
					if (COORDINATE_3D == TriggerObject->Get_CurCoord())
						Event_DeleteObject(TriggerObject);
					/*	TriggerObject->Set_Active(false);
						TriggerObject->Set_Render(false);*/
				}

				// Intro Trigger 생성
				CTriggerObject::TRIGGEROBJECT_DESC Desc = {};
				Desc.vHalfExtents = { 7.f, 7.f, 7.f };
				Desc.iTriggerType = (_uint)TRIGGER_TYPE::EVENT_TRIGGER;
				Desc.szEventTag = TEXT("Chapter8_Boss_Intro");
				Desc.eConditionType = CTriggerObject::TRIGGER_ENTER;
				Desc.isReusable = false;
				Desc.eStartCoord = COORDINATE_3D;
				Desc.tTransform3DDesc.vInitialPosition = { 1.99f, 1.07f, -16.79f };

				CTrigger_Manager::GetInstance()->Create_TriggerObject(LEVEL_STATIC, LEVEL_CHAPTER_8, &Desc);

				GameEvent_End();
			}
		}
	}
}

void CGameEventExecuter_C8::Chapter8_Postit_Arm_Changing(_float _fTimeDelta)
{
	m_fTimer += _fTimeDelta;

	if (Step_Check(STEP_0)) {

		// 1. 현재 카메라 데이터 저장 후 Arm, Length, AtOffset 변경
		CCamera_Manager::GetInstance()->Set_ResetData(CCamera_Manager::TARGET_2D);
		CCamera_Manager::GetInstance()->Start_Changing_ArmLength(CCamera_Manager::TARGET_2D, 1.5f, 95.70f, EASE_IN_OUT);
		CCamera_Manager::GetInstance()->Start_Turn_ArmVector(CCamera_Manager::TARGET_2D, 1.5f, XMVectorSet(-0.3410f, 0.6239f, -0.7032f, 0.f), EASE_IN_OUT);
		CCamera_Manager::GetInstance()->Start_Changing_AtOffset(CCamera_Manager::TARGET_2D, 1.5f, XMVectorSet(0.f, 20.f, 0.f, 0.f), EASE_IN_OUT);

		// 1. Camera Tracking Time 변경
		CCamera_2D* pCamera = static_cast<CCamera_2D*>(CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET_2D));

		pCamera->Set_TrackingTime(1.f);

		// 3. 플레이어 인풋락
		CPlayer* pPlayer = Get_Player();
		pPlayer->Set_BlockPlayerInput(true);

		_vector vPos = pPlayer->Get_ControllerTransform()->Get_State(CTransform::STATE_POSITION);

		E_DIRECTION eDir = pPlayer->Get_2DDirection();

		switch ((_uint)eDir) {
		case (_uint)E_DIRECTION::DOWN:
			pPlayer->Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMVectorSetY(vPos, -120.f), 1.f));
			break;
		case (_uint)E_DIRECTION::UP:
			//pPlayer->Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMVectorSetY(vPos, 120.f), 1.f));
			break;
		}

		Next_Step(true);
	}
	else if (Step_Check(STEP_1)) {
		if (m_fTimer >= 2.f) {

			// 4. Camera Arm 원상 복구
			CCamera_Manager::GetInstance()->Start_ResetArm_To_SettingPoint(CCamera_Manager::TARGET_2D, 1.5f);

			CPlayer* pPlayer = Get_Player();

			_vector vPos = pPlayer->Get_ControllerTransform()->Get_State(CTransform::STATE_POSITION);

			E_DIRECTION eDir = pPlayer->Get_2DDirection();

			switch ((_uint)eDir) {
			case (_uint)E_DIRECTION::DOWN:
				//pPlayer->Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMVectorSetY(vPos, -120.f), 1.f));
				break;
			case (_uint)E_DIRECTION::UP:
				pPlayer->Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMVectorSetY(vPos, 120.f), 1.f));
				break;
			}

			Next_Step(true);
		}
	}
	else if (Step_Check(STEP_2)) {
		if (m_fTimer >= 1.7f) {
			CPlayer* pPlayer = Get_Player();

			pPlayer->Set_BlockPlayerInput(false);
			// 1. Camera Tracking Time 변경
			CCamera_2D* pCamera = static_cast<CCamera_2D*>(CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET_2D));

			pCamera->Set_TrackingTime(0.3f);

			GameEvent_End();
		}
	}
}

void CGameEventExecuter_C8::Chapter8_BookDrop(_float _fTimeDelta)
{
	m_fTimer += _fTimeDelta;
	CPlayer* pPlayer = Get_Player();
	auto pBook = Get_Book();

	if (Step_Check(STEP_0))
	{
		if (Is_Start())
		{
			pPlayer->Set_BlockPlayerInput(true);

			//CCamera_Manager::GetInstance()->Set_ResetData(CCamera_Manager::TARGET);
			CCamera_Manager::GetInstance()->Change_CameraTarget(pPlayer,1.f);
		}
		Next_Step_Over(1.f);
	}
	else if (Step_Check(STEP_1))
	{
		if (Is_Start())
		{
			if (nullptr != pBook)
				pBook->Start_DropBook();
			// 암 x3 y2 이동
			CCamera_Manager::GetInstance()->Start_Changing_AtOffset(CCamera_Manager::TARGET,
				1.f,
				XMVectorSet(4.f, 5.f, 0.f, 0.f),
				EASE_IN_OUT
			);
			// 암 1 땡기기
			CCamera_Manager::GetInstance()->Start_Changing_ArmLength_Increase(CCamera_Manager::TARGET, 2.f,
				15.f, EASE_IN_OUT);
			CCamera_Manager::GetInstance()->Start_Turn_AxisRight(CCamera_Manager::TARGET, 1.5f, XMConvertToRadians(20.f), XMConvertToRadians(30.f));

		}

		if (m_isPlag == false && m_fTimer > 1.f)
		{
			CCamera_Manager::GetInstance()->Start_FadeOut(1.f);
			m_isPlag = true;
		}


		if (Next_Step_Over(2.f))
		{
			Event_DeleteObject(pBook);
			pBook = nullptr;
			Uimgr->Remove_Book();
		}
	}
	else if (Step_Check(STEP_2))
	{
		auto pLayer = m_pGameInstance->Find_Layer(m_iCurLevelID, L"Layer_Book");
		if(true == pLayer->Is_Empty() && Is_Start())
		{
			// 포지션 다시 잡아주기.
			///pBook->Set_Position(XMVectorSet(2.f, 0.4f, -17.3f, 1.f));
			
			CGameObject* pGameObject = nullptr;
			CBook::BOOK_DESC Desc = {};

			Desc.isInitOverride = true;
			Desc.tTransform3DDesc.vInitialPosition = _float3(-50.f, 0.4f, 19.0f);
			Desc.tTransform3DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);
			Desc.iCurLevelID = m_iCurLevelID;
			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Book"),
				m_iCurLevelID, L"Layer_Book", &pGameObject, &Desc)))
				return ;

			pBook = static_cast<CBook*>(pGameObject);
			pBook->Set_Freezing(true);

			Uimgr->Set_Book(pBook);

			pBook->Set_BlendingRatio(1.f);
			CCamera_Manager::GetInstance()->Start_ResetArm_To_SettingPoint(CCamera_Manager::TARGET, 0.1f);
		}
		else if (true == m_isStart && pBook != nullptr && m_isPlag == false && m_fTimer > 0.5f)
		{
			pBook->Set_Animation(CBook::CLOSED_IDLE);
			m_isPlag = true;
		}

		if (Next_Step(m_fTimer > 1.f && m_isPlag))
		{
			CCamera_Manager::GetInstance()->Start_FadeIn(0.5f);
		}
	}	
	else
	{

		//if (nullptr != pBook)
			//pBook->Set_Freezing(false);

		pPlayer->Set_BlockPlayerInput(false);
		GameEvent_End();
	}

}

void CGameEventExecuter_C8::Chapter8_BookFreezing_Off(_float _fTimeDelta)
{

	m_fTimer += _fTimeDelta;
	CPlayer* pPlayer = Get_Player();
	auto pBook = Get_Book();

	if (Step_Check(STEP_0))
	{
		if (Is_Start())
		{
			pPlayer->Set_BlockPlayerInput(true);
		
		}
		Next_Step_Over(2.f);
		
	}
	else if (Step_Check(STEP_1))
	{
		if (Is_Start())
		{
			pBook->Start_FreezingOff();
		}
		Next_Step_Over(1.2f);
	}	
	else 
	{
		if (Is_Start())
		{
			auto pLayer = m_pGameInstance->Find_Layer(m_iCurLevelID, L"Layer_Gear");


			if (nullptr != pLayer)
			{
				auto GameObjects = pLayer->Get_GameObjects();

				if (false == GameObjects.empty())
				{
					for_each(GameObjects.begin(), GameObjects.end(), [](CGameObject* pGameObject) {
						static_cast<CContainerObject*>(pGameObject)->Set_PartAnimPlaying(CGear::GEAR_PART_GEAR, true);
						static_cast<CContainerObject*>(pGameObject)->Set_PartAnimPlaying(CGear::GEAR_PART_TEETH, true);
						});
				}
			}

			auto pPortal = static_cast<CSection_2D_PlayMap*>(SECTION_MGR->Find_Section(SECTION_MGR->Get_Cur_Section_Key()))->Get_Portal(0);
			static_cast<CContainerObject*>(pPortal)->Set_Active(true);

			pBook->Set_Freezing(false);

			pPlayer->Set_BlockPlayerInput(false);
		
		
			CFriend* pThrash = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Thrash"));


			if (nullptr != pThrash)
			{
				_vector vPos = pThrash->Get_FinalPosition();
				CTriggerObject::TRIGGEROBJECT_DESC Desc = {};
				Desc.vHalfExtents = { 100.f, 100.0f, 0.f };
				Desc.iTriggerType = (_uint)TRIGGER_TYPE::EVENT_TRIGGER;
				Desc.szEventTag = TEXT("Chapter8_2D_In");
				Desc.eConditionType = CTriggerObject::TRIGGER_ENTER;
				Desc.isReusable = false;
				Desc.eStartCoord = COORDINATE_2D;
				Desc.tTransform2DDesc.vInitialPosition = {XMVectorGetX(vPos),XMVectorGetY(vPos), 0.f};

				CSection* pBookSection = CSection_Manager::GetInstance()->Find_Section(TEXT("Chapter8_P1920"));
				CTrigger_Manager::GetInstance()->Create_TriggerObject(LEVEL_STATIC, LEVEL_CHAPTER_8, &Desc, pBookSection);
			}
		}
			GameEvent_End();
	}
	
	// dk...

}

void CGameEventExecuter_C8::Chapter8_2D_In(_float _fTimeDelta)
{
	m_fTimer += _fTimeDelta;
	CPlayer* pPlayer = Get_Player();

	if (Step_Check(STEP_0))
	{
		if (Is_Start())
		{
			pPlayer->Set_BlockPlayerInput(true);
		}
		Next_Step_Over(0.8f);
	}
	else if (Step_Check(STEP_1))
	{
		if (Is_Start())
		{
			CFriend* pPip = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Pip"));
			if (pPip)
			{
				CDialog_Manager::GetInstance()->Set_NPC(pPip);
			}
			CDialog_Manager::GetInstance()->Set_DialogId(L"Chapter8_2D_In");
		}
		else
			if (Next_Step(!CDialog_Manager::GetInstance()->Get_DisPlayDialogue()))
				pPlayer->Set_BlockPlayerInput(true);
	}
	else
	{
		CFriend_Controller::GetInstance()->Start_Train();
		pPlayer->Set_BlockPlayerInput(false);
		GameEvent_End();
	}
}

void CGameEventExecuter_C8::Chapter8_TransformZip(_float _fTimeDelta)
{
	m_fTimer += _fTimeDelta;
	CPlayer* pPlayer = Get_Player();
	CZip_C8* pZip = static_cast<CZip_C8*>( m_pGameInstance->Get_GameObject_Ptr(m_iCurLevelID,TEXT("Layer_Zip"), 0));
	if (Step_Check(STEP_0))
	{
		if (Is_Start())
		{
			AUTOMOVE_COMMAND tCommand{};
			tCommand.eType = AUTOMOVE_TYPE::MOVE_TO;
			tCommand.iAnimIndex = (_uint)CPlayer::ANIM_STATE_2D::PLAYER_RUN_SWORD_RIGHT;
			tCommand.vTarget =  { -390.f, - 111.f, 0.f };
			pPlayer->Add_AutoMoveCommand(tCommand);

			tCommand.eType = AUTOMOVE_TYPE::LOOK_DIRECTION;
			tCommand.iAnimIndex = (_uint)CPlayer::ANIM_STATE_2D::PLAYER_IDLE_SWORD_RIGHT;
			pPlayer->Add_AutoMoveCommand(tCommand);

			pPlayer->Start_AutoMove(true);
		}
		if(false == pPlayer->Is_AutoMoving())
		{
			pPlayer->Set_2DDirection(F_DIRECTION::RIGHT);
			Next_Step(true);
		}

	}
	else if (Step_Check(STEP_1))
	{
		//DIALOG
		CDialog_Manager* pDM = CDialog_Manager::GetInstance();
		if (Is_Start())
		{
			CCamera_Manager::GetInstance()->Change_CameraTarget(pZip, 1.f);
			pDM->Set_DialogId(TEXT("Chapter8_MeetZip"));
			//Uimgr->Set_DialogId(m_strDialogueIndex, m_strCurSecion);

			_vector vPos = pZip->Get_FinalPosition();
			_float3 vPosition; XMStoreFloat3(&vPosition, vPos);
			pDM->Set_DialoguePos(vPosition);
			//Uimgr->Set_DialoguePos(vPos);
			pDM->Set_DisPlayDialogue(true);
		}

		if (false == pDM->Get_DisPlayDialogue())
		{
			CCamera_Manager::GetInstance()->Change_CameraTarget(Get_Player(), 1.f);
			Next_Step(true);
		}

	}
	else if(Step_Check(STEP_2))
	{
		if (Is_Start())
		{
			pPlayer->TransformToCyberJot(pZip);
		}
		if (false == pPlayer->Get_Body()->Is_DuringAnimation()) {
			
			// Boss CutScene Trigger 실행
			CTrigger_Manager::GetInstance()->Register_TriggerEvent(TEXT("Chapter8_Going_To_Boss"), 0);

			// Player Shape 켜기
			Get_Player()->Get_ActorDynamic()->Set_ShapeEnable(CPlayer::PART_BODY, true);

			Next_Step(true);
		}
	}
	else
	{
		GameEvent_End();
	}

}

void CGameEventExecuter_C8::Chapter8_Boss_Intro(_float _fTimeDelta)
{
	m_fTimer += _fTimeDelta;

	if (Step_Check(STEP_0))
	{
		if (CCamera_Manager::TARGET == CCamera_Manager::GetInstance()->Get_CameraType()) {
			Next_Step(true);
		}
	}
	else if (Step_Check(STEP_1)) {
		if (m_fTimer >= 0.9f) {
			CCamera_Manager::GetInstance()->Set_NextCutSceneData(TEXT("Chapter8_Boos_Intro"));
			CCamera_Manager::GetInstance()->Change_CameraType(CCamera_Manager::CUTSCENE, true, 0.8f);
			Next_Step(true);
		}
	}
	else if (Step_Check(STEP_2)) {
		CCamera* pCamera = CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::CUTSCENE);
		if (false == pCamera->Is_Switching()) {
			// boss intro 시작
			static_cast<CButterGrump*>(m_pGameInstance->Get_GameObject_Ptr(m_iCurLevelID, TEXT("Layer_Boss"), 0))->Play_Intro(0);
			Next_Step(true);
		}
	}
	else if (Step_Check(STEP_3)) {
		if (m_fTimer >= 5.f) {
			if (Is_Start()) {
				Get_Player()->LookDirectionXZ_Dynamic({ 0.f, 0.f, 1.f });
				Get_Player()->Get_Body()->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_IDLE_NERVOUS_TURN_01_GT);
			}
		}

		CCamera_CutScene* pCamera = static_cast<CCamera_CutScene*>(CCamera_Manager::GetInstance()->Get_CurrentCamera());
		if (true == pCamera->Is_Finish_CutScene()) {
			Next_Step(true);
		}
	}
	else if (Step_Check(STEP_4)) {
		if (Is_Start()) {
			CCamera_Manager::GetInstance()->Start_FadeOut(0.7f);
		}

		// 3. FadeOut이 끝난 후 CutScene Camera -> Target Camera로 전환
		if (m_fTimer > 0.7f) {
			static_cast<CCamera_CutScene*>(CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::CUTSCENE))->Set_Pause_After_CutScene(false);
			Next_Step(true);
		}
	}
	// 4. 전환 후 Target Camera로(다음 프레임) FadeIn 시작 + 기존 CutScene Camera를 다시 밝게 만들기
	else if (Step_Check(STEP_5)) {
		if (Is_Start()) {
			CCamera_Manager::GetInstance()->Start_FadeIn(0.7f);
			CCamera_Manager::GetInstance()->Set_FadeRatio(CCamera_Manager::CUTSCENE, 1.f, true);
			Next_Step(true);
		}
	}
	else
	{
		GameEvent_End();
	}
}

void CGameEventExecuter_C8::Chapter8_Going_To_Boss(_float _fTimeDelta)
{
	m_fTimer += _fTimeDelta;

	if (Step_Check(STEP_0)) {
		if (COORDINATE_3D == Get_Player()->Get_CurCoord()) {
			CCamera_Manager::GetInstance()->Set_NextCutSceneData(TEXT("Going_To_Humgrump"));
			CCamera_Manager::GetInstance()->Change_CameraType(CCamera_Manager::CUTSCENE);
			Get_Player()->Set_State(CPlayer::ENGAGE_BOSS);
			Next_Step(true);
		}
	}
	else if (Step_Check(STEP_1)) {
		CCamera_CutScene* pCamera = static_cast<CCamera_CutScene*>(CCamera_Manager::GetInstance()->Get_CurrentCamera());
		
		if (m_fTimer >= 11.5f) {
			if (Is_Start()) {
				CGameObject* pBoss = m_pGameInstance->Get_GameObject_Ptr(m_iCurLevelID, TEXT("Layer_Boss"), 0);
				static_cast<CButterGrump*>(pBoss)->Play_Intro(2);
			}
		}
		
		if (true == pCamera->Is_Finish_CutScene()) {
			Next_Step(true);
		}
	}
	else if (Step_Check(STEP_2)) {
		if (Is_Start()) {
			CCamera_Manager::GetInstance()->Start_FadeOut(0.7f);
		}

		// 3. FadeOut이 끝난 후 CutScene Camera -> Target Camera로 전환
		if (m_fTimer > 0.7f) {
			static_cast<CCamera_CutScene*>(CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::CUTSCENE))->Set_Pause_After_CutScene(false);
			Next_Step(true);
		}
	}
	// 4. 전환 후 Target Camera로(다음 프레임) FadeIn 시작 + 기존 CutScene Camera를 다시 밝게 만들기
	else if (Step_Check(STEP_3)) {
		if (Is_Start()) {
			CCamera_Manager::GetInstance()->Start_FadeIn(0.7f);
			CCamera_Manager::GetInstance()->Set_FadeRatio(CCamera_Manager::CUTSCENE, 1.f, true);

			// Camera Pivot 설정
			CCameraPivot* pPivot = static_cast<CCameraPivot*>(m_pGameInstance->Get_GameObject_Ptr(m_iCurLevelID, TEXT("Layer_CameraPivot"), 0));
			CGameObject* pBoss = m_pGameInstance->Get_GameObject_Ptr(m_iCurLevelID, TEXT("Layer_Boss"), 0);
			pPivot->Set_MainTarget(pBoss);
			pPivot->Set_Active(true);
			CCamera_Manager::GetInstance()->Change_CameraTarget(pPivot, 0.f);

			CCamera_Target* pCamera = static_cast<CCamera_Target*>(CCamera_Manager::GetInstance()->Get_CurrentCamera());
			pCamera->Get_Arm()->Set_ArmVector(_vector{ 0.f,0.f, -1.f });
			pCamera->Set_Position(_vector{ 0.53f, 60.35f, -8.0f });
			pCamera->Get_Arm()->Set_Length(24.6f);
			pCamera->Set_FreezeEnter(CCamera_Target::FREEZE_X, XMVectorSet(-0.3150f, 0.1552f, -0.9363f, 0.f), 0);
			pCamera->Set_FreezeEnter(CCamera_Target::FREEZE_Z, XMVectorSet(-0.3150f, 0.1552f, -0.9363f, 0.f), 0);

			// Boss 체력바
			CUI::UIOBJDESC pDesc = {};
			pDesc.iCurLevelID = m_iCurLevelID;

			Uimgr->Set_BossForUI(static_cast<CButterGrump*>(pBoss));
			m_pGameInstance->Add_GameObject_ToLayer(m_iCurLevelID, TEXT("Prototype_GameObject_BossHP"), pDesc.iCurLevelID, TEXT("Layer_UI"), &pDesc);

			// Player State 변경
			Get_Player()->Set_State(CPlayer::CYBER_FLY);

			Next_Step(true);
		}
	}
	else
	{
		GameEvent_End();
	}
}

_bool CGameEventExecuter_C8::Change_PlayMap(_float _fStartTime)
{
	LEVEL_ID eCurLevelID = (LEVEL_ID)m_pGameInstance->Get_CurLevelID();

	// 맵 설치
	if (m_fTimer > _fStartTime && 0 == m_iSubStep)
	{
		Event_ChangeMapObject(LEVEL_CHAPTER_8, L"Chapter_08_Play_Desk.mchc", L"Layer_MapObject");
		m_iSubStep++;
		return false;

	}
	_fStartTime += 0.2f;
	//아이템 추가

	if (true == CEvent_Manager::GetInstance()->Is_MapLoad() && m_fTimer > _fStartTime && 1 == m_iSubStep)
	{
		auto pLayer = m_pGameInstance->Find_Layer(m_iCurLevelID, L"Layer_Social3DNPC");

		if (nullptr != pLayer)
		{
			auto GameObjects = pLayer->Get_GameObjects();
			for (auto& pObject : GameObjects)
			{
				pObject->Set_Active(true);
			}
		}

		CPlayerData_Manager::GetInstance()->Spawn_Bulb(LEVEL_STATIC, (LEVEL_ID)eCurLevelID);
		CPlayerData_Manager::GetInstance()->Spawn_PlayerItem(LEVEL_STATIC, (LEVEL_ID)eCurLevelID, TEXT("Bomb_Stamp"), _float3(-15.54f, 26.06f, 16.56f), { 1.f,1.f,1.f });
		CPlayerData_Manager::GetInstance()->Spawn_PlayerItem(LEVEL_STATIC, (LEVEL_ID)eCurLevelID, TEXT("Sword"), _float3(42.22f, 15.82f, -0.45f), { 2.f,2.f,2.f });
		CPlayerData_Manager::GetInstance()->Spawn_PlayerItem(LEVEL_STATIC, (LEVEL_ID)eCurLevelID, TEXT("Stop_Stamp"), _float3(45.13f, 49.74f, 23.34f), { 1.f,1.f,1.f });
		CPlayerData_Manager::GetInstance()->Spawn_PlayerItem(LEVEL_STATIC, (LEVEL_ID)eCurLevelID, TEXT("Tilting_Glove"), _float3(30.55f, 30.83f, 23.34f));
		m_iSubStep++;
		return false;

	}
	_fStartTime += 0.2f;

	// 몬스터 추가
	if (m_fTimer > _fStartTime &&  2 == m_iSubStep)
	{

		CGameObject* pObject = nullptr;

		const json* pJson = m_pGameInstance->Find_Json_InLevel(TEXT("Chapter8_Monsters_3D"), LEVEL_CHAPTER_8);

		if (nullptr == pJson)
			return false;
		if (pJson->contains("3D"))
		{
			_wstring strLayerTag = L"Layer_Monster";
			_wstring strMonsterTag = L"";

			for (auto Json : (*pJson)["3D"])
			{
				CMonster::MONSTER_DESC MonsterDesc3D = {};

				MonsterDesc3D.iCurLevelID = LEVEL_CHAPTER_8;
				MonsterDesc3D.eStartCoord = COORDINATE_3D;

				if (Json.contains("Position"))
				{
					for (_int j = 0; j < 3; ++j)
					{
						*(((_float*)&MonsterDesc3D.tTransform3DDesc.vInitialPosition) + j) = Json["Position"][j];
					}
				}
				if (Json.contains("Scaling"))
				{
					for (_int j = 0; j < 3; ++j)
					{
						*(((_float*)&MonsterDesc3D.tTransform3DDesc.vInitialScaling) + j) = Json["Scaling"][j];
					}
				}
				if (Json.contains("LayerTag"))
				{
					strLayerTag = STRINGTOWSTRING(Json["LayerTag"]);
				}

				if (Json.contains("MonsterTag"))
				{
					strMonsterTag = STRINGTOWSTRING(Json["MonsterTag"]);
				}
				else
					return false;

				if (Json.contains("SneakMode"))
				{
					if (Json.contains("SneakWayPointIndex"))
					{
						MonsterDesc3D.eWayIndex = Json["SneakWayPointIndex"];
					}
					else
						return false;
					MonsterDesc3D.isSneakMode = Json["SneakMode"];
				}

				if (Json.contains("IsStay"))
				{
					MonsterDesc3D.isStay = Json["IsStay"];
					if (Json.contains("vLook"))
					{
						for (_int j = 0; j < 3; ++j)
						{
							*(((_float*)&MonsterDesc3D.vLook) + j) = Json["vLook"][j];
						}
					}
				}

				if (Json.contains("IsIgnoreGround"))
				{
					MonsterDesc3D._isIgnoreGround = Json["IsIgnoreGround"];
				}

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, strMonsterTag, LEVEL_CHAPTER_8, strLayerTag, &pObject, &MonsterDesc3D)))
					return false;
			}
		}

		if (FAILED(CFormation_Manager::GetInstance()->Initialize()))
			return false;


		//CGameObject* pObject = nullptr;

		_float3 vPos = { 50.5f, 30.3f, 9.f };
		CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_Bomb"), COORDINATE_3D, &pObject, &vPos);
		//static_cast<CBomb*>(pObject)->Set_Time_Off();
		static_cast<CBomb*>(pObject)->Set_LifeTime(5.f);

		vPos = { 50.5f, 30.3f, 8.f };
		CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_Bomb"), COORDINATE_3D, &pObject, &vPos);
		//static_cast<CBomb*>(pObject)->Set_Time_Off();
		static_cast<CBomb*>(pObject)->Set_LifeTime(5.f);

		vPos = { 49.5f, 30.3f, 9.f };
		CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_Bomb"), COORDINATE_3D, &pObject, &vPos);
		//static_cast<CBomb*>(pObject)->Set_Time_Off();
		static_cast<CBomb*>(pObject)->Set_LifeTime(5.f);

		vPos = { 49.5f, 30.3f, 8.f };
		CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_Bomb"), COORDINATE_3D, &pObject, &vPos);
		//static_cast<CBomb*>(pObject)->Set_Time_Off();
		static_cast<CBomb*>(pObject)->Set_LifeTime(5.f);



		m_iSubStep++;
		return true;
	}
	return false;
}


CGameEventExecuter_C8* CGameEventExecuter_C8::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CGameEventExecuter_C8* pInstance = new CGameEventExecuter_C8(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGameEventExecuter_C8");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGameEventExecuter_C8::Clone(void* _pArg)
{
	CGameEventExecuter_C8* pInstance = new CGameEventExecuter_C8(*this);
	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CGameEventExecuter_C8");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGameEventExecuter_C8::Free()
{
	__super::Free();
}


HRESULT CGameEventExecuter_C8::Cleanup_DeadReferences()
{
	return S_OK;
}
