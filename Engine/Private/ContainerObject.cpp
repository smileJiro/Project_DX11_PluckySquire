#include "ContainerObject.h"

CContainerObject::CContainerObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CGameObject(_pDevice, _pContext)
    , m_PartObjects{}
{
}

CContainerObject::CContainerObject(const CContainerObject& _Prototype)
    : CGameObject(_Prototype)
    , m_PartObjects{ _Prototype.m_PartObjects }
{
    for (CGameObject* pPartObject : m_PartObjects)
        Safe_AddRef(pPartObject);
}

HRESULT CContainerObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CContainerObject::Initialize(void* _pArg)
{
    CONTAINEROBJ_DESC* pDesc = static_cast<CONTAINEROBJ_DESC*>(_pArg);

    if(0 != pDesc->iNumPartObjects)
        m_PartObjects.resize(pDesc->iNumPartObjects); // 리사이즈 해두고 인덱스 접근한다 , 다만 nullptr 체크해야겠지

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    return S_OK;
}

void CContainerObject::Priority_Update(_float _fTimeDelta)
{
    for (auto& pPartObj : m_PartObjects)
    {
        if (nullptr != pPartObj && true == pPartObj->Is_Active())
            pPartObj->Priority_Update(_fTimeDelta);
    }
}

void CContainerObject::Update(_float _fTimeDelta)
{
    for (auto& pPartObj : m_PartObjects)
    {
        if (nullptr != pPartObj && true == pPartObj->Is_Active())
            pPartObj->Update(_fTimeDelta);
    }
}

void CContainerObject::Late_Update(_float _fTimeDelta)
{
    for (auto& pPartObj : m_PartObjects)
    {
        if (nullptr != pPartObj && true == pPartObj->Is_Active())
            pPartObj->Late_Update(_fTimeDelta);
    }
}

HRESULT CContainerObject::Render()
{
    return S_OK;
}

CComponent* CContainerObject::Find_Part_Component(_uint _iPartObjectIndex, const _wstring& _strPartComponentTag)
{
    if (_iPartObjectIndex >= m_PartObjects.size())
        return nullptr;


    return m_PartObjects[_iPartObjectIndex]->Find_Component(_strPartComponentTag);
}

HRESULT CContainerObject::Add_PartObject(CGameObject* _pPartObject)
{
    /* 이 객체를 구성하기위한 부품(몸, 헤드, 무기, 이펙트, 사운드) 객체들 */
    if (nullptr == _pPartObject)
        return E_FAIL;

    m_PartObjects.push_back(_pPartObject);

    return S_OK;
}

CGameObject* CContainerObject::Get_PartObject(_int _iPartObjectIndex)
{
    if (_iPartObjectIndex >= m_PartObjects.size())
        return nullptr;

    return m_PartObjects[_iPartObjectIndex];
}

void CContainerObject::Free()
{
    for (CGameObject* pPartObject : m_PartObjects)
        Safe_Release(pPartObject);
    m_PartObjects.clear();

    __super::Free();
}

HRESULT CContainerObject::Safe_Release_DeadObjects()
{
    for (auto& pPartObject : m_PartObjects)
    {
        if (nullptr == pPartObject)
            continue;
        if (pPartObject->Is_Dead())
        {
            Safe_Release(pPartObject);
        }
    }


    return S_OK;
}
