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
#include "Spear_Soldier.h"
#include "Friend_Controller.h"
#include "Friend_Thrash.h"
#include "Spear_Soldier.h"
#include "Trigger_Manager.h"
#include "Dialog_Manager.h"
#include "PlayerData_Manager.h"
#include "Section_2D_PlayMap.h"
#include "Portal_Default.h"


IMPLEMENT_SINGLETON(CExcavatorGame)
CExcavatorGame::CExcavatorGame()
    :CBase()
    ,m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CExcavatorGame::Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    m_pDevice = _pDevice;
    Safe_AddRef(m_pDevice);
    m_pContext = _pContext;
    Safe_AddRef(m_pContext);

    if (FAILED(Ready_ExcavatorParts()))
        return E_FAIL;

    m_isPlaying = true;
    m_iCurProgress = PROGRESS_LAST;
    m_iSwitchCount = 0;

    m_eCurState = STATE_MOVE_R;
    State_Change();
    return S_OK;
}

HRESULT CExcavatorGame::Start_Game()
{
    m_iCurProgress = PROGRESS_1;
    Enter_Progress();

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
    case Client::CExcavatorGame::PROGRESS_5:
        isClearProgress = Update_Progress_5(_fTimeDelta);
        break;
    case Client::CExcavatorGame::PROGRESS_6:
        isClearProgress = Update_Progress_6(_fTimeDelta);
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
    if (m_iProgressLevel == 0)
    {
        CLayer* pLayer = m_pGameInstance->Find_Layer(LEVEL_CHAPTER_6, m_strMonsterLayerTag);
        assert(pLayer);
        list<CGameObject*> GameObjects = pLayer->Get_GameObjects();
        if (true == GameObjects.empty())
        {
            OpenDoors();
            m_pThrash->Move_Position(_float2(50.0f, -50.0f), CFriend::DIR_UP);
            ++m_iProgressLevel;
        }
    }
    else if (m_iProgressLevel == 1)
    {
        _int iNumSwitches = static_cast<CExcavator_Centre*>(m_ExcavatorParts[EXCAVATOR_CENTRE])->Get_NumActiveSwitches();
        if (iNumSwitches == 2)
        {
            m_pSaw->Set_CurState(CSaw::STATE_ATTACK);
            m_fDelayTime += _fTimeDelta;
            if (1.5f < m_fDelayTime)
            {
                _wstring strDialogueTag = TEXT("Chapter6_Boss_Progress1_End");
                CDialog_Manager::GetInstance()->Set_DialogId(strDialogueTag.c_str());
                CDialog_Manager::GetInstance()->Set_NPC(m_pThrash);
                

                m_fDelayTime = 0.f;
                ++m_iProgressLevel;
            }
        }
    }
    else if (m_iProgressLevel == 2)
    {
        if (false == CDialog_Manager::GetInstance()->Get_DisPlayDialogue())
        {
            m_iProgressLevel = 0;
            return true;
        }
    }
    return false;
}

