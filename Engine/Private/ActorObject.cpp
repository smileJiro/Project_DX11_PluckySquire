#include "ActorObject.h"
#include "GameInstance.h"

CActorObject::CActorObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CGameObject(_pDevice, _pContext)
{
}

CActorObject::CActorObject(const CActorObject& _Prototype)
    : CGameObject(_Prototype)
{
}

HRESULT CActorObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CActorObject::Initialize(void* _pArg)
{
    ACTOROBJECT_DESC* pDesc = static_cast<ACTOROBJECT_DESC*>(_pArg);

    // Desc Save 

    // Desc Add

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components(pDesc)))
        return E_FAIL;


    return S_OK;
}

HRESULT CActorObject::Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition)
{
    if (FAILED(__super::Change_Coordinate(_eCoordinate, _pNewPosition)))
        return E_FAIL;

    if (nullptr != m_pActorCom)
    {
        if (FAILED(m_pActorCom->Change_Coordinate(_eCoordinate, _pNewPosition)))
            return E_FAIL;
    }



    return S_OK;
}


void CActorObject::Active_OnEnable()
{
    if (nullptr != m_pActorCom)
        m_pActorCom->Set_Active(true);
}

void CActorObject::Active_OnDisable()
{
    if(nullptr != m_pActorCom)
        m_pActorCom->Set_Active(false);
}

HRESULT CActorObject::Ready_Components(ACTOROBJECT_DESC* _pDesc)
{
    _int iStaticLevelID = m_pGameInstance->Get_StaticLevelID();
    
    /* Com_Actor */
    switch (_pDesc->eActorType)
    {
    case Engine::ACTOR_TYPE::STATIC:
        if (FAILED(Add_Component(iStaticLevelID, TEXT("Prototype_Component_Actor_Static"),
            TEXT("Com_Actor"), reinterpret_cast<CComponent**>(&m_pActorCom), _pDesc->pActorDesc)))
            return E_FAIL;
        break;
    case Engine::ACTOR_TYPE::KINEMATIC:
        if (FAILED(Add_Component(iStaticLevelID, TEXT("Prototype_Component_Actor_Kinematic"),
            TEXT("Com_Actor"), reinterpret_cast<CComponent**>(&m_pActorCom), _pDesc->pActorDesc)))
            return E_FAIL;
        break;
    case Engine::ACTOR_TYPE::DYNAMIC:
        if (FAILED(Add_Component(iStaticLevelID, TEXT("Prototype_Component_Actor_Dynamic"),
            TEXT("Com_Actor"), reinterpret_cast<CComponent**>(&m_pActorCom), _pDesc->pActorDesc)))
            return E_FAIL;
        break;
    default:
        break;
    }


    return S_OK;
}

void CActorObject::Free()
{
    Safe_Release(m_pActorCom);

    __super::Free();
}

HRESULT CActorObject::Cleanup_DeadReferences()
{
    return S_OK;
}
