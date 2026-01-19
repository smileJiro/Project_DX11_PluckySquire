#include "Level_Manager.h"

#include "GameInstance.h"
#include "Level.h"
CLevel_Manager::CLevel_Manager()
    : m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CLevel_Manager::Initialize(_uint _iNumLevels, const char* const* ppLevelNames)
{
    m_LevelNames.resize(_iNumLevels);

    if (_iNumLevels == 0) return E_FAIL;

    m_LevelNames.clear();
    m_LevelNames.resize(_iNumLevels);

    m_isLoadable.clear();
    m_isLoadable.resize(_iNumLevels, false);


    for (_uint i = 0; i < _iNumLevels; ++i)
    {
        const char* pName = (ppLevelNames ? ppLevelNames[i] : nullptr);

        if (pName && pName[0] != '\0')
        {
            m_LevelNames[i] = pName;     // 소유 복사 (OK)
            m_isLoadable[i] = true;      // 로드 가능
        }
        else
        {
            // 비실존 레벨 "Loading", "Static" 등
            m_LevelNames[i] = "";
            m_isLoadable[i] = false;
        }
    }

    return S_OK;
}

HRESULT CLevel_Manager::Level_Enter(_int _iLevelID, CLevel* _pNewLevel)
{
    if(nullptr != m_pCurLevel)
        Safe_Release(m_pCurLevel);

    m_pCurLevel = _pNewLevel;
    m_iCurLevelID = _iLevelID;
    return S_OK;
}

HRESULT CLevel_Manager::Level_Exit()
{
    return S_OK;
}

void CLevel_Manager::Update(_float _fTimeDelta)
{
    if (m_pCurLevel)
    {
       m_pCurLevel->Update(_fTimeDelta);
    }
   
}

HRESULT CLevel_Manager::Render()
{
    if (m_pCurLevel)
    {
        m_pCurLevel->Render();
    }
        

    return S_OK;
}

CLevel_Manager* CLevel_Manager::Create(_uint _iNumLevels, const char* const* ppLevelNames)
{
    CLevel_Manager* pInstance = new CLevel_Manager();

    if (FAILED(pInstance->Initialize(_iNumLevels, ppLevelNames)))
    {
        MSG_BOX("Failed to Created : CLevel_Manager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_Manager::Free()
{
    // 메모리나 해제 시 문제가 발생한다면 m_pGameInstance 객체를 부모에서 사용하고있진 않은지 체크해보자.
    // 일반적인 소멸자라고 가정한다면 아래와 같은 호출순서가 더 일반적이다.
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pCurLevel);
    __super::Free();
}
