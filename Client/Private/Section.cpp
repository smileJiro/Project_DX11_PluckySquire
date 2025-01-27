#include "stdafx.h"
#include "Section.h"
#include "GameInstance.h"
#include "Layer.h"

CSection::CSection(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : m_pDevice(_pDevice)
    , m_pContext(_pContext)
    , m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
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

    /* 해당 함수는 Pooling 객체 여부 고려하지 않음. 무조건 삭제 */
    m_pLayer->Clear_GameObjects();
}

void CSection::Active_OnEnable()
{
    SetActive_GameObjects(true);
}

void CSection::Active_OnDisable()
{
    SetActive_GameObjects(false);
}

CSection* CSection::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CSection* pInstance = new CSection(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed Create CSection Class");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CSection::Free()
{
    Safe_Release(m_pLayer);

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);

    __super::Free();
}
