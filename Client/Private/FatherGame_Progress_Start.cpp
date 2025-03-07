#include "stdafx.h"
#include "FatherGame_Progress_Start.h"
#include "GameInstance.h"
#include "Section_Manager.h"
#include "Section_2D_PlayMap.h"
#include "Goblin.h"
#include "PortalLocker.h"
#include "Portal_Default.h"

CFatherGame_Progress_Start::CFatherGame_Progress_Start(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CFatherGame_Progress(_pDevice, _pContext)
{
}

HRESULT CFatherGame_Progress_Start::Initialize(void* _pArg)
{
    FATHERGAME_PROGRESS_START_DESC* pDesc = static_cast<CFatherGame_Progress_Start::FATHERGAME_PROGRESS_START_DESC*>(_pArg);
    m_iNumMonsters = pDesc->iNumMonsters;
    m_strMonsterPrototypeTag = pDesc->strMonsterPrototypeTag;
    
    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CFatherGame_Progress_Start::Progress_Enter()
{
    if (true == m_isClearProgress)
    {
        MSG_BOX("이미 클리어 된 프로그레스입니다.");
        return E_FAIL;
    }

    /* 1. 몬스터 생성 */
    CGameObject* pGameObject = nullptr;

    CGoblin::MONSTER_DESC GoblinDesc;
    GoblinDesc.iCurLevelID = LEVEL_CHAPTER_6;
    GoblinDesc.eStartCoord = COORDINATE_3D;
    GoblinDesc.isCoordChangeEnable = false;

    for (_uint i = 0; i < m_iNumMonsters; ++i)
    {
        _float3 vRandomPos = { 0.0f, 0.4f, 0.0f };
        vRandomPos.x = m_pGameInstance->Compute_Random(-5.0f, 5.0f);
        vRandomPos.z = m_pGameInstance->Compute_Random(-5.0f, -7.0f);
        GoblinDesc.Build_3D_Transform(vRandomPos);

        if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, m_strMonsterPrototypeTag, LEVEL_CHAPTER_6, TEXT("Layer_Monster"), &pGameObject, &GoblinDesc)))
            return E_FAIL;

        m_pMonsters.push_back(static_cast<CMonster*>(pGameObject));
        Safe_AddRef(pGameObject);
    }

    /* 2. PortalDefender 3곳에 생성 */
    m_PortalLockers.resize((size_t)LOCKER_LAST);


    
    {/* PortalLocker ZetPack */
        CGameObject* pGameObject = nullptr;
        CPortalLocker::PORTALLOCKER_DESC PortalLockerDesc;
        CPortal* pTargetPortal = static_cast<CPortal_Default*>(static_cast<CSection_2D_PlayMap*>(CSection_Manager::GetInstance()->Find_Section(TEXT("Chapter6_SKSP_01")))->Get_Portal(0));
        if (nullptr == pTargetPortal)
            return E_FAIL;
        PortalLockerDesc.pTargetPortal = pTargetPortal;
        PortalLockerDesc.ePortalLockerType = CPortalLocker::TYPE_PURPLE;
        PortalLockerDesc.strSectionKey = TEXT("Chapter6_SKSP_01");

        if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_PortalLocker"), LEVEL_CHAPTER_6, TEXT("Layer_PortalLocker"), &pGameObject, &PortalLockerDesc)))
            return E_FAIL;

        m_PortalLockers[LOCKER_ZETPACK] = static_cast<CPortalLocker*>(pGameObject);
        Safe_AddRef(m_PortalLockers[LOCKER_ZETPACK]);
        if (FAILED(CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(PortalLockerDesc.strSectionKey, pGameObject, SECTION_2D_PLAYMAP_OBJECT)))
            return E_FAIL;
    }/* PortalLocker ZetPack */
    

    {/* PortalLocker PartHead */
        CGameObject* pGameObject = nullptr;
        CPortalLocker::PORTALLOCKER_DESC PortalLockerDesc;
        CPortal* pTargetPortal = static_cast<CPortal_Default*>(static_cast<CSection_2D_PlayMap*>(CSection_Manager::GetInstance()->Find_Section(TEXT("Chapter6_SKSP_04")))->Get_Portal(0));
        if (nullptr == pTargetPortal)
            return E_FAIL;
        PortalLockerDesc.pTargetPortal = pTargetPortal;
        PortalLockerDesc.ePortalLockerType = CPortalLocker::TYPE_YELLOW;
        PortalLockerDesc.strSectionKey = TEXT("Chapter6_SKSP_04");

        if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_PortalLocker"), LEVEL_CHAPTER_6, TEXT("Layer_PortalLocker"), &pGameObject, &PortalLockerDesc)))
            return E_FAIL;

        m_PortalLockers[LOCKER_PARTHEAD] = static_cast<CPortalLocker*>(pGameObject);
        Safe_AddRef(m_PortalLockers[LOCKER_PARTHEAD]);

        if (FAILED(CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(PortalLockerDesc.strSectionKey, pGameObject, SECTION_2D_PLAYMAP_OBJECT)))
            return E_FAIL;
    }/* PortalLocker PartHead */
    
    

    // 1. 모성 

