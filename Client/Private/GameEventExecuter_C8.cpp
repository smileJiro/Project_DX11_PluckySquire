#include "stdafx.h"
#include "GameEventExecuter_C8.h"
#include "Trigger_Manager.h"

#include "Camera_Manager.h"
#include "Camera_Target.h"
#include "Section_2D_PlayMap.h"
#include "Section_Manager.h"
#include "Player.h"

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
	_wstring strEventTag;
}

void CGameEventExecuter_C8::Update(_float _fTimeDelta)
{
	if (!Is_Dead())
	{
		switch ((CTrigger_Manager::EVENT_EXECUTER_ACTION_TYPE)m_iEventExcuterAction)
		{
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
		default:
			break;
		}
	}

}

void CGameEventExecuter_C8::Late_Update(_float _fTimeDelta)
{
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
