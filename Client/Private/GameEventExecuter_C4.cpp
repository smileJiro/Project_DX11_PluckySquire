#include "stdafx.h"
#include "GameEventExecuter_C4.h"
#include "Trigger_Manager.h"

#include "Camera_Manager.h"
#include "Camera_Target.h"
#include "Camera_2D.h"
#include "Camera_CutScene.h"
#include "Postit_Page.h"
#include "Player.h"
#include "Section_2D_PlayMap.h"
#include "Section_Manager.h"
#include "Dialog_Manager.h"

CGameEventExecuter_C4::CGameEventExecuter_C4(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CGameEventExecuter(_pDevice, _pContext)
{
}


HRESULT CGameEventExecuter_C4::Initialize(void* _pArg)
{
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
	_wstring strEventTag;
}

void CGameEventExecuter_C4::Update(_float _fTimeDelta)
{
	if (!Is_Dead())
	{
		switch ((CTrigger_Manager::EVENT_EXECUTER_ACTION_TYPE)m_iEventExcuterAction)
		{
		case Client::CTrigger_Manager::C02P0910_LIGHTNING_BOLT_SPAWN:
			break;
		case Client::CTrigger_Manager::CHAPTER4_INTRO:
			Chapter4_Intro(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER4_RIDE_ZIPLINE:
			Chapter4_Ride_Zipline(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER4_EVENT_FLAG:
			Chapter4_Event_Flag(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER4_STORYSEQUENCE:
			Chapter4_StorySequence(_fTimeDelta);
			break;
		default:
			break;
		}
	}
}

void CGameEventExecuter_C4::Late_Update(_float _fTimeDelta)
{
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
		if (m_fTimer >= 0.5f) {
			CCamera_Manager::GetInstance()->Set_NextCutSceneData(TEXT("Chapter4_Intro"));
			CCamera_Manager::GetInstance()->Change_CameraType(CCamera_Manager::CUTSCENE, true, 0.8f);
			Next_Step(true);
		}
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

void CGameEventExecuter_C4::Chapter4_StorySequence(_float _fTimeDelta)
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
			// 암 5 땡기기
			CCamera_Manager::GetInstance()->Start_Changing_ArmLength_Increase(CCamera_Manager::TARGET,1.f,
				10.f, EASE_IN_OUT);
			//CCamera_Manager::GetInstance()->Start_Changing_ArmLength_Decrease(CCamera_Manager::TARGET, 5.f,
			//	1.f, EASE_IN_OUT);
			CCamera_Manager::GetInstance()->Start_Turn_AxisRight(CCamera_Manager::TARGET, 1.f, XMConvertToRadians(20.f), XMConvertToRadians(5.f));

			// 암 타겟오프셋 x3 y2 z-3 이동
			CCamera_Manager::GetInstance()->Start_Changing_AtOffset(CCamera_Manager::TARGET,
				1.f,
				XMVectorSet(3.f, 2.f, -1.f, 0.f),
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
			CSection_2D* pSection = static_cast<CSection_2D*>(SECTION_MGR->Find_Section(L"Chapter4_SKSP_Postit"));
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
			// 애니메이션 끝나고, 인터벌 조금 준뒤 다음 다이얼로그 스타트
			Next_Step(!pPage->Is_DuringAnimation() && m_fTimer > 1.5f);
		}
	}
	else if (Step_Check(STEP_3))
	{
		if (Is_Start())
		{
			// 말함
			if (nullptr == m_pTargetObject)
				assert(nullptr);
			CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_pTargetObject);
			pPage->Anim_Action(CPostit_Page::POSTIT_PAGE_TALK_1, true);
			CDialog_Manager::GetInstance()->Set_DialogId(L"Postit_Page_01");
		}
		else
			Next_Step(!CDialog_Manager::GetInstance()->Get_DisPlayDialogue());
	}

	else if (Step_Check(STEP_4))
	{
		if (Is_Start())
		{
			CPostit_Page* pPage = dynamic_cast<CPostit_Page*>(m_pTargetObject);
			pPage->Anim_Action(CPostit_Page::POSTIT_PAGE_DISAPPEAR, false);
		}
		Next_Step_Over(1.f);
	}
	else
	{
		CSection* pSection = SECTION_MGR->Find_Section(L"Chapter4_SKSP_Postit");

		if (nullptr != pSection)
			static_cast<CSection_2D_PlayMap*>(pSection)->Set_PortalActive(true);

		CPlayer* pPlayer = Get_Player();
		pPlayer->Set_BlockPlayerInput(false);
		CCamera_Manager::GetInstance()->Start_ResetArm_To_SettingPoint(CCamera_Manager::TARGET, 0.5f);

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
