#include "stdafx.h"
#include "FatherGame_Progress_Start.h"
#include "GameInstance.h"
#include "Goblin.h"

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
    GoblinDesc.Build_3D_Transform(_float3(-5.0f, 1.0f, 0.0f));
    GoblinDesc.eStartCoord = COORDINATE_3D;
    

    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Goblin"), LEVEL_CHAPTER_6, TEXT("Layer_Monster"),&pGameObject, &GoblinDesc)))
        return E_FAIL;
    m_pMonsters.push_back(static_cast<CMonster*>(pGameObject));
    /* 2. PortalDefender 4곳에 생성 */

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

        /* 1. Clear 이벤트 호출 */

        /* 2. 자기 자신의 Active 상태를 False로 변경 */
        Event_SetActive(this, false);
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