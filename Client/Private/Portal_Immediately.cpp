#include "stdafx.h"
#include "Portal_Immediately.h"
#include "GameInstance.h"
#include "ModelObject.h"
#include "Section_Manager.h"
#include "Player.h"

#include "Camera_Manager.h"
#include "Camera_Target.h"
#include "Camera_2D.h"

CPortal_Immediately::CPortal_Immediately(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CPortal(_pDevice, _pContext)
{
}

CPortal_Immediately::CPortal_Immediately(const CPortal_Immediately& _Prototype)
    :CPortal(_Prototype)
{
}

HRESULT CPortal_Immediately::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPortal_Immediately::Initialize(void* _pArg)
{

    m_ePortalType = PORTAL_IMMEDIATELY;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (nullptr == _pArg)
        return E_FAIL;

    PORTAL_DESC* pDesc = static_cast<PORTAL_DESC*>(_pArg);

    return S_OK;
}

void CPortal_Immediately::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	if (OBJECT_GROUP::PLAYER == _pOtherObject->Get_ObjectGroupID() ) 
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(_pOtherObject);

        NORMAL_DIRECTION eNormal;
		Use_Portal(pPlayer, &eNormal);

		pPlayer->Set_PortalNormal(eNormal);

	}
}

HRESULT CPortal_Immediately::Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition)
{
    if (FAILED(m_pControllerTransform->Change_Coordinate(_eCoordinate, _pNewPosition)))
        return E_FAIL;
    return S_OK;
}


void CPortal_Immediately::Active_OnEnable()
{
    __super::Active_OnEnable();
}

HRESULT CPortal_Immediately::Ready_PartObjects(PORTAL_DESC* _pDesc)
{

    CModelObject::MODELOBJECT_DESC ModelDesc{};
    ModelDesc.Build_2D_Model(LEVEL_STATIC,
        L"Portal_2D_Immediately",
        L"Prototype_Component_Shader_VtxPosTex",
        (_uint)PASS_VTXPOSTEX::SPRITE2D
    );
    ModelDesc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);

    m_PartObjects[PORTAL_PART_2D] = static_cast<CModelObject*>
        (m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &ModelDesc));
    if (nullptr == m_PartObjects[PORTAL_PART_2D])
    {
        MSG_BOX("Portal 2D Immediately Sprite Model Creation Failed");
        return E_FAIL;
    }
    return S_OK;
}

void CPortal_Immediately::Active_OnDisable()
{
    __super::Active_OnEnable();
}

CPortal_Immediately* CPortal_Immediately::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CPortal_Immediately* pInstance = new CPortal_Immediately(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Created CPortal_Immediately Failed");
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}

CGameObject* CPortal_Immediately::Clone(void* _pArg)
{
    CPortal_Immediately* pInstance = new CPortal_Immediately(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Clone CPortal_Immediately Failed");
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}

void CPortal_Immediately::Free()
{
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pEffectSystem);
    __super::Free();
}