#include "stdafx.h"
#include "GameEventExecuter_C2.h"
#include "Trigger_Manager.h"
#include "Section_Manager.h"
#include "Dialog_Manager.h"
#include "Pooling_Manager.h"
#include "Camera_Manager.h"
#include "Effect2D_Manager.h"
#include "Friend_Controller.h"
#include "Friend.h"

#include "GameInstance.h"
#include "Book.h"
#include "Magic_Hand.h"
#include "Camera_2D.h"
#include "UI_Manager.h"
#include "Effect_Manager.h"
#include "Section_2D.h"
#include "MapObject.h"
#include "Camera_Target.h"
#include "Camera_CutScene.h"
#include "2DMapActionObject.h"
#include "Postit_Page.h"
#include "CarriableObject.h"
#include "Section_2D_PlayMap.h"
#include "DraggableObject.h"
#include "PlayerData_Manager.h"

#include "Zippy.h"
#include "Room_Door.h"

CGameEventExecuter_C2::CGameEventExecuter_C2(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CGameEventExecuter(_pDevice, _pContext)
{
}


HRESULT CGameEventExecuter_C2::Initialize(void* _pArg)
{
	m_iCurLevelID = LEVEL_CHAPTER_2;
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	// 초기 설정 
	switch ((CTrigger_Manager::EVENT_EXECUTER_ACTION_TYPE)m_iEventExcuterAction)
	{
	case Client::CTrigger_Manager::C02P0910_LIGHTNING_BOLT_SPAWN:

		// 예시 코드
		// 이펙트를 생성하고 다음 이벤트 실행시간 맞추게 시간 타이머 설정.
		// C020910_Bolt_Spawn에서 시간이 지나면 다음 이벤트 실행(On_End 전달 & Executer 삭제)
		//Event_2DEffectCreate(EFFECT_LIGHTNINGBOLST, _float2(10.f,10.f), SECTION_MGR->Get_Cur_Section_Key())
		m_fMaxTimer = 1.5f;
		m_isPlag = false;
		break;
	case Client::CTrigger_Manager::C02P0910_MONSTER_SPAWN:
		break;
	default:
		break;
	}


	return S_OK;
}

void CGameEventExecuter_C2::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);
}

