#include "stdafx.h"
#include "Pressure_Plate.h"
#include "GameInstance.h"
#include "Section_Manager.h"
#include "CarriableObject.h"

CPressure_Plate::CPressure_Plate(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CModelObject(_pDevice, _pContext)
{
}

CPressure_Plate::CPressure_Plate(const CPressure_Plate& _Prototype)
	: CModelObject(_Prototype)
{
}

HRESULT CPressure_Plate::Initialize(void* _pArg)
{
    if (nullptr == _pArg)
        return E_FAIL;
    CModelObject::MODELOBJECT_DESC* pBodyDesc = static_cast<CModelObject::MODELOBJECT_DESC*>(_pArg);
    pBodyDesc->eStartCoord = COORDINATE_2D;
    pBodyDesc->isCoordChangeEnable = false;
    pBodyDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
    pBodyDesc->iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;

    pBodyDesc->tTransform2DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);

    pBodyDesc->eActorType = ACTOR_TYPE::LAST;
    pBodyDesc->pActorDesc = nullptr;
    pBodyDesc->iModelPrototypeLevelID_2D = LEVEL_STATIC;
    pBodyDesc->strModelPrototypeTag_2D = TEXT("Pressure_Plate");

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    m_p2DColliderComs.resize(1);
    ///* Test 2D Collider */
    CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};

    CircleDesc.pOwner = this;
    CircleDesc.fRadius = 50.f;
    CircleDesc.vScale = { 1.0f, 1.0f };
    CircleDesc.vOffsetPosition = { 0.f, 0.f };
    CircleDesc.isBlock = false;
    CircleDesc.isTrigger = true;
    CircleDesc.iCollisionGroupID = OBJECT_GROUP::MAPOBJECT;

    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
        TEXT("Com_Body2DCollider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &CircleDesc)))
        return E_FAIL;


   Set_AnimationLoop(COORDINATE::COORDINATE_2D, DOWN, false);
   Set_AnimationLoop(COORDINATE::COORDINATE_2D, DOWN_IDLE, true);
   Set_AnimationLoop(COORDINATE::COORDINATE_2D, IDLE, true);
   Set_AnimationLoop(COORDINATE::COORDINATE_2D, UP, false);

    Switch_Animation(IDLE);
    Register_OnAnimEndCallBack(bind(&CPressure_Plate::On_AnimEnd, this, placeholders::_1, placeholders::_2));


	return S_OK;
}

HRESULT CPressure_Plate::Render()
{
    __super::Render();

#ifdef _DEBUG
    for (auto& p2DCollider : m_p2DColliderComs)
    {
        p2DCollider->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));
    }
#endif // _DEBUG

	return S_OK;
}

void CPressure_Plate::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    OBJECT_GROUP eGroup = (OBJECT_GROUP)_pOtherCollider->Get_CollisionGroupID();
    
    if (OBJECT_GROUP::PLAYER == eGroup || OBJECT_GROUP::GIMMICK_OBJECT == eGroup)
    {
        if (DOWN != m_eState && DOWN_IDLE != m_eState)
        {
            Switch_Animation(DOWN);
            m_eState = DOWN;
        }
        ++m_iCollisionObjects;
    }
}

void CPressure_Plate::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    OBJECT_GROUP eGroup = (OBJECT_GROUP)_pOtherCollider->Get_CollisionGroupID();

    if (OBJECT_GROUP::PLAYER == eGroup || OBJECT_GROUP::GIMMICK_OBJECT == eGroup)
    {
        --m_iCollisionObjects;
        
        if (0 == m_iCollisionObjects)
        {
            Switch_Animation(UP);
            m_eState = UP;
        }
    }
}

void CPressure_Plate::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
    if (DOWN == m_eState)
    {
        Switch_Animation(DOWN_IDLE);
        m_eState = DOWN_IDLE;
    }

    else if (UP == m_eState)
    {
        Switch_Animation(IDLE);
        m_eState = IDLE;
    }
}

CPressure_Plate* CPressure_Plate::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CPressure_Plate* pInstance = new CPressure_Plate(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CPressure_Plate");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPressure_Plate::Clone(void* _pArg)
{
    CPressure_Plate* pInstance = new CPressure_Plate(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CPressure_Plate");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPressure_Plate::Free()
{
    __super::Free();
}
