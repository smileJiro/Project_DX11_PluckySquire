#include "stdafx.h"
#include "GameEventExecuter_C4.h"
#include "Trigger_Manager.h"

#include "Camera_Manager.h"
#include "Camera_Target.h"
#include "Camera_2D.h"
#include "Camera_CutScene.h"

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
		case Client::CTrigger_Manager::CHAPTER4_RIDE_ZIPLINE:
			Chapter4_Ride_Zipline(_fTimeDelta);
			break;
		case Client::CTrigger_Manager::CHAPTER4_EVENT_FLAG:
			Chapter4_Event_Flag(_fTimeDelta);
			break;
		default:
			break;
		}
	}

}

void CGameEventExecuter_C4::Late_Update(_float _fTimeDelta)
{
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
	// 2. CutScene이 끝났다면 CutScene 카메라로 FadeOut 시작
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
