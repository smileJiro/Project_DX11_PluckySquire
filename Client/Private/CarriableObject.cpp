#include "stdafx.h"
#include "CarriableObject.h"
#include "Player.h"
#include "Section_Manager.h"    
#include "Collision_Manager.h"    
#include "Collider_Circle.h"
#include "Actor_Dynamic.h"

CCarriableObject::CCarriableObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CModelObject(_pDevice, _pContext)
{
}

CCarriableObject::CCarriableObject(const CCarriableObject& _Prototype)
	:CModelObject(_Prototype)
{
}

HRESULT CCarriableObject::Render()
{
	return __super::Render();
}

HRESULT CCarriableObject::Initialize(void* _pArg)
{
	CModelObject::MODELOBJECT_DESC* pDesc = static_cast<CModelObject::MODELOBJECT_DESC*>(_pArg);

    pDesc->eStartCoord  = COORDINATE_3D;
    pDesc->iModelPrototypeLevelID_2D = pDesc->iCurLevelID;
    pDesc->iModelPrototypeLevelID_3D = pDesc->iCurLevelID;

    pDesc->iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;
    pDesc->iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;

    pDesc->iRenderGroupID_3D = RG_3D;
    pDesc->iPriorityID_3D = PR3D_GEOMETRY;

	pDesc->pActorDesc->pOwner = this;
    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;
    //m_pActorCom-> Set_ShapeEnable(0, true);

       /* Test 2D Collider */
    CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};
    CircleDesc.pOwner = this;
    CircleDesc.fRadius = 40.f;
    CircleDesc.vScale = { 1.0f, 1.0f };
    CircleDesc.vOffsetPosition = { 0.f, 0.f };
    CircleDesc.isBlock = false;
    CircleDesc.isTrigger = true;
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
        TEXT("Com_2DCollider"), reinterpret_cast<CComponent**>(&m_pBody2DColliderCom), &CircleDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CCarriableObject::Set_Carrier(CPlayer* _pCarrier)
{
	if (nullptr == _pCarrier)
	{
		Set_ParentMatrix(COORDINATE_3D, nullptr);
		Set_ParentMatrix(COORDINATE_2D, nullptr);
		Set_SocketMatrix(COORDINATE_3D, nullptr);
		Set_SocketMatrix(COORDINATE_2D, nullptr);
		m_pCarrier = nullptr;
		if(COORDINATE_3D == Get_CurCoord())
		{
			static_cast<CActor_Dynamic*>(m_pActorCom)->Update(0);
			static_cast<CActor_Dynamic*>(m_pActorCom)->Set_Dynamic();
		}
	}
	else
	{
		if (nullptr != m_pCarrier)
			return E_FAIL;
		m_pCarrier = _pCarrier;
	}
	return S_OK;
}

void CCarriableObject::Throw(_fvector _vForce)
{
	if (COORDINATE_3D == Get_CurCoord())
	{
		_float3 vForce;
		XMStoreFloat3(&vForce, _vForce);
		static_cast<CActor_Dynamic*>(m_pActorCom)->Add_Impulse(vForce);
	}
}

CCarriableObject* CCarriableObject::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CCarriableObject* pInstance = new CCarriableObject(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CarriableObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCarriableObject::Clone(void* _pArg)
{
	CCarriableObject* pInstance = new CCarriableObject(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CarriableObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCarriableObject::Free()
{
		__super::Free();
}

void CCarriableObject::Interact(CPlayer* _pUser)
{
	_pUser->Set_CarryingObject(this);
}

_bool CCarriableObject::Is_Interactable(CPlayer* _pUser)
{
	return false == _pUser->Is_CarryingObject();
}

_float CCarriableObject::Get_Distance(CPlayer* _pUser)
{
	_vector vMyPos = Get_FinalPosition();
	_vector vUserPos = _pUser->Get_FinalPosition();
	return  XMVectorGetX(XMVector3Length(vMyPos - vUserPos));
}

void CCarriableObject::Set_Kinematic(_bool _bKinematic)
{
	CActor_Dynamic* pDynamicActor = static_cast<CActor_Dynamic*>(m_pActorCom);
	if (_bKinematic)
	{
		pDynamicActor->Late_Update(0);
		pDynamicActor->Set_Kinematic();

	}
	else
	{
		pDynamicActor->Update(0);
		pDynamicActor->Set_Dynamic();
	}
}
