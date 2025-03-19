#include "stdafx.h"
#include "ExcavatorGame.h"
#include "GameInstance.h"
#include "Section_Manager.h"
#include "Excavator_Tread.h"
#include "Excavator_Centre.h"
#include "Saw.h"
#include "Camera_Manager.h"
#include "Effect2D_Manager.h"
#include "Turret.h"

#include "Door_Red.h"


IMPLEMENT_SINGLETON(CExcavatorGame)
CExcavatorGame::CExcavatorGame()
    :CBase()
    ,m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CExcavatorGame::Start_Game(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    m_pDevice = _pDevice;
    Safe_AddRef(m_pDevice);
    m_pContext = _pContext;
    Safe_AddRef(m_pContext);

    if (FAILED(Ready_ExcavatorParts()))
        return E_FAIL;



    m_isPlaying = true;
    m_iCurProgress = PROGRESS_1;
    m_iSwitchCount = 0;

    m_eCurState = STATE_MOVE_R;
    State_Change();
    return S_OK;
}

void CExcavatorGame::Update(_float _fTimeDelta)
{
    if (false == m_isPlaying)
        return;

    if (true == m_isGameEnd)
        return;

    Action_State(_fTimeDelta);

    if (true == Update_Progress(_fTimeDelta))
    {
        Exit_Progress();
        ++m_iCurProgress; // last 되면 업데이트도 x 
        if (PROGRESS_LAST <= m_iCurProgress)
        {
            m_isGameEnd = true;
            Game_End();
            return;
        }
        Enter_Progress();
    }

    State_Change();
}

_bool CExcavatorGame::Update_Progress(_float _fTimeDelta)
{
    _bool isClearProgress = false;
    switch (m_iCurProgress)
    {
    case Client::CExcavatorGame::PROGRESS_1:
        isClearProgress = Update_Progress_1(_fTimeDelta);
        break;
    case Client::CExcavatorGame::PROGRESS_2:
        isClearProgress =Update_Progress_2(_fTimeDelta);
        break;
    case Client::CExcavatorGame::PROGRESS_3:
        isClearProgress = Update_Progress_3(_fTimeDelta);
        break;
    case Client::CExcavatorGame::PROGRESS_4:
        isClearProgress = Update_Progress_4(_fTimeDelta);
        break;
    case Client::CExcavatorGame::PROGRESS_LAST:
        isClearProgress = false;
        break;
    default:
        break;
    }
    return isClearProgress;
}

_bool CExcavatorGame::Update_Progress_1(_float _fTimeDelta)
{
    return false;
}

_bool CExcavatorGame::Update_Progress_2(_float _fTimeDelta)
{
    return false;
}

_bool CExcavatorGame::Update_Progress_3(_float _fTimeDelta)
{
    return false;
}

_bool CExcavatorGame::Update_Progress_4(_float _fTimeDelta)
{

    return false;
}

void CExcavatorGame::Enter_Progress()
{
    /* 사용할 데이터 준비 작업 등. */
    switch (m_iCurProgress)
    {
    case Client::CExcavatorGame::PROGRESS_1:
        break;
    case Client::CExcavatorGame::PROGRESS_2:
        break;
    case Client::CExcavatorGame::PROGRESS_3:
        break;
    case Client::CExcavatorGame::PROGRESS_4:
        break;
    default:
        break;
    }
}

void CExcavatorGame::Exit_Progress()
{
    /* 사용했던 데이터 정리 */
    switch (m_iCurProgress)
    {
    case Client::CExcavatorGame::PROGRESS_1:
        break;
    case Client::CExcavatorGame::PROGRESS_2:
        break;
    case Client::CExcavatorGame::PROGRESS_3:
        break;
    case Client::CExcavatorGame::PROGRESS_4:
        break;
    default:
        break;
    }
}

void CExcavatorGame::Game_End()
{
    /* 멤버 정리. */
    for (auto& pExcavatorPart : m_ExcavatorParts)
    {
        Event_SetActive(pExcavatorPart, false);
    }

    m_isGameEnd = true;
}

_int CExcavatorGame::Minus_HP()
{
    if (STATE_DEAD == m_eCurState)
        return 0;
   
    if (0 >= m_iHP)
    {
        m_eCurState = STATE_DEAD;
        State_Change();
    }
    else
    {
        m_iHP -= 1;
        static_cast<CExcavator_Tread*>(m_ExcavatorParts[EXCAVATOR_PART::EXCAVATOR_TREAD_L])->Start_Part_HitRender();
        static_cast<CExcavator_Tread*>(m_ExcavatorParts[EXCAVATOR_PART::EXCAVATOR_TREAD_R])->Start_Part_HitRender();
    }

    return m_iHP;
}

