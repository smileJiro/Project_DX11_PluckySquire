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
/* Section */
#include "Section_2D_PlayMap.h"

/* Object */
#include "Portal_Default.h"
#include "Player.h"
#include "PortalLocker.h"
#include "ZetPack_Child.h"

/* Camera */
#include "Camera_CutScene.h"

/* Friend */
#include "Friend_Controller.h"
#include "Friend.h"

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
		case Client::CTrigger_Manager::CHAPTER6_FRIENDEVENT_0:
			Chapter6_FriendEvent_0(_fTimeDelta);
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
			if(nullptr == pTargetPortal)
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
			if(false == isFirst)
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
			CCamera_Manager::GetInstance()->Start_Changing_AtOffset(eCamType, 1.0f, XMVectorSet(0.0f,0.0f,0.0f,1.0f), RATIO_TYPE::EASE_IN);
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

void CGameEventExecuter_C6::Chapter6_FriendEvent_0(_float _fTimeDelta)
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
			_vector vPlayerPos = pPlayer->Get_FinalPosition() + XMVectorSet(0.0f, 160.f, 0.0f, 0.0f);
			_vector vThrashPos = pThrash->Get_FinalPosition() + XMVectorSet(0.0f, 200.f, 0.0f, 0.0f);
			_vector vVioletPos = pViolet->Get_FinalPosition() + XMVectorSet(0.0f, 200.f, 0.0f, 0.0f);
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
			m_iDialogueIndex = 0;
		}
		Next_Step_Over(2.0f);
	}
	else if (Step_Check(STEP_1)) // 1. 다이얼로그 시작.
	{
		if (Is_Start())
		{
			// Friend 대화 시작.
			_wstring strDialogueTag = TEXT("Chapter6_FriendEvent_0");
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
		CFriend_Controller::GetInstance()->Register_Friend_ToTrainList(TEXT("Thrash"));
		CFriend_Controller::GetInstance()->Register_Friend_ToTrainList(TEXT("Violet"));
		CFriend_Controller::GetInstance()->Start_Train();
		CCamera_Manager::GetInstance()->Start_ResetArm_To_SettingPoint(eCamType, 1.0f);
		pPlayer->Set_BlockPlayerInput(false);
		GameEvent_End();
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
