#include "stdafx.h"
#include "FatherGame_Progress_Start.h"
#include "GameInstance.h"
#include "FatherGame.h"
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



#ifdef _DEBUG
    cout << "FatherGame Progress_Start Start" << endl;
#endif // _DEBUG
    return S_OK;
}

void CFatherGame_Progress_Start::Progress_Update()
{
    if (true == m_isClearProgress)
        return;

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
        CFatherGame::GetInstance()->OpenPortalLocker(CFatherGame::LOCKER_ZETPACK);
        /* 1. Clear 이벤트 호출 */
        Event_Register_Trigger(TEXT("Chapter6_FatherGame_Progress_Start_Clear"));
        /* 2. 자기 자신의 Active 상태를 False로 변경 */
        Event_SetActive(this, false);

        /* 3. ZetPack Progress 시작 */
        CFatherGame::GetInstance()->Start_Progress(CFatherGame::FATHER_PROGRESS_ZETPACK);
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
    for (auto& pMonster : m_pMonsters)
        Safe_Release(pMonster);
    m_pMonsters.clear();
    
    __super::Free();
}