_bool CExcavatorGame::Update_Progress_2(_float _fTimeDelta)
{
    if (m_iProgressLevel == 0)
    {
        CLayer* pLayer = m_pGameInstance->Find_Layer(LEVEL_CHAPTER_6, m_strMonsterLayerTag);
        assert(pLayer);
        list<CGameObject*> GameObjects = pLayer->Get_GameObjects();
        if (true == GameObjects.empty())
        {
            OpenDoors();
            m_pThrash->Move_Position(_float2(50.0f, -50.0f), CFriend::DIR_UP);
            ++m_iProgressLevel;
        }
    }
    else if (m_iProgressLevel == 1)
    {
        _int iSawAttackCount = m_pSaw->Get_AttackCount();
        if (iSawAttackCount == 1)
        {
            CPlayer* pPlayer = CPlayerData_Manager::GetInstance()->Get_NormalPlayer_Ptr();
            _vector vPlayerPos = { -50.0f, -50.0f, 0.0f, 1.0f };
            AUTOMOVE_COMMAND AutoMove{};
            AutoMove.eType = AUTOMOVE_TYPE::MOVE_TO;
            AutoMove.fPostDelayTime = 0.0f;
            AutoMove.fPreDelayTime = 0.0f;
            AutoMove.iAnimIndex = (_uint)CPlayer::ANIM_STATE_2D::PLAYER_RUN_UP;
            AutoMove.vTarget = vPlayerPos;
            AutoMove.fMoveSpeedMag = 2.0f;


            AUTOMOVE_COMMAND AutoMove2{};
            AutoMove2.eType = AUTOMOVE_TYPE::LOOK_DIRECTION;
            AutoMove2.fPostDelayTime = 0.0f;
            AutoMove2.fPreDelayTime = 0.0f;
            AutoMove2.vTarget = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
            AutoMove2.iAnimIndex = (_uint)CPlayer::ANIM_STATE_2D::PLAYER_IDLE_UP;
            pPlayer->Add_AutoMoveCommand(AutoMove);
            pPlayer->Add_AutoMoveCommand(AutoMove2);
            pPlayer->Start_AutoMove(true);
            ++m_iProgressLevel;
        }
    }
    else if (m_iProgressLevel == 2)
    {
        m_fDelayTime += _fTimeDelta;
        if (1.0f < m_fDelayTime)
        {
            _wstring strDialogueTag = TEXT("Chapter6_Boss_Progress2_End"); // 톱날이 너무 빨라!
            CDialog_Manager::GetInstance()->Set_DialogId(strDialogueTag.c_str());
            CDialog_Manager::GetInstance()->Set_NPC(m_pThrash);


            m_fDelayTime = 0.f;
            ++m_iProgressLevel;
        }
    }
    else if (m_iProgressLevel == 3)
    {
        if (false == CDialog_Manager::GetInstance()->Get_DisPlayDialogue())
        {
            m_iProgressLevel = 0;
            return true;
        }
    }
    return false;
}

_bool CExcavatorGame::Update_Progress_3(_float _fTimeDelta)
{
    if (m_iProgressLevel == 0)
    {
        CLayer* pLayer = m_pGameInstance->Find_Layer(LEVEL_CHAPTER_6, m_strMonsterLayerTag);
        assert(pLayer);
        list<CGameObject*> GameObjects = pLayer->Get_GameObjects();
        if (true == GameObjects.empty())
        {
            OpenDoors();
            m_pThrash->Move_Position(_float2(50.0f, -50.0f), CFriend::DIR_UP);
            ++m_iProgressLevel;
        }
    }
    else if (m_iProgressLevel == 1)
    {
        CPlayer* pPlayer = CPlayerData_Manager::GetInstance()->Get_NormalPlayer_Ptr();
        _vector vPlayerPos = { -50.0f, -50.0f, 0.0f, 1.0f };
        AUTOMOVE_COMMAND AutoMove{};
        AutoMove.eType = AUTOMOVE_TYPE::MOVE_TO;
        AutoMove.fPostDelayTime = 0.0f;
        AutoMove.fPreDelayTime = 0.0f;
        AutoMove.iAnimIndex = (_uint)CPlayer::ANIM_STATE_2D::PLAYER_RUN_UP;
        AutoMove.vTarget = vPlayerPos;
        AutoMove.fMoveSpeedMag = 2.0f;

        AUTOMOVE_COMMAND AutoMove2{};
        AutoMove2.eType = AUTOMOVE_TYPE::LOOK_DIRECTION;
        AutoMove2.fPostDelayTime = 0.0f;
        AutoMove2.fPreDelayTime = 0.0f;
        AutoMove2.vTarget = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        AutoMove2.iAnimIndex = (_uint)CPlayer::ANIM_STATE_2D::PLAYER_IDLE_UP;

        pPlayer->Add_AutoMoveCommand(AutoMove);
        pPlayer->Add_AutoMoveCommand(AutoMove2);
        pPlayer->Start_AutoMove(true);
        ++m_iProgressLevel;
    }
    else if (m_iProgressLevel == 2)
    {
        m_fDelayTime += _fTimeDelta;
        if (1.0f < m_fDelayTime)
        {
            m_pTurret_Left->Close_Turret();
            _wstring strDialogueTag = TEXT("Chapter6_Boss_Progress3_End"); // 멈춰도장을 사용하자!
            CDialog_Manager::GetInstance()->Set_DialogId(strDialogueTag.c_str());
            CDialog_Manager::GetInstance()->Set_NPC(m_pThrash);

            m_fDelayTime = 0.f;
            ++m_iProgressLevel;
        }
    }
    else if (m_iProgressLevel == 3)
    {
        if (false == CDialog_Manager::GetInstance()->Get_DisPlayDialogue())
        {
            m_iProgressLevel = 0;
            return true;
        }
    }
    return false;
}

