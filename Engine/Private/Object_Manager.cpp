#include "Object_Manager.h"

#include "GameInstance.h"
#include "ModelObject.h"
#include "Layer.h"

CObject_Manager::CObject_Manager()
    : m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CObject_Manager::Initialize(_uint _iNumLevels)
{
    if (nullptr != m_pLayers)
        return E_FAIL;

    // 힙 메모리에 정적 배열 생성.
    m_pLayers = new LAYERS[_iNumLevels];

    m_iNumLevels = _iNumLevels;

    return S_OK;
}

void CObject_Manager::Priority_Update(_float _fTimeDelta)
{
    //m_pGameInstance->Get_ThreadPool()->EnqueueJob([this]()
    //    {
    //        for (_uint i = 0; i < m_iNumLevels; ++i)
    //        {
    //            for (auto& Pair : m_pLayers[i])
    //            {
    //                Pair.second->Check_FrustumCulling();
    //            }
    //        }
    //    });

    for (_uint i = 0; i < m_iNumLevels; ++i)
    {
        for (auto& Pair : m_pLayers[i])
        {
            Pair.second->Priority_Update(_fTimeDelta);
        }
    }

}

void CObject_Manager::Update(_float _fTimeDelta)
{
    for (_uint i = 0; i < m_iNumLevels; ++i)
    {
        for (auto& Pair : m_pLayers[i])
        {
            Pair.second->Update(_fTimeDelta);            
        }
    }
}

void CObject_Manager::Late_Update(_float _fTimeDelta)
{
    for (_uint i = 0; i < m_iNumLevels; ++i)
    {
        for (auto& Pair : m_pLayers[i])
        {
            Pair.second->Late_Update(_fTimeDelta);
        }
    }
}

HRESULT CObject_Manager::Level_Exit(_uint _iLevelID)
{
    if (FAILED(Clear(_iLevelID)))
        return E_FAIL;

    return S_OK;
}

_bool CObject_Manager::Is_EmptyLayer(_uint _iLevelID, const _wstring& _strLayerTag)
{
    if (_iLevelID >= m_iNumLevels)
        return true;

    CLayer* pLayer = Find_Layer(_iLevelID, _strLayerTag);

    if (nullptr == pLayer)
        return true;

    else
        return pLayer->Is_Empty();

}

CLayer* CObject_Manager::Find_Layer(_uint _iLevelID, const _wstring& _strLayerTag)
{
    if (_iLevelID >= m_iNumLevels)
        return nullptr;

    auto iter = m_pLayers[_iLevelID].find(_strLayerTag);
    if (iter == m_pLayers[_iLevelID].end())
        return nullptr;

    return iter->second;
}

CComponent* CObject_Manager::Find_Component(_uint _iLevelID, const _wstring& _strLayerTag, const _wstring& _strComponentTag, _uint _iObjectIndex)
{
    CLayer* pLayer = Find_Layer(_iLevelID, _strLayerTag);
    if (nullptr == pLayer)
        return nullptr;

    return pLayer->Find_Component(_strComponentTag, _iObjectIndex);
}

CComponent* CObject_Manager::Find_Part_Component(_uint _iLevelID, const _wstring& _strLayerTag, _uint _iPartObjectIndex, const _wstring& _strPartComponentTag, _uint _iObjectIndex)
{
    /* 컨테이너 오브젝트에 해당하는 객체가 포함되어있는 레이어를 먼저 찾는다. */
    CLayer* pLayer = Find_Layer(_iLevelID, _strLayerTag);
    if (nullptr == pLayer)
        return nullptr;

    return pLayer->Find_Part_Component(_iPartObjectIndex, _strPartComponentTag, _iObjectIndex);
}

HRESULT CObject_Manager::Clear(_uint _iLevelID)
{
    if (_iLevelID >= m_iNumLevels)
        return E_FAIL;

    for (auto& Pair : m_pLayers[_iLevelID])
    {
        Safe_Release(Pair.second);
    }

    m_pLayers[_iLevelID].clear();

    return S_OK;
}

HRESULT CObject_Manager::Add_GameObject_ToLayer(_uint _iPrototypeLevelID, const _wstring& _strPrototypeTag, _uint _iLevelID, const _wstring& _strLayerTag, void* pArg)
{
    // 프로토타입 매니저에 접근하여 객체를 복사 받아온다.
    CGameObject* pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(Engine::PROTOTYPE::PROTO_GAMEOBJ, _iPrototypeLevelID, _strPrototypeTag, pArg));
    if (nullptr == pGameObject)
        return E_FAIL;

    // 객체가 정상적으로 Clone 되었다면, 
    CLayer* pLayer = Find_Layer(_iLevelID, _strLayerTag);
    if (nullptr == pLayer)
    {
        // Layer가 존재하지 않는다면,
        pLayer = CLayer::Create();
        pLayer->Add_GameObject(pGameObject);
        m_pLayers[_iLevelID].emplace(_strLayerTag, pLayer);
    }
    else
    {
        // Layer가 존재한다면, Add_GameObject();
        pLayer->Add_GameObject(pGameObject);
    }

    return S_OK;
}