void CGameEventExecuter_C2::Update(_float _fTimeDelta)
{
	if (!Is_Dead())
	{
		switch ((CTrigger_Manager::EVENT_EXECUTER_ACTION_TYPE)m_iEventExcuterAction)
		{
		case Client::CTrigger_Manager::C02P0910_LIGHTNING_BOLT_SPAWN:
			if (false == m_isPlag)
			{
				C020910_Bolt_Spawn(_fTimeDelta);
			}
			break;
		case Client::CTrigger_Manager::C02P0910_MONSTER_SPAWN:
			C020910_Monster_Spawn(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER2_BOOKMAGIC:
			Chapter2_BookMagic(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER2_INTRO:
			Chapter2_Intro(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER2_HUMGRUMP:
			Chapter2_Humgrump(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER2_LUNCHBOX_APPEAR:
			Chapter2_Lunchbox_Appear(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER2_LUNCHBOX_OPEN:
			Chapter2_Lunchbox_Open(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER2_BETTLE_PAGE:
			Chapter2_Bettle_Page(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER2_OPENBOOKEVENT:
			Chapter2_OpenBookEvent(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER2_STORYSEQUENCE:
			Chapter2_StorySequence(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER2_AFTER_OPENING_BOOK:
			Chapter2_After_Opening_Book(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER2_GOING_TO_ARTIA:
			Chapter2_Going_To_Artia(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER2_FRIENDEVENT_0:
			Chapter2_FriendEvent_0(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER2_FRIENDEVENT_1:
			Chapter2_FriendEvent_1(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER2_PIP_0:
			Chapter2_Pip_0(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::START_TRAIN:
			Start_Train(_fTimeDelta);
			break;
		default:
			break;
		}
	}
	
}

void CGameEventExecuter_C2::Late_Update(_float _fTimeDelta)
{
}

void CGameEventExecuter_C2::C020910_Bolt_Spawn(_float _fTimeDelta)
{
	_wstring strSectionKey = TEXT("Chapter2_P0910");
	if (0.f == m_fTimer)
	{
		CEffect2D_Manager::GetInstance()->Play_Effect(TEXT("LightningBolt"), strSectionKey, XMMatrixTranslation(500.f, 10.f, 0.0f), 0.f, 4, false, 0.f);
		CEffect2D_Manager::GetInstance()->Play_Effect(TEXT("LightningBolt"), strSectionKey, XMMatrixTranslation(500.f, 10.f, 0.0f), 0.f, 5, false, 0.f, SECTION_2D_PLAYMAP_BACKGROUND);

		CEffect2D_Manager::GetInstance()->Play_Effect(TEXT("LightningBolt"), strSectionKey, XMMatrixTranslation(-450.f, -30.f, 0.0f), 0.f, 4, false, 0.f);
		CEffect2D_Manager::GetInstance()->Play_Effect(TEXT("LightningBolt"), strSectionKey, XMMatrixTranslation(-450.f, -30.f, 0.0f), 0.f, 5, false, 0.f, SECTION_2D_PLAYMAP_BACKGROUND);
	}

	m_fTimer += _fTimeDelta;

	if (m_fMaxTimer <= m_fTimer)
	{
		m_fTimer = 0.f;
		m_isPlag = true;
		GameEvent_End();
	}
}

void CGameEventExecuter_C2::C020910_Monster_Spawn(_float _fTimeDelta)
{
	CGameObject* pObject = nullptr;
	CZippy::MONSTER_DESC Zippy_Desc;
	Zippy_Desc.iCurLevelID = LEVEL_CHAPTER_2;
	Zippy_Desc.tTransform2DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);

	_float3 vPos = { 500.0f, 10.f, 0.f };
	_wstring strSectionKey = TEXT("Chapter2_P0910");

	Zippy_Desc.tTransform2DDesc.vInitialPosition = vPos;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Zippy"), LEVEL_CHAPTER_2, TEXT("Layer_Monster"), &pObject, &Zippy_Desc)))
		return;

	CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(strSectionKey, pObject);

	vPos = { -450.0f, -30.f, 0.f };
	Zippy_Desc.tTransform2DDesc.vInitialPosition = vPos;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Zippy"), LEVEL_CHAPTER_2, TEXT("Layer_Monster"), &pObject, &Zippy_Desc)))
		return;

	CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(strSectionKey, pObject);

	GameEvent_End();
}

void CGameEventExecuter_C2::Chapter2_BookMagic(_float _fTimeDelta)
{

	m_fTimer += _fTimeDelta;

	// 책 중앙으로 카메라 이동
	if (Step_Check(STEP_0))
	{
		if (Is_Start())
		{
			CBook* pBook = Get_Book();
			if (nullptr != pBook)
				CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET_2D)->Change_Target(pBook);
		}
		Next_Step(true);
	}
	// 하늘 어두워지기?
	// 
	// 책 발작, 이펙트 추가
	// 어두워지기
	else if (Step_Check(STEP_1))
	{
		if (Is_Start())
		{
			CSection_2D* pSection = static_cast<CSection_2D*>(SECTION_MGR->Find_Section(SECTION_MGR->Get_Cur_Section_Key()));


			auto pLayer = pSection->Get_Section_Layer(SECTION_PLAYMAP_2D_RENDERGROUP::SECTION_2D_PLAYMAP_BACKGROUND);

			const auto& Objects = pLayer->Get_GameObjects();

			for_each(Objects.begin(), Objects.end(), [](CGameObject* pGameObject) {
				auto pActionObj = dynamic_cast<C2DMapActionObject*>(pGameObject);

				if (nullptr != pActionObj)
				{
					if (C2DMapActionObject::ACTIVE_TYPE_DYNAMIC_BACKGROUND == pActionObj->Get_ActionType())
						pActionObj->Ready_Action();
				}
				});
		}

		Next_Step_Over(2.2f);
	}
	// 이펙트
	else if (Step_Check(STEP_2))
	{
		if (Is_Start())
		{
			// 딜탐, 인덱스, 루프, 라이프타임 
			CEffect2D_Manager::GetInstance()->Play_Effect(L"beam", SECTION_MGR->Get_Cur_Section_Key(), XMMatrixTranslation(-236.f, 217.f, 0.f), 2.8f, 0, true, 999.f);// 빔쏘는거
			CEffect2D_Manager::GetInstance()->Play_Effect(L"EffectBack", SECTION_MGR->Get_Cur_Section_Key(), XMMatrixTranslation(-276.f, 181.4f, 0.f), 0.f, 0); // 백 시작
			CEffect2D_Manager::GetInstance()->Play_Effect(L"EffectBack", SECTION_MGR->Get_Cur_Section_Key(), XMMatrixTranslation(-276.f, 181.4f, 0.f), 0.8f, 1, true, 999.f); //백 루프
			CEffect2D_Manager::GetInstance()->Play_Effect(L"hum", SECTION_MGR->Get_Cur_Section_Key(), XMMatrixTranslation(-276.f, 170.4f, 0.f), 0.f, 0);
			CEffect2D_Manager::GetInstance()->Play_Effect(L"hum", SECTION_MGR->Get_Cur_Section_Key(), XMMatrixTranslation(-276.f, 170.4f, 0.f), 1.8f, 1, false, 999.f);
			CEffect2D_Manager::GetInstance()->Play_Effect(L"storm", SECTION_MGR->Get_Cur_Section_Key(), XMMatrixTranslation(10.f, 280.f, 0.f), 3.0f, 0, false);
			CEffect2D_Manager::GetInstance()->Play_Effect(L"storm", SECTION_MGR->Get_Cur_Section_Key(), XMMatrixTranslation(10.f, 280.f, 0.f), 3.6f, 1, true, 999.f);
		}

		if (m_fTimer > 3.0f && !m_isPlag)
		{
			m_isPlag = true;
			m_pGameInstance->Load_Lights(TEXT("../Bin/DataFiles/DirectLights/Chapter2_Night_Main.json"));
			m_pGameInstance->Load_IBL(TEXT("../Bin/DataFiles/IBL/Chapter2_Night_Main.json"));
		}

		Next_Step_Over(4.5f);
	}
	else if (Step_Check(STEP_3))
	{
		if (Is_Start())
		{
			auto pBook = Get_Book();
			if (nullptr != pBook)
				pBook->Execute_AnimEvent(5);
			CEffect_Manager::GetInstance()->Active_EffectPosition(TEXT("Book_MagicDust"), true, XMVectorSet(2.f, 0.4f, -17.3f, 1.f));
			CCamera_2D* pCamera = static_cast<CCamera_2D*>(CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET_2D));
			pCamera->Set_CameraMode(CCamera_2D::MOVE_TO_CUSTOMARM);

			ARM_DATA tData = {};
			tData.fMoveTimeAxisRight = { 5.f, 0.f };
			tData.fRotationPerSecAxisRight = { XMConvertToRadians(-10.f), XMConvertToRadians(-1.f) };
			tData.iRotationRatioType = EASE_IN_OUT;
			tData.fLength = 20.f;
			tData.fLengthTime = { 5.f, 0.f };
			tData.iLengthRatioType = EASE_OUT;

			pCamera->Set_CustomArmData(tData);

			pCamera->Start_Shake_ByCount(0.2f, 0.1f, 10, CCamera::SHAKE_XY);
			pCamera->Start_Changing_AtOffset(3.f, XMVectorSet(-0.7f, 2.f, 0.f, 0.f), EASE_IN_OUT);
		}

		if (m_fTimer >= 5.8f && m_fTimer - _fTimeDelta <= 5.8f) {
			static_cast<CCamera_2D*>(CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET_2D))->Start_Shake_ByCount(0.2f, 0.1f, 10, CCamera::SHAKE_XY);
		}

		Next_Step_Over(6.5f);
	}
	else if (Step_Check(STEP_4))
	{
		if (Is_Start())
		{
			CCamera_2D* pCamera = static_cast<CCamera_2D*>(CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET_2D));
			pCamera->Set_NextArmData(TEXT("Book_Horizon"), 0);
			pCamera->Start_Changing_AtOffset(0.5f, XMVectorSet(0.f, 0.f, 0.f, 0.f), EASE_IN_OUT);
			pCamera->Set_CameraMode(CCamera_2D::MOVE_TO_NEXTARM);

			CGameObject* pPlayer = Get_Player();
			CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET_2D)->Change_Target(pPlayer);
		}

		Next_Step(CCamera_2D::DEFAULT == CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET_2D)->Get_CameraMode());
	}
	// 바닥 부서지고, 플레이어 떨구는거. 
	else if (Step_Check(STEP_5))
	{
		if (Is_Start())
		{
			CSection_2D* pSection = static_cast<CSection_2D*>(SECTION_MGR->Find_Section(SECTION_MGR->Get_Cur_Section_Key()));


			auto pLayer = pSection->Get_Section_Layer(SECTION_PLAYMAP_2D_RENDERGROUP::SECTION_2D_PLAYMAP_OBJECT);

			const auto& Objects = pLayer->Get_GameObjects();

			for_each(Objects.begin(), Objects.end(), [](CGameObject* pGameObject) {
				auto pActionObj = dynamic_cast<C2DMapActionObject*>(pGameObject);

				if (nullptr != pActionObj)
				{
					if (C2DMapActionObject::ACTIVE_TYPE_ACTIONANIM == pActionObj->Get_ActionType())
						pActionObj->Ready_Action();
				}
				});
		}

		if (m_iSubStep == 0 && m_fTimer > 0.6f)
		{
			//플레이어 떨궈 
			auto pPlayer = Get_Player();
			if (nullptr != pPlayer)
				pPlayer->Set_GravityCompOn(true);
			m_iSubStep++;

		}

		if (m_iSubStep == 1 && m_fTimer > 2.2f)
		{
			//플레이어 떨굼 멈추고 렌더 꺼 
			auto pPlayer = Get_Player();
			if (nullptr != pPlayer)
			{
				pPlayer->Set_GravityCompOn(false);
				pPlayer->Set_Render(false);
			}
			// 태웅 추가 : 여기서 찍찍이 사망.
			CFriend* pPip = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Pip"));
			Event_DeleteObject(pPip);
			CFriend_Controller::GetInstance()->Erase_Friend(TEXT("Pip"));
			CFriend_Controller::GetInstance()->Erase_Friend_FromTrainList(TEXT("Pip"));
			// 태웅 추가 : 여기서 찍찍이 사망.
			m_iSubStep++;
		}


		Next_Step_Over(3.f);
	}
	else if (Step_Check(STEP_6))
	{
		// 책장 넘겨
		if (Is_Start())
		{
			_float3 vPos = { 0.f, 0.f, 1.f };

			Event_Book_Main_Section_Change_Start(1, &vPos);
			CSection* pSection = SECTION_MGR->Find_Section(L"Chapter1_P0708");
			if (nullptr != pSection)
				static_cast<CSection_2D*>(pSection)->Set_NextPageTag(L"Chapter2_P0102");
		}

		Next_Step_Over(3.f);
	}
	else if (Step_Check(STEP_7))
	{
		if (Is_Start())
		{
			m_pGameInstance->Load_Lights(TEXT("../Bin/DataFiles/DirectLights/Chapter2_Bright.json"));
			m_pGameInstance->Load_IBL(TEXT("../Bin/DataFiles/IBL/Chapter2_Bright.json"));
		}
	}
	else
		GameEvent_End();
}

void CGameEventExecuter_C2::Chapter2_Intro(_float _fTimeDelta)
{
	m_fTimer += _fTimeDelta;
	if (Step_Check(STEP_0))
	{
		if (Is_Start())
		{
			CCamera_Manager::GetInstance()->Change_CameraType(CCamera_Manager::CUTSCENE);
			CCamera_Manager::GetInstance()->Set_NextCutSceneData(TEXT("Chapter2_Intro"));

		}

		if (CSection_Manager::GetInstance()->Is_WordPos_Capcher()) {
			CBook* pBook = static_cast<CBook*>(m_pGameInstance->Get_GameObject_Ptr(LEVEL_CHAPTER_2, TEXT("Layer_Book"), 0));
			pBook->Set_Animation(CBook::CLOSED_IDLE);

			Next_Step(true);
		}
	}
	else if (Step_Check(STEP_1))
	{
		Next_Step_Over(2.8f);
	}
	else if (Step_Check(STEP_2))
	{
		if (Is_Start())
		{
			CRoom_Door* pDoor = static_cast<CRoom_Door*>(m_pGameInstance->Get_GameObject_Ptr(LEVEL_CHAPTER_2, TEXT("Layer_RoomDoor"), 0));
			pDoor->Start_Turn_DoorKnob(true);
		}
		Next_Step_Over(0.5f);
	}
	else if(Step_Check(STEP_3))
	{
		CRoom_Door* pDoor = static_cast<CRoom_Door*>(m_pGameInstance->Get_GameObject_Ptr(LEVEL_CHAPTER_2, TEXT("Layer_RoomDoor"), 0));

		if (Is_Start())
		{
			pDoor->Start_Turn_Door(true);
		}
		
		if (false == pDoor->Is_Turn_DoorKnob()) {
			pDoor->Start_Turn_DoorKnob(true);
			Next_Step(true);
		}
	}
	else if (Step_Check(STEP_4)) {

		CCamera_CutScene* pCamera = static_cast<CCamera_CutScene*>(CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::CUTSCENE));

		if (true == pCamera->Is_Finish_CutScene()) {
			if (m_fTimer >= 1.3f) {
				CBook* pBook = static_cast<CBook*>(m_pGameInstance->Get_GameObject_Ptr(LEVEL_CHAPTER_2, TEXT("Layer_Book"), 0));
				pBook->Set_Animation(CBook::OPEN_TO_FLAT);
				Next_Step(true);
			}
		}
	}
	else if(Step_Check(STEP_5)) {
		
		if (m_fTimer >= 3.f) {
			CCamera_Manager::GetInstance()->Change_CameraType(CCamera_Manager::TARGET_2D, true, 0.5f);
			Next_Step(true);
		}
	}
	else if (Step_Check(STEP_6)) {
		if (m_fTimer >= 0.9f) {
			CUI_Manager::GetInstance()->Set_PlayNarration(TEXT("Chapter1_P0102_Narration_01"));
			Next_Step(true);
		}
	}
	else {
		GameEvent_End();
	}
}

void CGameEventExecuter_C2::Chapter2_Humgrump(_float _fTimeDelta)
{
	m_fTimer += _fTimeDelta;

	if (Step_Check(STEP_0))
	{

		if (Is_Start())
		{
			static_cast<CMagic_Hand*>(m_pGameInstance->Get_GameObject_Ptr(LEVEL_CHAPTER_2, TEXT("Layer_MagicHand"), 0))->Set_Start(true);

			m_pGameInstance->Load_Lights(TEXT("../Bin/DataFiles/DirectLights/Chapter2_Night_Main.json"));
			m_pGameInstance->Load_IBL(TEXT("../Bin/DataFiles/IBL/Chapter2_Night_Main.json"));
			

			CCamera_2D* pCamera = static_cast<CCamera_2D*>(CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET_2D));

			ARM_DATA tData = {};
			tData.fMoveTimeAxisRight = { 1.5f, 0.f };
			tData.fRotationPerSecAxisRight = { XMConvertToRadians(-11.f), XMConvertToRadians(-1.f) };
			tData.iRotationRatioType = EASE_IN_OUT;
			tData.fLength = 9.0f;
			tData.fLengthTime = { 1.5f, 0.f };
			tData.iLengthRatioType = EASE_IN_OUT;

			pCamera->Set_CustomArmData(tData);
			pCamera->Set_CameraMode(CCamera_2D::MOVE_TO_CUSTOMARM);
			pCamera->Start_Changing_AtOffset(1.5f, XMVectorSet(0.f, 0.f, -3.f, 0.f), EASE_IN_OUT);
			pCamera->Start_Zoom(1.5f, (CCamera::ZOOM_LEVEL)CCamera::LEVEL_5, EASE_IN_OUT);
			//pCamera->Start_Changing_AtOffset(3.f, XMVectorSet(0.f, 4.f, 0.f, 0.f), EASE_IN_OUT);

		}

		Change_PlayMap();

		CPlayer* pPlayer = Get_Player();

		if (COORDINATE_3D == pPlayer->Get_CurCoord())
		{
			CCamera_Manager::GetInstance()->Change_CameraType(CCamera_Manager::CUTSCENE);
			CCamera_Manager::GetInstance()->Set_NextCutSceneData(TEXT("CutScene_Humgrump"));

			Next_Step(true);
		}
	}
	else if (Step_Check(STEP_1)) {

		if (m_fTimer >= 13.f) {
			CSection* pSection = CSection_Manager::GetInstance()->Find_Section(TEXT("Chapter2_P1314"));

			// Event Trigger 생성
			CTriggerObject::TRIGGEROBJECT_DESC Desc = {};
			Desc.vHalfExtents = { 10.f, 10.f, 0.f };
			Desc.iTriggerType = (_uint)TRIGGER_TYPE::EVENT_TRIGGER;
			Desc.szEventTag = TEXT("Chapter2_After_Opening_Book");
			Desc.eConditionType = CTriggerObject::TRIGGER_ENTER;
			Desc.isReusable = false; // 한 번 하고 삭제할 때
			Desc.eStartCoord = COORDINATE_2D;
			Desc.tTransform2DDesc.vInitialPosition = { 70.f, 130.f, 0.f };

			CTrigger_Manager::GetInstance()->Create_TriggerObject(LEVEL_STATIC, LEVEL_CHAPTER_2, &Desc, pSection);
			
			// 기존 NPC 삭제

			// 스레쉬와 바이올렛 생성

			// 책 상태 변경
			CBook* pBook = Get_Book();
			pBook->Switch_Animation(CBook::CLOSED_IDLE);
			CFriend* pThrash = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Thrash"));
			CFriend* pViolet = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Violet"));
			CSection_Manager::GetInstance()->Remove_GameObject_FromSectionLayer(TEXT("Chapter2_P1112"), pThrash);
			CSection_Manager::GetInstance()->Remove_GameObject_FromSectionLayer(TEXT("Chapter2_P1112"), pViolet);

			CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter2_P1314"), pThrash);
			CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter2_P1314"), pViolet);
			_vector vPortalPos = XMVectorSetW(XMLoadFloat3(&Desc.tTransform2DDesc.vInitialPosition), 1.0f);
			pThrash->Set_Position(vPortalPos + XMVectorSet(-100.f, -100.f, 0.0f, 1.0f));
			pThrash->Set_Direction(CFriend::DIR_RIGHT);
			pViolet->Set_Position(vPortalPos + XMVectorSet(100.f, -100.f, 0.0f, 1.0f));
			pViolet->Set_Direction(CFriend::DIR_LEFT);
			GameEvent_End();
		}
	}
}

void CGameEventExecuter_C2::Chapter2_Lunchbox_Appear(_float _fTimeDelta)
{
	m_fTimer += _fTimeDelta;

	if (Step_Check(STEP_0))
	{
		if (Is_Start())
		{
			CPlayer* pPlayer = Get_Player();
			pPlayer->Set_BlockPlayerInput(true);
			CCamera_Manager::GetInstance()->Set_ResetData(CCamera_Manager::TARGET);

			CCamera_Manager::GetInstance()->Start_Changing_ArmLength_Increase(CCamera_Manager::TARGET, 1.5f, 25.f, EASE_IN_OUT);
			/*
			CCamera_Manager::GetInstance()->Start_Zoom(CCamera_Manager::TARGET, 1.f,
				CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET)->Get_CurrentZoomLevel() + 5, EASE_IN_OUT
			);*/
		}
		Next_Step_Over(1.5f);
	}
	else if (Step_Check(STEP_1))
	{
		if (Is_Start())
		{

			CCamera_Manager::GetInstance()->Start_Changing_AtOffset(CCamera_Manager::TARGET,
				0.5f,
				XMVectorSet(-10.f, -3.f, 0.f, 0.f),
				EASE_IN_OUT
			);
			CCamera_Manager::GetInstance()->Start_Changing_ArmLength_Decrease(CCamera_Manager::TARGET, 1.f, 10.f, EASE_IN_OUT);

		}
		Next_Step_Over(1.f);

	}
	else if (Step_Check(STEP_2))
	{
		if (Is_Start())
			CDialog_Manager::GetInstance()->Set_DialogId(L"Postit_Page_01");
		else
			Next_Step(!CDialog_Manager::GetInstance()->Get_DisPlayDialogue());
	}
	else if (Step_Check(STEP_3))
	{
		if (Is_Start())
		{
			CPlayer* pPlayer = Get_Player();
			pPlayer->Set_BlockPlayerInput(false);
			CCamera_Manager::GetInstance()->Start_ResetArm_To_SettingPoint(CCamera_Manager::TARGET, 1.f);
		}

		Next_Step(true);
	}
	else
	{
		GameEvent_End();
	}
}

void CGameEventExecuter_C2::Chapter2_Lunchbox_Open(_float _fTimeDelta)
{
	m_fTimer += _fTimeDelta;

	if (Step_Check(STEP_0))
	{
		if (Is_Start())
		{
			CPlayer* pPlayer = Get_Player();
			pPlayer->Set_BlockPlayerInput(true);

		}
		Next_Step_Over(1.5f);
	}
	else if (Step_Check(STEP_1))
	{
		if (Is_Start())
		{
			CCamera_Manager::GetInstance()->Set_ResetData(CCamera_Manager::TARGET);

			CCamera_Manager::GetInstance()->Start_Zoom(CCamera_Manager::TARGET, 1.f,
				CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET)->Get_CurrentZoomLevel() -3, EASE_IN_OUT
			);

			CCamera_Manager::GetInstance()->Start_Changing_AtOffset(CCamera_Manager::TARGET,
				1.f,
				XMVectorSet(0.f, 3.f, 0.f, 0.f),
				EASE_IN_OUT
				);

			CCamera_Manager::GetInstance()->Start_Turn_AxisY(CCamera_Manager::TARGET, 1.f, XMConvertToRadians(-20.f), XMConvertToRadians(-2.f));
			CCamera_Manager::GetInstance()->Start_Turn_AxisRight(CCamera_Manager::TARGET, 1.f, XMConvertToRadians(-10.f), XMConvertToRadians(-2.f));

		}
		Next_Step_Over(1.5f);
	}
	else if (Step_Check(STEP_2))
	{

		if (Is_Start())
		{
			CSection_2D* pSection = static_cast<CSection_2D*>(SECTION_MGR->Find_Section(L"Chapter2_SKSP_Postit"));
			auto pLayer = pSection->Get_Section_Layer(SECTION_PLAYMAP_2D_RENDERGROUP::SECTION_2D_PLAYMAP_BACKGROUND);

			const auto& Objects = pLayer->Get_GameObjects();
			if (Objects.size() != 1)
				assert(nullptr);
			m_TargetObjects.push_back(Objects.front());
			if (nullptr == m_TargetObjects[0])
				assert(nullptr);

			CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_TargetObjects[0]);
			if (nullptr != pPage)
			{
				pPage->Set_Render(true);
				pPage->Anim_Action(CPostit_Page::POSTIT_PAGE_APPEAR,false);
			}

		}
		else 
		{
			if (nullptr == m_TargetObjects[0])
				assert(nullptr);
			CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_TargetObjects[0]);
			
			Next_Step(!pPage->Is_DuringAnimation() && m_fTimer > 1.8f);
		}
	}
	else if (Step_Check(STEP_3))
	{
		if (Is_Start())
		{
			CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_TargetObjects[0]);
			pPage->Anim_Action(CPostit_Page::POSTIT_PAGE_TALK_1, true);
			CDialog_Manager::GetInstance()->Set_DialogId(L"Postit_Page_02");
		}
		else
			Next_Step(!CDialog_Manager::GetInstance()->Get_DisPlayDialogue());
	}
	else if (Step_Check(STEP_4))
	{
		if (Is_Start())
		{
			CPlayer* pPlayer = Get_Player();
			pPlayer->Set_BlockPlayerInput(false);
			if (nullptr == m_TargetObjects[0])
				assert(nullptr);
			CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_TargetObjects[0]);

			pPage->Anim_Action(CPostit_Page::POSTIT_PAGE_DISAPPEAR, false);
		}
		else
		{
			if (nullptr == m_TargetObjects[0])
				assert(nullptr);
			CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_TargetObjects[0]);

			if (Next_Step(!pPage->Is_DuringAnimation()))
			{
				CCamera_Manager::GetInstance()->Start_ResetArm_To_SettingPoint(CCamera_Manager::TARGET,0.5f);
				pPage->Set_Render(false);
			}
		}
	}

	else
	{
		GameEvent_End();
	}
}

