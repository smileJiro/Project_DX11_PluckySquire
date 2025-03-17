#include "stdafx.h"
#include "GameEventExecuter_C4.h"
#include "Trigger_Manager.h"

#include "Camera_Manager.h"
#include "Camera_Target.h"
#include "Camera_2D.h"
#include "Camera_CutScene.h"
#include "Player.h"
#include "Section_2D_PlayMap.h"
#include "Section_Manager.h"
#include "Dialog_Manager.h"
#include "2DMapActionObject.h"
#include "Monster.h"
#include "DraggableObject.h"
#include "DynamicCastleGate.h"
#include "Friend_Controller.h"
#include "Friend.h"

CGameEventExecuter_C4::CGameEventExecuter_C4(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CGameEventExecuter(_pDevice, _pContext)
{
}


HRESULT CGameEventExecuter_C4::Initialize(void* _pArg)
{
	m_iCurLevelID = LEVEL_CHAPTER_4;
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

void CGameEventExecuter_C4::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);

}

void CGameEventExecuter_C4::Update(_float _fTimeDelta)
{
	if (!Is_Dead())
	{
		switch ((CTrigger_Manager::EVENT_EXECUTER_ACTION_TYPE)m_iEventExcuterAction)
		{
		case Client::CTrigger_Manager::CHAPTER4_3D_OUT_01:
			Chapter4_3D_Out_01(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER4_3D_OUT_02:
			Chapter4_3D_Out_02(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER4_INTRO:
			Chapter4_Intro(_fTimeDelta);		
			break;
		case Client::CTrigger_Manager::CHAPTER4_2D_INTRO:
			Chapter4_2D_Intro(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER4_INTRO_POSTIT_SEQUENCE:
			Chapter4_Intro_Postit_Sequence(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER4_RIDE_ZIPLINE:
			Chapter4_Ride_Zipline(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER4_EVENT_FLAG:
			Chapter4_Event_Flag(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER4_GATEEVENT:
			Chapter4_GateEvent(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER4_STORYSEQUENCE:
			Chapter4_StorySequence(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::FRIEND_MAPENTER:
			Friend_MapEnter(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::START_TRAIN:
			Start_Train(_fTimeDelta);
			break;
		default:
			break;
		}
	}
}

void CGameEventExecuter_C4::Late_Update(_float _fTimeDelta)
{
}

void CGameEventExecuter_C4::Chapter4_2D_Intro(_float _fTimeDelta)
{
	m_fTimer += _fTimeDelta;

	if (Step_Check(STEP_0))
	{
		if (Is_Start())
			CDialog_Manager::GetInstance()->Set_DialogId(L"Chapter4_2D_Intro");
		else
			Next_Step(!CDialog_Manager::GetInstance()->Get_DisPlayDialogue());
	}
	else
	{
	
		CFriend* pThrash = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Thrash"));
		CFriend* pViolet = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Violet"));
		CFriend_Controller::GetInstance()->Register_Friend_ToTrainList(TEXT("Thrash"));
		CFriend_Controller::GetInstance()->Register_Friend_ToTrainList(TEXT("Violet"));
		CFriend_Controller::GetInstance()->Start_Train(); // 체이스 시작
		//CFriend_Controller::GetInstance()->End_Train(); // 체이스 끝
		GameEvent_End();
	}
}

void CGameEventExecuter_C4::Chapter4_Intro(_float _fTimeDelta)
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
			CCamera_Manager::GetInstance()->Set_NextCutSceneData(TEXT("Chapter4_Intro"));
			CCamera_Manager::GetInstance()->Change_CameraType(CCamera_Manager::CUTSCENE, true, 0.8f);
			Next_Step(true);
		}
	}
	else if (Step_Check(STEP_2)) {
		CCamera_CutScene* pCamera = static_cast<CCamera_CutScene*>(CCamera_Manager::GetInstance()->Get_CurrentCamera());
		if (true == pCamera->Is_Finish_CutScene()) {
			Next_Step(true);
		}
	}
	else if (Step_Check(STEP_3)) {
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
	else if (Step_Check(STEP_4)) {
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

void CGameEventExecuter_C4::Chapter4_Intro_Postit_Sequence(_float _fTimeDelta)
{
	m_fTimer += _fTimeDelta;


	if (Step_Check(STEP_0))
	{
		function fCamerafunc = []() {

			//// 암 5 땡기기
			//CCamera_Manager::GetInstance()->Start_Changing_ArmLength_Increase(CCamera_Manager::TARGET, 1.f,
			//	10.f, EASE_IN_OUT);
			////CCamera_Manager::GetInstance()->Start_Changing_ArmLength_Decrease(CCamera_Manager::TARGET, 5.f,
			////	1.f, EASE_IN_OUT);
			//CCamera_Manager::GetInstance()->Start_Turn_AxisRight(CCamera_Manager::TARGET, 1.f, XMConvertToRadians(40.f), XMConvertToRadians(25.f));

			//// 암 타겟오프셋 x3 y2 z-3 이동
			CCamera_Manager::GetInstance()->Start_Changing_AtOffset(CCamera_Manager::TARGET,
				1.f,
				XMVectorSet(0.f, -2.f, 0.f, 0.f),
				EASE_IN_OUT);

			};
		Postit_Process_Start(L"Chapter4_SKSP_Postit", 1.f, true, fCamerafunc);
	}
	else if (Step_Check(STEP_1))
	{
		Postit_Process_PageAppear();
	}
	else if (Step_Check(STEP_2))
	{
		Postit_Process_PageTalk(L"Postit_Page_02");
	}
	else if (Step_Check(STEP_3))
	{
		//// 암 타겟오프셋 x3 y2 z-3 이동
		if (Is_Start())
		{
			CCamera_Manager::GetInstance()->Start_Changing_AtOffset(CCamera_Manager::TARGET,
				1.f,
				XMVectorSet(4.f, 0.f, -0.8f, 0.f),
				EASE_IN_OUT);
		}
		// 카메라 무브
		Next_Step_Over(1.f);
	}
	else if (Step_Check(STEP_4))
	{
		Postit_Process_PageTalk(L"Postit_Page_03");
	}
	else if (Step_Check(STEP_5))
	{
		Postit_Process_End(1.f);
	}
	else
		GameEvent_End();
}

void CGameEventExecuter_C4::Chapter4_Ride_Zipline(_float _fTimeDelta)
{
	m_fTimer += _fTimeDelta;

	if (Step_Check(STEP_0))
	{
		if (Is_Start())
		{
			CCamera_Manager::GetInstance()->Set_NextArmData(TEXT("Chapter4_Zipline"), 0);
			CCamera_Manager::GetInstance()->Change_CameraMode(CCamera_2D::MOVE_TO_NEXTARM);
			static_cast<CCamera_2D*>(CCamera_Manager::GetInstance()->Get_CurrentCamera())->Set_TrackingTime(0.4f);
		}

		Next_Step(true);
	}
	else if (Step_Check(STEP_1))
	{
		_uint iCameraMode = CCamera_Manager::GetInstance()->Get_CameraMode(CCamera_Manager::TARGET_2D);

		if (CCamera_2D::DEFAULT == iCameraMode) {
			CCamera_Manager::GetInstance()->Change_CameraMode(CCamera_2D::ZIPLINE);

			Next_Step(true);
		}
	}
	else
		GameEvent_End();
}

void CGameEventExecuter_C4::Chapter4_Event_Flag(_float _fTimeDelta)
{
	m_fTimer += _fTimeDelta;

	// 1. CutScene 시작
	if (Step_Check(STEP_0)) {
		if (Is_Start())
		{
			CCamera_Manager::GetInstance()->Set_NextCutSceneData(TEXT("Chapter4_Flag"));
			CCamera_Manager::GetInstance()->Change_CameraType(CCamera_Manager::CUTSCENE, true, 0.8f);
		}

		_bool isFinishCutScene = static_cast<CCamera_CutScene*>(CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::CUTSCENE))->Is_Finish_CutScene();

		if (true == isFinishCutScene) {
			Next_Step(true);
		}


	}
	// 2. CutScene이 끝났다면 CutScene 카메라로 GameOver 시작
	else if (Step_Check(STEP_1)) {
		if (Is_Start()) {
			CCamera_Manager::GetInstance()->Start_FadeOut(0.3f);
		}

		// 3. FadeOut이 끝난 후 CutScene Camera -> Target Camera로 전환
		if (m_fTimer > 0.3f) {
			static_cast<CCamera_CutScene*>(CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::CUTSCENE))->Set_Pause_After_CutScene(false);
			Next_Step(true);
		}
	}

	// 4. 전환 후 Target Camera로(다음 프레임) FadeIn 시작 + 기존 CutScene Camera를 다시 밝게 만들기
	else if (Step_Check(STEP_2)) {
		if (Is_Start()) {
			CCamera_Manager::GetInstance()->Start_FadeIn(0.3f);
			CCamera_Manager::GetInstance()->Set_FadeRatio(CCamera_Manager::CUTSCENE, 1.f, true);
			Next_Step(true);
		}
	}
	else
		GameEvent_End();
}

void CGameEventExecuter_C4::Chapter4_GateEvent(_float _fTimeDelta)
{
	m_fTimer += _fTimeDelta;
	if (Step_Check(STEP_0))
	{
		if (Is_Start())
		{
			CCamera_Manager::GetInstance()->Start_Changing_ArmLength_Increase(CCamera_Manager::TARGET_2D,
				1.f,5.f, EASE_IN_OUT);
			CPlayer* pPlayer = Get_Player();
			
			if(nullptr != pPlayer)
				pPlayer->Set_BlockPlayerInput(true);
			
		}
		Next_Step_Over(1.f);
	}
	else if (Step_Check(STEP_1))
	{
		if (Is_Start())
		{
			auto pLayer = m_pGameInstance->Find_Layer(m_pGameInstance->Get_CurLevelID(), L"Layer_MapGimmick");
			if (nullptr != pLayer)
			{
				auto GameObjects = pLayer->Get_GameObjects();

				auto iter = find_if(GameObjects.begin(), GameObjects.end(), [](CGameObject* _pObj) {
					return nullptr != dynamic_cast<CDynamicCastleGate*>(_pObj);
					});
				if (GameObjects.end() != iter)
				{
					CDynamicCastleGate* pGate = static_cast<CDynamicCastleGate*>(*iter);
					m_TargetObjects.push_back(pGate);
					pGate->Collapse();
				}
			}
		}
		Next_Step_Over(3.5f);

	}
	else if (Step_Check(STEP_2))
	{
		if (Is_Start())
		{
			CPlayer* pPlayer = Get_Player();
			pPlayer->Set_Position({ -133.f, -477.f, 0.f });

			if (nullptr != m_TargetObjects[0])
				static_cast<CDynamicCastleGate*>(m_TargetObjects[0])->WorldMap_ReCapture();

		}
		Next_Step_Over(0.3f);
	}
	else if (Step_Check(STEP_3))
	{
		if (Is_Start())
		{
			CPlayer* pPlayer = Get_Player();
			pPlayer->Set_Position({ 0.f, 0.f, 0.f });

			static_cast<CSection_2D_PlayMap*>(SECTION_MGR->Find_Section(L"Chapter4_SKSP_04"))->Set_PortalActive(true);

		}
		Next_Step(true);
	}	
	else if (Step_Check(STEP_4))
	{
		if (Is_Start())
		{
			CCamera_Manager::GetInstance()->Start_Changing_ArmLength_Decrease(CCamera_Manager::TARGET_2D,
				1.f, 5.f, EASE_IN_OUT);
			CPlayer* pPlayer = Get_Player();

			if (nullptr != pPlayer)
				pPlayer->Set_BlockPlayerInput(false);
		}
	}
	else
		GameEvent_End();
}

void CGameEventExecuter_C4::Chapter4_StorySequence(_float _fTimeDelta)
{
	m_fTimer += _fTimeDelta;

	function fCamerafunc = []() {

		// 암 5 땡기기
		CCamera_Manager::GetInstance()->Start_Changing_ArmLength_Increase(CCamera_Manager::TARGET, 1.f,
			10.f, EASE_IN_OUT);
		//CCamera_Manager::GetInstance()->Start_Changing_ArmLength_Decrease(CCamera_Manager::TARGET, 5.f,
		//	1.f, EASE_IN_OUT);
		CCamera_Manager::GetInstance()->Start_Turn_AxisRight(CCamera_Manager::TARGET, 1.f, XMConvertToRadians(40.f), XMConvertToRadians(25.f));

		//// 암 타겟오프셋 x3 y2 z-3 이동
		CCamera_Manager::GetInstance()->Start_Changing_AtOffset(CCamera_Manager::TARGET,
			1.f,
			XMVectorSet(0.f, 0.f, -1.f, 0.f),
			EASE_IN_OUT);

		};
	if (true == Postit_Process(L"Chapter4_SKSP_Postit", L"Postit_Page_01", 1.f, CPostit_Page::POSTIT_PAGE_POSTION_TYPE_D, false, fCamerafunc))
	{
		CSection* pSection = SECTION_MGR->Find_Section(L"Chapter4_SKSP_Postit");

		if (nullptr != pSection)
			static_cast<CSection_2D_PlayMap*>(pSection)->Set_PortalActive(true);

		// 투디 트리거를 생성합니다.
		CTriggerObject::TRIGGEROBJECT_DESC Desc = {};
		Desc.vHalfExtents = { 100.f, 100.f, 0.f };
		Desc.iTriggerType = (_uint)TRIGGER_TYPE::EVENT_TRIGGER;
		Desc.szEventTag = TEXT("Chapter4_2D_In");
		Desc.eConditionType = CTriggerObject::TRIGGER_ENTER;
		Desc.isReusable = false; // 한 번 하고 삭제할 때
		Desc.eStartCoord = COORDINATE_2D;
		Desc.tTransform2DDesc.vInitialPosition = { -568.347412f,-22.5205593f, 0.f };

		CSection* pBookSection = CSection_Manager::GetInstance()->Find_Section(TEXT("Chapter4_P0708"));
		CTrigger_Manager::GetInstance()->Create_TriggerObject(LEVEL_STATIC, LEVEL_CHAPTER_2, &Desc, pBookSection);

		GameEvent_End();
	}
}

void CGameEventExecuter_C4::Chapter4_3D_Out_01(_float _fTimeDelta)
{
	m_fTimer += _fTimeDelta;

	if (Step_Check(STEP_0))
	{
		if (Is_Start())
		{
			CPlayer* pPlayer = Get_Player();
			pPlayer->Set_BlockPlayerInput(true);
			// 1. 카메라 위치 박스로 세팅. target을 바꿔버리면 위화감이 좀 잇을거같으니 걍 암오프셋만 좀 바까주자.
			CCamera_Manager::GetInstance()->Set_ResetData(CCamera_Manager::TARGET_2D);

			CCamera_Manager::GetInstance()->Start_Changing_AtOffset(CCamera_Manager::TARGET_2D,
				0.5f,
				XMVectorSet(0.f, 0.f, 0.5f, 0.f),
				EASE_IN_OUT);
		}

		Next_Step_Over(1.f);
	}
	else if (Step_Check(STEP_1))
	{

		// 대화 1 재생
		if (Is_Start())
			CDialog_Manager::GetInstance()->Set_DialogId(L"Chapter4_3D_Out_01_Dialog_01");
		else
			Next_Step(!CDialog_Manager::GetInstance()->Get_DisPlayDialogue());
	}
	else if (Step_Check(STEP_2))
	{

		if (Is_Start())
		{
			CSection_2D* pSection = static_cast<CSection_2D*>(SECTION_MGR->Find_Section(L"Chapter4_P0708"));

			auto pLayer = pSection->Get_Section_Layer(SECTION_PLAYMAP_2D_RENDERGROUP::SECTION_2D_PLAYMAP_BACKGROUND);

			const auto& Objects = pLayer->Get_GameObjects();

			auto iter = find_if(Objects.begin(), Objects.end(), [](CGameObject* pGameObject) {
				auto pActionObj = dynamic_cast<C2DMapActionObject*>(pGameObject);

				//이때 카메라 체이싱
				if (nullptr != pActionObj)
				{
					if (C2DMapActionObject::ACTIVE_TYPE_ACTIONANIM == pActionObj->Get_ActionType())
						return true;
				}
				return false;

				});
			if (Objects.end() != iter)
			{
				CCamera_Manager::GetInstance()->Change_CameraTarget(*iter);
				CCamera_Manager::GetInstance()->Start_Changing_ArmLength_Decrease(CCamera_Manager::TARGET_2D, 1.f,
					3.f, EASE_IN_OUT);
			}

			// 카메라 위치 2로 세팅. 나무 올리기 위함.
			//CCamera_Manager::GetInstance()->Start_Changing_AtOffset(CCamera_Manager::TARGET_2D,
			//	0.5f,
			//	XMVectorSet(-0.5f, 0.f, -0.5f, 0.f),
			//	EASE_IN_OUT);
		}

		Next_Step_Over(1.f);
	}
	// 여기 바이올렛 추가되면 애니메이션 추가.
	else if (Step_Check(STEP_3))
	{

		if (Is_Start())
		{
			CSection_2D* pSection = static_cast<CSection_2D*>(SECTION_MGR->Find_Section(L"Chapter4_P0708"));

			auto pLayer = pSection->Get_Section_Layer(SECTION_PLAYMAP_2D_RENDERGROUP::SECTION_2D_PLAYMAP_BACKGROUND);

			const auto& Objects = pLayer->Get_GameObjects();

			for_each(Objects.begin(), Objects.end(), [](CGameObject* pGameObject) {
				auto pActionObj = dynamic_cast<C2DMapActionObject*>(pGameObject);

				//섹션에 있는 액션맵오브젝트 남김없이 액션 실행(애니메이션 재생!)
				if (nullptr != pActionObj)
				{
					if (C2DMapActionObject::ACTIVE_TYPE_ACTIONANIM == pActionObj->Get_ActionType())
						pActionObj->Ready_Action();
				}

				});



			CDraggableObject::DRAGGABLE_DESC tDraggableDesc = {};
			tDraggableDesc.iModelPrototypeLevelID_3D = LEVEL_STATIC;
			tDraggableDesc.iCurLevelID = m_pGameInstance->Get_CurLevelID();
			tDraggableDesc.strModelPrototypeTag_3D = TEXT("SM_Plastic_Block_04");
			tDraggableDesc.eStartCoord = COORDINATE_3D;
			tDraggableDesc.vBoxHalfExtents = { 1.02f,1.02f,1.02f };
			tDraggableDesc.vBoxOffset = { 0.f,tDraggableDesc.vBoxHalfExtents.y,0.f };
			tDraggableDesc.tTransform3DDesc.vInitialPosition = { 0.f, 0.f, 15.f };

			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_DraggableObject"),
				m_pGameInstance->Get_CurLevelID(), TEXT("Layer_Draggable"), &tDraggableDesc)))
				return;


			// 동시에 3D 몹들 생성.
			const json* pJson = m_pGameInstance->Find_Json_InLevel(TEXT("Chapter4_Monsters_3D"), m_pGameInstance->Get_CurLevelID());

			CGameObject* pObject;


			CMonster::MONSTER_DESC MonsterDesc3D = {};

			MonsterDesc3D.iCurLevelID = m_pGameInstance->Get_CurLevelID();
			MonsterDesc3D.eStartCoord = COORDINATE_3D;

			if (pJson->contains("3D"))
			{
				_wstring strLayerTag = L"Layer_Monster";
				_wstring strMonsterTag = L"";

				for (_int i = 0; i < (*pJson)["3D"].size(); ++i)
				{
					if ((*pJson)["3D"][i].contains("Position"))
					{
						for (_int j = 0; j < 3; ++j)
						{
							*(((_float*)&MonsterDesc3D.tTransform3DDesc.vInitialPosition) + j) = (*pJson)["3D"][i]["Position"][j];
						}
					}
					if ((*pJson)["3D"][i].contains("Scaling"))
					{
						for (_int j = 0; j < 3; ++j)
						{
							*(((_float*)&MonsterDesc3D.tTransform3DDesc.vInitialScaling) + j) = (*pJson)["3D"][i]["Scaling"][j];
						}
					}
					if ((*pJson)["3D"][i].contains("LayerTag"))
					{
						strLayerTag = STRINGTOWSTRING((*pJson)["3D"][i]["LayerTag"]);
					}

					if ((*pJson)["3D"][i].contains("MonsterTag"))
					{
						strMonsterTag = STRINGTOWSTRING((*pJson)["3D"][i]["MonsterTag"]);
					}
					else
						return;

					if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, strMonsterTag, m_pGameInstance->Get_CurLevelID(), strLayerTag, &pObject, &MonsterDesc3D)))
						return;

				}
			}


		}

		Next_Step_Over(2.f);
	}
	else if (Step_Check(STEP_4))
	{
		if (Is_Start())
		{
			// 카메라 원복
			CPlayer* pPlayer = Get_Player();

			CCamera_Manager::GetInstance()->Change_CameraTarget(pPlayer);
			CCamera_Manager::GetInstance()->Start_ResetArm_To_SettingPoint(CCamera_Manager::TARGET_2D, 1.f);
		}
		Next_Step_Over(1.f);
		}
	else if (Step_Check(STEP_5))
	{
		// 대화 2 재생
		if (Is_Start())
		{	
		
			CDialog_Manager::GetInstance()->Set_DialogId(L"Chapter4_3D_Out_01_Dialog_02");
			Event_ChangeMapObject(LEVEL_CHAPTER_4, L"Chapter_04_Play_Desk.mchc", L"Layer_MapObject");
		}
		else
			Next_Step(!CDialog_Manager::GetInstance()->Get_DisPlayDialogue());
	}
	else
	{
		CPlayer* pPlayer = Get_Player();

		pPlayer->Set_BlockPlayerInput(false);
		GameEvent_End();
	}

}

void CGameEventExecuter_C4::Chapter4_3D_Out_02(_float _fTimeDelta)
{
	m_fTimer += _fTimeDelta;

	if (Step_Check(STEP_0))
	{

		// 1. 카메라 위치 박스로 세팅. target을 바꿔버리면 위화감이 좀 잇을거같으니 걍 암오프셋만 좀 바까주자.
		CCamera_Manager::GetInstance()->Set_ResetData(CCamera_Manager::TARGET);

		CCamera_Manager::GetInstance()->Start_Changing_AtOffset(CCamera_Manager::TARGET,
			0.5f,
			XMVectorSet(-.5f, 0.f, 0.0f, 0.f),
			EASE_IN_OUT);

		Next_Step_Over(0.5f);
	}
	else if (Step_Check(STEP_1))
	{
		// 대화 1 재생
		if (Is_Start())
			CDialog_Manager::GetInstance()->Set_DialogId(L"Chapter4_3D_Out_02_Dialog_01");
		else
			Next_Step(!CDialog_Manager::GetInstance()->Get_DisPlayDialogue());
	}
	else if (Step_Check(STEP_2))
	{
		// 카메라 원복
		CCamera_Manager::GetInstance()->Start_ResetArm_To_SettingPoint(CCamera_Manager::TARGET, 0.5f);
		Next_Step_Over(0.5f);
	}
	else
		GameEvent_End();
}

void CGameEventExecuter_C4::Friend_MapEnter(_float _fTimeDelta)
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
		Next_Step_Over(1.f);
	}
	else if (Step_Check(STEP_1))
	{
		/* 1. Save Reset ArmData */
		if (Is_Start())
		{
			pPlayer->Set_2DDirection(F_DIRECTION::UP);
			pPlayer->Set_BlockPlayerInput(true);
			CFriend_Controller::GetInstance()->End_Train();
			_vector vPlayerPos = pPlayer->Get_FinalPosition() + XMVectorSet(0.0f, 140.f, 0.0f, 0.0f);
			_vector vThrashPos = pThrash->Get_FinalPosition() + XMVectorSet(0.0f, 140.f, 0.0f, 0.0f);
			_vector vVioletPos = pViolet->Get_FinalPosition() + XMVectorSet(0.0f, 140.f, 0.0f, 0.0f);
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
			AutoMove2.vTarget = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
			AutoMove2.iAnimIndex = (_uint)CPlayer::ANIM_STATE_2D::PLAYER_IDLE_UP;

			pPlayer->Add_AutoMoveCommand(AutoMove);
			pPlayer->Add_AutoMoveCommand(AutoMove2);
			pPlayer->Start_AutoMove(true);
			pThrash->Move_Position(_float2(XMVectorGetX(vThrashPos), XMVectorGetY(vThrashPos)), CFriend::DIR_UP);
			pViolet->Move_Position(_float2(XMVectorGetX(vVioletPos), XMVectorGetY(vVioletPos)), CFriend::DIR_UP);

			CCamera_Manager::GetInstance()->Set_ResetData(eCamType);
		}
		Next_Step_Over(1.5f);
	}
	else
	{
		//CFriend_Controller::GetInstance()->Register_Friend_ToTrainList(TEXT("Thrash"));
		//CFriend_Controller::GetInstance()->Register_Friend_ToTrainList(TEXT("Violet"));
		//CFriend_Controller::GetInstance()->Start_Train();
		CCamera_Manager::GetInstance()->Start_ResetArm_To_SettingPoint(eCamType, 1.0f);
		pPlayer->Set_BlockPlayerInput(false);
		GameEvent_End();
	}
}

CGameEventExecuter_C4* CGameEventExecuter_C4::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CGameEventExecuter_C4* pInstance = new CGameEventExecuter_C4(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGameEventExecuter_C4");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGameEventExecuter_C4::Clone(void* _pArg)
{
	CGameEventExecuter_C4* pInstance = new CGameEventExecuter_C4(*this);
	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CGameEventExecuter_C4");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGameEventExecuter_C4::Free()
{
	__super::Free();
}


HRESULT CGameEventExecuter_C4::Cleanup_DeadReferences()
{
	return S_OK;
}
