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

HRESULT CSection::Initialize(SECTION_DESC* pDesc)
{
    if (nullptr == pDesc)
        return E_FAIL;
    /* Section 이름 데이터 저장? */

    m_iLayerGroupCount = pDesc->iLayerGroupCount;
    m_Layers = new CLayer*[m_iLayerGroupCount];
    m_iGroupID = pDesc->iGroupID;
    m_iPriorityID = pDesc->iPriorityID;

    /* Create Layer */
    for (_uint i = 0; i < m_iLayerGroupCount; i++)
    {
        m_Layers[i] = CLayer::Create();
    if (nullptr == m_Layers[i])
        return E_FAIL;
    }

    Set_Active(false);

    return S_OK;
}

HRESULT CSection::Add_GameObject_ToSectionLayer(CGameObject* _pGameObject, _uint _iLayerIndex)
{
    if (nullptr == _pGameObject)
        return E_FAIL;


    if (!Has_Exist_Layer(_iLayerIndex))
        return E_FAIL;

     if (FAILED(m_Layers[_iLayerIndex]->Add_GameObject(_pGameObject)))
        return E_FAIL;

    _pGameObject->Enter_Section(m_strName);
    _pGameObject->Set_Active(Is_Active());
    Safe_AddRef(_pGameObject);
    return S_OK; 
}

HRESULT CSection::Remove_GameObject_ToSectionLayer(CGameObject* _pGameObject)
{
    if (nullptr == _pGameObject)
        return E_FAIL;

    auto pLayer = Get_Include_Layer(_pGameObject);
    if (nullptr != pLayer) 
    {
        Safe_Release(_pGameObject);
        _pGameObject->Exit_Section(m_strName);
        return pLayer->Remove_GameObject(_pGameObject);
    }
    else
        return E_FAIL;
}

HRESULT CSection::SetActive_GameObjects(_bool _isActive, _bool _isAllActive, _uint _iLayerIndex)
{
    if (!Has_Exist_Layer(_iLayerIndex))
        return E_FAIL;

    if (_isAllActive)
        for (_uint i = 0; i < m_iLayerGroupCount; i++)
            m_Layers[i]->SetActive_GameObjects(_isActive);
    else
        m_Layers[_iLayerIndex]->SetActive_GameObjects(_isActive);


    return S_OK;
}

HRESULT CSection::Add_RenderGroup_GameObjects()
{
    for (_uint i = 0; i < m_iLayerGroupCount; i++)
    {
        auto& Objects = m_Layers[i]->Get_GameObjects();
        for_each(Objects.begin(), Objects.end(), [&](CGameObject* pGameObject) {
            if(pGameObject->Is_Active() && pGameObject->Is_Render())
                pGameObject->Register_RenderGroup(m_iGroupID, m_iPriorityID);
            });
    }

    return S_OK;
}

HRESULT CSection::Cleanup_DeadReferences(_bool _isAllCheck, _uint _iLayerIndex)
{
    if (!Has_Exist_Layer(_iLayerIndex))
        return E_FAIL;


    if (_isAllCheck)
        for (_uint i = 0; i < m_iLayerGroupCount; i++)
            m_Layers[i]->Cleanup_DeadReferences();
    else
        m_Layers[_iLayerIndex]->Cleanup_DeadReferences();

    return S_OK;
}

void CSection::Clear_GameObjects(_bool _isAllClear, _uint _iLayerIndex)
{
    if (!_isAllClear && !Has_Exist_Layer(_iLayerIndex))
        return;

    /* 해당 함수는 Pooling 객체 여부 고려하지 않음. 무조건 삭제 */
    if (_isAllClear)
        for (_uint i = 0; i < m_iLayerGroupCount; i++)
        {
            auto GameObjects = m_Layers[i]->Get_GameObjects();

            for (auto& pGameObject : GameObjects)
                Safe_Release(pGameObject);
            m_Layers[i]->Clear_GameObjects();
        }
    else
    {
        auto GameObjects = m_Layers[_iLayerIndex]->Get_GameObjects();

        for (auto& pGameObject : GameObjects)
            Safe_Release(pGameObject);

        m_Layers[_iLayerIndex]->Clear_GameObjects();
    }

}

_bool CSection::Is_CurSection(CGameObject* _pGameObject)
{
    auto pLayer = Get_Include_Layer(_pGameObject);
    
    return pLayer != nullptr;
}

CLayer* CSection::Get_Include_Layer(CGameObject* _pGameObject)
{
    for (_uint i = 0; i < m_iLayerGroupCount; i++)
    {
        auto& Objects = m_Layers[i]->Get_GameObjects();
        auto iter = find_if(Objects.begin(), Objects.end(),[&_pGameObject](CGameObject* pGameObject) {
            return _pGameObject->Get_GameObjectInstanceID() == pGameObject->Get_GameObjectInstanceID();
            });

        if (iter != Objects.end())
            return m_Layers[i];
    }

    return nullptr;
}



void CSection::Active_OnEnable()
{
    SetActive_GameObjects(true);
}

void CSection::Active_OnDisable()
{
    SetActive_GameObjects(false);
}


void CSection::Free()
{
    for (_uint i = 0; i < m_iLayerGroupCount; i++)
        Safe_Release(m_Layers[i]);
    Safe_Delete_Array(m_Layers);

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);

    __super::Free();
}
