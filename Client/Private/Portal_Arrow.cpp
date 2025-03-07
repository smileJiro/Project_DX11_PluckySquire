#include "stdafx.h"
#include "Portal_Arrow.h"
#include "GameInstance.h"
#include "ModelObject.h"
#include "Section_Manager.h"
#include "Player.h"

#include "Camera_Manager.h"
#include "Camera_Target.h"
#include "Camera_2D.h"

CPortal_Arrow::CPortal_Arrow(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CPortal(_pDevice, _pContext)
{
}

CPortal_Arrow::CPortal_Arrow(const CPortal_Arrow& _Prototype)
    :CPortal(_Prototype)
{
}

HRESULT CPortal_Arrow::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPortal_Arrow::Initialize(void* _pArg)
{

    m_ePortalType = PORTAL_ARROW;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (nullptr == _pArg)
        return E_FAIL;

    PORTAL_DESC* pDesc = static_cast<PORTAL_DESC*>(_pArg);


    if (FAILED(Ready_PartObjects(pDesc)))
        return E_FAIL;


    return S_OK;
}

void CPortal_Arrow::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
}

void CPortal_Arrow::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{

	if (OBJECT_GROUP::PLAYER == _pOtherObject->Get_ObjectGroupID() ) 
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(_pOtherObject);


        if (COORDINATE_2D == pPlayer->Get_CurCoord()) {
            static_cast<CCamera_Target*>(CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET))->Set_InitialData(m_strSectionName, m_iPortalIndex);
        }
        else if (COORDINATE_3D == pPlayer->Get_CurCoord()) {
            static_cast<CCamera_2D*>(CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET_2D))->Set_Current_PortalID(m_iPortalIndex);
        }


  //      NORMAL_DIRECTION eNormal = Get_PortalNormal();
		//Use_Portal(pPlayer);

		//pPlayer->Set_PortalNormal(eNormal);
        pPlayer->JumpTo_Portal(this);
	}
}

HRESULT CPortal_Arrow::Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition)
{
    if (FAILED(m_pControllerTransform->Change_Coordinate(_eCoordinate, _pNewPosition)))
        return E_FAIL;
    return S_OK;
}


void CPortal_Arrow::Active_OnEnable()
{
    __super::Active_OnEnable();
}

HRESULT CPortal_Arrow::Ready_PartObjects(PORTAL_DESC* _pDesc)
{

    CModelObject::MODELOBJECT_DESC ModelDesc{};
    ModelDesc.Build_2D_Model(LEVEL_STATIC,
        L"Portal_2D_Arrow",
        L"Prototype_Component_Shader_VtxPosTex",
        (_uint)PASS_VTXPOSTEX::SPRITE2D
    );
    ModelDesc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);

    m_PartObjects[PORTAL_PART_2D] = static_cast<CModelObject*>
        (m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &ModelDesc));
    if (nullptr == m_PartObjects[PORTAL_PART_2D])
    {
        MSG_BOX("Portal 2D Arrow Sprite Model Creation Failed");
        return E_FAIL;
    }
    ((CModelObject*)m_PartObjects[PORTAL_PART_2D])->Set_AnimationLoop(COORDINATE_2D,0,true);
    ((CModelObject*)m_PartObjects[PORTAL_PART_2D])->Switch_Animation(0);
    return S_OK;
}

HRESULT CPortal_Arrow::Ready_Components(PORTAL_DESC* _pDesc)
{
    CCollider* pCollider = nullptr;
    /* Test 2D Collider */
    CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};
    CircleDesc.pOwner = this;
    CircleDesc.fRadius = 100.f;
    CircleDesc.vScale = { 0.03f, 0.1f };
    CircleDesc.isBlock = false;
    CircleDesc.iCollisionGroupID = OBJECT_GROUP::INTERACTION_OBEJCT;
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&pCollider), &CircleDesc)))
        return E_FAIL;

    m_p2DColliderComs.push_back(pCollider);

    return S_OK;
}

void CPortal_Arrow::Active_OnDisable()
{
    __super::Active_OnDisable();
}

CPortal_Arrow* CPortal_Arrow::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CPortal_Arrow* pInstance = new CPortal_Arrow(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Created CPortal_Arrow Failed");
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}

CGameObject* CPortal_Arrow::Clone(void* _pArg)
{
    CPortal_Arrow* pInstance = new CPortal_Arrow(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Clone CPortal_Arrow Failed");
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}

void CPortal_Arrow::Free()
{
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pEffectSystem);
    __super::Free();
}