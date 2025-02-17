#include "stdafx.h"
#include "CarriableObject.h"
#include "Player.h"
#include "Section_Manager.h"    
    
#include "Collider_Circle.h"
#include "Actor_Dynamic.h"
#include "Controller_Model.h"
#include "Model.h"

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
	CARRIABLE_DESC* pDesc = static_cast<CARRIABLE_DESC*>(_pArg);

	m_eCarriableId = pDesc->eCrriableObjId;

    pDesc->eStartCoord  = COORDINATE_3D;
    pDesc->iModelPrototypeLevelID_2D = pDesc->iCurLevelID;
    pDesc->iModelPrototypeLevelID_3D = pDesc->iCurLevelID;

    pDesc->iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;
    pDesc->iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;

    pDesc->iRenderGroupID_3D = RG_3D;
    pDesc->iPriorityID_3D = PR3D_GEOMETRY;
	pDesc->iObjectGroupID = OBJECT_GROUP::INTERACTION_OBEJCT;

	pDesc->pActorDesc->pOwner = this;
    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;
    //m_pActorCom-> Set_ShapeEnable(0, true);

       /* Test 2D Collider */
	m_p2DColliderComs.resize(1);
    CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};
    CircleDesc.pOwner = this;
    CircleDesc.fRadius = 40.f;
    CircleDesc.vScale = { 1.0f, 1.0f };
    CircleDesc.vOffsetPosition = { 0.f, 0.f };
    CircleDesc.isBlock = false;
    CircleDesc.isTrigger = true;
	CircleDesc.iCollisionGroupID = OBJECT_GROUP::INTERACTION_OBEJCT;
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
        TEXT("Com_2DCollider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &CircleDesc)))
        return E_FAIL;
	m_pBody2DColliderCom = m_p2DColliderComs[0];
	Safe_AddRef(m_pBody2DColliderCom);

    return S_OK;
}

void CCarriableObject::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);	
	COORDINATE eCurCoord = m_pControllerTransform->Get_CurCoord();
	if (nullptr != m_pParentBodyMatrices[eCurCoord])
	{
		_matrix matWorld = XMLoadFloat4x4(&m_WorldMatrices[eCurCoord]);
		_matrix matBody= XMLoadFloat4x4(m_pParentBodyMatrices[eCurCoord]);
		matWorld.r[3] += matBody.r[3];
		matWorld.r[3].m128_f32[3] = 1.f;
		XMStoreFloat4x4(&m_WorldMatrices[eCurCoord], matWorld);
	}
}

void CCarriableObject::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
	COORDINATE eCurCoord = m_pControllerTransform->Get_CurCoord();
	if(nullptr != m_pParentBodyMatrices[eCurCoord])
	{
		_matrix matWorld = XMLoadFloat4x4(&m_WorldMatrices[eCurCoord]);
		_matrix matBody = XMLoadFloat4x4(m_pParentBodyMatrices[eCurCoord]);
		matWorld.r[3] += matBody.r[3];
		matWorld.r[3].m128_f32[3] = 1.f;
		XMStoreFloat4x4(&m_WorldMatrices[eCurCoord], matWorld);
		//cout << "Position : " << matWorld.r[3].m128_f32[0] << ", "<< matWorld.r[3].m128_f32[1] << ", " << matWorld.r[3].m128_f32[2] << endl;
	}
	if (COORDINATE_2D == Get_CurCoord() && false == Is_Carrying())
	{
		m_f2DUpForce -= 9.8f * _fTimeDelta * 300;\
		m_f2DFloorDistance += m_f2DUpForce * _fTimeDelta;  
		if (0 > m_f2DFloorDistance)
		{
			m_f2DFloorDistance = 0;
			m_b2DOnGround = true;
			m_bThrowing = false;
			m_f2DUpForce = 0;
		}
		else if (0 == m_f2DFloorDistance)
		{
			m_bThrowing = false;
			m_b2DOnGround = true;
		}
		else
		{
			m_b2DOnGround = false;
		}
		_vector vMyPosition = Get_FinalPosition();
		_vector vPosition =m_v2DGroundPosition + m_v2DThrowHorizeForce * _fTimeDelta;
		vPosition = XMVectorSetY(vPosition, XMVectorGetY(vPosition) + m_f2DFloorDistance);
		Set_Position(vPosition);
	}

}

HRESULT CCarriableObject::Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition)
{

	if (FAILED(__super::Change_Coordinate(_eCoordinate, _pNewPosition)))
		return E_FAIL;
	if (Is_Carrying())
	{
		Set_Position({ 0,0,0 });
		Get_ControllerTransform()->Rotation(0, _vector{ 0,1,0 });
		Set_SocketMatrix(_eCoordinate, m_pCarrier->Get_CarryingOffset_Ptr(_eCoordinate));
		Set_ParentMatrix(_eCoordinate, m_pCarrier->Get_ControllerTransform()->Get_WorldMatrix_Ptr(_eCoordinate));
		
		m_pControllerModel->Get_Model(COORDINATE_3D)->Set_Active(COORDINATE_3D == _eCoordinate);
		m_pControllerModel->Get_Model(COORDINATE_2D)->Set_Active(COORDINATE_2D == _eCoordinate);
	}
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
		Set_ParentBodyMatrix(COORDINATE_3D, nullptr);
		Set_ParentBodyMatrix(COORDINATE_2D, nullptr);
		m_pCarrier = nullptr;

	}
	else
	{
		if (nullptr != m_pCarrier)
			return E_FAIL;
		m_pCarrier = _pCarrier;
		m_f2DFloorDistance = m_pCarrier->Get_CarryingOffset_Ptr(COORDINATE_2D)->_42;
		m_f2DUpForce = 0.f;
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
	else
	{
		m_v2DThrowHorizeForce = _vForce;
		m_v2DGroundPosition = m_pCarrier->Get_FinalPosition();
		m_f2DFloorDistance = m_pCarrier->Get_CarryingOffset_Ptr(COORDINATE_2D)->_42;
		m_f2DUpForce = 0.f;
		m_b2DOnGround = false;
		m_bThrowing = true;
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
	Safe_Release(m_pBody2DColliderCom);

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

_float CCarriableObject::Get_Distance(COORDINATE _eCoord, CPlayer* _pUser)
{
	return XMVector3Length(m_pControllerTransform->Get_Transform(_eCoord)->Get_State(CTransform::STATE_POSITION)
		- _pUser->Get_ControllerTransform()->Get_Transform(_eCoord)->Get_State(CTransform::STATE_POSITION)).m128_f32[0];
}

void CCarriableObject::Set_Kinematic(_bool _bKinematic)
{
	CActor_Dynamic* pDynamicActor = static_cast<CActor_Dynamic*>(m_pActorCom);
	if (_bKinematic)
	{
		pDynamicActor->Set_Kinematic();

	}
	else
	{
		pDynamicActor->Update(0);
		pDynamicActor->Set_Dynamic();
		pDynamicActor->Late_Update(0);
	}
}