HRESULT CExcavatorGame::Ready_ExcavatorParts()
{
    m_ExcavatorParts.resize((size_t)EXCAVATOR_PART::EXCAVATOR_LAST);
    {
        CExcavator_Centre::CENTRE_DESC Desc;
        Desc.iCurLevelID = LEVEL_CHAPTER_6;
        Desc.Build_2D_Transform(_float2(-1000.0f, 360.0f));
        Desc.tTransform2DDesc.fSpeedPerSec = 500.f;
        CGameObject* pGameObject = nullptr;
        if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHAPTER_6, TEXT("Prototype_GameObject_Excavator_Centre"), LEVEL_CHAPTER_6, TEXT("Layer_Excavator"), &pGameObject, &Desc)))
            return E_FAIL;
        CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter6_P0708"), pGameObject);
        m_ExcavatorParts[EXCAVATOR_PART::EXCAVATOR_CENTRE] = static_cast<CCharacter*>(pGameObject);
        Safe_AddRef(m_ExcavatorParts[EXCAVATOR_PART::EXCAVATOR_CENTRE]);
    }

    {
        CExcavator_Tread::TREAD_DESC Desc;
        Desc.iCurLevelID = LEVEL_CHAPTER_6;
        Desc.Build_2D_Transform(_float2(-1800.0f, 360.0f));
        Desc.tTransform2DDesc.fSpeedPerSec = 500.f;
        CGameObject* pGameObject = nullptr;
        if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHAPTER_6, TEXT("Prototype_GameObject_Excavator_Tread"), LEVEL_CHAPTER_6, TEXT("Layer_Excavator"), &pGameObject, &Desc)))
            return E_FAIL;
        CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter6_P0708"), pGameObject);
        m_ExcavatorParts[EXCAVATOR_PART::EXCAVATOR_TREAD_L] = static_cast<CCharacter*>(pGameObject);
        Safe_AddRef(m_ExcavatorParts[EXCAVATOR_PART::EXCAVATOR_CENTRE]);


        Desc.Build_2D_Transform(_float2(-200.0f, 360.0f));
        Desc.tTransform2DDesc.fSpeedPerSec = 500.f;
        pGameObject = nullptr;
        if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHAPTER_6, TEXT("Prototype_GameObject_Excavator_Tread"), LEVEL_CHAPTER_6, TEXT("Layer_Excavator"), &pGameObject, &Desc)))
            return E_FAIL;
        CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter6_P0708"), pGameObject);
        m_ExcavatorParts[EXCAVATOR_PART::EXCAVATOR_TREAD_R] = static_cast<CCharacter*>(pGameObject);
        Safe_AddRef(m_ExcavatorParts[EXCAVATOR_PART::EXCAVATOR_CENTRE]);
    }

    {/* Part CENTRE_SAW */
        CSaw::SAW_DESC Desc{};
        Desc.iCurLevelID = LEVEL_CHAPTER_6;
        Desc.Build_2D_Transform(_float2(0.0f, 360.0f), _float2(1.0f, 1.0f));
        CGameObject* pGameObject = nullptr;
        if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHAPTER_6, TEXT("Prototype_GameObject_Saw"), LEVEL_CHAPTER_6, TEXT("Layer_Excavator"), &pGameObject, &Desc)))
            return E_FAIL;
        CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter6_P0708"), pGameObject, SECTION_2D_PLAYMAP_EFFECT);
        m_ExcavatorParts[EXCAVATOR_PART::EXCAVATOR_SAW] = static_cast<CCharacter*>(pGameObject);
        Safe_AddRef(m_ExcavatorParts[EXCAVATOR_PART::EXCAVATOR_SAW]);

    }/* Part CENTRE_SAW */

    /* Door Red*/
    {
        CDoor_Red* pOutRed;
        CDoor_Red::DOOR_RED_DESC DoorRedDesc = {};
        DoorRedDesc.tTransform2DDesc.vInitialPosition = _float3(-450.f, 65.f, 0.f);
        DoorRedDesc.iCurLevelID = LEVEL_CHAPTER_6;
        DoorRedDesc.isHorizontal = true;
        DoorRedDesc.eSize = CDoor_2D::LARGE;
        DoorRedDesc.eInitialState = CDoor_2D::OPENED;
        DoorRedDesc.strSectionTag = L"Chapter6_P0708";
        DoorRedDesc.strLayerTag = L"";
        DoorRedDesc.isCountLayer = false;

        if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_DoorRed"),
            LEVEL_CHAPTER_6, L"Layer_MapGimmick", reinterpret_cast<CGameObject**>(&pOutRed), &DoorRedDesc)))
            return E_FAIL;
        
        if (nullptr != pOutRed)
        {
            Safe_AddRef(pOutRed);
            m_Doors.push_back(pOutRed);
        }

        DoorRedDesc.tTransform2DDesc.vInitialPosition = _float3(-155.f, 65.f, 0.f);

        if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_DoorRed"),
            LEVEL_CHAPTER_6, L"Layer_MapGimmick", reinterpret_cast<CGameObject**>(&pOutRed), &DoorRedDesc)))
            return E_FAIL;

        if (nullptr != pOutRed)
        {
            Safe_AddRef(pOutRed);
            m_Doors.push_back(pOutRed);
        }

        DoorRedDesc.tTransform2DDesc.vInitialPosition = _float3(140.f, 65.f, 0.f);

        if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_DoorRed"),
            LEVEL_CHAPTER_6, L"Layer_MapGimmick", reinterpret_cast<CGameObject**>(&pOutRed), &DoorRedDesc)))
            return E_FAIL;

        if (nullptr != pOutRed)
        {
            Safe_AddRef(pOutRed);
            m_Doors.push_back(pOutRed);
        }

        DoorRedDesc.tTransform2DDesc.vInitialPosition = _float3(435.f, 65.f, 0.f);

        if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_DoorRed"),
            LEVEL_CHAPTER_6, L"Layer_MapGimmick", reinterpret_cast<CGameObject**>(&pOutRed), &DoorRedDesc)))
            return E_FAIL;

        if (nullptr != pOutRed)
        {
            Safe_AddRef(pOutRed);
            m_Doors.push_back(pOutRed);
        }
    }
    

    {/* Turret Left */
        CTurret::MODELOBJECT_DESC Desc{};
        Desc.iCurLevelID = LEVEL_CHAPTER_6;
        Desc.Build_2D_Transform(_float2(-780.f, -50.0f), _float2(1.0f, 1.0f));
        CGameObject* pGameObject = nullptr;
        if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHAPTER_6, TEXT("Prototype_GameObject_Turret"), LEVEL_CHAPTER_6, TEXT("Layer_Excavator"), &pGameObject, &Desc)))
            return E_FAIL;

        CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter6_P0708"), pGameObject);
        m_pTurret_Left = static_cast<CTurret*>(pGameObject);
        Safe_AddRef(m_pTurret_Left);
    }/* Turret Left */
    
    {/* Turret Right */
        CTurret::MODELOBJECT_DESC Desc{};
        Desc.iCurLevelID = LEVEL_CHAPTER_6;
        Desc.Build_2D_Transform(_float2(780.f, -50.0f), _float2(1.0f, 1.0f));
        CGameObject* pGameObject = nullptr;
        if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHAPTER_6, TEXT("Prototype_GameObject_Turret"), LEVEL_CHAPTER_6, TEXT("Layer_Excavator"), &pGameObject, &Desc)))
            return E_FAIL;

        CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter6_P0708"), pGameObject);
        m_pTurret_Right = static_cast<CTurret*>(pGameObject);
        Safe_AddRef(m_pTurret_Right);
        m_pTurret_Right->Open_Turret();
    }/* Turret Right */



    return S_OK;
}