void CGameEventExecuter_C2::Chapter2_Bettle_Page(_float _fTimeDelta)
{
	m_fTimer += _fTimeDelta;

	if (Step_Check(STEP_0))
	{
		if (Is_Start())
		{
			CPlayer* pPlayer = Get_Player();
			pPlayer->Set_BlockPlayerInput(true);

		}
		Next_Step_Over(0.5f);
	}
	else if (Step_Check(STEP_1))
	{
		if (Is_Start())
		{
			CCamera_Manager::GetInstance()->Set_ResetData(CCamera_Manager::TARGET);

			CCamera_Manager::GetInstance()->Start_Zoom(CCamera_Manager::TARGET, 1.f,
				CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET)->Get_CurrentZoomLevel() - 1, EASE_IN_OUT
			);

			CCamera_Manager::GetInstance()->Start_Changing_AtOffset(CCamera_Manager::TARGET,
				1.f,
				XMVectorSet(0.f, 3.5f, 0.f, 0.f),
				EASE_IN_OUT
			);

	/*		CCamera_Manager::GetInstance()->Start_Turn_AxisY(CCamera_Manager::TARGET, 1.f, XMConvertToRadians(-20.f), XMConvertToRadians(-2.f));
			CCamera_Manager::GetInstance()->Start_Turn_AxisRight(CCamera_Manager::TARGET, 1.f, XMConvertToRadians(-10.f), XMConvertToRadians(-2.f));*/

		}
		Next_Step_Over(1.5f);
	}
	else if (Step_Check(STEP_2))
	{

		if (Is_Start())
		{
			CSection_2D* pSection = static_cast<CSection_2D*>(SECTION_MGR->Find_Section(L"Chapter2_SKSP_Postit"));
			auto pLayer = pSection->Get_Section_Layer(SECTION_PLAYMAP_2D_RENDERGROUP::SECTION_2D_PLAYMAP_BACKGROUND);

			const auto& Objects = pLayer->Get_GameObjects();
			if (Objects.size() != 1)
				assert(nullptr);
			m_TargetObjects.push_back(Objects.front());
			if (nullptr == m_TargetObjects[0])
				assert(nullptr);

			CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_TargetObjects[0]);
			if (nullptr != pPage)
			{
				pPage->Set_Render(true);
				pPage->Anim_Action(CPostit_Page::POSTIT_PAGE_APPEAR, false);
			}

		}
		else
		{
			if (nullptr == m_TargetObjects[0])
				assert(nullptr);
			CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_TargetObjects[0]);

			Next_Step(!pPage->Is_DuringAnimation() && m_fTimer > 1.8f);
		}
	}
	else if (Step_Check(STEP_3))
	{
		if (Is_Start())
		{
			CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_TargetObjects[0]);
			pPage->Anim_Action(CPostit_Page::POSTIT_PAGE_TALK_1, true);
			CDialog_Manager::GetInstance()->Set_DialogId(L"Postit_Page_03");
		}
		else
			Next_Step(!CDialog_Manager::GetInstance()->Get_DisPlayDialogue());
	}
	else if (Step_Check(STEP_4))
	{
		if (Is_Start())
		{
			CPlayer* pPlayer = Get_Player();
			pPlayer->Set_BlockPlayerInput(false);
			if (nullptr == m_TargetObjects[0])
				assert(nullptr);
			CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_TargetObjects[0]);

			pPage->Anim_Action(CPostit_Page::POSTIT_PAGE_DISAPPEAR, false);
		}
		else
		{
			if (nullptr == m_TargetObjects[0])
				assert(nullptr);
			CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_TargetObjects[0]);

			if (Next_Step(!pPage->Is_DuringAnimation()))
			{
				CCamera_Manager::GetInstance()->Start_ResetArm_To_SettingPoint(CCamera_Manager::TARGET, 0.5f);
				pPage->Set_Render(false);
			}
		}
	}

	else
	{
		GameEvent_End();
	}
}

