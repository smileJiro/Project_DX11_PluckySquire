#include "stdafx.h"
#include "GameEventExecuter_C8.h"
#include "Trigger_Manager.h"
#include "Dialog_Manager.h"
#include "Friend_Controller.h"

#include "Camera_Manager.h"
#include "Camera_Target.h"
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

#include "Camera_CutScene.h"
#include "Effect2D_Manager.h"
#include "2DMapActionObject.h"
#include "Room_Door.h"
#include "Book.h"
#include "PlayerData_Manager.h"

#include "PlayerBody.h"

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
		else if(Step_Check(STEP_1)) 
		{
			if (Is_Start())
				CCamera_Manager::GetInstance()->Start_FadeOut();
			else
				Next_Step_Over(1.5f);
		}
		else if(Step_Check(STEP_2)) 
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
						m_TargetObjects[BRIDGE]->Set_Position({902.f,-107.f});
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
				CTrigger_Manager::GetInstance()->Create_TriggerObject(LEVEL_STATIC, LEVEL_CHAPTER_2, &Desc, pBookSection);




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

				for (auto iter = Objects.begin() ; iter != Objects.end() ; )
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
				Desc.vColliderExtents = { 1.f, 1.f};
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
			}
			Next_Step_Over(1.5f);
		}
		// 움직이기 시작한다. 플레이어 무빙을 해제한다.
		else if (Step_Check(STEP_3))
		{
			if (Is_Start())
			{
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
				auto pLayer = m_pGameInstance->Find_Layer(m_iCurLevelID,L"Layer_FloorWord");
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
			//Change_PlayMap(1.f);

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
			}
			_vector vDir = XMVectorSetW(XMVector2Normalize(vTargetPos - m_TargetObjects[BRIDGE]->Get_FinalPosition()), 0.f);
			m_TargetObjects[BRIDGE]->Get_ControllerTransform()->Go_Direction(vDir, _fTimeDelta);
			//static_cast<CModelObject*>(m_TargetObjects[BRIDGE])->Get_ControllerTransform()->MoveTo(vTargetPos,_fTimeDelta);


			if(Next_Step(m_TargetObjects[BRIDGE]->Get_ControllerTransform()->Compute_Distance(vTargetPos) < 5.f))
				m_TargetObjects[PIP]->Set_Active(false);
		
		}
		else if (Step_Check(STEP_8))
		{
			if (Is_Start())
			{
				Event_DeleteObject(m_TargetObjects[BLOCKER]);
			}
			Next_Step(true);
		}
		else if (Step_Check(STEP_9))
		{
			Next_Step_Over(5.f);
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
			static_cast<CFriend_Pip*>(pFriend)->Move_Position(_float2(1053.0f, 96.0f), CFriend::DIR_DOWN);
		}

		Next_Step_Over(1.f);
	}
	else if (Step_Check(STEP_1))
	{
		if (Is_Start())
		{
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
			Next_Step(!CDialog_Manager::GetInstance()->Get_DisPlayDialogue());
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
}

void CGameEventExecuter_C8::Chapter8_Friend_Appear_Thrash(_float _fTimeDelta)
{
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

			// 2. 어리둥절 애니메이션 재생
			pPlayer->Swicth_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_IDLE_NERVOUS_TURN_01_GT);
		}
		
		
		if (false == static_cast<CPlayerBody*>(pPlayer->Get_Body())->Is_DuringAnimation() &&
			m_fTimer >= 0.7f) {
			Next_Step(true);
		}
	}
	else if (Step_Check(STEP_1)) {
		// 3. CutScene 재생
		CCamera_Manager::GetInstance()->Set_NextCutSceneData(TEXT("Chapter8_Map_Intro"));
		CCamera_Manager::GetInstance()->Change_CameraType(CCamera_Manager::CUTSCENE, true, 0.8f);
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
		GameEvent_End();
	}
}

