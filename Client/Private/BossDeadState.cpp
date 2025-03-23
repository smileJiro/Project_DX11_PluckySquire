#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "BossDeadState.h"
#include "Monster.h"
#include "FSM.h"
#include "Camera_2D.h"

#include "Effect_Manager.h"
#include "Camera_Manager.h"

CBossDeadState::CBossDeadState()
{
}

HRESULT CBossDeadState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}


void CBossDeadState::State_Enter()
{
	m_pOwner->Set_AnimChangeable(false);

	//CCamera_Manager::GetInstance()->Start_Turn_AxisRight(CCamera_Manager::TARGET, 5.5f, XMConvertToRadians(-10.f), (_uint)RATIO_TYPE::EASE_IN);
	CCamera_Manager::GetInstance()->Start_Turn_AxisY(CCamera_Manager::TARGET, 4.5f, XMConvertToRadians(-75.f), (_uint)RATIO_TYPE::EASE_IN);
	CCamera_Manager::GetInstance()->Start_Changing_ArmLength_Increase(CCamera_Manager::TARGET, 4.5f, 7.f, (_uint)RATIO_TYPE::EASE_IN);
	CCamera_Manager::GetInstance()->Start_Changing_AtOffset(CCamera_Manager::TARGET, 4.5f, XMVectorSet(0.0f, -4.f, 8.0f, 1.0f), (_uint)RATIO_TYPE::EASE_IN);
	CCamera_Manager::GetInstance()->Start_Shake_ByCount(CCamera_Manager::TARGET, 7.5f, 0.8f, 90000, CCamera::SHAKE_Y, 0.0f);
	//_int iZoomLevel = CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET)->Get_CurrentZoomLevel();
	//CCamera_Manager::GetInstance()->Start_Zoom(CCamera_Manager::TARGET, 3.0f, max(iZoomLevel - 3, 0), (_uint)RATIO_TYPE::EASE_IN);

}

