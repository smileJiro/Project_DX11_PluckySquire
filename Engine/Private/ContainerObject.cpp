#include "ContainerObject.h"
#include "GameInstance.h"

CContainerObject::CContainerObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CActorObject(_pDevice, _pContext)
    , m_PartObjects{}
{
}

CContainerObject::CContainerObject(const CContainerObject& _Prototype)
    : CActorObject(_Prototype)
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
    __super::Priority_Update(_fTimeDelta);

    for (auto& pPartObj : m_PartObjects)
    {
        if (nullptr != pPartObj && true == pPartObj->Is_Active())
            pPartObj->Priority_Update(_fTimeDelta);
    }
}

void CContainerObject::Update(_float _fTimeDelta)
{
    __super::Update(_fTimeDelta);

    for (auto& pPartObj : m_PartObjects)
    {
        if (nullptr != pPartObj && true == pPartObj->Is_Active())
            pPartObj->Update(_fTimeDelta);
    }
}

void CContainerObject::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta);

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

HRESULT CContainerObject::Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition)
{
    if (FAILED(__super::Change_Coordinate(_eCoordinate, _pNewPosition)))
        return E_FAIL;

    for (CGameObject* pPartObject : m_PartObjects)
    {
        if (nullptr != pPartObject)
        {
            pPartObject->Change_Coordinate(_eCoordinate, nullptr);
        }
    }
    //if (FAILED(m_PartObjects[PART_BODY]->Change_Coordinate(_eCoordinate, nullptr)))
    //    return E_FAIL;

    return S_OK;
}

HRESULT CContainerObject::Register_RenderGroup(_uint _iGroupId, _uint _iPriorityID)
{
    for (CGameObject* pPartObject : m_PartObjects)
    {
        if (nullptr != pPartObject)
        {
            if (FAILED(pPartObject->Register_RenderGroup(_iGroupId, _iPriorityID)))
                return E_FAIL;
        }
    }

#ifdef _DEBUG
    m_pGameInstance->Add_RenderObject_New(_iGroupId, _iPriorityID, this); // Collider2D Render 및 debug관련 렌더를 container에서 수행.
#endif // _DEBUG


    return S_OK;
}

CComponent* CContainerObject::Find_Part_Component(_uint _iPartObjectIndex, const _wstring& _strPartComponentTag)
{
    if (_iPartObjectIndex >= m_PartObjects.size())
        return nullptr;


    return m_PartObjects[_iPartObjectIndex]->Find_Component(_strPartComponentTag);
}

HRESULT CContainerObject::Add_PartObject(CPartObject* _pPartObject)
{
    /* 이 객체를 구성하기위한 부품(몸, 헤드, 무기, 이펙트, 사운드) 객체들 */
    if (nullptr == _pPartObject)
        return E_FAIL;

    m_PartObjects.push_back(_pPartObject);

    return S_OK;
}

_bool CContainerObject::Is_PartActive(_uint _iPartID)
{
    if (m_PartObjects[_iPartID] == nullptr)
        return false;
    return m_PartObjects[_iPartID]->Is_Active();
}

void CContainerObject::Set_PartActive(_uint _iPartID, _bool _bValue)
{
    if(m_PartObjects[_iPartID])
       m_PartObjects[_iPartID]->Set_Active(_bValue);
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

HRESULT CContainerObject::Cleanup_DeadReferences()
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


    if (FAILED(__super::Cleanup_DeadReferences()))
        return E_FAIL;


    return S_OK;
}

#ifdef _DEBUG

HRESULT CContainerObject::Imgui_Render_ObjectInfos()
{
    CGameObject::Imgui_Render_ObjectInfos();
    ImGui::Begin("PartObject Infos");

    ImGui::Text("<Parts Infos>");

    static CGameObject* pPartObject = nullptr;
    _int iNumParts = (_uint)m_PartObjects.size();
    ImGui::Text("iNumParts : %d", iNumParts);

    if (ImGui::TreeNode("PartObjects"))
    {
        vector<const char*> strInstanceIDs;
        _int iSelectObjectIndex = 0;
        strInstanceIDs.clear();
        vector<_string> strPartNames;
        strPartNames.resize(m_PartObjects.size());
        int iIndex = 0;
        for (auto& pPart : m_PartObjects)
        {
            if (nullptr == pPart)
                continue;
            strPartNames[iIndex] = typeid(*pPart).name();
            _int iInstanceID = (_int)(pPart->Get_GameObjectInstanceID());
            strPartNames[iIndex] += "_" + to_string(iInstanceID);
            strInstanceIDs.push_back(strPartNames[iIndex++].c_str());
        }

        if (ImGui::ListBox(" ", &iSelectObjectIndex, strInstanceIDs.data(), (_int)strInstanceIDs.size()))
        {
            if (iSelectObjectIndex != -1)
            {
                pPartObject = m_PartObjects[iSelectObjectIndex];
            }

        }

        ImGui::TreePop();
    }


    if (nullptr != pPartObject)
        pPartObject->Imgui_Render_ObjectInfos();
    ImGui::End();
    return S_OK;
}

#endif // _DEBUG
