#include "GameObject.h"
#include "GameInstance.h"



_uint CGameObject::g_iInstanceIDCount = 0;
CGameObject::CGameObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : m_pDevice(_pDevice)
    , m_pContext(_pContext)
    , m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

CGameObject::CGameObject(const CGameObject& Prototype)
    : m_pDevice(Prototype.m_pDevice)
    , m_pContext(Prototype.m_pContext)
    , m_pGameInstance(Prototype.m_pGameInstance)
    , m_iCurLevelID(Prototype.m_iCurLevelID)
    , m_iInstanceID(g_iInstanceIDCount++)
{

    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

HRESULT CGameObject::Initialize_Prototype()
{

    return S_OK;
}

/* 게임 내에서 직접 사용되기 위한 사본 객체가 생성된다. */
HRESULT CGameObject::Initialize(void* _pArg)
{
    if (nullptr != _pArg)
    {
        // 상속 관계로 정의 된 구조체를 _pArg 로 넘기기때문에, 캐스팅하여 사용하면 원하는 파트의 데이터만을 취하는 것이 가능.
        // BackGroundDesc(최하위 상속객체) 로 생성하여 모든 Data를 포함. >>> 모든 값을 채운 후, 이를 계속 upcasting하며, 원하는 데이터 만을 취한다.
        GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(_pArg);
        m_iCurLevelID = pDesc->iCurLevelID;
		m_iCollisionGroupID = pDesc->iCollisionGroupID;
    }

    /* Create Controller Transform */
    m_pControllerTransform = CController_Transform::Create(m_pDevice, m_pContext, static_cast<CController_Transform::CON_TRANSFORM_DESC*>(_pArg));
    if (nullptr == m_pControllerTransform)
        return E_FAIL;

    return S_OK;
}

void CGameObject::Priority_Update(_float _fTimeDelta)
{
    Priority_Update_Component(_fTimeDelta);
}

void CGameObject::Update(_float _fTimeDelta)
{
    Update_Component(_fTimeDelta);
}

void CGameObject::Late_Update(_float _fTimeDelta)
{
    Late_Update_Component(_fTimeDelta);
}

HRESULT CGameObject::Render()
{
#ifdef _DEBUG
    for (auto& p2DCollider : m_p2DColliderComs)
    {
        p2DCollider->Render();
    }
#endif // _DEBUG

    return S_OK;
}

HRESULT CGameObject::Register_RenderGroup(_uint _iGroupId, _uint _iPriorityID)
{
    return m_pGameInstance->Add_RenderObject_New(_iGroupId, _iPriorityID, this);
}

void CGameObject::Priority_Update_Component(_float _fTimeDelta)
{
    for (auto& iter : m_Components)
    {
        if(true == iter.second->Is_Active())
            iter.second->Priority_Update(_fTimeDelta);
    }
}

void CGameObject::Update_Component(_float _fTimeDelta)
{
    for (auto& iter : m_Components)
    {
        if (true == iter.second->Is_Active())
            iter.second->Update(_fTimeDelta);
    }
}

void CGameObject::Late_Update_Component(_float _fTimeDelta)
{
    for (auto& iter : m_Components)
    {
        if (true == iter.second->Is_Active())
            iter.second->Late_Update(_fTimeDelta);
    }
}

CComponent* CGameObject::Find_Component(const _wstring& _strComponentTag)
{
    auto iter = m_Components.find(_strComponentTag);
    if (m_Components.end() == iter)
        return nullptr;

    return iter->second;
}

HRESULT CGameObject::Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition)
{
    if (FAILED(m_pControllerTransform->Change_Coordinate(_eCoordinate, _pNewPosition)))
        return E_FAIL;

    return S_OK;
}


HRESULT CGameObject::Add_Component(_uint _iPrototypeLevelID, const _wstring& _strPrototypeTag, const _wstring& _strComponentTag, CComponent** _ppOut, void* _pArg)
{
    /* Prototype Manager에서 Component를 Clone 후 map Container에 Insert , 추가로 파라미터로 들어온 _ppOut에 해당 컴포넌트 포인터 참조 */

    // 1. 이미 Components 안에 존재하는 Tag라면 예외로 본다.
    if (nullptr != Find_Component(_strComponentTag))
        return E_FAIL; 

    // 2. Clone
    CComponent* pComponent = static_cast<CComponent*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, _iPrototypeLevelID, _strPrototypeTag, _pArg));
    if (nullptr == pComponent)
        return E_FAIL;

    // 3. 정상적으로 Clone되었다면, Insert
    m_Components.emplace(_strComponentTag, pComponent);

    // 4. _ppOut 에도 참조시켜주고, AddRef();
    *_ppOut = pComponent;
    Safe_AddRef(pComponent);

    return S_OK;
}

