#include "Prototype_Manager.h"
#include "GameObject.h"
#include "Component.h"


CPrototype_Manager::CPrototype_Manager()
{
}

HRESULT CPrototype_Manager::Initialize(_uint _iNumLevels, _uint _iExcludeLevelID)
{
    if (nullptr != m_pPrototypes)
        return E_FAIL;

    // map<const _wstring, CBase*> == PROTOTYPE;
    m_pPrototypes = new PROTOTYPE[_iNumLevels];
    
    m_iNumLevels = _iNumLevels;
    m_iExcludeLevelID = _iExcludeLevelID;
    return S_OK;
}

HRESULT CPrototype_Manager::Add_Prototype(_uint _iLevelID, const _wstring& _strPrototypeTag, CBase* _pPrototype)
{
    // 레벨 인덱스가 맥스 인덱스보다 크거나, 이미 있는 Prototype인 경우, 오류로 본다.
    if (_iLevelID >= m_iNumLevels || nullptr != Find_Prototype(_iLevelID, _strPrototypeTag))
        return E_FAIL;

    m_pPrototypes[_iLevelID].emplace(_strPrototypeTag, _pPrototype);

    return S_OK;
}

CBase* CPrototype_Manager::Clone_Prototype(Engine::PROTOTYPE _eType, _uint _iLevelID, const _wstring& _strPrototypeTag, void* _pArg)
{
    CBase* pPrototype = Find_Prototype(_iLevelID, _strPrototypeTag);
    if (nullptr == pPrototype)
        return nullptr;

    CBase* pCopyObject = nullptr;
    switch (_eType)
    {
    case Engine::PROTOTYPE::PROTO_GAMEOBJ:
        pCopyObject = dynamic_cast<CGameObject*>(pPrototype)->Clone(_pArg);
        break;
    case Engine::PROTOTYPE::PROTO_COMPONENT:
        pCopyObject = dynamic_cast<CComponent*>(pPrototype)->Clone(_pArg);
        break;
    default:
        break;
    }

    if (nullptr == pCopyObject)
        return nullptr;

    return pCopyObject;
}

HRESULT CPrototype_Manager::Clear(_uint _iLevelID)
{
    if (_iLevelID >= m_iNumLevels)
        return E_FAIL;

    if (_iLevelID == m_iExcludeLevelID)
        return S_OK;

    for (auto& Pair : m_pPrototypes[_iLevelID])
    {
        Safe_Release(Pair.second);
    }

    m_pPrototypes[_iLevelID].clear();

    return S_OK;
}

HRESULT CPrototype_Manager::Level_Exit(_uint _iLevelID)
{
    if(FAILED(Clear(_iLevelID)))
        return E_FAIL;

    return S_OK;
}

CBase* CPrototype_Manager::Find_Prototype(_uint _iLevelID, const _wstring& _strPrototypeTag)
{
    // m_iNumLevels = enum의 마지막은 늘 END or LAST
    if (_iLevelID >= m_iNumLevels)
        return nullptr;

    auto iter = m_pPrototypes[_iLevelID].find(_strPrototypeTag);
    if (iter == m_pPrototypes[_iLevelID].end())
        return nullptr;

    return iter->second;
}

CPrototype_Manager* CPrototype_Manager::Create(_uint _iNumLevels, _uint _iExcludeLevelID)
{
    CPrototype_Manager* pInstance = new CPrototype_Manager();

    if (FAILED(pInstance->Initialize(_iNumLevels, _iExcludeLevelID)))
    {
        MSG_BOX("Failed to Created : CPrototype_Manager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPrototype_Manager::Free()
{
    for (_uint i = 0; i < m_iNumLevels; ++i)
    {
        for (auto& Pair : m_pPrototypes[i])
        {
            Safe_Release(Pair.second);

        }
        m_pPrototypes[i].clear();
    }
    // 맵 컨테이너 자체를 배열 형태로 동적할당 하였기 때문에.
    // 기존에는 정적배열(스택메모리)을 사용하고 그 안의 컨테이너를 넣는 방식.
    // 하지만 현재는 배열 자체를 힙 공간에 할당 하였다.

    Safe_Delete_Array(m_pPrototypes);

    __super::Free();
}
