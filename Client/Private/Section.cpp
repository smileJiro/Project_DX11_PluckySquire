#include "stdafx.h"
#include "Section.h"
#include "GameInstance.h"
#include "Layer.h"

CSection::CSection()
    : m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CSection::Initialize()
{
    /* Section 이름 데이터 저장? */

    /* Create Layer */
    m_pLayer =  CLayer::Create();
    if (nullptr == m_pLayer)
        return E_FAIL;

    return S_OK;
}

HRESULT CSection::Add_GameObject_ToSectionLayer(CGameObject* _pGameObject)
{
    if (nullptr == m_pLayer)
        return E_FAIL;

    if (FAILED(m_pLayer->Add_GameObject(_pGameObject)))
        return E_FAIL;

    return S_OK;
}

HRESULT CSection::SetActive_GameObjects(_bool _isActive)
{
    if (nullptr == m_pLayer)
        return E_FAIL;

    m_pLayer->SetActive_GameObjects(_isActive);

    return S_OK;
}

HRESULT CSection::Add_RenderGroup_GameObjects()
{
    if (nullptr == m_pLayer)
        return E_FAIL;

    m_pLayer->Add_RenderGroup_GameObjects();

    return S_OK;
}

HRESULT CSection::Cleanup_DeadReferences()
{
    if (nullptr == m_pLayer)
        return E_FAIL;

    m_pLayer->Cleanup_DeadReferences();

    return S_OK;
}

void CSection::Clear_GameObjects()
{
    if (nullptr == m_pLayer)
        return;

    m_pLayer->Clear_GameObjects();
}

CSection* CSection::Create()
{
    CSection* pInstance = new CSection();
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed Create CSection Class");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CSection::Free()
{
    Safe_Release(m_pGameInstance);


    __super::Free();
}
