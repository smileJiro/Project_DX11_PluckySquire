#include "stdafx.h"
#include "GameEventExecuter.h"
#include "Trigger_Manager.h"
#include "Section_Manager.h"
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

CGameEventExecuter::CGameEventExecuter(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CGameObject(_pDevice, _pContext)
    , m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CGameEventExecuter::Initialize_Prototype()
{
	return S_OK;
}




HRESULT CGameEventExecuter::Initialize(void* _pArg)
{
    EVENT_EXECUTER_DESC* pDesc = static_cast<EVENT_EXECUTER_DESC*>(_pArg);

    m_iEventExcuterAction =
        CTrigger_Manager::GetInstance()->Find_ExecuterAction(pDesc->strEventTag);
    m_strEventTag = pDesc->strEventTag;
    if (m_iEventExcuterAction == -1 ||
        m_iEventExcuterAction == CTrigger_Manager::EVENT_EXECUTER_ACTION_TYPE_LAST)
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
        break;
    case Client::CTrigger_Manager::C02P0910_MONSTER_SPAWN:
        break;
    default:
        break;
    }


	return S_OK;
}

void CGameEventExecuter::Priority_Update(_float _fTimeDelta)
{
	_wstring strEventTag;
}

void CGameEventExecuter::Update(_float _fTimeDelta)
{
    if (!Is_Dead())
    {
        switch ((CTrigger_Manager::EVENT_EXECUTER_ACTION_TYPE)m_iEventExcuterAction)
        {
        case Client::CTrigger_Manager::C02P0910_LIGHTNING_BOLT_SPAWN:
            C020910_Bolt_Spawn(_fTimeDelta);
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
        default:
            break;
        }
    }

}

void CGameEventExecuter::Late_Update(_float _fTimeDelta)
{
}

void CGameEventExecuter::C020910_Bolt_Spawn(_float _fTimeDelta)
{
	if (0.f == m_fTimer)
    {
        //_float3 vPos = { 500.0f, 10.f, 0.f };
        _wstring strSectionKey = TEXT("Chapter2_P0910");
        //CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_LightningBolt"), COORDINATE_2D, &vPos, nullptr, nullptr, &strSectionKey);
        //CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_LightningBolt"), COORDINATE_2D, &vPos, nullptr, nullptr, &strSectionKey);

        //vPos = { -450.0f, -30.f, 0.f };
        //CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_LightningBolt"), COORDINATE_2D, &vPos, nullptr, nullptr, &strSectionKey);
        //CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_LightningBolt"), COORDINATE_2D, &vPos, nullptr, nullptr, &strSectionKey);
        CEffect2D_Manager::GetInstance()->Play_Effect(TEXT("LightningBolt"), strSectionKey, XMMatrixTranslation(500.f, 10.f, 0.0f), 0.f, 4);
        CEffect2D_Manager::GetInstance()->Play_Effect(TEXT("LightningBolt"), strSectionKey, XMMatrixTranslation(500.f, 10.f, 0.0f), 0.f, 5);

        CEffect2D_Manager::GetInstance()->Play_Effect(TEXT("LightningBolt"), strSectionKey, XMMatrixTranslation(-450.f, -30.f, 0.0f), 0.f, 4);
        CEffect2D_Manager::GetInstance()->Play_Effect(TEXT("LightningBolt"), strSectionKey, XMMatrixTranslation(-450.f, -30.f, 0.0f), 0.f, 5);
    }


    m_fTimer += _fTimeDelta;

    if (m_fMaxTimer < m_fTimer)
        GameEvent_End();
}

void CGameEventExecuter::C020910_Monster_Spawn(_float _fTimeDelta)
{
    _float3 vPos={ 500.0f, 10.f, 0.f };
    _wstring strSectionKey = TEXT("Chapter2_P0910");
    CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_Zippy"), COORDINATE_2D, &vPos, nullptr, nullptr, &strSectionKey);

    vPos={ -450.0f, -30.f, 0.f };
    CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_Zippy"), COORDINATE_2D, &vPos, nullptr, nullptr, &strSectionKey);

    GameEvent_End();
}