_bool CExcavatorGame::Update_Progress_4(_float _fTimeDelta)
{
    if (m_iProgressLevel == 0)
    {
        if (true == m_pSaw->Is_Stoppable())
        {
            CExcavator_Switch* pSwitch = static_cast<CExcavator_Centre*>(m_ExcavatorParts[EXCAVATOR_CENTRE])->Get_ActiveSwitch();
            if (nullptr != pSwitch)
            {
                m_pThrash->Go_Switch(pSwitch);
                ++m_iProgressLevel;
            }
        }
    }
    else if (m_iProgressLevel == 1)
    {
        m_fDelayTime += _fTimeDelta;
        if (5.0f < m_fDelayTime)
        {
            _int iNumSwitches = static_cast<CExcavator_Centre*>(m_ExcavatorParts[EXCAVATOR_CENTRE])->Get_NumActiveSwitches();
            if (iNumSwitches == 1)
            {
                CPortal* pTargetPortal = static_cast<CPortal*>(static_cast<CSection_2D_PlayMap*>(CSection_Manager::GetInstance()->Find_Section(TEXT("Chapter6_P0708")))->Get_Portal(0));
                pTargetPortal->Set_FirstActive(true);
                m_pThrash->Move_Position(_float2(50.0f, -50.0f), CFriend::DIR_UP);

                m_fDelayTime = 0.f;

                m_iProgressLevel = 0;
                return true;
            }

        }
    }

    return false;
}

_bool CExcavatorGame::Update_Progress_5(_float _fTimeDelta)
{
    if (m_iProgressLevel == 0)
    {
        CLayer* pLayer = m_pGameInstance->Find_Layer(LEVEL_CHAPTER_6, m_strMonsterLayerTag);
        assert(pLayer);
        list<CGameObject*> GameObjects = pLayer->Get_GameObjects();
        if (true == GameObjects.empty())
        {
            OpenDoors();
            m_pThrash->Move_Position(_float2(50.0f, -50.0f), CFriend::DIR_UP);
            ++m_iProgressLevel;
        }
    }
    else if (m_iProgressLevel == 1)
    {
        if (true == m_pSaw->Is_Stoppable())
        {
            CExcavator_Switch* pSwitch = static_cast<CExcavator_Centre*>(m_ExcavatorParts[EXCAVATOR_CENTRE])->Get_ActiveSwitch();
            if (nullptr != pSwitch)
            {
                m_pThrash->Go_Switch(pSwitch);
                ++m_iProgressLevel;
            }
        }
    }
    else if (m_iProgressLevel == 2)
    {
        m_fDelayTime += _fTimeDelta;
        if (5.0f < m_fDelayTime)
        {
            _int iNumSwitches = static_cast<CExcavator_Centre*>(m_ExcavatorParts[EXCAVATOR_CENTRE])->Get_NumActiveSwitches();
            if (iNumSwitches == 0)
            {
                m_pThrash->Move_Position(_float2(50.0f, -50.0f), CFriend::DIR_UP);

                m_pTurret_Left->Close_Turret();
                _wstring strDialogueTag = TEXT("Chapter6_Boss_Progress5_End"); // 조절장치가 드러났어!
                CDialog_Manager::GetInstance()->Set_DialogId(strDialogueTag.c_str());
                CDialog_Manager::GetInstance()->Set_NPC(m_pThrash);
                m_pSaw->Set_CurState(CSaw::STATE_HIDE);
                ++m_iProgressLevel;
            }

        }
    }
    else if (m_iProgressLevel == 3)
    {
        if (false == CDialog_Manager::GetInstance()->Get_DisPlayDialogue())
        {
            m_iProgressLevel = 0;
            return true;
        }
    }
    return false;
}

