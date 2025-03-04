#include "stdafx.h"
#include "GameEventExecuter_C2.h"
#include "Trigger_Manager.h"
#include "Section_Manager.h"
#include "Dialog_Manager.h"
#include "Pooling_Manager.h"
#include "Camera_Manager.h"
#include "Effect2D_Manager.h"

#include "GameInstance.h"
#include "SampleBook.h"
#include "Magic_Hand.h"
#include "Camera_2D.h"
#include "UI_Manager.h"
#include "Effect_Manager.h"
#include "Section_2D.h"
#include "MapObject.h"
#include "Camera_Target.h"
#include "2DMapActionObject.h"
#include "Postit_Page.h"

#include "Zippy.h"

CGameEventExecuter_C2::CGameEventExecuter_C2(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CGameEventExecuter(_pDevice, _pContext)
{
}


HRESULT CGameEventExecuter_C2::Initialize(void* _pArg)
{
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
	_wstring strEventTag;
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
			CSampleBook* pBook = Get_Book();
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
			CEffect2D_Manager::GetInstance()->Play_Effect(L"hum", SECTION_MGR->Get_Cur_Section_Key(), XMMatrixTranslation(-276.f, 179.4f, 0.f), 0.f, 0);
			CEffect2D_Manager::GetInstance()->Play_Effect(L"hum", SECTION_MGR->Get_Cur_Section_Key(), XMMatrixTranslation(-276.f, 179.4f, 0.f), 1.8f, 1, false, 999.f);
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

			pCamera->Add_CustomArm(tData);

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
			CCamera_2D* pCamera = static_cast<CCamera_2D*>(CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET_2D));
			pCamera->Start_PostProcessing_Fade(CCamera::FADE_IN, 2.f);
			pCamera->Set_InitialData(XMVectorSet(-0.670150876f, 0.506217539f, -0.542809010f, 0.f), 46.20f, XMVectorSet(-15.f, 5.f, 0.f, 0.f), 5);
			pCamera->Set_NextArmData(TEXT("Intro"), 0);
			pCamera->Start_Changing_AtOffset(5.f, XMVectorSet(0.f, 0.f, 0.f, 0.f), EASE_IN_OUT);
			pCamera->Set_CameraMode(CCamera_2D::MOVE_TO_NEXTARM);
		}

		Next_Step_Over(7.f);
	}
	else if (Step_Check(STEP_1))
	{
		if (Is_Start())
		{
			//CUI_Manager::GetInstance()->Set_PlayNarration(TEXT("Chapter1_P0102_Narration_01"));
			m_iStep++;
		}
		Next_Step(true);
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

			pCamera->Add_CustomArm(tData);
			pCamera->Set_CameraMode(CCamera_2D::MOVE_TO_CUSTOMARM);
			pCamera->Start_Changing_AtOffset(1.5f, XMVectorSet(0.f, 0.f, -3.f, 0.f), EASE_IN_OUT);
			pCamera->Start_Zoom(1.5f, (CCamera::ZOOM_LEVEL)CCamera::LEVEL_5, EASE_IN_OUT);
			//pCamera->Start_Changing_AtOffset(3.f, XMVectorSet(0.f, 4.f, 0.f, 0.f), EASE_IN_OUT);

		}

		CPlayer* pPlayer = Get_Player();

		if (COORDINATE_3D == pPlayer->Get_CurCoord())
		{
			/*CCamera_Target* pCamera = static_cast<CCamera_Target*>(CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET));
			pCamera->Set_CameraMode(CCamera_Target::MOVE_TO_CUSTOMARM);

			ARM_DATA tData = {};
			tData.fMoveTimeAxisRight = { 3.f, 0.f };
			tData.fRotationPerSecAxisRight = { XMConvertToRadians(-15.f), XMConvertToRadians(-1.f) };
			tData.iRotationRatioType = EASE_IN_OUT;
			tData.fLength = 20.f;
			tData.fLengthTime = { 3.f, 0.f };
			tData.iLengthRatioType = EASE_OUT;

			pCamera->Add_CustomArm(tData);

			pCamera->Start_Changing_AtOffset(3.f, XMVectorSet(0.f, 4.f, 0.f, 0.f), EASE_IN_OUT);*/

			CCamera_Manager::GetInstance()->Change_CameraType(CCamera_Manager::CUTSCENE);
			CCamera_Manager::GetInstance()->Set_NextCutSceneData(TEXT("CutScene_Humgrump"));

			Next_Step(true);

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
			m_pTargetObject = Objects.front();
			if (nullptr == m_pTargetObject)
				assert(nullptr);

			CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_pTargetObject);
			if (nullptr != pPage)
			{
				pPage->Set_Render(true);
				pPage->Anim_Action(CPostit_Page::POSTIT_PAGE_APPEAR,false);
			}

		}
		else 
		{
			if (nullptr == m_pTargetObject)
				assert(nullptr);
			CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_pTargetObject);
			
			Next_Step(!pPage->Is_DuringAnimation() && m_fTimer > 1.8f);
		}
	}
	else if (Step_Check(STEP_3))
	{
		if (Is_Start())
		{
			CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_pTargetObject);
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
			if (nullptr == m_pTargetObject)
				assert(nullptr);
			CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_pTargetObject);

			pPage->Anim_Action(CPostit_Page::POSTIT_PAGE_DISAPPEAR, false);
		}
		else
		{
			if (nullptr == m_pTargetObject)
				assert(nullptr);
			CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_pTargetObject);

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
			m_pTargetObject = Objects.front();
			if (nullptr == m_pTargetObject)
				assert(nullptr);

			CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_pTargetObject);
			if (nullptr != pPage)
			{
				pPage->Set_Render(true);
				pPage->Anim_Action(CPostit_Page::POSTIT_PAGE_APPEAR, false);
			}

		}
		else
		{
			if (nullptr == m_pTargetObject)
				assert(nullptr);
			CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_pTargetObject);

			Next_Step(!pPage->Is_DuringAnimation() && m_fTimer > 1.8f);
		}
	}
	else if (Step_Check(STEP_3))
	{
		if (Is_Start())
		{
			CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_pTargetObject);
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
			if (nullptr == m_pTargetObject)
				assert(nullptr);
			CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_pTargetObject);

			pPage->Anim_Action(CPostit_Page::POSTIT_PAGE_DISAPPEAR, false);
		}
		else
		{
			if (nullptr == m_pTargetObject)
				assert(nullptr);
			CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_pTargetObject);

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
			m_pTargetObject = Objects.front();
			if (nullptr == m_pTargetObject)
				assert(nullptr);

			CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_pTargetObject);
			if (nullptr != pPage)
			{
				pPage->Set_Render(true);
				pPage->Anim_Action(CPostit_Page::POSTIT_PAGE_APPEAR, false);
			}

		}
		else
		{
			if (nullptr == m_pTargetObject)
				assert(nullptr);
			CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_pTargetObject);

			Next_Step(!pPage->Is_DuringAnimation() && m_fTimer > 1.8f);
		}
	}
	else if (Step_Check(STEP_3))
	{
		if (Is_Start())
		{
			CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_pTargetObject);
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
			CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_pTargetObject);
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
			if (nullptr == m_pTargetObject)
				assert(nullptr);
			CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_pTargetObject);

			pPage->Anim_Action(CPostit_Page::POSTIT_PAGE_DISAPPEAR, false);
		}
		else
		{
			if (nullptr == m_pTargetObject)
				assert(nullptr);
			CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_pTargetObject);

			if (Next_Step(!pPage->Is_DuringAnimation()))
			{
				CCamera_Target* pCamera = static_cast<CCamera_Target*>(CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET_2D));
				pCamera->Start_PostProcessing_Fade(CCamera::FADE_OUT, 0.5f);
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
			m_pTargetObject = Objects.front();
			if (nullptr == m_pTargetObject)
				assert(nullptr);

			CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_pTargetObject);
			if (nullptr != pPage)
			{
				pPage->Set_Render(true);
				pPage->Anim_Action(CPostit_Page::POSTIT_PAGE_APPEAR, false);
			}

		}
		else
		{
			if (nullptr == m_pTargetObject)
				assert(nullptr);
			CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_pTargetObject);

			Next_Step(!pPage->Is_DuringAnimation() && m_fTimer > 1.8f);
		}
	}
	else if (Step_Check(STEP_3))
	{
		if (Is_Start())
		{
			CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_pTargetObject);
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
			CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_pTargetObject);
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
			if (nullptr == m_pTargetObject)
				assert(nullptr);
			CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_pTargetObject);

			pPage->Anim_Action(CPostit_Page::POSTIT_PAGE_DISAPPEAR, false);
		}
		else
		{
			if (nullptr == m_pTargetObject)
				assert(nullptr);
			CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_pTargetObject);

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
		Next_Step_Over(1.0f);
	}
	else if (Step_Check(STEP_8))
	{
		if (Is_Start())
		{
			CCamera_Target* pCamera = static_cast<CCamera_Target*>(CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET));

			CCamera_Manager::GetInstance()->Start_FadeIn(0.5f);
			CPlayer* pPlayer = Get_Player();
			pPlayer->Set_BlockPlayerInput(false);
		}
	}
	else
	{
		GameEvent_End();
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