void CGameEventExecuter_C2::Chapter2_OpenBookEvent(_float _fTimeDelta)
{
	m_fTimer += _fTimeDelta;

	if (Step_Check(STEP_0))
	{
		if (Is_Start())
		{
			CPlayer* pPlayer = Get_Player();
			pPlayer->Set_BlockPlayerInput(true);

		}
		Next_Step(true);
	}
	else if (Step_Check(STEP_1))
	{
		if (Is_Start())
		{
			CCamera_Manager::GetInstance()->Set_ResetData(CCamera_Manager::TARGET);

			CCamera_Manager::GetInstance()->Start_Zoom(CCamera_Manager::TARGET, 1.f,
				CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET)->Get_CurrentZoomLevel() - 1, EASE_IN_OUT
			);

			CCamera_Manager::GetInstance()->Start_Changing_AtOffset(CCamera_Manager::TARGET,
				1.f,
				XMVectorSet(3.f, 3.f, 0.f, 0.f),
				EASE_IN_OUT
			);

			/*		CCamera_Manager::GetInstance()->Start_Turn_AxisY(CCamera_Manager::TARGET, 1.f, XMConvertToRadians(-20.f), XMConvertToRadians(-2.f));
					CCamera_Manager::GetInstance()->Start_Turn_AxisRight(CCamera_Manager::TARGET, 1.f, XMConvertToRadians(-10.f), XMConvertToRadians(-2.f));*/

		}
		Next_Step_Over(2.f);
	}
	else if (Step_Check(STEP_2))
	{

		if (Is_Start())
		{
			CSection_2D* pSection = static_cast<CSection_2D*>(SECTION_MGR->Find_Section(L"Chapter2_SKSP_Postit"));
			auto pLayer = pSection->Get_Section_Layer(SECTION_PLAYMAP_2D_RENDERGROUP::SECTION_2D_PLAYMAP_BACKGROUND);

			const auto& Objects = pLayer->Get_GameObjects();
			if (Objects.size() != 1)
				assert(nullptr);
			m_TargetObjects.push_back(Objects.front());
			if (nullptr == m_TargetObjects[0])
				assert(nullptr);

			CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_TargetObjects[0]);
			if (nullptr != pPage)
			{
				pPage->Set_Render(true);
				pPage->Anim_Action(CPostit_Page::POSTIT_PAGE_APPEAR, false);
			}

		}
		else
		{
			if (nullptr == m_TargetObjects[0])
				assert(nullptr);
			CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_TargetObjects[0]);

			Next_Step(!pPage->Is_DuringAnimation() && m_fTimer > 1.8f);
		}
	}
	else if (Step_Check(STEP_3))
	{
		if (Is_Start())
		{
			CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_TargetObjects[0]);
			pPage->Anim_Action(CPostit_Page::POSTIT_PAGE_TALK_1, true);
			CDialog_Manager::GetInstance()->Set_DialogId(L"Postit_Page_04");
		}
		else
			Next_Step(!CDialog_Manager::GetInstance()->Get_DisPlayDialogue());
	}
	else if (Step_Check(STEP_4))
	{
		if (Is_Start())
		{

			CCamera_Manager::GetInstance()->Start_Zoom(CCamera_Manager::TARGET, 1.5f,
				CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET)->Get_CurrentZoomLevel() - 1, EASE_IN_OUT
			);

			CCamera_Manager::GetInstance()->Start_Changing_ArmLength_Increase(CCamera_Manager::TARGET, 1.5f, 30.f, EASE_IN_OUT);

			CCamera_Manager::GetInstance()->Start_Changing_AtOffset(CCamera_Manager::TARGET,
				1.f,
				XMVectorSet(21.f, 0.f, 0.f, 0.f),
				EASE_IN_OUT
			);
			CCamera_Manager::GetInstance()->Start_Turn_AxisY(CCamera_Manager::TARGET, 1.f, XMConvertToRadians(20.f), XMConvertToRadians(2.f));
			CCamera_Manager::GetInstance()->Start_Turn_AxisRight(CCamera_Manager::TARGET, 1.f, XMConvertToRadians(-30.f), XMConvertToRadians(-4.f));

		}
		Next_Step_Over(2.5f);
	}
	else if (Step_Check(STEP_5))
	{
		if (Is_Start())
		{
			CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_TargetObjects[0]);
			pPage->Anim_Action(CPostit_Page::POSTIT_PAGE_TALK_1, true);
			CDialog_Manager::GetInstance()->Set_DialogId(L"Postit_Page_05");
		}
		else
			Next_Step(!CDialog_Manager::GetInstance()->Get_DisPlayDialogue());
	}
	else if (Step_Check(STEP_6))
	{
		if (Is_Start())
		{
			CPlayer* pPlayer = Get_Player();
			pPlayer->Set_BlockPlayerInput(false);
			if (nullptr == m_TargetObjects[0])
				assert(nullptr);
			CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_TargetObjects[0]);

			pPage->Anim_Action(CPostit_Page::POSTIT_PAGE_DISAPPEAR, false);
		}
		else
		{
			if (nullptr == m_TargetObjects[0])
				assert(nullptr);
			CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_TargetObjects[0]);

			if (Next_Step(!pPage->Is_DuringAnimation()))
			{
				CCamera_Target* pCamera = static_cast<CCamera_Target*>(CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET_2D));
				//pCamera->Start_PostProcessing_Fade(CCamera::FADE_OUT, 0.5f);
				CCamera_Manager::GetInstance()->Start_ResetArm_To_SettingPoint(CCamera_Manager::TARGET, 1.5f);
				pPage->Set_Render(false);
			}
		}
	}
	else if (Step_Check(STEP_7))
	{
		Next_Step_Over(0.3f);
	}
	else if (Step_Check(STEP_8))
	{
		if (Is_Start())
		{
			CCamera_Target* pCamera = static_cast<CCamera_Target*>(CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET_2D));

			//pCamera->Start_PostProcessing_Fade(CCamera::FADE_IN, 0.3f);
			CPlayer* pPlayer = Get_Player();
			pPlayer->Set_BlockPlayerInput(false);
		}
	}
	else
	{
		GameEvent_End();
	}
}