void CGameEventExecuter_C8::Change_PlayMap(_float _fStartTime)
{
	LEVEL_ID eCurLevelID = (LEVEL_ID)m_pGameInstance->Get_CurLevelID();

	// 맵 설치
	if (m_fTimer > _fStartTime && 0 == m_iSubStep)
	{
		Event_ChangeMapObject(LEVEL_CHAPTER_8, L"Chapter_08_Play_Desk.mchc", L"Layer_MapObject");
		m_iSubStep++;
	}
	//몬스터 추가
	_fStartTime += 0.1f;

	if (m_fTimer > _fStartTime && 1 == m_iSubStep)
	{


		CSpear_Soldier::MONSTER_DESC Spear_Soldier_Desc;
		Spear_Soldier_Desc.iCurLevelID = eCurLevelID;
		Spear_Soldier_Desc.eStartCoord = COORDINATE_3D;
		Spear_Soldier_Desc.tTransform3DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
		Spear_Soldier_Desc.tTransform3DDesc.vInitialPosition = _float3(13.f, 21.58f, 5.5f);
		Spear_Soldier_Desc.isSneakMode = true;
		Spear_Soldier_Desc.eWayIndex = SNEAKWAYPOINTINDEX::CHAPTER8_1;

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Spear_Soldier"), eCurLevelID, L"Layer_Monster", &Spear_Soldier_Desc)))
			return ;



		CBeetle::MONSTER_DESC Beetle_Desc;
		Beetle_Desc.iCurLevelID = eCurLevelID;
		Beetle_Desc.eStartCoord = COORDINATE_3D;
		Beetle_Desc.tTransform3DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
		Beetle_Desc.tTransform3DDesc.vInitialPosition = _float3(15.f, 11.1f, 3.4f);
		Beetle_Desc.isSneakMode = true;
		Beetle_Desc.eWayIndex = SNEAKWAYPOINTINDEX::CHAPTER8_BEETLE1;

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Beetle"), eCurLevelID, TEXT("Layer_Sneak_Beetle"), &Beetle_Desc)))
			return ;



		//const json* pJson = m_pGameInstance->Find_Json_InLevel(TEXT("Chapter8_Monsters_3D"), m_pGameInstance->Get_CurLevelID());

		//CGameObject* pObject;


		//CMonster::MONSTER_DESC MonsterDesc3D = {};

		//MonsterDesc3D.iCurLevelID = m_pGameInstance->Get_CurLevelID();
		//MonsterDesc3D.eStartCoord = COORDINATE_3D;

		//if (pJson->contains("3D"))
		//{
		//	_wstring strLayerTag = L"Layer_Monster";
		//	_wstring strMonsterTag = L"";

		//	for (_int i = 0; i < (*pJson)["3D"].size(); ++i)
		//	{
		//		if ((*pJson)["3D"][i].contains("Position"))
		//		{
		//			for (_int j = 0; j < 3; ++j)
		//			{
		//				*(((_float*)&MonsterDesc3D.tTransform3DDesc.vInitialPosition) + j) = (*pJson)["3D"][i]["Position"][j];
		//			}
		//		}
		//		if ((*pJson)["3D"][i].contains("Scaling"))
		//		{
		//			for (_int j = 0; j < 3; ++j)
		//			{
		//				*(((_float*)&MonsterDesc3D.tTransform3DDesc.vInitialScaling) + j) = (*pJson)["3D"][i]["Scaling"][j];
		//			}
		//		}
		//		if ((*pJson)["3D"][i].contains("LayerTag"))
		//		{
		//			strLayerTag = STRINGTOWSTRING((*pJson)["3D"][i]["LayerTag"]);
		//		}

		//		if ((*pJson)["3D"][i].contains("MonsterTag"))
		//		{
		//			strMonsterTag = STRINGTOWSTRING((*pJson)["3D"][i]["MonsterTag"]);
		//		}
		//		else
		//			return;

		//		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, strMonsterTag, m_pGameInstance->Get_CurLevelID(), strLayerTag, &pObject, &MonsterDesc3D)))
		//			return;

		//	}
		//}		
		m_iSubStep++;

	}
	_fStartTime += 0.1f;

	// 3D NPC들 렌더 
	if (m_fTimer > _fStartTime && 2 == m_iSubStep)
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
		CPlayerData_Manager::GetInstance()->Spawn_PlayerItem(LEVEL_STATIC, (LEVEL_ID)eCurLevelID, TEXT("Stop_Stamp"), _float3(45.13f, 50.24f, 23.34f), { 1.f,1.f,1.f });
		CPlayerData_Manager::GetInstance()->Spawn_PlayerItem(LEVEL_STATIC, (LEVEL_ID)eCurLevelID, TEXT("Tilting_Glove"), _float3(30.55f, 30.98f, 23.34f));
		m_iSubStep++;

	}
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