void CBossDeadState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;
	if (nullptr == m_pOwner)
		return;

	m_fAccTime += _fTimeDelta;

	_matrix matWorld = m_pOwner->Get_WorldMatrix();

	switch (m_iEffectCount)
	{
	case 0:
		if (0.f <= m_fAccTime)
		{
			m_pGameInstance->Set_SFXTargetVolume(TEXT("LCD_MUS_C09_BOSSBATTLE_STAGE2_LOOP_Stem_Group1"), 0.f, 5.f);
			m_pGameInstance->Set_SFXTargetVolume(TEXT("LCD_MUS_C09_BOSSBATTLE_STAGE2_LOOP_Stem_Group2"), 0.f, 5.f);
			m_pGameInstance->Set_BGMTargetVolume(0.f, 5.f);

			m_pGameInstance->Start_SFX(TEXT("A_sfx_C9DESK_LastBoss_Outro"), g_SFXVolume);
			m_pGameInstance->Start_SFX(TEXT("LCD_MUS_C09_LASTBOSS_OUTRO_FULL"), 1.5f);
			m_pGameInstance->Set_SFXTargetVolume(TEXT("LCD_MUS_C09_LASTBOSS_OUTRO_FULL"), g_SFXVolume);

			matWorld.r[3] += XMVectorSet(0.f, 17.f, 0.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_4"), true, matWorld);
			matWorld.r[3] += XMVectorSet(-6.f, 0.f, 0.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_1"), true, matWorld);
			matWorld.r[3] += XMVectorSet(9.f, 0.f, 4.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_2"), true, matWorld);
			matWorld.r[3] += XMVectorSet(-3.f, 2.f, -3.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_3"), true, matWorld);
			++m_iEffectCount;
		}
		break;
	case 1 :
		if (0.55f <= m_fAccTime)
		{
			matWorld.r[3] += XMVectorSet(0.f, 11.f, 0.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_4"), true, matWorld);
			matWorld.r[3] += XMVectorSet(7.f, 5.f, 0.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_1"), true, matWorld);
			matWorld.r[3] += XMVectorSet(-5.f, -2.f, 3.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_2"), true, matWorld);
			matWorld.r[3] += XMVectorSet(-3.f, 0.f, 0.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_3"), true, matWorld);
			++m_iEffectCount;
		}
		break;
	case 2:
		if (1.1f <= m_fAccTime)
		{
			matWorld.r[3] += XMVectorSet(0.f, 13.f, 0.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_4"), true, matWorld);
			matWorld.r[3] += XMVectorSet(-8.f, 0.f, 0.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_1"), true, matWorld);
			matWorld.r[3] += XMVectorSet(13.f, -1.f, 3.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_2"), true, matWorld);
			matWorld.r[3] += XMVectorSet(0.f, 2.f, 0.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_3"), true, matWorld);
			++m_iEffectCount;
		}
		break;
	case 3:
	{
		if (1.65f <= m_fAccTime)
		{
			matWorld.r[3] += XMVectorSet(0.f, 12.f, 0.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_4"), true, matWorld);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_1"), true, matWorld);
			matWorld.r[3] += XMVectorSet(0.f, 0.f, 2.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_2"), true, matWorld);
			matWorld.r[3] += XMVectorSet(-5.f, 0.f, 1.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_3"), true, matWorld);
			++m_iEffectCount;
		}
		break;
	}
	case 4:
	{
		if (2.2f <= m_fAccTime)
		{
			matWorld.r[3] += XMVectorSet(0.f, -3.f, 0.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_Red_1"), true, matWorld);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_Red_2"), true, matWorld);

			matWorld.r[3] += XMVectorSet(0.f, 4.f, 0.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_4"), true, matWorld);
			matWorld.r[3] += XMVectorSet(-3.f, -3.f, 1.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_1"), true, matWorld);
			

			++m_iEffectCount;
		}
		break;
	}
	case 5:
	{
		if (2.75f <= m_fAccTime)
		{
			matWorld.r[3] += XMVectorSet(0.f, 5.f, 0.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_Purple_1"), true, matWorld);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_Purple_2"), true, matWorld);

			matWorld.r[3] += XMVectorSet(0.f, 1.f, 2.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_4"), true, matWorld);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_2"), true, matWorld);


			++m_iEffectCount;
		}
		break;
	}
	case 6:
	{
		if (3.3f <= m_fAccTime)
		{
			matWorld.r[3] += XMVectorSet(0.f, 8.f, 0.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_4"), true, matWorld);
			matWorld.r[3] += XMVectorSet(0.f, 1.f, 0.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_3"), true, matWorld);			
			matWorld.r[3] += XMVectorSet(0.f, 3.f, -3.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_2"), true, matWorld);
			matWorld.r[3] += XMVectorSet(4.f, 0.f, 0.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_1"), true, matWorld);


			++m_iEffectCount;
		}

		break;
	}
	case 7:
		if (3.85f <= m_fAccTime)
		{
			matWorld.r[3] += XMVectorSet(0.f, 7.f, 0.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_4"), true, matWorld);
			matWorld.r[3] += XMVectorSet(0.f, 3.f, -3.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_3"), true, matWorld);
			matWorld.r[3] += XMVectorSet(3.f, 3.f, 3.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_2"), true, matWorld);
			matWorld.r[3] += XMVectorSet(2.f, 0.f, -2.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_1"), true, matWorld);
			++m_iEffectCount;
		}
		break;
	case 8:
	{
		if (4.3f <= m_fAccTime)
		{
			matWorld.r[3] += XMVectorSet(0.f, 6.f, 0.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_4"), true, matWorld);
			matWorld.r[3] += XMVectorSet(0.f, 0.f, -3.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_3"), true, matWorld);
			matWorld.r[3] += XMVectorSet(0.f, 3.f, -2.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_2"), true, matWorld);
			matWorld.r[3] += XMVectorSet(2.f, -2.f, -1.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_1"), true, matWorld);
			++m_iEffectCount;
		}
			break;
	}
	case 9:
	{
		if (4.85f <= m_fAccTime)
		{
			matWorld.r[3] += XMVectorSet(0.f, 7.f, 0.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_4"), true, matWorld);
			matWorld.r[3] += XMVectorSet(2.f, 0.f, -3.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_3"), true, matWorld);
			matWorld.r[3] += XMVectorSet(-2.f, -1.f, 5.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_2"), true, matWorld);
			matWorld.r[3] += XMVectorSet(6.f, 2.f, 1.f, 0.f);
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossDead_1"), true, matWorld);

			++m_iEffectCount;
		}
		break;
	}
	case 10:
	{
		if (5.4f <= m_fAccTime)
		{
			CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("BossEnd"), true, matWorld);

			++m_iEffectCount;
		}
		break;
	}
	case 11:
	{
		if (6.4f <= m_fAccTime) {
			///* ÅÂ¿õ Ãß°¡ */
			CCamera_Manager::GetInstance()->Start_FadeOut_White(1.0f);
			++m_iEffectCount;
		}
	}
	break;
	case 12:
	{
		if ((1.f - EPSILON) <= CCamera_Manager::GetInstance()->Get_DofBufferData().fFadeRatio) {
			CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET_2D)->Enter_Section(TEXT("Chapter8_P2526"));
			CCamera_Manager::GetInstance()->Change_CameraType(CCamera_Manager::TARGET_2D);
			CCamera_Manager::GetInstance()->Start_FadeIn_White(1.f);
		}
	}
	break;
	default:
		break;
	}


	//if (true == m_pOwner->Get_AnimChangeable())
	//	Event_ChangeBossState(BOSS_STATE::IDLE, m_pFSM);
}

void CBossDeadState::State_Exit()
{
}

CBossDeadState* CBossDeadState::Create(void* _pArg)
{
	CBossDeadState* pInstance = new CBossDeadState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CBossDeadState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossDeadState::Free()
{
	__super::Free();
}