void CGameEventExecuter_C2::Chapter2_StorySequence(_float _fTimeDelta)
{
	m_fTimer += _fTimeDelta;

	if (Step_Check(STEP_0))
	{
		// 키 인풋 막기.
		if (Is_Start())
		{
			CPlayer* pPlayer = Get_Player();
			pPlayer->Set_BlockPlayerInput(true);

		}
		Next_Step(true);
	}
	else if (Step_Check(STEP_1))
	{
		if (Is_Start())
		{
			// 암 1 땡기기
			CCamera_Manager::GetInstance()->Start_Changing_ArmLength_Increase(CCamera_Manager::TARGET, 1.f,
				1.f, EASE_IN_OUT);

			// 암 x3 y2 이동
			CCamera_Manager::GetInstance()->Start_Changing_AtOffset(CCamera_Manager::TARGET,
				1.f,
				XMVectorSet(3.f, 2.f, 0.f, 0.f),
				EASE_IN_OUT
			);
		}
		// 카메라 무브 후 1초 인터벌
		Next_Step_Over(2.f);
	}
	else if (Step_Check(STEP_2))
	{
		if (Is_Start())
		{
			// 책벌레 Appear 시작
			CSection_2D* pSection = static_cast<CSection_2D*>(SECTION_MGR->Find_Section(L"Chapter2_SKSP_Postit"));
			auto pLayer = pSection->Get_Section_Layer(SECTION_PLAYMAP_2D_RENDERGROUP::SECTION_2D_PLAYMAP_BACKGROUND);

			const auto& Objects = pLayer->Get_GameObjects();
			if (Objects.size() != 1)
				assert(nullptr);
			m_TargetObjects.push_back(Objects.front());
			if (nullptr == m_TargetObjects[0])
				assert(nullptr);

			CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_TargetObjects[0]);
			if (nullptr != pPage)
			{
				pPage->Set_Render(true);
				pPage->Anim_Action(CPostit_Page::POSTIT_PAGE_APPEAR, false);
			}

		}
		else
		{
			if (nullptr == m_TargetObjects[0])
				assert(nullptr);
			CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_TargetObjects[0]);
			// 애니메이션 끝나고, 인터벌 조금 준뒤 다음 다이얼로그 스타트
			Next_Step(!pPage->Is_DuringAnimation() && m_fTimer > 1.5f);
		}
	}
	else if (Step_Check(STEP_3))
	{
		if (Is_Start())
		{
			// 말함
			CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_TargetObjects[0]);
			pPage->Anim_Action(CPostit_Page::POSTIT_PAGE_TALK_1, true);
			CDialog_Manager::GetInstance()->Set_DialogId(L"Postit_Page_06");
		}
		else
			Next_Step(!CDialog_Manager::GetInstance()->Get_DisPlayDialogue());
	}
	else if (Step_Check(STEP_4))
	{
		if (Is_Start())
		{

			CONST_LIGHT LightDesc{};

			ZeroMemory(&LightDesc, sizeof LightDesc);

			LightDesc.vPosition = { 66.10f, 10.10f, -24.90f };
			LightDesc.vDirection = _float3(.0f, .0f, .0f);
			LightDesc.vRadiance = _float3(5.0f, 9.0f, 7.0f);
			LightDesc.vDiffuse = _float4(0.6f, 0.6f, 0.6f, 1.0f);
			LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 1.0f);
			LightDesc.vSpecular = _float4(0.4f, 0.9f, 0.7f, 1.0f);
			LightDesc.fFallOutStart = 2.738f;
			LightDesc.fFallOutEnd = 12.24f;
			LightDesc.isShadow = true;
			LightDesc.fShadowFactor = 1.f;
			if (FAILED(m_pGameInstance->Add_Light(LightDesc, LIGHT_TYPE::POINT)))
				assert(nullptr);


			// 끝나면 x5로 이동,
			CCamera_Manager::GetInstance()->Start_Changing_AtOffset(CCamera_Manager::TARGET,
				1.f,
				XMVectorSet(7.f, 2.f, 0.f, 0.f),
				EASE_IN_OUT
			);
		}
		Next_Step_Over(1.f);
	}
	else if (Step_Check(STEP_5))
	{
		// 다이얼로그 + 포스트잇 + 광원. 1.
		if (Is_Start())
		{

			CSection_2D* pSection = static_cast<CSection_2D*>(SECTION_MGR->Find_Section(L"Chapter2_SKSP_Postit"));


			auto pLayer = pSection->Get_Section_Layer(SECTION_PLAYMAP_2D_RENDERGROUP::SECTION_2D_PLAYMAP_OBJECT);

			const auto& Objects = pLayer->Get_GameObjects();

			for_each(Objects.begin(), Objects.end(), [](CGameObject* pGameObject) {
				auto pActionObj = dynamic_cast<C2DMapActionObject*>(pGameObject);

				if (nullptr != pActionObj)
				{
					if (C2DMapActionObject::ACTIVE_TYPE_ACTIONANIM == pActionObj->Get_ActionType()
						&& pActionObj->Get_MapObjectModelName() == L"Postit_02"
						)
						pActionObj->Ready_Action();
				}
				});


			CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_TargetObjects[0]);
			pPage->Anim_Action(CPostit_Page::POSTIT_PAGE_TALK_1, true);
			CDialog_Manager::GetInstance()->Set_DialogId(L"Postit_Page_07");
		}
		else
			Next_Step(!CDialog_Manager::GetInstance()->Get_DisPlayDialogue());
	}
	else if (Step_Check(STEP_6))
	{
		if (Is_Start())
		{
			// 끝나면 x5로 이동,
			CCamera_Manager::GetInstance()->Start_Changing_AtOffset(CCamera_Manager::TARGET,
				1.f,
				XMVectorSet(12.f, 2.f, 0.f, 0.f),
				EASE_IN_OUT
			);

		}
		Next_Step_Over(1.f);
	}
	else if (Step_Check(STEP_7))
	{
		// 다이얼로그 + 포스트잇 2.
		if (Is_Start())
		{
			CSection_2D* pSection = static_cast<CSection_2D*>(SECTION_MGR->Find_Section(L"Chapter2_SKSP_Postit"));


			auto pLayer = pSection->Get_Section_Layer(SECTION_PLAYMAP_2D_RENDERGROUP::SECTION_2D_PLAYMAP_OBJECT);

			const auto& Objects = pLayer->Get_GameObjects();

			for_each(Objects.begin(), Objects.end(), [](CGameObject* pGameObject) {
				auto pActionObj = dynamic_cast<C2DMapActionObject*>(pGameObject);

				if (nullptr != pActionObj)
				{
					if (C2DMapActionObject::ACTIVE_TYPE_ACTIONANIM == pActionObj->Get_ActionType()
						&& pActionObj->Get_MapObjectModelName() == L"Postit_01"
						)
						pActionObj->Ready_Action();
				}
				});


			CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_TargetObjects[0]);
			pPage->Anim_Action(CPostit_Page::POSTIT_PAGE_TALK_1, true);
			CDialog_Manager::GetInstance()->Set_DialogId(L"Postit_Page_08");
		}
		else
			Next_Step(!CDialog_Manager::GetInstance()->Get_DisPlayDialogue());
	}
	else if (Step_Check(STEP_8))
	{
		if (Is_Start())
		{
			// 끝나면 x5로 이동,
			CCamera_Manager::GetInstance()->Start_Changing_AtOffset(CCamera_Manager::TARGET,
				1.f,
				XMVectorSet(11.f, 4.f, 0.f, 0.f),
				EASE_IN_OUT
			);
		}
		Next_Step_Over(1.f);
	}
	else if (Step_Check(STEP_9))
	{
		// 다이얼로그 + 포스트잇 3.
		if (Is_Start())
		{
			CSection_2D* pSection = static_cast<CSection_2D*>(SECTION_MGR->Find_Section(L"Chapter2_SKSP_Postit"));


			auto pLayer = pSection->Get_Section_Layer(SECTION_PLAYMAP_2D_RENDERGROUP::SECTION_2D_PLAYMAP_OBJECT);

			const auto& Objects = pLayer->Get_GameObjects();

			for_each(Objects.begin(), Objects.end(), [](CGameObject* pGameObject) {
				auto pActionObj = dynamic_cast<C2DMapActionObject*>(pGameObject);

				if (nullptr != pActionObj)
				{
					if (C2DMapActionObject::ACTIVE_TYPE_ACTIONANIM == pActionObj->Get_ActionType()
						&& pActionObj->Get_MapObjectModelName() == L"Postit_03"
						)
						pActionObj->Ready_Action();
				}
				});

			CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_TargetObjects[0]);
			pPage->Anim_Action(CPostit_Page::POSTIT_PAGE_TALK_1, true);
			CDialog_Manager::GetInstance()->Set_DialogId(L"Postit_Page_09");
		}
		else
			Next_Step(!CDialog_Manager::GetInstance()->Get_DisPlayDialogue());
	}
	//else if (Step_Check(STEP_10))
	//{
	//	if (Is_Start())
	//	{
	//		//CCamera_Target* pCamera = static_cast<CCamera_Target*>(CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET));
	//		//pCamera->Change_Target(Get_Book(),4.f);
	//		////CCamera_Manager::GetInstance()->Start_Changing_ArmLength_Increase(CCamera_Manager::TARGET, 2.f, 20.f, EASE_IN_OUT);

	//		////CCamera_Manager::GetInstance()->Start_Changing_AtOffset(CCamera_Manager::TARGET,
	//		////	2.5f,
	//		////	XMVectorSet(-41.f, 15.f, 0.f, 0.f),
	//		////	EASE_IN_OUT
	//		////);
	//		//CCamera_Manager::GetInstance()->Start_Turn_AxisY(CCamera_Manager::TARGET, 1.f, XMConvertToRadians(-90.f), XMConvertToRadians(-90.f));
	//		//CCamera_Manager::GetInstance()->Start_Turn_AxisRight(CCamera_Manager::TARGET, 1.f, XMConvertToRadians(45.f), XMConvertToRadians(40.f));

	//	}
	//	else
	//		Next_Step_Over(5.f);
	//}
	else if (Step_Check(STEP_10))
	{
		if (Is_Start())
		{
			CCamera_Manager::GetInstance()->Start_ResetArm_To_SettingPoint(CCamera_Manager::TARGET, 0.01f);
			CCamera_Manager::GetInstance()->Start_FadeIn(1.f);
		}
		Next_Step_Over(1.1f);
	}
	else
	{

		CPlayer* pPlayer = Get_Player();
		pPlayer->Set_BlockPlayerInput(false);
		if (nullptr == m_TargetObjects[0])
			assert(nullptr);
		CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_TargetObjects[0]);

		pPlayer->Set_BlockPlayerInput(false);

		pPage->Anim_Action(CPostit_Page::POSTIT_PAGE_DISAPPEAR, false);
		GameEvent_End();
	}
		
}

