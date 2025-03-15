#include "stdafx.h"
#include "GameEventExecuter_C6.h"
#include "GameInstance.h"

/* Manager */
#include "Trigger_Manager.h"
#include "Section_Manager.h"
#include "Dialog_Manager.h"
#include "Pooling_Manager.h"
#include "Camera_Manager.h"
#include "Effect2D_Manager.h"
#include "FatherGame.h"
#include "UI_Manager.h"

/* Section */
#include "Section_2D_PlayMap.h"

/* Object */
#include "Portal_Default.h"
#include "Player.h"
#include "PortalLocker.h"
#include "ZetPack_Child.h"

/* Camera */
#include "Camera_CutScene.h"
#include "Camera_2D.h"

/* Npc */
#include "Npc_Humgrump.h"
#include "Npc_MoonBeard.h"

CGameEventExecuter_C6::CGameEventExecuter_C6(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CGameEventExecuter(_pDevice, _pContext)
{
}


HRESULT CGameEventExecuter_C6::Initialize(void* _pArg)
{
	m_iCurLevelID = LEVEL_CHAPTER_6;
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	// 초기 설정 
	switch ((CTrigger_Manager::EVENT_EXECUTER_ACTION_TYPE)m_iEventExcuterAction)
	{
	case Client::CTrigger_Manager::CHAPTER6_FATHERGAME_PROGRESS_START_CLEAR:
		break;
	default:
		break;
	}

	return S_OK;
}

void CGameEventExecuter_C6::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);

}