#ifdef _DEBUG
    cout << "FatherGame Progress_Start Start" << endl;
#endif // _DEBUG
    return S_OK;
}

void CFatherGame_Progress_Start::Progress_Update()
{
    if (true == m_isClearProgress)
        return;

    if(KEY_DOWN(KEY::I))
    {
        m_PortalLockers[0]->Start_FadeAlphaOut();
    }
    if (KEY_DOWN(KEY::J))
    {
        m_PortalLockers[0]->Start_FadeAlphaIn();
    }
    /* 1. Monster의 Dead를 체크하고, Dead 상태라면, vector에서 제거 후 KillCount 증가. */
    for (_uint i = 0; i < m_pMonsters.size(); ++i)
    {
        if (nullptr == m_pMonsters[i])
            continue;

        if (true == m_pMonsters[i]->Is_Dead())
        {
            Safe_Release(m_pMonsters[i]);
            m_pMonsters[i] = nullptr;
            ++m_iKillCount;
        }
    }

    /* 2. KillCount 와 iNumMonsters와 같을 때, >>> Clear 로 본다. */
    Progress_Clear();

}

HRESULT CFatherGame_Progress_Start::Progress_Clear()
{
    if (m_iNumMonsters == m_iKillCount)
    {
        m_isClearProgress = true;

        /* 0. Locker On */
        m_PortalLockers[LOCKER_ZETPACK]->Open_Locker();
        /* 1. Clear 이벤트 호출 */

        /* 2. 자기 자신의 Active 상태를 False로 변경 */
        Event_SetActive(this, false);
#ifdef _DEBUG
        cout << "FatherGame Progress_Start Clear" << endl;
#endif // _DEBUG
    }

    return S_OK;
}

HRESULT CFatherGame_Progress_Start::Progress_Exit()
{
    if (false == m_isClearProgress)
        return S_OK;

    /* Clear 후 Exit 호출이 정상적인 로직으로 판단 함. */
    {

    }
#ifdef _DEBUG
    cout << "FatherGame Progress_Start Exit" << endl;
#endif // _DEBUG
    return S_OK;
}

CFatherGame_Progress_Start* CFatherGame_Progress_Start::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg)
{
    CFatherGame_Progress_Start* pInstance = new CFatherGame_Progress_Start(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed Create : CFatherGame_Progress_Start");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFatherGame_Progress_Start::Free()
{
    if (false == m_isClearProgress)
    {
        Safe_Release(m_PortalLockers[LOCKER_ZETPACK]);
        Safe_Release(m_PortalLockers[LOCKER_PARTHEAD]);
    }

    for (auto& pMonster : m_pMonsters)
        Safe_Release(pMonster);
    m_pMonsters.clear();
    
    __super::Free();
}