void CGameEventExecuter_C2::Chapter2_After_Opening_Book(_float _fTimeDelta)
{
	m_fTimer += _fTimeDelta;

	if (Step_Check(STEP_0)) {
		if (m_fTimer >= 2.f) {
			// Player Lock
			CPlayer* pPlayer = Get_Player();
			pPlayer->Set_BlockPlayerInput(true);

			// 검 줍자는 대화 시작
			CDialog_Manager::GetInstance()->Set_DialogId(L"Dialogue_Into_HumgrumCastle");
			CFriend* pFriend = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Violet"));
			CDialog_Manager::GetInstance()->Set_NPC(pFriend);

			pFriend = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Thrash"));
			CDialog_Manager::GetInstance()->Set_NPC(pFriend);

			Next_Step(true);
		}
	}
	if (Step_Check(STEP_1)) {

		if (false == CDialog_Manager::GetInstance()->Get_DisPlayDialogue()) {
			CPlayer* pPlayer = Get_Player();
			pPlayer->Set_BlockPlayerInput(false);

			CFriend_Controller::GetInstance()->Register_Friend_ToTrainList(TEXT("Thrash"));
			CFriend_Controller::GetInstance()->Register_Friend_ToTrainList(TEXT("Violet"));
			CFriend_Controller::GetInstance()->Start_Train();

			GameEvent_End();
		}
	}
}

