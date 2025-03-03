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

void CActorObject::Add_ActorToScene()
{
    if (nullptr == m_pActorCom)
        return;

    if (true == m_pActorCom->Is_ActorInScene())
        return;

    m_pActorCom->Add_ActorToScene();
}

void CActorObject::Remove_ActorFromScene()
{
    if (nullptr == m_pActorCom)
        return;

    if (false == m_pActorCom->Is_ActorInScene())
        return;

    m_pActorCom->Remove_ActorFromScene();
}

_bool CActorObject::Is_ActorInScene()
{
    if (nullptr == m_pActorCom)
        return false;

    return m_pActorCom->Is_ActorInScene();
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

void CActorObject::Add_Impuls(_fvector _vForce)
{
    _float3 f3Force;
    XMStoreFloat3(&f3Force, _vForce);
    m_pActorCom->Add_Impulse(f3Force);
}

void CActorObject::Add_Force(_fvector _vForce)
{
    _float3 f3Force;
    XMStoreFloat3(&f3Force, _vForce);
    m_pActorCom->Add_Force(f3Force);
}

void CActorObject::Set_Dead()
{
    __super::Set_Dead();

    if (nullptr == m_pActorCom)
        return;

    m_pActorCom->Set_PxActorDisable();
}



void CActorObject::Active_OnEnable()
{
    if (nullptr != m_pActorCom && COORDINATE_3D == Get_CurCoord())
    {
        m_pActorCom->Set_Active(true);
    }

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
    if (nullptr == _pDesc->pActorDesc)
        return S_OK;

    _pDesc->pActorDesc->isAddActorToScene = _pDesc->isAddActorToScene;
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

    if (nullptr != m_pActorCom && COORDINATE_2D == _pDesc->eStartCoord)
        m_pActorCom->Set_Active(false);

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
