#include "Layer.h"
#include "ContainerObject.h"

CLayer::CLayer()
{
    
}

HRESULT CLayer::Add_GameObject(CGameObject* pGameObject)
{
    if (nullptr == pGameObject)
        return E_FAIL;

    m_GameObjects.push_back(pGameObject);

    return S_OK;
}

void CLayer::Priority_Update(_float fTimeDelta)
{
    for (auto& iter : m_GameObjects)
    {
        if(iter->Is_Active())
            iter->Priority_Update(fTimeDelta);
    }
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
            Safe_Release((*iter));
            iter = m_GameObjects.erase(iter);
            // 레퍼런스카운트? 어디서 AddRef를 해주지? 
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


CLayer* CLayer::Create()
{
    return new CLayer();
}

void CLayer::Free()
{
    __super::Free();

    for (auto& pGameObject : m_GameObjects)
    {
        Safe_Release(pGameObject);
    }
    m_GameObjects.clear();
}