_bool CExcavatorGame::Update_Progress_6(_float _fTimeDelta)
{
    if (m_iProgressLevel == 0)
    {
        CLayer* pLayer = m_pGameInstance->Find_Layer(LEVEL_CHAPTER_6, m_strMonsterLayerTag);
        assert(pLayer);
        list<CGameObject*> GameObjects = pLayer->Get_GameObjects();
        if (true == GameObjects.empty())
        {
            CPlayer* pPlayer = CPlayerData_Manager::GetInstance()->Get_NormalPlayer_Ptr();
            pPlayer->ErasePalm();
            m_pSaw->Set_CurState(CSaw::STATE_HIDE);
            m_pThrash->Move_Position(_float2(50.0f, -50.0f), CFriend::DIR_UP);
            ++m_iProgressLevel;
        }

    }
    else if (m_iProgressLevel == 1)
    {
        if (m_eCurState == STATE_DEAD)
        {
            m_pTurret_Left->Close_Turret();
            m_pTurret_Right->Close_Turret();

            _wstring strDialogueTag = TEXT("Chapter6_Boss_Progress6_End"); // 기차가 폭주한다!!
            CDialog_Manager::GetInstance()->Set_DialogId(strDialogueTag.c_str());
            CDialog_Manager::GetInstance()->Set_NPC(m_pThrash);
            ++m_iProgressLevel;
        }

    }
    else if (m_iProgressLevel == 2)
    {
        if (false == CDialog_Manager::GetInstance()->Get_DisPlayDialogue())
        {
            m_pThrash->Move_Position(_float2(150.f, 30.f), CFriend::DIR_RIGHT);

            CPlayer* pPlayer = CPlayerData_Manager::GetInstance()->Get_NormalPlayer_Ptr();
            _vector vPlayerPos = { 100.f, 50.0f, 0.0f, 1.0f };
            AUTOMOVE_COMMAND AutoMove{};
            AutoMove.eType = AUTOMOVE_TYPE::MOVE_TO;
            AutoMove.fPostDelayTime = 0.0f;
            AutoMove.fPreDelayTime = 0.0f;
            AutoMove.iAnimIndex = (_uint)CPlayer::ANIM_STATE_2D::PLAYER_RUN_RIGHT;
            AutoMove.vTarget = vPlayerPos;
            AutoMove.fMoveSpeedMag = 2.0f;

            AUTOMOVE_COMMAND AutoMove2{};
            AutoMove2.eType = AUTOMOVE_TYPE::LOOK_DIRECTION;
            AutoMove2.fPostDelayTime = 0.0f;
            AutoMove2.fPreDelayTime = 0.0f;
            AutoMove2.vTarget = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
            AutoMove2.iAnimIndex = (_uint)CPlayer::ANIM_STATE_2D::PLAYER_IDLE_RIGHT;

            pPlayer->Add_AutoMoveCommand(AutoMove);
            pPlayer->Add_AutoMoveCommand(AutoMove2);
            pPlayer->Start_AutoMove(true);

            m_fDelayTime = 0.0f;
            ++m_iProgressLevel;
        }

    }
    else if (m_iProgressLevel == 3)
    {
        m_fDelayTime += _fTimeDelta;
        if (4.0f < m_fDelayTime)
        {
            CPlayer* pPlayer = CPlayerData_Manager::GetInstance()->Get_NormalPlayer_Ptr();
            pPlayer->Set_State(CPlayer::IDLE);
            pPlayer->Set_BlockPlayerInput(false);
            _float3 fDefaultPos = {};
            Event_Book_Main_Section_Change_Start(1, &fDefaultPos);


            m_fDelayTime = 0.0f;
            ++m_iProgressLevel;
        }
    }
    else if (m_iProgressLevel == 4)
    {
        m_fDelayTime += _fTimeDelta;
        if (1.0f < m_fDelayTime)
        {
            CFriend* pViolet = CFriend_Controller::GetInstance()->Find_Friend(TEXT("Violet"));
            _wstring strCurSection = pViolet->Get_Include_Section_Name();
            CSection_Manager::GetInstance()->Remove_GameObject_FromSectionLayer(strCurSection, pViolet);
            CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(pViolet);
            pViolet->Set_CurState(CFriend::FRIEND_CHASE);

            m_pThrash->Change_Mode(CFriend::MODE_DEFAULT);
            m_pThrash->Set_CurState(CFriend::FRIEND_CHASE);
            CFriend_Controller::GetInstance()->Register_Friend_ToTrainList(TEXT("Violet"));
            CFriend_Controller::GetInstance()->Register_Friend_ToTrainList(TEXT("Thrash"));
            CFriend_Controller::GetInstance()->Start_Train();

            END_BGM();
            END_SFX(TEXT("LCD_MUS_C06_EXCAVATORBATTLE_Stem_Group1"));
            END_SFX(TEXT("LCD_MUS_C06_EXCAVATORBATTLE_Stem_Group2"));
            //m_iProgressLevel = 0;
            return true;
        }
    }

    return false;
}

