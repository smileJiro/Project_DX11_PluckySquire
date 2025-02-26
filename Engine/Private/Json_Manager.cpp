#include "Json_Manager.h"

CJson_Manager::CJson_Manager()
{
}

HRESULT CJson_Manager::Initialize(_uint _iNumLevels)
{
    m_iNumLevels = _iNumLevels;

    m_pJsons = new JSONS[m_iNumLevels];

    return S_OK;
}

HRESULT CJson_Manager::Load_Json(const _tchar* _szFilePath, _Out_ json* _pOutJson)
{
    if (nullptr == _pOutJson)
    {
        MSG_BOX("Json 객체가 존재하지 않습니다.");
        return E_FAIL;
    }

    std::ifstream inputFile(_szFilePath);
    if (!inputFile.is_open())
    {
        MSG_BOX("파일을 읽을 수가 없습니다.");
        inputFile.close();
        return E_FAIL;
    }
    
    inputFile >> *_pOutJson;
    inputFile.close();

    return S_OK;
}

HRESULT CJson_Manager::Load_Json_InLevel(const _tchar* _szFilePath, const _wstring& _strKey, _uint _iLevelIndex)
{
    if (_iLevelIndex >= m_iNumLevels)
    {
        return E_FAIL;
    }

    std::ifstream inputFile(_szFilePath);
    if (!inputFile.is_open())
    {
        MSG_BOX("파일을 읽을 수가 없습니다.");
        inputFile.close();
        return E_FAIL;
    }

    json* pJsonFile = new json;
    inputFile >> *pJsonFile;
    inputFile.close();
    
    m_pJsons[_iLevelIndex].emplace(_strKey, pJsonFile);

    return S_OK;
}

const json* CJson_Manager::Find_Json_InLevel(const _wstring& _strKey, _uint _iLevelIndex)
{
    if (_iLevelIndex >= m_iNumLevels)
    {
        return nullptr;
    }

    auto iter = m_pJsons[_iLevelIndex].find(_strKey);

    if (iter == m_pJsons[_iLevelIndex].end())
        return nullptr;

    return iter->second;
}


void CJson_Manager::Level_Exit(_uint _iLevelIndex)
{
    // 일단 하드 코딩.
    if (_iLevelIndex >= m_iNumLevels || 1 == _iLevelIndex)
    {
        return;
    }
    
    for (auto& Pair : m_pJsons[_iLevelIndex])
    {
        Safe_Delete(Pair.second);
    }
    m_pJsons[_iLevelIndex].clear();

}



CJson_Manager* CJson_Manager::Create(_uint _iNumLevels)
{
    CJson_Manager* pInstance = new CJson_Manager();

    if (FAILED(pInstance->Initialize(_iNumLevels)))
    {  
        MSG_BOX("Failed to Create JsonManager");
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}

void CJson_Manager::Free()
{
    for (_uint i = 0; i < m_iNumLevels; ++i)
    {
        for (auto& Pair : m_pJsons[i])
        {
            Safe_Delete(Pair.second);
        }
        m_pJsons[i].clear();
    }

    Safe_Delete_Array(m_pJsons);

    __super::Free();
}