void CGameEventExecuter_C6::Update(_float _fTimeDelta)
{
	if (!Is_Dead())
	{
		switch ((CTrigger_Manager::EVENT_EXECUTER_ACTION_TYPE)m_iEventExcuterAction)
		{
		case Client::CTrigger_Manager::CHAPTER6_INTRO:
			Chapter6_Intro(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER6_FATHERGAME_PROGRESS_START_CLEAR:
			//Chapter6_FatherGame_Progress_Fatherpart_2(_fTimeDelta);
			Chapter6_FatherGame_Progress_Start_Clear(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER6_FATHERGAME_PROGRESS_ZETPACK_CLEAR:
			Chapter6_FatherGame_Progress_ZetPack_Clear(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER6_FATHERGAME_PROGRESS_FATHERPART_1:
			Chapter6_FatherGame_Progress_Fatherpart_1(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER6_FATHERGAME_PROGRESS_FATHERPART_2:
			Chapter6_FatherGame_Progress_Fatherpart_2(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER6_FATHERGAME_PROGRESS_FATHERPART_3:
			Chapter6_FatherGame_Progress_Fatherpart_3(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER6_HUMGRUMP_REVOLT:
			Chapter6_Humgrump_Revolt(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER6_CHANGE_BOOK_TO_GREATE_HUMGRUMP:
			Chapter6_Change_Book_To_Greate_Humgrump(_fTimeDelta);
			break;
		default:
			break;
		}
	}

}

void CGameEventExecuter_C6::Late_Update(_float _fTimeDelta)
{
}

void CGameEventExecuter_C6::Chapter6_Intro(_float _fTimeDelta)
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
			CCamera_Manager::GetInstance()->Set_NextCutSceneData(TEXT("Chapter6_Intro"));
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
		GameEvent_End();
}


void CGameEventExecuter_C6::Chapter6_FatherGame_Progress_Start_Clear(_float _fTimeDelta)
{
	m_fTimer += _fTimeDelta;
	CCamera_Manager::CAMERA_TYPE eCamType = CCamera_Manager::TARGET;
	CPlayer* pPlayer = Get_Player();
	if (nullptr == pPlayer)
	{
		GameEvent_End();
		return;
	}
	/* 플레이어 인풋락  */
	pPlayer->Set_BlockPlayerInput(true);

	if (Step_Check(STEP_0))
	{

		if (Is_Start())
		{

			CPortal* pTargetPortal = static_cast<CPortal_Default*>(static_cast<CSection_2D_PlayMap*>(CSection_Manager::GetInstance()->Find_Section(TEXT("Chapter6_SKSP_01")))->Get_Portal(0));
			if (nullptr == pTargetPortal)
			{
				/* 플레이어 인풋락 해제 */
				pPlayer->Set_BlockPlayerInput(false);
				GameEvent_End();
				return;
			}

			/* 1. Save Reset ArmData */
			CCamera_Manager::GetInstance()->Set_ResetData(eCamType);
			/* 2. Change Arm Vector  */
			//CCamera_Manager::GetInstance()->Get_Camera();
			_float fTime = 1.2f;
			CCamera_Manager::GetInstance()->Start_Turn_AxisY(eCamType, fTime, XMConvertToRadians(20.f) / fTime, XMConvertToRadians(20.f) / fTime);
			CCamera_Manager::GetInstance()->Start_Turn_AxisRight(eCamType, fTime, XMConvertToRadians(-25.f) / fTime, XMConvertToRadians(-25.f) / fTime);

			/* 3. Target Change */
			_float2 v2DPos = {};
			XMStoreFloat2(&v2DPos, pTargetPortal->Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_State(CTransform::STATE_POSITION));
			_vector vPortalLockerWorldPos = CSection_Manager::GetInstance()->Get_WorldPosition_FromWorldPosMap(TEXT("Chapter6_SKSP_01"), v2DPos);
			XMStoreFloat4x4(&m_TargetWorldMatrix, XMMatrixTranslationFromVector(XMVectorSetW(vPortalLockerWorldPos, 1.0f)));
			CCamera_Manager::GetInstance()->Change_CameraTarget(&m_TargetWorldMatrix, fTime);

			/* 4. Change Camera Length */
			CCamera_Manager::GetInstance()->Start_Changing_ArmLength_Decrease(eCamType, fTime, 5.0f, RATIO_TYPE::EASE_IN);
		}
		Next_Step_Over(4.5f);
	}
	else if (Step_Check(STEP_1))
	{
		if (Is_Start())
		{
			CCamera_Manager::GetInstance()->Change_CameraTarget(pPlayer, 1.0f);
			//CCamera_Manager::GetInstance()->Start_Changing_ArmLength_Increase(eCamType, 1.0f, 5.0f, RATIO_TYPE::EASE_OUT);
			CCamera_Manager::GetInstance()->Start_ResetArm_To_SettingPoint(eCamType, 1.0f);
		}
		Next_Step_Over(1.5f);
	}
	else
	{
		pPlayer->Set_BlockPlayerInput(false);
		GameEvent_End();
	}
}

void CGameEventExecuter_C6::Chapter6_FatherGame_Progress_ZetPack_Clear(_float _fTimeDelta)
{
	m_fTimer += _fTimeDelta;
	CCamera_Manager::CAMERA_TYPE eCamType = CCamera_Manager::TARGET;
	CPlayer* pPlayer = Get_Player();
	CZetPack_Child* pZetPack_Child = CFatherGame::GetInstance()->Get_ZetPack_Child();
	if (nullptr == pPlayer)
	{
		GameEvent_End();
		return;
	}
	/* 플레이어 인풋락  */
	pPlayer->Set_BlockPlayerInput(true);

	if (Step_Check(STEP_0))
	{
		/* 1. Save Reset ArmData */
		CCamera_Manager::GetInstance()->Set_ResetData(eCamType);
		_float fTime = 1.0f;
		/* 4. Change Camera Length */
		CCamera_Manager::GetInstance()->Start_Changing_ArmLength_Increase(eCamType, fTime, 3.0f, RATIO_TYPE::EASE_IN);
		Next_Step_Over(1.0f);
	}
	else if (Step_Check(STEP_1)) // 1. 카메라 이동 
	{
		if (Is_Start())
		{
			/* 2. Change Arm Vector  */
			//CCamera_Manager::GetInstance()->Get_Camera();
			_float fTime = 1.8f;
			CCamera_Manager::GetInstance()->Start_Turn_AxisY(eCamType, fTime, XMConvertToRadians(75.f) / fTime, XMConvertToRadians(75.f) / fTime);
			CCamera_Manager::GetInstance()->Start_Turn_AxisRight(eCamType, fTime, XMConvertToRadians(-15.f) / fTime, XMConvertToRadians(-15.f) / fTime);

			/* 4. Change Camera Length */
			CCamera_Manager::GetInstance()->Start_Changing_ArmLength_Decrease(eCamType, fTime, 5.5f, RATIO_TYPE::EASE_IN);
		}


		Next_Step_Over(2.0f);
	}
	else if (Step_Check(STEP_2)) // 2. 찌릿이 대화 + 페이드 아웃
	{
		if (Is_Start())
		{
			/* 5. 다이얼로그 재생. */
			_int iDialogueIndex = pZetPack_Child->Get_DialogueIndex();
			_wstring strDialogueTag = TEXT("ZetPack_Child_");
			strDialogueTag += to_wstring(iDialogueIndex);
			CDialog_Manager::GetInstance()->Set_DialogId(strDialogueTag.c_str());
			pZetPack_Child->Plus_DialogueIndex();
		}

		/* 6. 다이얼로그 종료 체크 */
		if (false == CDialog_Manager::GetInstance()->Get_DisPlayDialogue())
		{
			static bool isFirst = false;
			if (false == isFirst)
			{
				isFirst = true;
				CCamera_Manager::GetInstance()->Start_FadeOut(0.5f);
				m_fTimer = 0.0f;
			}
			Next_Step_Over(0.7f);
		}
	}
	else if (Step_Check(STEP_3)) // 3. PartBody Cup CutScene
	{
		if (Is_Start())
		{
			CPortal* pTargetPortal = static_cast<CPortal_Default*>(static_cast<CSection_2D_PlayMap*>(CSection_Manager::GetInstance()->Find_Section(TEXT("Chapter6_SKSP_03")))->Get_Portal(0));
			if (nullptr == pTargetPortal)
			{
				/* 플레이어 인풋락 해제 */
				pPlayer->Set_BlockPlayerInput(false);
				GameEvent_End();
				return;
			}
			/* 3. Target Change */
			_float2 v2DPos = {};
			XMStoreFloat2(&v2DPos, pTargetPortal->Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_State(CTransform::STATE_POSITION));
			_vector vPortalLockerWorldPos = CSection_Manager::GetInstance()->Get_WorldPosition_FromWorldPosMap(TEXT("Chapter6_SKSP_03"), v2DPos);
			XMStoreFloat4x4(&m_TargetWorldMatrix, XMMatrixTranslationFromVector(XMVectorSetW(vPortalLockerWorldPos, 1.0f)));
			CCamera_Manager::GetInstance()->Change_CameraTarget(&m_TargetWorldMatrix, 0.0f);

			/* 4. Change Camera Length */
			CCamera_Manager::GetInstance()->Start_Changing_ArmLength(eCamType, 0.0f, 50.0f, RATIO_TYPE::EASE_IN);

			/* 5. Change Camera Arm */
			CCamera_Manager::GetInstance()->Start_Turn_ArmVector(eCamType, 0.0f, XMVector3Normalize(XMVectorSet(0.0f, 0.25f, -1.0f, 0.0f)), RATIO_TYPE::LERP);

			/* 6. FadeIn */
			CCamera_Manager::GetInstance()->Start_FadeIn();

			/* 7. Play Dialogue */
			_int iDialogueIndex = pZetPack_Child->Get_DialogueIndex();
			_wstring strDialogueTag = TEXT("ZetPack_Child_");
			strDialogueTag += to_wstring(iDialogueIndex);
			CDialog_Manager::GetInstance()->Set_DialogId(strDialogueTag.c_str());
			pZetPack_Child->Plus_DialogueIndex();
		}

		/* 8. 다이얼로그 종료 체크 */
		if (false == CDialog_Manager::GetInstance()->Get_DisPlayDialogue())
		{
			static bool isFirst = false;
			if (false == isFirst)
			{
				isFirst = true;
				CCamera_Manager::GetInstance()->Start_FadeOut(0.5f);
				m_fTimer = 0.0f;
			}
			Next_Step_Over(0.7f);
		}
	}
	else if (Step_Check(STEP_4)) // 4. 캔들게임 컵 촬영 
	{
		if (Is_Start())
		{
			CPortal* pTargetPortal = static_cast<CPortal_Default*>(static_cast<CSection_2D_PlayMap*>(CSection_Manager::GetInstance()->Find_Section(TEXT("Chapter6_SKSP_06")))->Get_Portal(0));
			if (nullptr == pTargetPortal)
			{
				/* 플레이어 인풋락 해제 */
				pPlayer->Set_BlockPlayerInput(false);
				GameEvent_End();
				return;
			}
			/* 3. Target Change */
			_float2 v2DPos = {};
			XMStoreFloat2(&v2DPos, pTargetPortal->Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_State(CTransform::STATE_POSITION));
			_vector vPortalLockerWorldPos = CSection_Manager::GetInstance()->Get_WorldPosition_FromWorldPosMap(TEXT("Chapter6_SKSP_06"), v2DPos);
			XMStoreFloat4x4(&m_TargetWorldMatrix, XMMatrixTranslationFromVector(XMVectorSetW(vPortalLockerWorldPos, 1.0f)));
			CCamera_Manager::GetInstance()->Change_CameraTarget(&m_TargetWorldMatrix, 0.0f);

			/* 4. Change Camera Length */
			CCamera_Manager::GetInstance()->Start_Changing_ArmLength(eCamType, 0.0f, 26.0f, RATIO_TYPE::EASE_IN);

			/* 5. Change Camera Arm */
			CCamera_Manager::GetInstance()->Start_Turn_ArmVector(eCamType, 0.0f, XMVector3Normalize(XMVectorSet(-1.0f, 0.8f, -1.0f, 0.0f)), RATIO_TYPE::LERP);

			/* 6. FadeIn */
			CCamera_Manager::GetInstance()->Start_FadeIn();

			/* 7. Play Dialogue */
			_int iDialogueIndex = pZetPack_Child->Get_DialogueIndex();
			_wstring strDialogueTag = TEXT("ZetPack_Child_");
			strDialogueTag += to_wstring(iDialogueIndex);
			CDialog_Manager::GetInstance()->Set_DialogId(strDialogueTag.c_str());
			pZetPack_Child->Plus_DialogueIndex();
		}

		/* 8. 다이얼로그 종료 체크 */
		if (false == CDialog_Manager::GetInstance()->Get_DisPlayDialogue())
		{
			static bool isFirst = false;
			if (false == isFirst)
			{
				isFirst = true;
				CCamera_Manager::GetInstance()->Start_FadeOut(0.5f);
				m_fTimer = 0.0f;
			}
			Next_Step_Over(0.7f);
		}
	}
	else if (Step_Check(STEP_5)) // 4. 중앙 컵 촬영 
	{
		if (Is_Start())
		{
			CPortal* pTargetPortal = static_cast<CPortal_Default*>(static_cast<CSection_2D_PlayMap*>(CSection_Manager::GetInstance()->Find_Section(TEXT("Chapter6_SKSP_04")))->Get_Portal(0));
			if (nullptr == pTargetPortal)
			{
				/* 플레이어 인풋락 해제 */
				pPlayer->Set_BlockPlayerInput(false);
				GameEvent_End();
				return;
			}
			/* 3. Target Change */
			_float2 v2DPos = {};
			XMStoreFloat2(&v2DPos, pTargetPortal->Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_State(CTransform::STATE_POSITION));
			_vector vPortalLockerWorldPos = CSection_Manager::GetInstance()->Get_WorldPosition_FromWorldPosMap(TEXT("Chapter6_SKSP_04"), v2DPos);
			XMStoreFloat4x4(&m_TargetWorldMatrix, XMMatrixTranslationFromVector(XMVectorSetW(vPortalLockerWorldPos, 1.0f)));
			CCamera_Manager::GetInstance()->Change_CameraTarget(&m_TargetWorldMatrix, 0.0f);

			/* 4. Change Camera Length */
			CCamera_Manager::GetInstance()->Start_Changing_ArmLength(eCamType, 0.0f, 30.0f, RATIO_TYPE::EASE_IN);

			/* 5. Change Camera Arm */
			_vector vNegativeZ = { 0.0f, 0.0f, -1.0f, 0.0f };
			vNegativeZ = XMVector3TransformNormal(vNegativeZ, XMMatrixRotationY(PI * -0.2f) * XMMatrixRotationX(XMConvertToRadians(10.f)));
			CCamera_Manager::GetInstance()->Start_Turn_ArmVector(eCamType, 0.0f, XMVector3Normalize(vNegativeZ), RATIO_TYPE::LERP);

			/* 6. FadeIn */
			CCamera_Manager::GetInstance()->Start_FadeIn();

			/* 7. Play Dialogue */
			_int iDialogueIndex = pZetPack_Child->Get_DialogueIndex();
			_wstring strDialogueTag = TEXT("ZetPack_Child_");
			strDialogueTag += to_wstring(iDialogueIndex);
			CDialog_Manager::GetInstance()->Set_DialogId(strDialogueTag.c_str());
			pZetPack_Child->Plus_DialogueIndex();
		}

		/* 8. 다이얼로그 종료 체크 */
		if (false == CDialog_Manager::GetInstance()->Get_DisPlayDialogue())
		{
			static bool isFirst = false;
			if (false == isFirst)
			{
				isFirst = true;
				CCamera_Manager::GetInstance()->Start_FadeOut(0.5f);
				m_fTimer = 0.0f;
			}
			Next_Step_Over(0.7f);
		}
	}
	else if (Step_Check(STEP_6)) // 5. 다시 플레이어
	{
		if (Is_Start())
		{
			/* 3. Target Change */
			CCamera_Manager::GetInstance()->Change_CameraTarget(pPlayer, 0.0f);

			/* 4. Change Camera Length */
			CCamera_Manager::GetInstance()->Start_Changing_ArmLength(eCamType, 0.0f, 5.5f, RATIO_TYPE::EASE_IN);

			/* 5. Change Camera Arm */
			_vector vNegativeZ = { 0.0f, 0.0f, -1.0f, 0.0f };
			vNegativeZ = XMVector3TransformNormal(vNegativeZ, XMMatrixRotationY(PI * 0.3f) * XMMatrixRotationX(PI * 0.12f));
			CCamera_Manager::GetInstance()->Start_Turn_ArmVector(eCamType, 0.0f, XMVector3Normalize(vNegativeZ), RATIO_TYPE::LERP);

			/* 6. FadeIn */
			CCamera_Manager::GetInstance()->Start_FadeIn();

			/* 7. Play Dialogue */
			_int iDialogueIndex = pZetPack_Child->Get_DialogueIndex();
			_wstring strDialogueTag = TEXT("ZetPack_Child_");
			strDialogueTag += to_wstring(iDialogueIndex);
			CDialog_Manager::GetInstance()->Set_DialogId(strDialogueTag.c_str());
			pZetPack_Child->Plus_DialogueIndex();
		}

		/* 8. 다이얼로그 종료 체크 */
		if (false == CDialog_Manager::GetInstance()->Get_DisPlayDialogue())
		{
			static bool isFirst = false;
			if (false == isFirst)
			{
				isFirst = true;
				CCamera_Manager::GetInstance()->Start_ResetArm_To_SettingPoint(eCamType, 1.0f);
				m_fTimer = 0.0f;
			}
			Next_Step_Over(1.0f);
		}
	}
	else
	{
		pPlayer->Set_BlockPlayerInput(false);
		GameEvent_End();
	}
}

void CGameEventExecuter_C6::Chapter6_FatherGame_Progress_Fatherpart_1(_float _fTimeDelta)
{
	// TODO :: 03.09
	m_fTimer += _fTimeDelta;
	CCamera_Manager::CAMERA_TYPE eCamType = CCamera_Manager::TARGET;
	CPlayer* pPlayer = Get_Player();
	CZetPack_Child* pZetPack_Child = CFatherGame::GetInstance()->Get_ZetPack_Child();
	if (nullptr == pPlayer)
	{
		GameEvent_End();
		return;
	}
	/* 플레이어 인풋락  */
	pPlayer->Set_BlockPlayerInput(true);

	if (Step_Check(STEP_0))
	{
		/* 1. Save Reset ArmData */
		CCamera_Manager::GetInstance()->Set_ResetData(eCamType);
		_float fTime = 1.0f;
		/* 4. Change Camera Length */
		CCamera_Manager::GetInstance()->Start_Changing_ArmLength_Increase(eCamType, fTime, 2.0f, RATIO_TYPE::EASE_IN);
		Next_Step_Over(1.5f);
	}
	else if (Step_Check(STEP_1)) // 1. 카메라 이동 
	{
		if (Is_Start())
		{
			/* 2. Change Arm Vector  */
			//CCamera_Manager::GetInstance()->Get_Camera();

			_float fTime = 1.5f;
			CCamera_Manager::GetInstance()->Start_Changing_AtOffset(eCamType, fTime, XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f), RATIO_TYPE::LERP);
			CCamera_Manager::GetInstance()->Start_Turn_AxisY(eCamType, fTime, XMConvertToRadians(-45.f) / fTime, XMConvertToRadians(-45.f) / fTime);
			CCamera_Manager::GetInstance()->Start_Turn_AxisRight(eCamType, fTime, XMConvertToRadians(-10.f) / fTime, XMConvertToRadians(-10.f) / fTime);

			/* 4. Change Camera Length */
			CCamera_Manager::GetInstance()->Start_Changing_ArmLength_Decrease(eCamType, fTime, 9.0f, RATIO_TYPE::EASE_IN);
		}


		Next_Step_Over(2.0f);
	}
	else if (Step_Check(STEP_2)) // 2. 찌릿이 대화 + 페이드 아웃
	{
		if (Is_Start())
		{
			/* 5. 다이얼로그 재생. */
			_int iDialogueIndex = pZetPack_Child->Get_DialogueIndex();
			_wstring strDialogueTag = TEXT("ZetPack_Child_");
			strDialogueTag += to_wstring(iDialogueIndex);
			CDialog_Manager::GetInstance()->Set_DialogId(strDialogueTag.c_str());
			pZetPack_Child->Plus_DialogueIndex();
		}

		/* 6. 다이얼로그 종료 체크 */
		if (false == CDialog_Manager::GetInstance()->Get_DisPlayDialogue())
		{
			Next_Step(true);
		}
	}
	else if (Step_Check(STEP_3))
	{
		if (Is_Start())
		{
			CCamera_Manager::GetInstance()->Start_ResetArm_To_SettingPoint(eCamType, 1.0f);
		}
		Next_Step_Over(1.2f);
	}
	else
	{
		pPlayer->Set_BlockPlayerInput(false);
		GameEvent_End();
	}
}

void CGameEventExecuter_C6::Chapter6_FatherGame_Progress_Fatherpart_2(_float _fTimeDelta)
{
	// TODO :: 03.09
	m_fTimer += _fTimeDelta;
	CCamera_Manager::CAMERA_TYPE eCamType = CCamera_Manager::TARGET;
	CPlayer* pPlayer = Get_Player();
	CZetPack_Child* pZetPack_Child = CFatherGame::GetInstance()->Get_ZetPack_Child();
	if (nullptr == pPlayer)
	{
		GameEvent_End();
		return;
	}
	/* 플레이어 인풋락  */
	pPlayer->Set_BlockPlayerInput(true);

	if (Step_Check(STEP_0))
	{
		if (Is_Start())
		{
			/* 1. Save Reset ArmData */
			CCamera_Manager::GetInstance()->Set_ResetData(eCamType);

			CCamera_Manager::GetInstance()->Start_FadeOut();
		}

		Next_Step_Over(1.2f);
	}
	else if (Step_Check(STEP_1)) // 1. 카메라 이동 
	{
		if (Is_Start())
		{
			CPortal* pTargetPortal = static_cast<CPortal_Default*>(static_cast<CSection_2D_PlayMap*>(CSection_Manager::GetInstance()->Find_Section(TEXT("Chapter6_SKSP_03")))->Get_Portal(0));
			if (nullptr == pTargetPortal)
			{
				/* 플레이어 인풋락 해제 */
				pPlayer->Set_BlockPlayerInput(false);
				GameEvent_End();
				return;
			}
			/* 3. Target Change */
			_float2 v2DPos = {};
			XMStoreFloat2(&v2DPos, pTargetPortal->Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_State(CTransform::STATE_POSITION));
			_vector vPortalLockerWorldPos = CSection_Manager::GetInstance()->Get_WorldPosition_FromWorldPosMap(TEXT("Chapter6_SKSP_04"), v2DPos);
			XMStoreFloat4x4(&m_TargetWorldMatrix, XMMatrixTranslationFromVector(XMVectorSetW(vPortalLockerWorldPos + XMVectorSet(-4.5f, 3.0f, 0.0f, 0.0f), 1.0f)));
			CCamera_Manager::GetInstance()->Change_CameraTarget(&m_TargetWorldMatrix, 0.0f);

			/* 4. Change Camera Length */
			CCamera_Manager::GetInstance()->Start_Changing_ArmLength(eCamType, 0.0f, 37.0f, RATIO_TYPE::EASE_IN);

			/* 5. Change Camera Arm */
			_vector vNegativeZ = { 0.05f, 0.12f, -1.0f, 0.0f };
			//vNegativeZ = XMVector3TransformNormal(vNegativeZ, XMMatrixRotationY(PI * -0.2f) * XMMatrixRotationX(XMConvertddToRadians(10.f)));
			CCamera_Manager::GetInstance()->Start_Turn_ArmVector(eCamType, 0.0f, XMVector3Normalize(vNegativeZ), RATIO_TYPE::LERP);
			//CCamera_Manager::GetInstance().
			/* 6. FadeIn */
			CCamera_Manager::GetInstance()->Start_FadeIn();

		}



		Next_Step_Over(3.0f);
	}
	else if (Step_Check(STEP_2)) // 1. 카메라 이동 
	{
		if (Is_Start())
		{
			CPortal* pTargetPortal = static_cast<CPortal_Default*>(static_cast<CSection_2D_PlayMap*>(CSection_Manager::GetInstance()->Find_Section(TEXT("Chapter6_SKSP_04")))->Get_Portal(0));
			if (nullptr == pTargetPortal)
			{
				/* 플레이어 인풋락 해제 */
				pPlayer->Set_BlockPlayerInput(false);
				GameEvent_End();
				return;
			}
			_float fTime = 1.5f;
			/* 3. Target Change */
			_float2 v2DPos = {};
			XMStoreFloat2(&v2DPos, pTargetPortal->Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_State(CTransform::STATE_POSITION));
			_vector vPortalLockerWorldPos = CSection_Manager::GetInstance()->Get_WorldPosition_FromWorldPosMap(TEXT("Chapter6_SKSP_04"), v2DPos);
			XMStoreFloat4x4(&m_TargetWorldMatrix, XMMatrixTranslationFromVector(XMVectorSetW(vPortalLockerWorldPos, 1.0f)));
			CCamera_Manager::GetInstance()->Change_CameraTarget(&m_TargetWorldMatrix, fTime);

			/* At Offset */
			CCamera_Manager::GetInstance()->Start_Changing_AtOffset(eCamType, 1.0f, XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), RATIO_TYPE::EASE_IN);
			/* 4. Change Camera Length */
			CCamera_Manager::GetInstance()->Start_Changing_ArmLength(eCamType, fTime, 8.0f, RATIO_TYPE::EASE_IN);

			/* 5. Change Camera Arm */
			_vector vNegativeZ = { 0.0f, 0.0f, -1.0f, 0.0f };
			vNegativeZ = XMVector3TransformNormal(vNegativeZ, XMMatrixRotationY(PI * -0.18f) * XMMatrixRotationX(XMConvertToRadians(5.f)));
			CCamera_Manager::GetInstance()->Start_Turn_ArmVector(eCamType, fTime, XMVector3Normalize(vNegativeZ), RATIO_TYPE::LERP);
		}
		Next_Step_Over(2.0f);
	}
	else if (Step_Check(STEP_3)) // 1. 카메라 이동 
	{
		if (Is_Start())
		{
			/* 7. Play Dialogue */
			_int iDialogueIndex = pZetPack_Child->Get_DialogueIndex();
			_wstring strDialogueTag = TEXT("ZetPack_Child_");
			strDialogueTag += to_wstring(iDialogueIndex);
			CDialog_Manager::GetInstance()->Set_DialogId(strDialogueTag.c_str());
			pZetPack_Child->Plus_DialogueIndex();
		}

		/* 8. 다이얼로그 종료 체크 */
		if (false == CDialog_Manager::GetInstance()->Get_DisPlayDialogue())
		{
			static bool isFirst = false;
			if (false == isFirst)
			{
				isFirst = true;
				CFatherGame::GetInstance()->OpenPortalLocker(CFatherGame::LOCKER_PARTHEAD);

				m_fTimer = 0.0f;
			}
			Next_Step_Over(5.0f);
		}

	}
	else if (Step_Check(STEP_4)) // 1. 카메라 이동 
	{
		if (Is_Start())
		{
			/* 6. FadeIn */
			CCamera_Manager::GetInstance()->Start_FadeOut();
		}

		Next_Step_Over(1.2f);
	}
	else
	{
		/* 3. Target Change */
		CCamera_Manager::GetInstance()->Change_CameraTarget(pPlayer, 0.0f);
		CCamera_Manager::GetInstance()->Start_FadeIn();
		CCamera_Manager::GetInstance()->Start_ResetArm_To_SettingPoint(eCamType, 0.0f);
		pPlayer->Set_BlockPlayerInput(false);
		GameEvent_End();
	}

}

void CGameEventExecuter_C6::Chapter6_FatherGame_Progress_Fatherpart_3(_float _fTimeDelta)
{
	// TODO :: 03.09
	m_fTimer += _fTimeDelta;
	CCamera_Manager::CAMERA_TYPE eCamType = CCamera_Manager::TARGET;
	CPlayer* pPlayer = Get_Player();
	CZetPack_Child* pZetPack_Child = CFatherGame::GetInstance()->Get_ZetPack_Child();
	if (nullptr == pPlayer)
	{
		GameEvent_End();
		return;
	}
	/* 플레이어 인풋락  */
	pPlayer->Set_BlockPlayerInput(true);

	if (Step_Check(STEP_0))
	{
		/* 1. Save Reset ArmData */
		CCamera_Manager::GetInstance()->Set_ResetData(eCamType);
		Next_Step_Over(1.5f);
	}
	else if (Step_Check(STEP_1)) // 1. 카메라 이동 
	{
		if (Is_Start())
		{
			/* 2. Change Arm Vector  */
			//CCamera_Manager::GetInstance()->Get_Camera();

			_float fTime = 1.0f;
			CCamera_Manager::GetInstance()->Start_Changing_AtOffset(eCamType, fTime, XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f), RATIO_TYPE::LERP);
			CCamera_Manager::GetInstance()->Start_Turn_AxisY(eCamType, fTime, XMConvertToRadians(45.f) / fTime, XMConvertToRadians(45.f) / fTime);
			CCamera_Manager::GetInstance()->Start_Turn_AxisRight(eCamType, fTime, XMConvertToRadians(-5.f) / fTime, XMConvertToRadians(-5.f) / fTime);

			/* 4. Change Camera Length */
			CCamera_Manager::GetInstance()->Start_Changing_ArmLength_Decrease(eCamType, fTime, 6.0f, RATIO_TYPE::EASE_IN);
		}


		Next_Step_Over(1.2f);
	}
	else if (Step_Check(STEP_2)) // 2. 찌릿이 대화 + 페이드 아웃
	{
		if (Is_Start())
		{
			/* 5. 다이얼로그 재생. */
			_int iDialogueIndex = pZetPack_Child->Get_DialogueIndex();
			_wstring strDialogueTag = TEXT("ZetPack_Child_");
			strDialogueTag += to_wstring(iDialogueIndex);
			CDialog_Manager::GetInstance()->Set_DialogId(strDialogueTag.c_str());
			pZetPack_Child->Plus_DialogueIndex();
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

void CGameEventExecuter_C6::Chapter6_Humgrump_Revolt(_float _fTimeDelta)
{
	m_fTimer += _fTimeDelta;

	if (Step_Check(STEP_0))
	{
		if (Is_Start()) {
			/* 플레이어 자동 이동 */
			CPlayer* pPlayer = Get_Player();

			AUTOMOVE_COMMAND tCommand = {};
			tCommand.eType = AUTOMOVE_TYPE::MOVE_TO;
			tCommand.iAnimIndex = (_uint)CPlayer::ANIM_STATE_2D::PLAYER_RUN_SWORD_UP;
			tCommand.vTarget = XMVectorSet(0.f, -435.f, 0.0f, 1.f);
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
		}
		
		/* 중앙으로 카메라 Target 변경 */
		if (m_fTimer > 0.8f) {
			static _float4x4 matCenter = {};
			_vector vCenter = XMVectorSet(0.f, -180.f, 0.f, 1.f);

			memcpy(&matCenter.m[3], &vCenter, sizeof(_float4));

			CCamera_Manager::GetInstance()->Change_CameraTarget(&matCenter, 0.5f);

			Next_Step(true);
		}
	}
	else if (Step_Check(STEP_1)) {

		CPlayer* pPlayer = Get_Player();

		if (false == pPlayer->Is_AutoMoving()) {
			pPlayer->Set_BlockPlayerInput(true);

			CDialog_Manager::GetInstance()->Set_DialogId(L"Dialogue_Before_Humgrump_Revolt");

			CLayer* pLayer = m_pGameInstance->Find_Layer(m_iCurLevelID, TEXT("Layer_NPC"));

			list<CGameObject*> pObjects = pLayer->Get_GameObjects();

			for (auto& pObject : pObjects) {
				CModelObject* pModel = dynamic_cast<CModelObject*>(pObject);

				if (nullptr == pModel)
					continue;

				if (TEXT("Humgrump") == pModel->Get_ModelPrototypeTag(COORDINATE_2D)) {
					pModel->Switch_Animation(CNpc_Humgrump::CHAPTER6_TALK);
					CDialog_Manager::GetInstance()->Set_NPC(pModel);
					m_pTargetObject = pObject;
				}

				if (TEXT("MoonBeard") == pModel->Get_ModelPrototypeTag(COORDINATE_2D)) {
					CDialog_Manager::GetInstance()->Set_NPC(pModel);
					m_pTargetObject_Second = pObject;
				}
			}

			Next_Step(true);
		}
	}
	else if (Step_Check(STEP_2)) {

		if (4 == CDialog_Manager::GetInstance()->Get_CurrentLineIndex()) {

			if (Is_Start()) {
				static_cast<CModelObject*>(m_pTargetObject)->Switch_Animation(CNpc_Humgrump::CHAPTER6_IDLE);
				static_cast<CModelObject*>(m_pTargetObject_Second)->Switch_Animation(CNpc_MoonBeard::CHAPTER6_GIVE_IT_AREST);
			}
		}

		if (5 == CDialog_Manager::GetInstance()->Get_CurrentLineIndex()) {
			static_cast<CModelObject*>(m_pTargetObject_Second)->Switch_Animation(CNpc_MoonBeard::CHAPTER6_GIVE_IT_AREST_IDLE);
			Next_Step(true);
		}
	}
	else if (Step_Check(STEP_3)) {
		if (7 == CDialog_Manager::GetInstance()->Get_CurrentLineIndex() && 0 == m_iSubStep) {

			// 1. 험그럼프 손가락 올리기 전
			static_cast<CModelObject*>(m_pTargetObject)->Switch_Animation(CNpc_Humgrump::CHAPTER6_TALK_STAND);

			// 2. Cage 생성
			CGameObject* pObject = nullptr;

			CModelObject::MODELOBJECT_DESC Desc = {};
			Desc.tTransform2DDesc.vInitialPosition = _float3(0.f, 710.0f, 0.f);
			Desc.iCurLevelID = m_iCurLevelID;
			Desc.eStartCoord = COORDINATE_2D;
			Desc.isCoordChangeEnable = false;
			Desc.strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
			Desc.iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;
			Desc.tTransform2DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);

			Desc.iModelPrototypeLevelID_2D = LEVEL_CHAPTER_6;
			Desc.strModelPrototypeTag_2D = TEXT("Cage");

			/* 첫 번째 Cage */
			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"),
				m_iCurLevelID, TEXT("Layer_MovingObject"), &pObject, &Desc);

			CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter6_P1314"), pObject, SECTION_2D_PLAYMAP_EFFECT);

			/* 두 번째 Cage */
			Desc.tTransform2DDesc.vInitialPosition = _float3(563.87f, 1037.0f, 0.f);

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"),
				m_iCurLevelID, TEXT("Layer_MovingObject"), &pObject, &Desc);

			CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter6_P1314"), pObject, SECTION_2D_PLAYMAP_EFFECT);

			m_iSubStep++;
		}
		else if (1 == m_iSubStep) {
			if (false == CDialog_Manager::GetInstance()->Get_DisPlayDialogue()) {

				// 3. 험그럼프 손가락 올리기
				if (m_fTimer >= 0.4f) {
					if (Is_Start()) {
						static_cast<CModelObject*>(m_pTargetObject)->Switch_Animation(CNpc_Humgrump::CHAPTER6_FINGER_RAISE);

						m_iSubStep++;
					}
				}
			}
		}
		else if (2 == m_iSubStep) {
			// 4. 두 번쨰 다이얼로그 시작
			if (m_fTimer >= 0.6f) {
				CDialog_Manager::GetInstance()->Set_DialogId(L"Dialogue_Humgrump_Revolt");
				CDialog_Manager::GetInstance()->Set_NPC(m_pTargetObject);

				m_iSubStep++;
			}
		}
		else if (3 == m_iSubStep) {
			if (false == static_cast<CModelObject*>(m_pTargetObject)->Is_DuringAnimation()) {
				static_cast<CModelObject*>(m_pTargetObject)->Switch_Animation(CNpc_Humgrump::CHAPTER6_FINGER_TALK);
			}

			// 3. 험그럼프 손가락 내리기
			if (false == CDialog_Manager::GetInstance()->Get_DisPlayDialogue()) {
				static_cast<CModelObject*>(m_pTargetObject)->Switch_Animation(CNpc_Humgrump::CHAPTER6_FINGER_DOWN);
				Next_Step(true);
			}
		}
	}
	else if (Step_Check(STEP_4)) {

		static _float fPreIncrease = {};
		static _float2 fTime = { 0.8f, 0.f };

		if (m_fTimer >= 0.2f) {

			// 4. Cage 떨어뜨리기
			_float fRatio = m_pGameInstance->Calculate_Ratio(&fTime, _fTimeDelta, EASE_IN);
			_float fIncrease = m_pGameInstance->Lerp(0.f, 1150.f, fRatio);

			// 5. 달수염 놀라기
			if (fRatio >= 0.9f) {
				if (Is_Start())
					static_cast<CModelObject*>(m_pTargetObject_Second)->Switch_Animation(CNpc_MoonBeard::CHAPTER6_SURPRISED);
			}

			if (fRatio >= (1.f - EPSILON)) {

				CCamera_Manager::GetInstance()->Start_Shake_ByTime(CCamera_Manager::TARGET_2D, 0.3f, 0.02f);
				Next_Step(true);
			}

			_float fValue = fIncrease - fPreIncrease;
			fPreIncrease = fIncrease;


			CLayer* pLayer = m_pGameInstance->Find_Layer(m_iCurLevelID, TEXT("Layer_MovingObject"));

			list<CGameObject*> pObjects = pLayer->Get_GameObjects();

			for (auto& pObject : pObjects) {
				_vector vPos = pObject->Get_ControllerTransform()->Get_State(CTransform::STATE_POSITION);
				vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) - fValue);
				pObject->Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, vPos);
			}
		}
	}
	else if (Step_Check(STEP_5)) {

		// 6. 달수염 뒤돌기
		if (m_fTimer >= 0.3f) {
			if (Is_Start()) {
				static_cast<CModelObject*>(m_pTargetObject_Second)->Switch_Animation(CNpc_MoonBeard::CHAPTER6_SURPRISED_TURN);
				static_cast<CNpc_MoonBeard*>(m_pTargetObject_Second)->Set_Opposite_Side();
			}
		}

		// 7. 험그럼프 Windup 시작
		if (false == static_cast<CModelObject*>(m_pTargetObject_Second)->Is_DuringAnimation()) {
			static_cast<CModelObject*>(m_pTargetObject_Second)->Switch_Animation(CNpc_MoonBeard::CHAPTER6_SURPRISED_IDLE);
			static_cast<CModelObject*>(m_pTargetObject)->Switch_Animation(CNpc_Humgrump::CHAPTER6_WINDUP);
			Next_Step(true);
		}

	}
	else if (Step_Check(STEP_6)) {
		if (false == static_cast<CModelObject*>(m_pTargetObject)->Is_DuringAnimation()) {
			// 8. 험그럼프 Beam, Beam 이펙트 시작, 진동
			static_cast<CModelObject*>(m_pTargetObject)->Switch_Animation(CNpc_Humgrump::CHAPTER6_BEAM);

			CEffect2D_Manager::GetInstance()->Play_Effect(TEXT("Beam"), TEXT("Chapter6_P1314"), XMMatrixTranslation(70.f, -105.07f, 0.f), 0.f, 0, true, 2.5f, SECTION_2D_PLAYMAP_OBJECT);
			CCamera_Manager::GetInstance()->Start_Shake_ByTime(CCamera_Manager::TARGET_2D, 2.5f, 0.02f);

			Next_Step(true);
		}

	}
	else if (Step_Check(STEP_7)) {

		if (m_fTimer >= 0.2f) {
			if (Is_Start())
				static_cast<CModelObject*>(m_pTargetObject_Second)->Switch_Animation(CNpc_MoonBeard::CHAPTER6_ZAP_INTO);
		}

		if (false == static_cast<CModelObject*>(m_pTargetObject_Second)->Is_DuringAnimation()) {
			static_cast<CModelObject*>(m_pTargetObject_Second)->Switch_Animation(CNpc_MoonBeard::CHAPTER6_ZAP_IDLE);

			Next_Step(true);
		}
	}
	else if (Step_Check(STEP_8)) {
		
		// 8.5. 바이올렛이 소리치기
		if (0 == m_iSubStep) {

			m_iSubStep++;
		}
		
		if (m_fTimer >= 2.5f) {
			if (Is_Start()) {
				// 9. 험그럼프 Beam 끝, 할배 죽기 시작
				static_cast<CModelObject*>(m_pTargetObject)->Switch_Animation(CNpc_Humgrump::CHAPTER6_BEAM_END);
				static_cast<CModelObject*>(m_pTargetObject_Second)->Switch_Animation(CNpc_MoonBeard::CHAPTER6_ZAP_DEATH);
			}
		}

		if (false == static_cast<CModelObject*>(m_pTargetObject)->Is_DuringAnimation()) {
			static_cast<CModelObject*>(m_pTargetObject)->Switch_Animation(CNpc_Humgrump::CHAPTER6_TRANSFORM_IDLE);

			Next_Step(true);
		}
	}
	else if (Step_Check(STEP_9)) {
		if (m_fTimer >= 4.f) {
			static_cast<CModelObject*>(m_pTargetObject)->Switch_Animation(CNpc_Humgrump::CHAPTER6_TRANSFORM_TURN);

			Next_Step(true);
		}
	}
	// 10. 험그럼프로 줌인, 마지막 대사 말하기
	else if (Step_Check(STEP_10)) {
		if (false == static_cast<CModelObject*>(m_pTargetObject)->Is_DuringAnimation()) {
			static_cast<CModelObject*>(m_pTargetObject)->Switch_Animation(CNpc_Humgrump::CHAPTER6_TRANSFORM_TURN_TALK);
			CDialog_Manager::GetInstance()->Set_DialogId(L"Dialogue_After_Humgrump_Revolt");
			CDialog_Manager::GetInstance()->Set_NPC(m_pTargetObject);
			CCamera_Manager::GetInstance()->Change_CameraTarget(m_pTargetObject, 0.5f);
			CCamera_Manager::GetInstance()->Start_Changing_ArmLength_Decrease(CCamera_Manager::TARGET_2D, 0.5f, 4.f, EASE_IN_OUT);
			Next_Step(true);
		}
	}
	// 11. 험그럼프 웃기
	else if (Step_Check(STEP_11)) {
		if (false == CDialog_Manager::GetInstance()->Get_DisPlayDialogue()) {
			if (Is_Start())
				static_cast<CModelObject*>(m_pTargetObject)->Switch_Animation(CNpc_Humgrump::CHAPTER6_TRANSFORM_LAUGH_INTO);
		}

		if (false == static_cast<CModelObject*>(m_pTargetObject)->Is_DuringAnimation()) {
			static_cast<CModelObject*>(m_pTargetObject)->Switch_Animation(CNpc_Humgrump::CHAPTER6_TRANSFORM_LAUGH_IDLE);

			CEffect2D_Manager::GetInstance()->Play_Effect(TEXT("Humgrump_Ha"), TEXT("Chapter6_P1314"), XMMatrixTranslation(-271.54, -73.07f, 0.f), 0.f, 1, true, 10.5f, SECTION_2D_PLAYMAP_EFFECT);
			CEffect2D_Manager::GetInstance()->Play_Effect(TEXT("Humgrump_Ha"), TEXT("Chapter6_P1314"), XMMatrixTranslation(-211.54, -53.07f, 0.f), 0.5f, 1, true, 10.5f, SECTION_2D_PLAYMAP_EFFECT);
			CEffect2D_Manager::GetInstance()->Play_Effect(TEXT("Humgrump_Ha"), TEXT("Chapter6_P1314"), XMMatrixTranslation(-151.54, -73.07f, 0.f), 1.f, 1, true, 10.5f, SECTION_2D_PLAYMAP_EFFECT);
			Next_Step(true);
		}
	}
	// 12. 일정 시간 후 책장 넘기면서 나레이션 재생 시작, 험그럼프 책으로 바뀌는 이벤트 트리거 실행
	else if (Step_Check(STEP_12)) {
		if (m_fTimer >= 3.5f) {
			_float3 fDefaultPos = {};

			Event_Book_Main_Section_Change_Start(1, &fDefaultPos);
			CTrigger_Manager::GetInstance()->Register_TriggerEvent(TEXT("Chapter6_Change_Book_To_Greate_Humgrump"), 0);
			GameEvent_End();
		}
	}
}

