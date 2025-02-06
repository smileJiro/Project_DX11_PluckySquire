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

		_matrix OwnerWorldMatrix = m_pOwner->Get_FinalWorldMatrix();
		_float4x4 FinalMatrix = {};
		XMStoreFloat4x4(&FinalMatrix, XMLoadFloat4x4(&m_OffsetMatrix) * OwnerWorldMatrix);
		PxMat44 PxFinalMatrix((_float*)(&FinalMatrix));
		PxTransform pxTransform{ PxFinalMatrix };
		if(pxTransform.isValid())
			static_cast<PxRigidDynamic*>(m_pActor)->setKinematicTarget(pxTransform);
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

HRESULT CActor_Dynamic::Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition)
{
	if (FAILED(__super::Change_Coordinate(_eCoordinate, _pNewPosition)))
		return E_FAIL;

	if (COORDINATE_2D == _eCoordinate)
		return S_OK;

	if (nullptr == _pNewPosition)
		return S_OK;

	PxRigidDynamic* pDynamic = static_cast<PxRigidDynamic*>(m_pActor);

	_float3 vPos = {};
	XMStoreFloat3(&vPos, m_pOwner->Get_FinalPosition());
	static_cast<PxRigidDynamic*>(m_pActor)->setGlobalPose(PxTransform(_pNewPosition->x, _pNewPosition->y, _pNewPosition->z));

	return S_OK;
}

void CActor_Dynamic::Set_Kinematic()
{
	m_eActorType = ACTOR_TYPE::KINEMATIC;
	PxRigidDynamic* pDynamic = static_cast<PxRigidDynamic*>(m_pActor);
	pDynamic->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true); // Kinematic 
}

void CActor_Dynamic::Set_Dynamic()
{
	m_eActorType = ACTOR_TYPE::DYNAMIC;
	PxRigidDynamic* pDynamic = static_cast<PxRigidDynamic*>(m_pActor);
	PxTransform pxTransform;
	pDynamic->getKinematicTarget(pxTransform);
	pDynamic->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false); // Kinematic 
	pDynamic->setGlobalPose(pxTransform);
}

void CActor_Dynamic::Set_SleepThreshold(_float _fThreshold)
{
	PxRigidDynamic* pDynamic = static_cast<PxRigidDynamic*>(m_pActor);
	pDynamic->setSleepThreshold(_fThreshold);
}

_bool CActor_Dynamic::Is_Sleeping()
{
	return static_cast<PxRigidDynamic*>(m_pActor)->isSleeping();
}

_vector CActor_Dynamic::Get_LinearVelocity()
{
	PxVec3 vPxVec = static_cast<PxRigidDynamic*>(m_pActor)->getLinearVelocity();
	_vector vLinearVelocity = { vPxVec.x, vPxVec .y, vPxVec .z};
	return vLinearVelocity;
}

_vector CActor_Dynamic::Get_AngularVelocity()
{
	PxVec3 vPxVec = static_cast<PxRigidDynamic*>(m_pActor)->getAngularVelocity();
	_vector vAngularVelocity = { vPxVec.x, vPxVec.y, vPxVec.z };
	return vAngularVelocity;
}

void CActor_Dynamic::Set_LinearVelocity(_fvector _vDirection, _float _fVelocity)
{
	// true : 객체가 물리공간상에서 수면상태인 경우 깨운다는 파라미터임 >>> false면 수면상태인 경우 깨우지 않음.
	static_cast<PxRigidDynamic*>(m_pActor)->setLinearVelocity(PxVec3(XMVectorGetX(_vDirection) * _fVelocity, XMVectorGetY(_vDirection) * _fVelocity, XMVectorGetZ(_vDirection) * _fVelocity), m_isActive);
}

void CActor_Dynamic::Set_LinearVelocity(_fvector _vVelocity)
{
	static_cast<PxRigidDynamic*>(m_pActor)->setLinearVelocity(PxVec3{ XMVectorGetX(_vVelocity),XMVectorGetY(_vVelocity) ,XMVectorGetZ(_vVelocity) }, m_isActive);
}

void CActor_Dynamic::Set_AngularVelocity(const _float3& _vAngularVelocity)
{
	PxVec3 vAngularVelocity = PxVec3(_vAngularVelocity.x, _vAngularVelocity.y, _vAngularVelocity.z);
	
	if(true == vAngularVelocity.isFinite())
		static_cast<PxRigidDynamic*>(m_pActor)->setAngularVelocity(vAngularVelocity, m_isActive);
}

