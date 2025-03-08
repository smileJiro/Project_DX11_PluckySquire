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
/* Object */
#include "Player.h"
#include "PortalLocker.h"

CGameEventExecuter_C6::CGameEventExecuter_C6(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CGameEventExecuter(_pDevice, _pContext)
{
}


HRESULT CGameEventExecuter_C6::Initialize(void* _pArg)
{
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
	_wstring strEventTag;
}

void CGameEventExecuter_C6::Update(_float _fTimeDelta)
{
	if (!Is_Dead())
	{
		switch ((CTrigger_Manager::EVENT_EXECUTER_ACTION_TYPE)m_iEventExcuterAction)
		{
		case Client::CTrigger_Manager::CHAPTER6_FATHERGAME_PROGRESS_START_CLEAR:
			Chapter6_FatherGame_Progress_Start_Clear(_fTimeDelta);
			break;
		default:
			break;
		}
	}

}

void CGameEventExecuter_C6::Late_Update(_float _fTimeDelta)
{
}

void CGameEventExecuter_C6::Chapter6_FatherGame_Progress_Start_Clear(_float _fTimeDelta)
{
	m_fTimer += _fTimeDelta;
	CCamera_Manager::CAMERA_TYPE eCamType = CCamera_Manager::TARGET;
	if (Step_Check(STEP_0))
	{

		if (Is_Start())
		{
			CPortalLocker* pPortalLocker = CFatherGame::GetInstance()->Get_PortalLocker(CFatherGame::LOCKER_ZETPACK);

			if(nullptr == pPortalLocker)
			{
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
			XMStoreFloat2(&v2DPos, pPortalLocker->Get_FinalPosition());
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
			CPlayer* pPlayer = Get_Player();

			if (nullptr == pPlayer)
				GameEvent_End();

			CCamera_Manager::GetInstance()->Change_CameraTarget(pPlayer, 1.0f);
			//CCamera_Manager::GetInstance()->Start_Changing_ArmLength_Increase(eCamType, 1.0f, 5.0f, RATIO_TYPE::EASE_OUT);
			CCamera_Manager::GetInstance()->Start_ResetArm_To_SettingPoint(eCamType, 1.0f);
		}
		Next_Step_Over(1.5f);
	}
	else
	{
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
