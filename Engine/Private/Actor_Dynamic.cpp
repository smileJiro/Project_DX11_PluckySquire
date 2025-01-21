#include "Actor_Dynamic.h"
#include "ActorObject.h"

CActor_Dynamic::CActor_Dynamic(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, ACTOR_TYPE _eActorType)
    :CActor(_pDevice, _pContext, _eActorType)
{
}

CActor_Dynamic::CActor_Dynamic(const CActor_Dynamic& _Prototype)
    :CActor(_Prototype)
{
}

HRESULT CActor_Dynamic::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

    return S_OK;
}

HRESULT CActor_Dynamic::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

    return S_OK;
}

void CActor_Dynamic::Priority_Update(_float _fTimeDelta)
{
}

void CActor_Dynamic::Update(_float _fTimeDelta)
{
	/* GameObject들 업데이트 후 Component 업데이트 되니까. 여기서 object의 위치를 따라가는 Kinematic을 만들겠삼. */
	if (ACTOR_TYPE::KINEMATIC == m_eActorType)
	{
		if (nullptr == m_pOwner)
			return;

		_float3 vPos = {};
		XMStoreFloat3(&vPos, m_pOwner->Get_Position());

		static_cast<PxRigidDynamic*>(m_pActor)->setKinematicTarget(PxTransform(vPos.x, vPos.y, vPos.z));
	}

}

void CActor_Dynamic::Late_Update(_float _fTimeDelta)
{
	if (ACTOR_TYPE::DYNAMIC == m_eActorType)
	{
		if (nullptr == m_pOwner)
			return;

		if (COORDINATE_2D == m_pOwner->Get_CurCoord())
			return;

		PxTransform DynamicTransform = static_cast<PxRigidDynamic*>(m_pActor)->getGlobalPose();
		_matrix TranslationMatrix = XMMatrixTranslation(DynamicTransform.p.x, DynamicTransform.p.y, DynamicTransform.p.z);
		_matrix QuatMatrix = DirectX::XMMatrixRotationQuaternion(DirectX::XMVectorSet(DynamicTransform.q.x, DynamicTransform.q.y, DynamicTransform.q.z, DynamicTransform.q.w));
		_float4x4 WorldMatrix = {};
		XMStoreFloat4x4(&WorldMatrix, QuatMatrix * TranslationMatrix);
		m_pOwner->Set_WorldMatrix(WorldMatrix);
	}

#ifdef _DEBUG
	CActor::Late_Update(_fTimeDelta); // Debug_Render (Trigger Shape)
#endif // _DEBUG

}

void CActor_Dynamic::Set_LinearVelocity(_vector _vDirection, _float _fVelocity)
{

	// true : 객체가 물리공간상에서 수면상태인 경우 깨운다는 파라미터임 >>> false면 수면상태인 경우 깨우지 않음.
	static_cast<PxRigidDynamic*>(m_pActor)->setLinearVelocity(PxVec3(XMVectorGetX(_vDirection) * _fVelocity, XMVectorGetY(_vDirection) * _fVelocity, XMVectorGetZ(_vDirection) * _fVelocity), true);
}

void CActor_Dynamic::Set_AngularVelocity(const _float3& _vAngularVelocity)
{
	static_cast<PxRigidDynamic*>(m_pActor)->setAngularVelocity(PxVec3(_vAngularVelocity.x, _vAngularVelocity.y, _vAngularVelocity.z), true);
}

void CActor_Dynamic::Add_Force(const _float3& _vForce)
{
	static_cast<PxRigidDynamic*>(m_pActor)->addForce(PxVec3(_vForce.x, _vForce.y, _vForce.z), PxForceMode::eFORCE, true);
}

void CActor_Dynamic::Add_Impulse(const _float3& _vForce)
{
	static_cast<PxRigidDynamic*>(m_pActor)->addForce(PxVec3(_vForce.x, _vForce.y, _vForce.z), PxForceMode::eIMPULSE, true);
}

void CActor_Dynamic::Turn_TargetDirection(_vector _vDirection)
{
	_vector vLook = m_pOwner->Get_ControllerTransform()->Get_State(CTransform::STATE_LOOK);

	PxVec3 vForward = { XMVectorGetX(vLook),XMVectorGetY(vLook), XMVectorGetZ(vLook) };
	PxVec3 vTargetDirection = { XMVectorGetX(_vDirection),XMVectorGetY(_vDirection), XMVectorGetZ(_vDirection), };

	PxVec3 vRotationAxis = vForward.cross(vTargetDirection);

	float fAngleDiff = acos(vForward.dot(vTargetDirection));

	PxVec3 vAngularVelocity = vRotationAxis.getNormalized() * fAngleDiff * 4.0f;

	static_cast<PxRigidDynamic*>(m_pActor)->setAngularVelocity(vAngularVelocity, true);
}


CActor_Dynamic* CActor_Dynamic::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _bool _isKinematic)
{
	ACTOR_TYPE eActorType = ACTOR_TYPE::DYNAMIC;
	if (true == _isKinematic)
		eActorType = ACTOR_TYPE::KINEMATIC;

	CActor_Dynamic* pInstance = new CActor_Dynamic(_pDevice, _pContext, eActorType);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CActor_Dynamic");
		Safe_Release(pInstance);
	}
	return pInstance;
}


CComponent* CActor_Dynamic::Clone(void* _pArg)
{
	CActor_Dynamic* pInstance = new CActor_Dynamic(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CActor_Dynamic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CActor_Dynamic::Free()
{

	__super::Free();
}