void CGameEventExecuter_C2::Chapter2_Going_To_Artia(_float _fTimeDelta)
{
	m_fTimer += _fTimeDelta;

	if (Step_Check(STEP_0)) {
		if (m_fTimer >= 1.5f) {
			// Player Lock
			CPlayer* pPlayer = Get_Player();
			pPlayer->Set_BlockPlayerInput(true);

			// 아르티아 대화 시작
			CDialog_Manager::GetInstance()->Set_DialogId(L"Dialogue_After_Drawing_On_Sword");
			CFriend* pFriend = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Violet"));
			CDialog_Manager::GetInstance()->Set_NPC(pFriend);

			pFriend = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Thrash"));
			CDialog_Manager::GetInstance()->Set_NPC(pFriend);

			Next_Step(true);
		}
	}
	else if (Step_Check(STEP_1)) {
		if (true == CDialog_Manager::GetInstance()->Get_isLastDialog()) {
			Next_Step(true);
		}
	}
	else if (Step_Check(STEP_2)) {

		if (m_fTimer >= 2.f) {
			// Chapter 전환
			CTrigger_Manager::GetInstance()->Register_TriggerEvent(L"Next_Chapter_Event", 0);
			GameEvent_End();
		}
	}
}

void CGameEventExecuter_C2::Chapter2_FriendEvent_0(_float _fTimeDelta)
{
	m_fTimer += _fTimeDelta;
	CCamera_Manager::CAMERA_TYPE eCamType = CCamera_Manager::TARGET_2D;
	CPlayer* pPlayer = Get_Player();

	if (nullptr == pPlayer)
	{
		GameEvent_End();
		return;
	}
	/* 플레이어 인풋락  */
	

	if (Step_Check(STEP_0))
	{
		/* 1. Save Reset ArmData */
		if (Is_Start())
		{
			pPlayer->Set_2DDirection(F_DIRECTION::DOWN);
			pPlayer->Set_BlockPlayerInput(true);
			CCamera_Manager::GetInstance()->Set_ResetData(eCamType);
			m_iDialogueIndex = 0;
		}
		Next_Step_Over(2.0f);
	}
	else if (Step_Check(STEP_1)) // 1. 다이얼로그 시작.
	{
		if (Is_Start())
		{
			// Friend 대화 시작.
			_wstring strDialogueTag = TEXT("Chapter2_FriendEvent_0");
			//strDialogueTag += to_wstring(m_iDialogueIndex);
			CDialog_Manager::GetInstance()->Set_DialogId(strDialogueTag.c_str());
			CFriend* pThrash = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Thrash"));
			CDialog_Manager::GetInstance()->Set_NPC(pThrash);
			CFriend* pViolet = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Violet"));
			CDialog_Manager::GetInstance()->Set_NPC(pViolet);

		}

		/* 6. 다이얼로그 종료 체크 */
		if (false == CDialog_Manager::GetInstance()->Get_DisPlayDialogue())
		{
			Next_Step(true);
		}

	}
	else if (Step_Check(STEP_2)) // 2. 모잼
	{
		if (Is_Start())
		{
			// Friend 대화 시작.
			//_wstring strDialogueTag = TEXT("Mojam");
			////strDialogueTag += to_wstring(m_iDialogueIndex);
			//CDialog_Manager::GetInstance()->Set_DialogId(strDialogueTag.c_str());
			pPlayer->Set_State(CPlayer::MOJAM);
			CFriend* pThrash = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Thrash"));
			CFriend* pViolet = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Violet"));
			pThrash->Change_CurState(CFriend::FRIEND_MOJAM);
			pViolet->Change_CurState(CFriend::FRIEND_MOJAM);
			CFriend_Controller::GetInstance()->Register_Friend_ToTrainList(TEXT("Thrash"));
			CFriend_Controller::GetInstance()->Register_Friend_ToTrainList(TEXT("Violet"));
		}

		/* 6. 다이얼로그 종료 체크 */
		if (false == CDialog_Manager::GetInstance()->Get_DisPlayDialogue() && CPlayer::STATE::IDLE == pPlayer->Get_CurrentStateID())
		{
			Next_Step(true);
		}

	}
	else
	{
		CFriend_Controller::GetInstance()->Start_Train();
		CCamera_Manager::GetInstance()->Start_ResetArm_To_SettingPoint(eCamType, 1.0f);
		pPlayer->Set_BlockPlayerInput(false);
		GameEvent_End();
	}
}

void CGameEventExecuter_C2::Chapter2_FriendEvent_1(_float _fTimeDelta)
{
	m_fTimer += _fTimeDelta;
	CCamera_Manager::CAMERA_TYPE eCamType = CCamera_Manager::TARGET_2D;
	CPlayer* pPlayer = Get_Player();
	CFriend* pThrash = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Thrash"));
	CFriend* pViolet = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Violet"));
	if (nullptr == pPlayer)
	{
		GameEvent_End();
		return;
	}
	/* 플레이어 인풋락  */


	if (Step_Check(STEP_0))
	{
		/* 1. Save Reset ArmData */
		if (Is_Start())
		{
			pPlayer->Set_2DDirection(F_DIRECTION::UP);
			pPlayer->Set_BlockPlayerInput(true);
			CFriend_Controller::GetInstance()->End_Train();
			_vector vPlayerPos = pPlayer->Get_FinalPosition();
			_vector vThrashPos = vPlayerPos + XMVectorSet(100.f, 100.f, 0.0f, 0.0f);
			_vector vVioletPos = vPlayerPos + XMVectorSet(-100.f, 100.f, 0.0f, 0.0f);
			pThrash->Move_Position(_float2(XMVectorGetX(vThrashPos), XMVectorGetY(vThrashPos)), CFriend::DIR_DOWN);
			pViolet->Move_Position(_float2(XMVectorGetX(vVioletPos), XMVectorGetY(vVioletPos)), CFriend::DIR_DOWN);

			CCamera_Manager::GetInstance()->Set_ResetData(eCamType);
			m_iDialogueIndex = 0;
		}
		Next_Step_Over(1.5f);
	}
	else if (Step_Check(STEP_1)) // 1. 다이얼로그 시작.
	{
		if (Is_Start())
		{
			// Friend 대화 시작.
			_wstring strDialogueTag = TEXT("Chapter2_FriendEvent_1");
			//strDialogueTag += to_wstring(m_iDialogueIndex);
			CDialog_Manager::GetInstance()->Set_DialogId(strDialogueTag.c_str());

			CFriend* pThrash = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Thrash"));
			CDialog_Manager::GetInstance()->Set_NPC(pThrash);
			CFriend* pViolet = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Violet"));
			CDialog_Manager::GetInstance()->Set_NPC(pViolet);
		}

		/* 6. 다이얼로그 종료 체크 */
		if (false == CDialog_Manager::GetInstance()->Get_DisPlayDialogue())
		{
			Next_Step(true);
		}

	}
	else
	{

		CCamera_Manager::GetInstance()->Start_ResetArm_To_SettingPoint(eCamType, 1.0f);
		pPlayer->Set_BlockPlayerInput(false);
		GameEvent_End();
	}
}

void CGameEventExecuter_C2::Chapter2_Pip_0(_float _fTimeDelta)
{
	m_fTimer += _fTimeDelta;
	CCamera_Manager::CAMERA_TYPE eCamType = CCamera_Manager::TARGET_2D;
	CPlayer* pPlayer = Get_Player();

	if (nullptr == pPlayer)
	{
		GameEvent_End();
		return;
	}
	/* 플레이어 인풋락  */


	if (Step_Check(STEP_0))
	{
		/* 1. Save Reset ArmData */
		if (Is_Start())
		{
			pPlayer->Set_2DDirection(F_DIRECTION::UP);
			pPlayer->Set_BlockPlayerInput(true);
			CCamera_Manager::GetInstance()->Set_ResetData(eCamType);
			m_iDialogueIndex = 0;
		}
		Next_Step_Over(0.5f);
	}
	else if (Step_Check(STEP_1)) // 1. 다이얼로그 시작.
	{
		if (Is_Start())
		{
			// Friend 대화 시작.
			_wstring strDialogueTag = TEXT("Chapter2_Pip_0");
			//strDialogueTag += to_wstring(m_iDialogueIndex);
			CDialog_Manager::GetInstance()->Set_DialogId(strDialogueTag.c_str());
			CFriend* pPip = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Pip"));
			CDialog_Manager::GetInstance()->Set_NPC(pPip);
		}

		/* 6. 다이얼로그 종료 체크 */
		if (false == CDialog_Manager::GetInstance()->Get_DisPlayDialogue())
		{
			Next_Step(true);
		}

	}
	else if (Step_Check(STEP_2)) // 2. 모잼
	{
		if (Is_Start())
		{
			CFriend* pPip = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Pip"));
			pPip->Change_CurState(CFriend::FRIEND_MOJAM);
			CFriend_Controller::GetInstance()->Register_Friend_ToTrainList(TEXT("Pip"));
		}
		Next_Step(true);
	}
	else
	{
		CFriend_Controller::GetInstance()->Start_Train();
		CCamera_Manager::GetInstance()->Start_ResetArm_To_SettingPoint(eCamType, 1.0f);
		pPlayer->Set_BlockPlayerInput(false);
		GameEvent_End();
	}
}