void CGameEventExecuter_C6::Chapter6_Change_Book_To_Greate_Humgrump(_float _fTimeDelta)
{
	m_fTimer += _fTimeDelta;

	if (Step_Check(STEP_0)) {
		// 1. Narration이 끝났을 때 Book으로(가운데) 타겟 변경
		if (true == CUI_Manager::GetInstance()->is_EndNarration()) {

			CGameObject* pBook = m_pGameInstance->Get_GameObject_Ptr(m_iCurLevelID, TEXT("Layer_Book"), 0);

			CCamera_Manager::GetInstance()->Change_CameraTarget(pBook, 0.5f);
			CCamera_Manager::GetInstance()->Set_NextArmData(TEXT("BookFlipping_Horizon"), 0);
			CCamera_Manager::GetInstance()->Change_CameraMode(CCamera_2D::MOVE_TO_NEXTARM);

			CCamera_2D* pCamera = static_cast<CCamera_2D*>(CCamera_Manager::GetInstance()->Get_CurrentCamera());
			pCamera->Start_Changing_LengthValue(pCamera->Get_LengthValue(), 1.f);

			Next_Step(true);
		}
	}
	else if (Step_Check(STEP_1)) {

		// 2. 책 덮기
		if (m_fTimer >= 1.f && 0 == m_iSubStep) {
			
			CBook* pBook = static_cast<CBook*>(m_pGameInstance->Get_GameObject_Ptr(LEVEL_CHAPTER_6, TEXT("Layer_Book"), 0));
			pBook->Set_Animation(CBook::CLOSE_L_TO_R);

			m_iSubStep++;
		}

		// 2. 책 오른쪽을 바라보게 타겟 변경, Arm, Zoom, Length 조절
		if (1 == m_iSubStep) {
			CBook* pBook = static_cast<CBook*>(m_pGameInstance->Get_GameObject_Ptr(LEVEL_CHAPTER_6, TEXT("Layer_Book"), 0));
			if (false == pBook->Is_DuringAnimation()) {
				_float2 vRightBook2DPos = { 640.f, 0.f };
				_vector vRightBookPos = CSection_Manager::GetInstance()->Get_WorldPosition_FromWorldPosMap(TEXT("Chapter6_P1516"), vRightBook2DPos);

				memcpy(&m_TargetWorldMatrix.m[3], &vRightBookPos, sizeof(_float4));

				CCamera_Manager::GetInstance()->Change_CameraTarget(&m_TargetWorldMatrix, 0.8f);
				CCamera_Manager::GetInstance()->Start_Changing_ArmLength(CCamera_Manager::TARGET_2D, 0.8f, 23.1590042, EASE_IN_OUT);
				CCamera_Manager::GetInstance()->Start_Turn_ArmVector(CCamera_Manager::TARGET_2D, 0.8f, XMVectorSet(0.0f, 0.907808542f, -0.419384748f, 0.f), EASE_IN_OUT);
				CCamera_Manager::GetInstance()->Start_Zoom(CCamera_Manager::TARGET_2D, 0.8f, CCamera::LEVEL_4, EASE_IN_OUT);

				Next_Step(true);
			}
		}
	}
	else if (Step_Check(STEP_2)) {
		// 3. 험그럼프 책으로 변경
		if (m_fTimer >= 1.f) {
			CBook* pBook = static_cast<CBook*>(m_pGameInstance->Get_GameObject_Ptr(LEVEL_CHAPTER_6, TEXT("Layer_Book"), 0));
			pBook->Start_BookCover_Blending();

			Next_Step(true);
		}
	}
	else if (Step_Check(STEP_3)) {
		// 4. CutScene Camera로 변경
		if (m_fTimer >= 4.f) {
			CCamera_Manager::GetInstance()->Change_CameraType(CCamera_Manager::CUTSCENE, true, 0.2f);
			CCamera_Manager::GetInstance()->Set_NextCutSceneData(TEXT("Chapter6_Great_Humgrump"));

			Next_Step(true);
		}
	}
	else if (Step_Check(STEP_4)) {
		// 5. Level 변경
		if (m_fTimer >= 5.f) {
			CTrigger_Manager::GetInstance()->Register_TriggerEvent(L"Next_Chapter_Event", 0);
			GameEvent_End();
		}
	}
}


CGameEventExecuter_C6* CGameEventExecuter_C6::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CGameEventExecuter_C6* pInstance = new CGameEventExecuter_C6(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGameEventExecuter_C6");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGameEventExecuter_C6::Clone(void* _pArg)
{
	CGameEventExecuter_C6* pInstance = new CGameEventExecuter_C6(*this);
	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CGameEventExecuter_C6");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGameEventExecuter_C6::Free()
{
	__super::Free();
}


HRESULT CGameEventExecuter_C6::Cleanup_DeadReferences()
{
	return S_OK;
}