void CGameEventExecuter::Chapter2_BookMagic(_float _fTimeDelta)
{
    // 책 중앙으로 카메라 이동
    if (0 == m_iStep) {
        
        m_fTimer += _fTimeDelta;

        if (false == m_isStart) {
            CGameObject* pBook = m_pGameInstance->Get_GameObject_Ptr(m_pGameInstance->Get_CurLevelID(), TEXT("Layer_Book"), 0);
            CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET_2D)->Change_Target(pBook);
        
            m_isStart = true;
        }

        if (m_fTimer > 1.5f) {
            m_isStart = false;
            m_iStep++;
            m_fTimer = 0.f;
        }
    }
    // 하늘 어두워지기?
    // 
    // 책 발작, 이펙트 추가
    else if (1 == m_iStep) {
        m_fTimer += _fTimeDelta;

        if (false == m_isStart) {
            static_cast<CSampleBook*>(m_pGameInstance->Get_GameObject_Ptr(LEVEL_CHAPTER_2, TEXT("Layer_Book"), 0))->Execute_AnimEvent(5);
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

            m_isStart = true;
        }

        if (m_fTimer >= 5.8f && m_fTimer - _fTimeDelta <= 5.8f) {
            static_cast<CCamera_2D*>(CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET_2D))->Start_Shake_ByCount(0.2f, 0.1f, 10, CCamera::SHAKE_XY);
        }

        if (m_fTimer >= 6.5f) {
            m_isStart = false;
            m_iStep++;
            m_fTimer = 0.f;
        }
    }
    else if (2 == m_iStep) {
        if (false == m_isStart) {
            CCamera_2D* pCamera = static_cast<CCamera_2D*>(CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET_2D));
            pCamera->Set_NextArmData(TEXT("Book_Horizon"), 0);
            pCamera->Start_Changing_AtOffset(0.5f, XMVectorSet(0.f, 0.f, 0.f, 0.f), EASE_IN_OUT);
            pCamera->Set_CameraMode(CCamera_2D::MOVE_TO_NEXTARM);

            CGameObject* pPlayer = m_pGameInstance->Get_GameObject_Ptr(m_pGameInstance->Get_CurLevelID(), TEXT("Layer_Player"), 0);
            CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET_2D)->Change_Target(pPlayer);
            
            m_isStart = true;
        }

        if (CCamera_2D::DEFAULT == CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET_2D)->Get_CameraMode()) {
            //CCamera_2D* pCamera = static_cast<CCamera_2D*>(CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET_2D));
            //pCamera->Set_NextArmData(TEXT("Book_Horizon"), 0);
            m_isStart = false;
            m_iStep++;
            m_fTimer = 0.f;
        }
    }
    else if (3 == m_iStep) {
        m_fTimer += _fTimeDelta;

        if (m_fTimer >= 1.f) {
            // 돌 떨어지기?
           
        }
       if (m_fTimer >= 3.f) {
            m_isStart = false;
            m_iStep++;
            m_fTimer = 0.f;
        }
    }
    else if (4 == m_iStep) {
        m_fTimer += _fTimeDelta;

        // 책장 넘기기
        if (false == m_isStart) {
            _float3 vPos = { 0.f, 0.f, 1.f };

            Event_Book_Main_Section_Change_Start(1, &vPos);
            CSection* pSection = SECTION_MGR->Find_Section(L"Chapter1_P0708");
            if (nullptr != pSection)
                static_cast<CSection_2D*>(pSection)->Set_NextPageTag(L"Chapter2_P0102");
            
            m_isStart = true;
        }

        if (m_fTimer > 3.f) {
            m_isStart = false;
            m_iStep++;
            m_fTimer = 0.f;
        }
        

    }
    else if (5 == m_iStep) {
        m_fTimer += _fTimeDelta;

        if (false == m_isStart) {
            CUI_Manager::GetInstance()->Set_PlayNarration(TEXT("Chapter1_P1112_Narration_01"));

            m_isStart = true;
        }
    }
    else
        GameEvent_End();
}

void CGameEventExecuter::Chapter2_Intro(_float _fTimeDelta)
{
    if (0 == m_iStep) {

        m_fTimer += _fTimeDelta;

        if (false == m_isStart) {
            CCamera_2D* pCamera = static_cast<CCamera_2D*>(CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET_2D));
            pCamera->Start_PostProcessing_Fade(CCamera::FADE_IN, 2.f);
            pCamera->Set_Data(XMVectorSet(-0.670150876f, 0.506217539f, -0.542809010f, 0.f), 46.20f, XMVectorSet(-15.f, 5.f, 0.f, 0.f));
            pCamera->Set_NextArmData(TEXT("Intro"), 0);
            pCamera->Set_CameraMode(CCamera_2D::MOVE_TO_NEXTARM);

            m_isStart = true;
        }

        if (7.f >= m_fTimer) {
            m_isStart = false;
            m_iStep++;
            m_fTimer = 0.f;
        }
    }
    else if (1 == m_iStep) {
        CUI_Manager::GetInstance()->Set_PlayNarration(TEXT("Chapter1_P0102_Narration_01"));

        GameEvent_End();
    }
}


void CGameEventExecuter::GameEvent_End()
{
    if (!Is_Dead())
    {
        CTrigger_Manager::GetInstance()->On_End(m_strEventTag);
        Event_DeleteObject(this);
    }
}



CGameEventExecuter* CGameEventExecuter::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CGameEventExecuter* pInstance = new CGameEventExecuter(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CGameEventExecuter");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CGameEventExecuter::Clone(void* _pArg)
{
    CGameEventExecuter* pInstance = new CGameEventExecuter(*this);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CGameEventExecuter");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CGameEventExecuter::Free()
{
    __super::Free();
}


HRESULT CGameEventExecuter::Cleanup_DeadReferences()
{
    Safe_Release(m_pGameInstance);
	return S_OK;
}
