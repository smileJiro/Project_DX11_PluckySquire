#include "GameObject.h"
#include "GameInstance.h"



_uint CGameObject::g_iIDCount = 0;
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
    , m_Colliders(Prototype.m_Colliders)
    , m_iGameObjectID(g_iIDCount++)
{

    for (auto& pCollider : m_Colliders)
        Safe_AddRef(pCollider);

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
    }

    /* Create Controller Transform */
    m_pControllerTransform = CController_Transform::Create(m_pDevice, m_pContext, static_cast<CController_Transform::CON_TRANSFORM_DESC*>(_pArg));
    if (nullptr == m_pControllerTransform)
        return E_FAIL;

    return S_OK;
}

void CGameObject::Priority_Update(_float _fTimeDelta)
{
}

void CGameObject::Update(_float _fTimeDelta)
{
}

void CGameObject::Late_Update(_float _fTimeDelta)
{
}

HRESULT CGameObject::Render()
{
    return S_OK;
}

void CGameObject::OnContains(CGameObject* _pContainObject, CCollider* _pContainCollider)
{
}

void CGameObject::OnCollision_Enter(CCollider* _pMyCollider, CGameObject* _pOtherObject, CCollider* _pOtherCollider)
{
}

void CGameObject::OnCollision(CCollider* _pMyCollider, CGameObject* _pOtherObject, CCollider* _pOtherCollider)
{
}

void CGameObject::OnCollision_Exit(CCollider* _pMyCollider, CGameObject* _pOtherObject, CCollider* _pOtherCollider)
{
}

CComponent* CGameObject::Find_Component(const _wstring& _strComponentTag)
{
    auto iter = m_Components.find(_strComponentTag);
    if (m_Components.end() == iter)
        return nullptr;

    return iter->second;
}

HRESULT CGameObject::Change_Coordinate(COORDINATE _eCoordinate, const _float3& _vPosition)
{
    if (FAILED(m_pControllerTransform->Change_Coordinate(_eCoordinate, _vPosition)))
        return E_FAIL;
    
    return S_OK;
}


CCollider* CGameObject::Get_Collider(_uint _iCollIndex)
{
    if (m_Colliders.size() <= _iCollIndex)
        return nullptr;

    return m_Colliders[_iCollIndex];
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

    for (auto& pCollider : m_Colliders)
        Safe_Release(pCollider);
    m_Colliders.clear();

    Safe_Release(m_pControllerTransform);
    Safe_Release(m_pRayCom);


    Safe_Release(m_pGameInstance);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);

    __super::Free();

}