void CExcavatorGame::State_Change()
{
    if (m_ePreState == m_eCurState)
        return;

    switch (m_eCurState)
    {
    case Client::CExcavatorGame::STATE_MOVE_R:
        State_Change_Move_R();
        break;
    case Client::CExcavatorGame::STATE_IDLE:
        State_Change_Idle();
        break;
    case Client::CExcavatorGame::STATE_SAW:
        State_Change_Saw();
        break;
    case Client::CExcavatorGame::STATE_HIT:
        State_Change_Hit();
        break;
    case Client::CExcavatorGame::STATE_DEAD:
        State_Change_Dead();
        break;
    default:
        break;
    }

    m_ePreState = m_eCurState;
}

void CExcavatorGame::State_Change_Move_R()
{
    /* Parts 우측 이동 */
    m_vMoveTime.y = 0.0f;
    static_cast<CExcavator_Tread*>(m_ExcavatorParts[EXCAVATOR_PART::EXCAVATOR_TREAD_L])->Start_Wheel();
    static_cast<CExcavator_Tread*>(m_ExcavatorParts[EXCAVATOR_PART::EXCAVATOR_TREAD_R])->Start_Wheel();
}

void CExcavatorGame::State_Change_Idle()
{
    /* 셰이크 되며 제자리에 가만히 */

    static_cast<CExcavator_Tread*>(m_ExcavatorParts[EXCAVATOR_PART::EXCAVATOR_TREAD_L])->Stop_Wheel();
    static_cast<CExcavator_Tread*>(m_ExcavatorParts[EXCAVATOR_PART::EXCAVATOR_TREAD_R])->Stop_Wheel();
}