void CActor_Dynamic::Set_AngularVelocity(_fvector _vAngularVelocity)
{
	PxVec3 vAngularVelocity = PxVec3(XMVectorGetX(_vAngularVelocity), XMVectorGetY(_vAngularVelocity), XMVectorGetZ(_vAngularVelocity));

	if (true == vAngularVelocity.isFinite())
		static_cast<PxRigidDynamic*>(m_pActor)->setAngularVelocity(vAngularVelocity, m_isActive);
}

void CActor_Dynamic::Set_Rotation(_fvector _vAxis, _float _fRadian)
{
	PxRigidDynamic* pDynamicActor = static_cast<PxRigidDynamic*>(m_pActor);
	PxTransform currentTransform = pDynamicActor->getGlobalPose();
	PxVec3 currentPosition = currentTransform.p;

	// 새로운 회전 생성 (라디안 단위 각도로 설정)
	PxVec3 rotationAxis(XMVectorGetX(_vAxis), XMVectorGetY(_vAxis), XMVectorGetZ(_vAxis));
	PxQuat newRotation(_fRadian, rotationAxis.getNormalized());

	// 새로운 변환 적용
	PxTransform newTransform(currentPosition, newRotation);
	pDynamicActor->setGlobalPose(newTransform, m_isActive); 
}

void CActor_Dynamic::Set_Rotation(_fvector _vLook)
{
	_vector vLook = XMVector3Normalize(_vLook);
	PxRigidDynamic* pDynamicActor = static_cast<PxRigidDynamic*>(m_pActor);
	PxTransform currentTransform = pDynamicActor->getGlobalPose();
	PxVec3 currentPosition = currentTransform.p;

	_vector vForward = _vector{0,0,1,0};
	PxQuat newRotation;
	if (XMVector3NearEqual(vForward, vLook, XMVectorReplicate(1e-6f))) {
		newRotation = PxQuat(PxIdentity); // 동일한 방향이면 단위 쿼터니언 반환
	}

	_float fAngle = acos(XMVectorGetX(XMVector3Dot(vForward, vLook))); // 라디안 값
	if (XMVectorGetX(vLook) < 0)
		fAngle = -fAngle;
	PxVec3 pxAxis = PxVec3(0, 1,0);
	newRotation = PxQuat(fAngle, pxAxis);
	// 새로운 변환 적용
	PxTransform newTransform(currentPosition, newRotation);
	pDynamicActor->setGlobalPose(newTransform, m_isActive); 
}

void CActor_Dynamic::Set_LinearDamping(_float _fValue)
{
	static_cast<PxRigidDynamic*>(m_pActor)->setLinearDamping(_fValue);

}

void CActor_Dynamic::Set_AngularDamping(_float _fValue)
{
	static_cast<PxRigidDynamic*>(m_pActor)->setAngularDamping(_fValue);
}

void CActor_Dynamic::Add_Force(const _float3& _vForce)
{
	static_cast<PxRigidDynamic*>(m_pActor)->addForce(PxVec3(_vForce.x, _vForce.y, _vForce.z), PxForceMode::eFORCE, m_isActive);
}

void CActor_Dynamic::Add_Impulse(const _float3& _vForce)
{
	static_cast<PxRigidDynamic*>(m_pActor)->addForce(PxVec3(_vForce.x, _vForce.y, _vForce.z), PxForceMode::eIMPULSE, m_isActive);
}


void CActor_Dynamic::Set_ActorOffsetMatrix(_fmatrix _ActorOffsetMatrix)
{
	if (m_eActorType == ACTOR_TYPE::DYNAMIC)
		return;
	else
	{
		XMStoreFloat4x4(&m_OffsetMatrix, _ActorOffsetMatrix);

		_matrix OwnerWorldMatrix = m_pOwner->Get_WorldMatrix();
		_vector vScale = {};
		_vector vPosition = {};
		_vector vQuat = {};
		XMMatrixDecompose(&vScale, &vQuat, &vPosition, _ActorOffsetMatrix * OwnerWorldMatrix);

		PxTransform Transform;
		Transform.p = PxVec3(XMVectorGetX(vPosition), XMVectorGetY(vPosition), XMVectorGetZ(vPosition));
		Transform.q = PxQuat(XMVectorGetX(vQuat), XMVectorGetY(vQuat), XMVectorGetZ(vQuat), XMVectorGetW(vQuat));

		static_cast<PxRigidDynamic*>(m_pActor)->setKinematicTarget(Transform);
	}
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