HRESULT CObject_Manager::Add_GameObject_ToLayer(_uint _iPrototypeLevelID, const _wstring& _strPrototypeTag, _uint _iLevelID, const _wstring& _strLayerTag, CGameObject** _ppOut, void* pArg)
{
    // 프로토타입 매니저에 접근하여 객체를 복사 받아온다.
    CGameObject* pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(Engine::PROTOTYPE::PROTO_GAMEOBJ, _iPrototypeLevelID, _strPrototypeTag, pArg));
    if (nullptr == pGameObject)
        return E_FAIL;
    
    // 객체가 정상적으로 Clone 되었다면, 
    CLayer* pLayer = Find_Layer(_iLevelID, _strLayerTag);
    if (nullptr == pLayer)
    {
        // Layer가 존재하지 않는다면,
        pLayer = CLayer::Create();
        pLayer->Add_GameObject(pGameObject);
        m_pLayers[_iLevelID].emplace(_strLayerTag, pLayer);
    }
    else
    {
        // Layer가 존재한다면, Add_GameObject();
        pLayer->Add_GameObject(pGameObject);
    }

    *_ppOut = pGameObject;

    return S_OK;
}

HRESULT CObject_Manager::Add_GameObject_ToLayer(_uint _iLevelID, const _wstring& _strLayerTag, CGameObject* _pGameObject)
{
    if (nullptr == _pGameObject)
        return E_FAIL;

    CLayer* pLayer = Find_Layer(_iLevelID, _strLayerTag);
    if (nullptr == pLayer)
    {
        // Layer가 존재하지 않는다면,
        pLayer = CLayer::Create();
        pLayer->Add_GameObject(_pGameObject);
        m_pLayers[_iLevelID].emplace(_strLayerTag, pLayer);
    }
    else
    {
        // Layer가 존재한다면, Add_GameObject();
        pLayer->Add_GameObject(_pGameObject);
    }

    return S_OK;
}

void CObject_Manager::Set_Layer_Culling(_uint _iLevelID, const _wstring& _strLayerTag, _bool _isCulling)
{
    CLayer* pLayer = Find_Layer(_iLevelID, _strLayerTag);
    if (nullptr == pLayer)
        return;

    pLayer->Set_LayerCulling(_isCulling);
}

CGameObject* CObject_Manager::Get_PickingModelObjectByCursor(_uint _iLevelID, const _wstring& _strLayerTag, _float2 _fCursorPos)
{
    if (_iLevelID > m_iNumLevels)
        assert(nullptr);

    CLayer* pLayer = Find_Layer(_iLevelID, _strLayerTag);
    if (nullptr == pLayer)
        return nullptr;

    const list<CGameObject*>& GameObjectsList = pLayer->Get_GameObjects();
    CGameObject* pPickingObject = nullptr;
    for (auto& pGameObject : GameObjectsList)
    {
        CModelObject* pModelObject = dynamic_cast<CModelObject*>(pGameObject);
        _float fMinDst = 9999.f;
        if (nullptr == pModelObject)
            continue;
        else
        {
            // MeshObject 인 경우,
            _bool bResult = false;
            _float fDst = 10000.0f;
            bResult = pModelObject->Is_PickingCursor_Model_Test(_fCursorPos, fDst);
            if (bResult)
            {
                if (fMinDst > fDst)
                {
                    // Dst가 더 짧은 Object가 등장한 경우.
                    fMinDst = fDst;
                    pPickingObject = pModelObject;
                }
            }
        }
    }


    return pPickingObject;



    //return nullptr;
}

CGameObject* CObject_Manager::Find_NearestObject_Scaled(_uint _iLevelID, const _wstring& _strLayerTag, CController_Transform* const _pConTransform, CGameObject* pExceptionObject)
{
    ///* 매개변수로 들어온 Transform 과 선택한 레이어의 오브젝트들간의 거리를 비교하여 레이어에 있는 오브젝트중 가장 가까운 오브젝트를 반환. */
    //CLayer* pLayer =  Find_Layer(_iLevelID, _strLayerTag);
    //if (nullptr == pLayer)
    //    return nullptr;

    //const list<CGameObject*>& GameObjectsList = pLayer->Get_GameObjects();
    //CGameObject* pNearestObject = nullptr;
    //_float fMinDst = 9999.f;
    //_float fDst = 10000.0f;
    //for (auto& pGameObject : GameObjectsList)
    //{
    //    fDst = pGameObject->Get_ControllerTransform()->Compute_Distance(_pConTransform->Get_State(CTransform::STATE_POSITION));
    //    if (fMinDst > fDst)
    //    {
    //        // Dst가 더 짧은 Object가 등장한 경우.
    //        if (nullptr != pExceptionObject)
    //        {
    //            // 만약 매개변수로 들어온  현재 타겟오브젝트와 같은 녀석이라면, 컨티뉴
    //            if (pGameObject == pExceptionObject)
    //                continue;
    //        }

    //        fMinDst = fDst;
    //        pNearestObject = pGameObject;

    //    }
    //}
    //return pNearestObject;



    return nullptr;
}

CGameObject* CObject_Manager::Get_GameObject_Ptr(_int _iLevelID, const _wstring& _strLayerTag, _int _iObjectIndex)
{
    CLayer* pLayer = Find_Layer(_iLevelID, _strLayerTag);
    if (nullptr == pLayer)
        return nullptr;

    return pLayer->Get_GameObject_Ptr(_iObjectIndex);
}

CObject_Manager* CObject_Manager::Create(_uint iNumLevels)
{
    CObject_Manager* pInstance = new CObject_Manager();

    if (FAILED(pInstance->Initialize(iNumLevels)))
    {
        MSG_BOX("Failed to Created : CObject_Manager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CObject_Manager::Free()
{
    // 모든 Layer 삭제.

    for (_uint i = 0; i < m_iNumLevels; ++i)
    {
        for (auto& Pair : m_pLayers[i])
        {
            Safe_Release(Pair.second);
        }
        m_pLayers[i].clear();
    }
    Safe_Delete_Array(m_pLayers);

    Safe_Release(m_pGameInstance);

    __super::Free();

}
