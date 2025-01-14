#include "Level_Manager.h"

#include "GameInstance.h"
#include "Level.h"
CLevel_Manager::CLevel_Manager()
    : m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
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

CLevel_Manager* CLevel_Manager::Create()
{
    return new CLevel_Manager();
}

void CLevel_Manager::Free()
{
    // 메모리나 해제 시 문제가 발생한다면 m_pGameInstance 객체를 부모에서 사용하고있진 않은지 체크해보자.
    // 일반적인 소멸자라고 가정한다면 아래와 같은 호출순서가 더 일반적이다.
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pCurLevel);
    __super::Free();
}