HRESULT CGameObject::Add_Component(const _wstring& _strComponentTag, CComponent* _pComponent)
{
    if (nullptr != Find_Component(_strComponentTag))
        return E_FAIL;

    if (nullptr == _pComponent)
        return E_FAIL;

    m_Components.emplace(_strComponentTag, _pComponent);

    return S_OK;
}

void CGameObject::Free()
{
    for (auto& Pair : m_Components)
        Safe_Release(Pair.second);
    m_Components.clear();

    for (auto& p2DCollider : m_p2DColliderComs)
        Safe_Release(p2DCollider);
    m_p2DColliderComs.clear();

    Safe_Release(m_pControllerTransform);
    Safe_Release(m_pRayCom);
    Safe_Release(m_pGravityCom);

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);

    __super::Free();

}

#ifdef _DEBUG

HRESULT CGameObject::Imgui_Render_ObjectInfos()
{
    /* Current Coord */
    if (nullptr == m_pControllerTransform)
        return E_FAIL;

    COORDINATE eCurCoord = m_pControllerTransform->Get_CurCoord();
    _string strCurCoord = "Current Coord : ";
    switch (eCurCoord)
    {
    case Engine::COORDINATE_2D:
        strCurCoord += "2D";
        break;
    case Engine::COORDINATE_3D:
        strCurCoord += "3D";
        break;
    case Engine::COORDINATE_LAST:
        strCurCoord += "LAST";
        break;
    }
    ImGui::Text(strCurCoord.c_str());


    /* Coordinate Change Enable */
    _bool isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();
    _string strCoordChangeEnable = "CoordChangeEnable : ";
    if (true == isCoordChangeEnable)
        strCoordChangeEnable += "true";
    else
        strCoordChangeEnable += "false";
    ImGui::Text(strCoordChangeEnable.c_str());


    /* Active */
    _bool isActive = Is_Active();
    _string strActive = "Active : ";
    if (true == isActive)
        strActive += "true";
    else
        strActive += "false";
    ImGui::Text(strActive.c_str());
    ImGui::SameLine();
    if (ImGui::Button("ActiveOnOff")) { isActive ^= 1; Set_Active(isActive); }

    /* isRender */
    _bool isRender = Is_Render();
    _string strRender = "Render : ";
    if (true == isRender)
        strRender += "true";
    else
        strRender += "false";
    ImGui::Text(strRender.c_str());
    ImGui::SameLine();
    if (ImGui::Button("RenderOnOff")) { isRender ^= 1; Set_Render(isRender); }

    /* Transform Data */
    ImGui::Separator();
    ImGui::Text("<Transform Data>");
    ImGui::Text("World Matrix");
    ImGui::PushItemWidth(200.f);
    _float4 vRight = {};
    XMStoreFloat4(&vRight, m_pControllerTransform->Get_State(CTransform::STATE_RIGHT));
    ImGui::BeginDisabled();/* 수정 불가 영역 시작. */
    ImGui::InputFloat4("vRight", (float*)&vRight, " %.2f", ImGuiInputTextFlags_EnterReturnsTrue);

    _float4 vUp = {};
    XMStoreFloat4(&vUp, m_pControllerTransform->Get_State(CTransform::STATE_UP));
    ImGui::InputFloat4("vUp", (float*)&vUp, " %.2f", ImGuiInputTextFlags_EnterReturnsTrue);

    _float4 vLook = {};
    XMStoreFloat4(&vLook, m_pControllerTransform->Get_State(CTransform::STATE_LOOK));
    ImGui::InputFloat4("vLook", (float*)&vLook, " %.2f", ImGuiInputTextFlags_EnterReturnsTrue);
    ImGui::EndDisabled();/* 수정 불가 영역 끝. */

    _float4 vPosition = {};
    ImGui::PushItemWidth(150.f);
    XMStoreFloat4(&vPosition, m_pControllerTransform->Get_State(CTransform::STATE_POSITION));
    if (ImGui::InputFloat3("", (float*)&vPosition, " %.2f", ImGuiInputTextFlags_EnterReturnsTrue))
        Set_Position(XMLoadFloat4(&vPosition));
    ImGui::SameLine();
    ImGui::Text("%.2f", vPosition.w);
    ImGui::SameLine();
    ImGui::Text(" vPosition");


    ImGui::PushItemWidth(150.f);
    _float3 vScale = Get_FinalScale();
    if (ImGui::InputFloat3("       vScale", (float*)&vScale, " %.2f", ImGuiInputTextFlags_EnterReturnsTrue))
        Set_Scale(vScale);
    ImGui::PopItemWidth();


    return S_OK;
}

#endif // _DEBUG