void CExcavatorGame::State_Change_Saw()
{
    /* Idle + Saw */
}

void CExcavatorGame::State_Change_Hit()
{
    /* 몸 하얗게 */
}

void CExcavatorGame::State_Change_Dead()
{
    /* 폭파 이펙트 왕창 재생 하며 우측으로 이동 .*/
    static_cast<CExcavator_Tread*>(m_ExcavatorParts[EXCAVATOR_PART::EXCAVATOR_TREAD_L])->Start_Wheel();
    static_cast<CExcavator_Tread*>(m_ExcavatorParts[EXCAVATOR_PART::EXCAVATOR_TREAD_R])->Start_Wheel();

    CCamera_Manager::GetInstance()->Off_Shake();
    CCamera_Manager::GetInstance()->Start_Shake_ByCount(CCamera_Manager::TARGET_2D, 4.f, 0.7f, 2000, CCamera::SHAKE_Y, 0.0f);

    m_vMoveTime.x = 7.f;
    m_vMoveTime.y = 0.f;
}

void CExcavatorGame::Action_State(_float _fTimeDelta)
{
    switch (m_eCurState)
    {
    case Client::CExcavatorGame::STATE_MOVE_R:
        Action_State_Move_R(_fTimeDelta);
        break;
    case Client::CExcavatorGame::STATE_IDLE:
        Action_State_Idle(_fTimeDelta);
        break;
    case Client::CExcavatorGame::STATE_SAW:
        Action_State_Saw(_fTimeDelta);
        break;
    case Client::CExcavatorGame::STATE_HIT:
        Action_State_Hit(_fTimeDelta);
        break;
    case Client::CExcavatorGame::STATE_DEAD:
        Action_State_Dead(_fTimeDelta);
        break;
    default:
        break;
    }
}

void CExcavatorGame::Action_State_Move_R(_float _fTimeDelta)
{
    m_vMoveTime.y += _fTimeDelta;
    if (m_vMoveTime.x <= m_vMoveTime.y)
    {
        m_vMoveTime.y = 0.0f;
        m_eCurState = STATE_IDLE;
    }
    else
    {
        for (auto& pPart : m_ExcavatorParts)
        {
            pPart->Move(XMLoadFloat2(&m_vMoveSpeed), _fTimeDelta);
        }
    }
}

void CExcavatorGame::Action_State_Idle(_float _fTimeDelta)
{
}

void CExcavatorGame::Action_State_Saw(_float _fTimeDelta)
{
}

void CExcavatorGame::Action_State_Hit(_float _fTimeDelta)
{
}

void CExcavatorGame::Action_State_Dead(_float _fTimeDelta)
{
    m_vMoveTime.y += _fTimeDelta;
    if (m_vMoveTime.x <= m_vMoveTime.y)
    {
        m_vMoveTime.y = 0.0f;


        Game_End();
       
    }
    else
    {
        _int iRand = rand();
        if (iRand % 3 == 0)
        {
            static_cast<CExcavator_Centre*>(m_ExcavatorParts[EXCAVATOR_PART::EXCAVATOR_CENTRE])->Render_DeadEffect();
        }
        for (auto& pPart : m_ExcavatorParts)
        {
            pPart->Move(XMLoadFloat2(&m_vMoveSpeed), _fTimeDelta);

            static_cast<CExcavator_Tread*>(m_ExcavatorParts[EXCAVATOR_PART::EXCAVATOR_TREAD_L])->Start_Part_HitRender();
            static_cast<CExcavator_Tread*>(m_ExcavatorParts[EXCAVATOR_PART::EXCAVATOR_TREAD_R])->Start_Part_HitRender();
            static_cast<CExcavator_Centre*>(m_ExcavatorParts[EXCAVATOR_PART::EXCAVATOR_CENTRE])->Start_Part_HitRender();
        }
    }
}

void CExcavatorGame::Free()
{
    for (auto& pPart : m_ExcavatorParts)
    {
        Safe_Release(pPart);
    }
    m_ExcavatorParts.clear();

    for (auto& pDoor : m_Doors)
    {
        Safe_Release(pDoor);
    }
    m_Doors.clear();

    Safe_Release(m_pTurret_Left);
    Safe_Release(m_pTurret_Right);

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);
    __super::Free();
}