void CGameEventExecuter_C2::Change_PlayMap()
{
	// 맵 설치
	if (m_fTimer > 1.f && 0 == m_iSubStep)
	{
		m_iSubStep++;
		Event_ChangeMapObject(LEVEL_CHAPTER_2, L"Chapter_02_Play_Desk.mchc", L"Layer_MapObject");
	}

	// 몬스터 추가
	if (m_fTimer > 1.1f && 1 == m_iSubStep)
	{
		m_iSubStep++;
		CGameObject* pObject = nullptr;
		const json* pJson = m_pGameInstance->Find_Json_InLevel(TEXT("Chapter2_Monsters_3D"), m_pGameInstance->Get_CurLevelID());

		if (nullptr == pJson)
			return;
		if (pJson->contains("3D"))
		{
			_wstring strLayerTag = L"Layer_Monster";
			_wstring strMonsterTag = L"";

			for (auto Json : (*pJson)["3D"])
			{
				CMonster::MONSTER_DESC MonsterDesc3D = {};

				MonsterDesc3D.iCurLevelID = m_pGameInstance->Get_CurLevelID();
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

				if (Json.contains("SneakMode"))
				{
					if (Json.contains("SneakWayPointIndex"))
					{
						MonsterDesc3D.eWayIndex = Json["SneakWayPointIndex"];
					}
					MonsterDesc3D.isSneakMode = Json["SneakMode"];
				}

				if (Json.contains("IsStay"))
				{
					MonsterDesc3D.isStay = Json["IsStay"];
				}

				if (Json.contains("IsIgnoreGround"))
				{
					MonsterDesc3D._isIgnoreGround = Json["IsIgnoreGround"];
				}

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, strMonsterTag, m_pGameInstance->Get_CurLevelID(), strLayerTag, &pObject, &MonsterDesc3D)))
					return;
			}
		}
	}

	// 3D 오브젝트 추가 
	if (m_fTimer > 1.2f && 2 == m_iSubStep)
	{

		LEVEL_ID eCurLevelID = (LEVEL_ID)m_pGameInstance->Get_CurLevelID();

		m_iSubStep++;
		//주사위
		CCarriableObject::CARRIABLE_DESC tCarriableDesc{};
		tCarriableDesc.eStartCoord = COORDINATE_3D;
		tCarriableDesc.iCurLevelID = m_pGameInstance->Get_CurLevelID();
		tCarriableDesc.tTransform3DDesc.vInitialPosition = _float3(15.f, 6.8f, 21.5f);
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(eCurLevelID, TEXT("Prototype_GameObject_Dice"), eCurLevelID, TEXT("Layer_Domino"), &tCarriableDesc)))
			return;
		CModelObject::MODELOBJECT_DESC tModelDesc{};
		tModelDesc.eStartCoord = COORDINATE_3D;
		tModelDesc.iCurLevelID = eCurLevelID;
		_float fDominoXPosition = 14.47f;
		_float fDominoYPosition = 1.31f;
		_float fDominoZPosition = 24.3f;
		_float fDominoXPositionStep = -3.5f;
		tModelDesc.tTransform3DDesc.vInitialPosition = _float3(fDominoXPosition, fDominoYPosition, fDominoZPosition);
		tModelDesc.tTransform3DDesc.vInitialScaling = _float3(1.5f, 1.5f, 1.5f);

		tModelDesc.strModelPrototypeTag_3D = TEXT("Domino_4");
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(eCurLevelID, TEXT("Prototype_GameObject_Domino"), eCurLevelID, TEXT("Layer_Domino"), &tModelDesc)))
			return;
		tModelDesc.tTransform3DDesc.vInitialPosition.x += fDominoXPositionStep;
		tModelDesc.strModelPrototypeTag_3D = TEXT("Domino_2");
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(eCurLevelID, TEXT("Prototype_GameObject_Domino"), eCurLevelID, TEXT("Layer_Domino"), &tModelDesc)))
			return;
		tModelDesc.tTransform3DDesc.vInitialPosition.x += fDominoXPositionStep;
		tModelDesc.tTransform3DDesc.vInitialPosition.y += 0.001f;
		tModelDesc.strModelPrototypeTag_3D = TEXT("Domino_3");
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(eCurLevelID, TEXT("Prototype_GameObject_Domino"), eCurLevelID, TEXT("Layer_Domino"), &tModelDesc)))
			return;
		tModelDesc.tTransform3DDesc.vInitialPosition.x += fDominoXPositionStep;
		tModelDesc.strModelPrototypeTag_3D = TEXT("Domino_1");
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(eCurLevelID, TEXT("Prototype_GameObject_Domino"), eCurLevelID, TEXT("Layer_Domino"), &tModelDesc)))
			return;

		// 도미노
		//2번째 도미노
		tCarriableDesc.tTransform3DDesc.vInitialPosition = _float3(48.13f, 2.61f, -5.02f);
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(eCurLevelID, TEXT("Prototype_GameObject_Dice"), eCurLevelID, TEXT("Layer_Domino"), &tCarriableDesc)))
			return;

		fDominoXPosition = 64.5f;
		fDominoYPosition = 0.0;
		fDominoZPosition = -0.54f;
		fDominoXPositionStep = -3.5f;
		tModelDesc.tTransform3DDesc.vInitialPosition = _float3(fDominoXPosition, fDominoYPosition, fDominoZPosition);
		tModelDesc.strModelPrototypeTag_3D = TEXT("Domino_1");
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(eCurLevelID, TEXT("Prototype_GameObject_Domino"), eCurLevelID, TEXT("Layer_Domino"), &tModelDesc)))
			return;


		tModelDesc.tTransform3DDesc.vInitialPosition.x += fDominoXPositionStep;
		tModelDesc.strModelPrototypeTag_3D = TEXT("Domino_3");
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(eCurLevelID, TEXT("Prototype_GameObject_Domino"), eCurLevelID, TEXT("Layer_Domino"), &tModelDesc)))
			return;


		CDraggableObject::DRAGGABLE_DESC tDraggableDesc = {};
		tDraggableDesc.iModelPrototypeLevelID_3D = LEVEL_STATIC;
		tDraggableDesc.iCurLevelID = eCurLevelID;
		tDraggableDesc.strModelPrototypeTag_3D = TEXT("SM_Plastic_Block_04");
		tDraggableDesc.eStartCoord = COORDINATE_3D;
		tDraggableDesc.vBoxHalfExtents = { 1.02f,1.02f,1.02f };
		tDraggableDesc.vBoxOffset = { 0.f,tDraggableDesc.vBoxHalfExtents.y,0.f };
		tDraggableDesc.tTransform3DDesc.vInitialPosition = { -47.f, 5.82f, 15.f };

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_DraggableObject"),
			eCurLevelID, TEXT("Layer_Draggable"), &tDraggableDesc)))
			return;

		CPlayerData_Manager::GetInstance()->Spawn_PlayerItem(LEVEL_STATIC, (LEVEL_ID)eCurLevelID, TEXT("Flipping_Glove"), _float3(62.31f, 6.28f, -21.097f));
		CPlayerData_Manager::GetInstance()->Spawn_Bulb(LEVEL_STATIC, (LEVEL_ID)eCurLevelID);
	}
}


CGameEventExecuter_C2* CGameEventExecuter_C2::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CGameEventExecuter_C2* pInstance = new CGameEventExecuter_C2(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGameEventExecuter_C2");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGameEventExecuter_C2::Clone(void* _pArg)
{
	CGameEventExecuter_C2* pInstance = new CGameEventExecuter_C2(*this);
	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CGameEventExecuter_C2");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGameEventExecuter_C2::Free()
{
	__super::Free();
}


HRESULT CGameEventExecuter_C2::Cleanup_DeadReferences()
{
	return S_OK;
}
