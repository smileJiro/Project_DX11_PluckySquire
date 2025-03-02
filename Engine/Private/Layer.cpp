#include "Layer.h"
#include "ContainerObject.h"
#include "GameInstance.h"

CLayer::CLayer()
    :m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CLayer::Add_GameObject(CGameObject* _pGameObject)
{
    if (nullptr == _pGameObject)
        return E_FAIL;

    m_GameObjects.push_back(_pGameObject);

    return S_OK;
}

HRESULT CLayer::Remove_GameObject(CGameObject* _pGameObject)
{
    auto iter = find_if(m_GameObjects.begin(), m_GameObjects.end(), [&_pGameObject](CGameObject* pGameObject) {
        return _pGameObject->Get_GameObjectInstanceID() == pGameObject->Get_GameObjectInstanceID();
        });
    if (iter != m_GameObjects.end())
    {
        m_GameObjects.erase(iter);
        return S_OK;
    }
    else
        return E_FAIL;
}

void CLayer::Priority_Update(_float fTimeDelta)
{
    for (auto& iter : m_GameObjects)
    {
        if(iter->Is_Active())
            iter->Priority_Update(fTimeDelta);
    }

    //m_pGameInstance->Get_ThreadPool()->EnqueueJob([this]()
    //    {
    //        for (auto& iter : m_GameObjects)
    //        {
    //            if (iter->Is_Active())
    //                iter->Check_FrustumCulling();
    //        }
    //    });

}

void CLayer::Update(_float fTimeDelta)
{
    for (auto& iter : m_GameObjects)
    {
        if (iter->Is_Active())
            iter->Update(fTimeDelta);
    }
}

void CLayer::Late_Update(_float fTimeDelta)
{
    if (m_GameObjects.empty())
        return;


    for (auto iter = m_GameObjects.begin(); iter != m_GameObjects.end(); )
    {
        /* Late Update 까진 수행. Renderer에는 AddRef 해서 들어갔으니, 실제 객체가 삭제되는 것은 Renderer에서 삭제 될 것임. */
        if (true == (*iter)->Is_Active())
            (*iter)->Late_Update(fTimeDelta);
        else
        {
            /* 만약 Active가 False인 애들 중 Dead Object를 참조하고있었다면, 올바르게 참조 해제. */
            (*iter)->Cleanup_DeadReferences();
        }


        if ((*iter)->Is_Dead())
        {
            if (true == (*iter)->Is_Pooling())
            {
                /* 풀링등록된 오브젝트인 경우 */
                (*iter)->Set_Active(false); // 이벤트 처리 안해도 어짜피 LateUpdate까진 돌리니까. 렌더까지 될 것임.
                (*iter)->Set_Alive();
                ++iter;
            }
            else
            {
                /* 일반적인 오브젝트인 경우 */
                Safe_Release((*iter));
                iter = m_GameObjects.erase(iter);
            }
            
        }
        else
        {
            ++iter;
        }
        
    }

}

CComponent* CLayer::Find_Component(const _wstring& _strComponentTag, _uint _iObjectIndex)
{
    if (_iObjectIndex >= m_GameObjects.size())
        return nullptr;

    auto iter = m_GameObjects.begin();

    for (_uint i = 0; i < _iObjectIndex; ++i)
        ++iter;

    return (*iter)->Find_Component(_strComponentTag);
}

CComponent* CLayer::Find_Part_Component(_uint _iPartObjectIndex, const _wstring& _strPartComponentTag, _uint _iObjectIndex)
{
    if (_iObjectIndex >= m_GameObjects.size())
        return nullptr;

    auto iter = m_GameObjects.begin();

    for (_uint i = 0; i < _iObjectIndex; ++i)
        ++iter;

    return static_cast<CContainerObject*>(*iter)->Find_Part_Component(_iPartObjectIndex, _strPartComponentTag);
}

void CLayer::SetActive_GameObjects(_bool _isActive)
{
    if (true == m_GameObjects.empty())
        return;

    //auto iter = m_GameObjects.begin();

    //for (; iter != m_GameObjects.end(); )
    //{
    //    (*iter)->Set_Active(_isActive);
    //    ++iter;
    //}

    for (auto& pGameObject : m_GameObjects)
    {
        
        pGameObject->Set_Active(_isActive);
    }
}

void CLayer::Cleanup_DeadReferences()
{
    /* Pooling Object를 고려하지 않고 사망한 객체라면 죽인다. */
    /* 해당 함수는 Framework에서 사용하기 위함이 아닌, 외부에 공개되는 함수로 Layer로서의 역할만을 고려한다. */
    /* Pooling Object에 대한 처리는 Layer의 LateUpdate(); 와 같은 Framework적으로 처리되어야하는 */
    if (true == m_GameObjects.empty())
        return;

    auto& iter = m_GameObjects.begin();
    for (iter; iter != m_GameObjects.end(); )
    {
        if (true == (*iter)->Is_Dead())
        {
            Safe_Release((*iter));
            iter = m_GameObjects.erase(iter);
        }
        else
            ++iter;
    }
}

void CLayer::Clear_GameObjects()
 {
    for (auto& pGameObject : m_GameObjects)
        Safe_Release(pGameObject);

    m_GameObjects.clear();
}

CGameObject* CLayer::Get_GameObject_Ptr(_int _iObjectIndex)
{
    if (true == m_GameObjects.empty())
        return nullptr;

    auto iter = m_GameObjects.begin();
    _int iIndex = 0;

    for (; iter != m_GameObjects.end(); )
    {
        if (iIndex == _iObjectIndex)
        {
            break;
        }
        ++iIndex;
        ++iter;
    }

    return *iter;
}


CLayer* CLayer::Create()
{
    return new CLayer();
}

void CLayer::Free()
{
    Clear_GameObjects();

    Safe_Release(m_pGameInstance);
    __super::Free();
}