void CExcavatorGame::Enter_Progress()
{
    /* 사용할 데이터 준비 작업 등. */
    switch (m_iCurProgress)
    {
    case Client::CExcavatorGame::PROGRESS_1:
    {
        /* 문 닫기 */
        CloseDoors();
        m_pThrash = dynamic_cast<CFriend_Thrash*>(CFriend_Controller::GetInstance()->Find_Friend(TEXT("Thrash")));
        assert(m_pThrash);
        Safe_AddRef(m_pThrash);
        /* 몬스터 2마리 생성 */
        m_strMonsterLayerTag = TEXT("Layer_Boss_Soldier");
        { /* Left >> Right */
            CGameObject* pGameObject;

            wstring strLayerTag = TEXT("Layer_Monster");

            CSpear_Soldier::SPEARSOLDIER_DESC Desc;
            Desc.iCurLevelID = LEVEL_CHAPTER_6;
            Desc.Build_2D_Transform(_float2(-1000.f, -80.f));
            Desc.eStartCoord = COORDINATE_2D;
            Desc.isC6BossMode = true;
            Desc.eDirection = F_DIRECTION::RIGHT;

            if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Spear_Soldier"), LEVEL_CHAPTER_6, m_strMonsterLayerTag, &pGameObject, &Desc)))
                return;

            CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter6_P0708"), pGameObject);
            static_cast<CMonster*>(pGameObject)->Set_Target(m_pThrash);
        } /* Left >> Right */

                /* 몬스터 2마리 생성 */
        { /* Left >> Right */
            CGameObject* pGameObject;

            wstring strLayerTag = TEXT("Layer_Monster");

            CSpear_Soldier::SPEARSOLDIER_DESC Desc;
            Desc.iCurLevelID = LEVEL_CHAPTER_6;
            Desc.Build_2D_Transform(_float2(1000.f, -150.f));
            Desc.eStartCoord = COORDINATE_2D;
            Desc.isC6BossMode = true;
            Desc.eDirection = F_DIRECTION::LEFT;

            if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Spear_Soldier"), LEVEL_CHAPTER_6, m_strMonsterLayerTag, &pGameObject, &Desc)))
                return;

            CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter6_P0708"), pGameObject);
            static_cast<CMonster*>(pGameObject)->Set_Target(m_pThrash);
        } /* Left >> Right */

        m_pThrash->Set_FightLayerTag(m_strMonsterLayerTag);
        m_pThrash->Change_Mode(CFriend::MODE_FIGHT);
    }
        break;
    case Client::CExcavatorGame::PROGRESS_2:
    {
        /* 문 닫기 */
        CloseDoors();

        /* 몬스터 2마리 생성 */
        m_strMonsterLayerTag = TEXT("Layer_Boss_Soldier");
        { /* Left >> Right */
            CGameObject* pGameObject;

            wstring strLayerTag = TEXT("Layer_Monster");

            CSpear_Soldier::SPEARSOLDIER_DESC Desc;
            Desc.iCurLevelID = LEVEL_CHAPTER_6;
            Desc.Build_2D_Transform(_float2(-1000.f, -80.f));
            Desc.eStartCoord = COORDINATE_2D;
            Desc.isC6BossMode = true;
            Desc.eDirection = F_DIRECTION::RIGHT;

            if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Spear_Soldier"), LEVEL_CHAPTER_6, m_strMonsterLayerTag, &pGameObject, &Desc)))
                return;

            CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter6_P0708"), pGameObject);
            static_cast<CMonster*>(pGameObject)->Set_Target(m_pThrash);
        } /* Left >> Right */

                /* 몬스터 2마리 생성 */
        { /* Left >> Right */
            CGameObject* pGameObject;

            wstring strLayerTag = TEXT("Layer_Monster");

            CSpear_Soldier::SPEARSOLDIER_DESC Desc;
            Desc.iCurLevelID = LEVEL_CHAPTER_6;
            Desc.Build_2D_Transform(_float2(1000.f, -150.f));
            Desc.eStartCoord = COORDINATE_2D;
            Desc.isC6BossMode = true;
            Desc.eDirection = F_DIRECTION::LEFT;

            if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Spear_Soldier"), LEVEL_CHAPTER_6, m_strMonsterLayerTag, &pGameObject, &Desc)))
                return;

            CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter6_P0708"), pGameObject);
            static_cast<CMonster*>(pGameObject)->Set_Target(m_pThrash);
        } /* Left >> Right */

        m_pThrash->Change_Mode(CFriend::MODE_FIGHT);
    }
        break;
    case Client::CExcavatorGame::PROGRESS_3:
    {
        /* 문 닫기 */
        CloseDoors();

        /* 몬스터 2마리 생성 */
        m_strMonsterLayerTag = TEXT("Layer_Boss_Soldier");
        { /* Left >> Right */
            CGameObject* pGameObject;

            wstring strLayerTag = TEXT("Layer_Monster");

            CSpear_Soldier::SPEARSOLDIER_DESC Desc;
            Desc.iCurLevelID = LEVEL_CHAPTER_6;
            Desc.Build_2D_Transform(_float2(-1000.f, -80.f));
            Desc.eStartCoord = COORDINATE_2D;
            Desc.isC6BossMode = true;
            Desc.eDirection = F_DIRECTION::RIGHT;

            if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Spear_Soldier"), LEVEL_CHAPTER_6, m_strMonsterLayerTag, &pGameObject, &Desc)))
                return;

            CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter6_P0708"), pGameObject);
            static_cast<CMonster*>(pGameObject)->Set_Target(m_pThrash);
        } /* Left >> Right */

                /* 몬스터 2마리 생성 */
        { /* Left >> Right */
            CGameObject* pGameObject;

            wstring strLayerTag = TEXT("Layer_Monster");

            CSpear_Soldier::SPEARSOLDIER_DESC Desc;
            Desc.iCurLevelID = LEVEL_CHAPTER_6;
            Desc.Build_2D_Transform(_float2(1000.f, -150.f));
            Desc.eStartCoord = COORDINATE_2D;
            Desc.isC6BossMode = true;
            Desc.eDirection = F_DIRECTION::LEFT;

            if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Spear_Soldier"), LEVEL_CHAPTER_6, m_strMonsterLayerTag, &pGameObject, &Desc)))
                return;

            CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter6_P0708"), pGameObject);
            static_cast<CMonster*>(pGameObject)->Set_Target(m_pThrash);
        } /* Left >> Right */
        m_pThrash->Change_Mode(CFriend::MODE_FIGHT);

        // 좌측 터렛 가동
        m_pTurret_Left->Open_Turret();
    }
        break;
    case Client::CExcavatorGame::PROGRESS_4:
    {
        CPortal* pTargetPortal = static_cast<CPortal*>(static_cast<CSection_2D_PlayMap*>(CSection_Manager::GetInstance()->Find_Section(TEXT("Chapter6_P0708")))->Get_Portal(1));
        pTargetPortal->Set_FirstActive(true);
        pTargetPortal = static_cast<CPortal*>(static_cast<CSection_2D_PlayMap*>(CSection_Manager::GetInstance()->Find_Section(TEXT("Chapter6_P0708")))->Get_Portal(2));
        pTargetPortal->Set_FirstActive(true);
        
        
        m_pThrash->Change_Mode(CFriend::MODE_FIGHT);
    }
        break;
    case Client::CExcavatorGame::PROGRESS_5:
    {
        /* 문 닫기 */
        CloseDoors();

        /* 몬스터 2마리 생성 */
        m_strMonsterLayerTag = TEXT("Layer_Boss_Soldier");
        { /* Left >> Right */
            CGameObject* pGameObject;

            wstring strLayerTag = TEXT("Layer_Monster");

            CSpear_Soldier::SPEARSOLDIER_DESC Desc;
            Desc.iCurLevelID = LEVEL_CHAPTER_6;
            Desc.Build_2D_Transform(_float2(-1000.f, -80.f));
            Desc.eStartCoord = COORDINATE_2D;
            Desc.isC6BossMode = true;
            Desc.eDirection = F_DIRECTION::RIGHT;

            if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Spear_Soldier"), LEVEL_CHAPTER_6, m_strMonsterLayerTag, &pGameObject, &Desc)))
                return;

            CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter6_P0708"), pGameObject);
            static_cast<CMonster*>(pGameObject)->Set_Target(m_pThrash);
        } /* Left >> Right */

                /* 몬스터 2마리 생성 */
        { /* Left >> Right */
            CGameObject* pGameObject;

            wstring strLayerTag = TEXT("Layer_Monster");

            CSpear_Soldier::SPEARSOLDIER_DESC Desc;
            Desc.iCurLevelID = LEVEL_CHAPTER_6;
            Desc.Build_2D_Transform(_float2(1000.f, -150.f));
            Desc.eStartCoord = COORDINATE_2D;
            Desc.isC6BossMode = true;
            Desc.eDirection = F_DIRECTION::LEFT;

            if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Spear_Soldier"), LEVEL_CHAPTER_6, m_strMonsterLayerTag, &pGameObject, &Desc)))
                return;

            CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter6_P0708"), pGameObject);
            static_cast<CMonster*>(pGameObject)->Set_Target(m_pThrash);
        } /* Left >> Right */

        CPlayer* pPlayer = CPlayerData_Manager::GetInstance()->Get_NormalPlayer_Ptr();
        pPlayer->ErasePalm();
        m_pTurret_Right->Open_Turret();

        m_pThrash->Change_Mode(CFriend::MODE_FIGHT);
    }
    break;
    case Client::CExcavatorGame::PROGRESS_6:
    {

        /* 몬스터 3마리 생성 */
        m_strMonsterLayerTag = TEXT("Layer_Boss_Soldier");
        { /* Left >> Right */
            CGameObject* pGameObject;

            wstring strLayerTag = TEXT("Layer_Monster");

            CSpear_Soldier::SPEARSOLDIER_DESC Desc;
            Desc.iCurLevelID = LEVEL_CHAPTER_6;
            Desc.Build_2D_Transform(_float2(-1000.f, -80.f));
            Desc.eStartCoord = COORDINATE_2D;
            Desc.isC6BossMode = true;
            Desc.eDirection = F_DIRECTION::RIGHT;

            if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Spear_Soldier"), LEVEL_CHAPTER_6, m_strMonsterLayerTag, &pGameObject, &Desc)))
                return;

            CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter6_P0708"), pGameObject);
            static_cast<CMonster*>(pGameObject)->Set_Target(m_pThrash);
            
        } /* Left >> Right */

        { /* Left >> Right */
            CGameObject* pGameObject;

            wstring strLayerTag = TEXT("Layer_Monster");

            CSpear_Soldier::SPEARSOLDIER_DESC Desc;
            Desc.iCurLevelID = LEVEL_CHAPTER_6;
            Desc.Build_2D_Transform(_float2(1000.f, -150.f));
            Desc.eStartCoord = COORDINATE_2D;
            Desc.isC6BossMode = true;
            Desc.eDirection = F_DIRECTION::LEFT;

            if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Spear_Soldier"), LEVEL_CHAPTER_6, m_strMonsterLayerTag, &pGameObject, &Desc)))
                return;

            CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter6_P0708"), pGameObject);
            static_cast<CMonster*>(pGameObject)->Set_Target(m_pThrash);
            
        } /* Left >> Right */

        CPlayer* pPlayer = CPlayerData_Manager::GetInstance()->Get_NormalPlayer_Ptr();
        pPlayer->ErasePalm();
        m_pTurret_Right->Open_Turret();
        m_pTurret_Left->Open_Turret();

        m_pThrash->Change_Mode(CFriend::MODE_FIGHT);
    }
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
    case Client::CExcavatorGame::PROGRESS_5:
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

void CExcavatorGame::OpenDoors()
{
    for (auto& pDoor : m_Doors)
    {
        pDoor->Set_OpeningDoor();
        CCamera_Manager::GetInstance()->Start_Shake_ByCount(CCamera_Manager::TARGET_2D, 0.5f, 0.2f, 100, CCamera::SHAKE_Y, 0.0f);
    }
}

void CExcavatorGame::CloseDoors()
{
    for (auto& pDoor : m_Doors)
    {
        pDoor->Set_ClosingDoor();
        CCamera_Manager::GetInstance()->Start_Shake_ByCount(CCamera_Manager::TARGET_2D, 0.5f, 0.2f, 100, CCamera::SHAKE_Y, 0.0f);
    }
}

_int CExcavatorGame::Minus_HP()
{
    if (STATE_DEAD == m_eCurState)
        return 0;
   
    if (0 == m_iHP)
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
        m_pSaw = static_cast<CSaw*>(pGameObject);
        Safe_AddRef(m_pSaw);

    }/* Part CENTRE_SAW */

    
    {/* Door Red*/
        CDoor_Red* pOutRed;
        CDoor_Red::DOOR_RED_DESC DoorRedDesc = {};
        DoorRedDesc.tTransform2DDesc.vInitialPosition = _float3(-450.f, 60.f, 0.f);
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
        
        Safe_AddRef(pOutRed);
        m_Doors.push_back(pOutRed);

        DoorRedDesc.tTransform2DDesc.vInitialPosition = _float3(-155.f, 60.f, 0.f);

        if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_DoorRed"),
            LEVEL_CHAPTER_6, L"Layer_MapGimmick", reinterpret_cast<CGameObject**>(&pOutRed), &DoorRedDesc)))
            return E_FAIL;

        Safe_AddRef(pOutRed);
        m_Doors.push_back(pOutRed);

        DoorRedDesc.tTransform2DDesc.vInitialPosition = _float3(140.f, 60.f, 0.f);

        if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_DoorRed"),
            LEVEL_CHAPTER_6, L"Layer_MapGimmick", reinterpret_cast<CGameObject**>(&pOutRed), &DoorRedDesc)))
            return E_FAIL;

        Safe_AddRef(pOutRed);
        m_Doors.push_back(pOutRed);

        DoorRedDesc.tTransform2DDesc.vInitialPosition = _float3(435.f, 60.f, 0.f);

        if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_DoorRed"),
            LEVEL_CHAPTER_6, L"Layer_MapGimmick", reinterpret_cast<CGameObject**>(&pOutRed), &DoorRedDesc)))
            return E_FAIL;

        Safe_AddRef(pOutRed);
        m_Doors.push_back(pOutRed);
    }/* Door Red*/
    

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
    START_SFX_DELAY(TEXT("A_sfx_excavator_start"), 0.1f, g_SFXVolume, false);
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
    CCamera_Manager::GetInstance()->Start_Shake_ByCount(CCamera_Manager::TARGET_2D, 8.f, 0.8f, 2000, CCamera::SHAKE_Y, 0.0f);

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

            _float fRand = m_pGameInstance->GetInstance()->Compute_Random(0.0f, 4.f);
            _int iRand2 = round(fRand);
            if(iRand2 == 0)
                START_SFX_DELAY(TEXT("A_sfx_tank_fire_0"), 0.0f, g_SFXVolume * 1.0f, false);
            else if (iRand2 == 1)
                START_SFX_DELAY(TEXT("A_sfx_tank_fire_1"), 0.0f, g_SFXVolume * 1.0f, false);
            else if (iRand2 == 2)
                START_SFX_DELAY(TEXT("A_sfx_tank_projectile_explosion-0"), 0.1f, g_SFXVolume * 1.0f, false);
            else if (iRand2 == 3)
                START_SFX_DELAY(TEXT("A_sfx_tank_projectile_explosion-3"), 0.0f, g_SFXVolume * 1.0f, false);
            else if (iRand2 == 4)
                START_SFX_DELAY(TEXT("A_sfx_tank_projectile_explosion-2"), 0.1f, g_SFXVolume * 1.0f, false);

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

    Safe_Release(m_pSaw);
    Safe_Release(m_pThrash);
    Safe_Release(m_pTurret_Left);
    Safe_Release(m_pTurret_Right);

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);
    __super::Free();
}
