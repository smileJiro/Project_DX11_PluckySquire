#include "Controller_Transform.h"
#include "GameInstance.h"
#include "Transform_2D.h"
#include "Transform_3D.h"

CController_Transform::CController_Transform(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: m_pDevice(_pDevice)
	, m_pContext(_pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}


HRESULT CController_Transform::Initialize(CON_TRANSFORM_DESC* _pDesc)
{
	// Save
	m_eCurCoord = _pDesc->eStartCoord;
	m_isCoordChangeEnable = _pDesc->isCoordChangeEnable;
	// Add

	if (FAILED(Ready_Transforms(_pDesc)))
		return E_FAIL;


	return S_OK;
}

void CController_Transform::Update_AutoRotation(_float _fTimeDelta)
{
	switch (m_eCurCoord)
	{
	case Engine::COORDINATE_2D:
		break;
	case Engine::COORDINATE_3D:
		static_cast<CTransform_3D*>( m_pTransforms[m_eCurCoord])->Update_AutoRotationY(_fTimeDelta);		break;
	case Engine::COORDINATE_LAST:
		break;
	default:
		break;
	}

}

HRESULT CController_Transform::Change_Coordinate(COORDINATE _eCoordinate, const _float3& _vPosition)
{
	if (_eCoordinate == m_eCurCoord)
		return S_OK;

	if (false == m_isCoordChangeEnable)
		return E_FAIL;

	if (nullptr == m_pTransforms[_eCoordinate])
		return E_FAIL;

	m_eCurCoord = _eCoordinate;

	m_pTransforms[m_eCurCoord]->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&_vPosition), 1.0f));

	return S_OK;
}

_bool CController_Transform::Go_Straight(_float _fTimeDelta)
{
	return m_pTransforms[m_eCurCoord]->Go_Straight(_fTimeDelta);
}

_bool CController_Transform::Go_Backward(_float _fTimeDelta)
{
	return m_pTransforms[m_eCurCoord]->Go_Backward(_fTimeDelta);
}

_bool CController_Transform::Go_Left(_float _fTimeDelta)
{
	return m_pTransforms[m_eCurCoord]->Go_Left(_fTimeDelta);
}

_bool CController_Transform::Go_Right(_float _fTimeDelta)
{
	return m_pTransforms[m_eCurCoord]->Go_Right(_fTimeDelta);
}

_bool CController_Transform::Go_Up(_float _fTimeDelta)
{
	return m_pTransforms[m_eCurCoord]->Go_Up(_fTimeDelta);
}

_bool CController_Transform::Go_Down(_float _fTimeDelta)
{
	return m_pTransforms[m_eCurCoord]->Go_Down(_fTimeDelta);
}

void CController_Transform::Go_Direction(_vector _vDirection, _float _fTimeDelta)
{
	m_pTransforms[m_eCurCoord]->Go_Direction(_vDirection, _fTimeDelta);
}

void CController_Transform::Rotation(_float _fRadian, _fvector _vAxis)
{
	m_pTransforms[m_eCurCoord]->Rotation(_fRadian, _vAxis);
}

void CController_Transform::RotationXYZ(const _float3& _vRadianXYZ)
{
	m_pTransforms[m_eCurCoord]->RotationXYZ(_vRadianXYZ);
}

void CController_Transform::RotationQuaternion(const _float3& _vRadianXYZ)
{
	m_pTransforms[m_eCurCoord]->RotationQuaternion(_vRadianXYZ);
}

void CController_Transform::RotationQuaternionW(const _float4& _vQuaternion)
{
	m_pTransforms[m_eCurCoord]->RotationQuaternionW(_vQuaternion);
}

void CController_Transform::Turn(_float _fRadian, _fvector _vAxis)
{
	m_pTransforms[m_eCurCoord]->Turn(_fRadian, _vAxis);
}

void CController_Transform::TurnAngle(_float _fRadian, _fvector _vAxis)
{
	m_pTransforms[m_eCurCoord]->TurnAngle(_fRadian, _vAxis);
}

_float CController_Transform::Compute_Distance(_fvector _vTargetPos)
{
	return 	m_pTransforms[m_eCurCoord]->Compute_Distance(_vTargetPos);
}

HRESULT CController_Transform::Bind_ShaderResource(CShader* pShader, const _char* pConstantName)
{
 	return m_pTransforms[m_eCurCoord]->Bind_ShaderResource(pShader, pConstantName);
}

void CController_Transform::LookAt_3D(_fvector _vAt)
{
	if (COORDINATE_2D == m_eCurCoord)
		return;

	if (nullptr == m_pTransforms[m_eCurCoord])
		return;

	static_cast<CTransform_3D*>(m_pTransforms[m_eCurCoord])->LookAt(_vAt);
}

CTransform* CController_Transform::Get_Transform() const
{
	return m_pTransforms[m_eCurCoord];
}

CTransform* CController_Transform::Get_Transform(COORDINATE _eCoordinate) const
{
	return m_pTransforms[_eCoordinate];
}

_matrix CController_Transform::Get_WorldMatrix() const
{
	return m_pTransforms[m_eCurCoord]->Get_WorldMatrix();
}

_matrix CController_Transform::Get_WorldMatrix(COORDINATE _eCoordinate) const
{
	return m_pTransforms[_eCoordinate]->Get_WorldMatrix();
}

const _float4x4* CController_Transform::Get_WorldMatrix_Ptr() const
{
	return m_pTransforms[m_eCurCoord]->Get_WorldMatrix_Ptr();
}

const _float4x4* CController_Transform::Get_WorldMatrix_Ptr(COORDINATE _eCoordinate) const
{
	return m_pTransforms[_eCoordinate]->Get_WorldMatrix_Ptr();

}

_float3 CController_Transform::Get_Scale() const
{
	return m_pTransforms[m_eCurCoord]->Get_Scale();
}

_vector CController_Transform::Get_State(CTransform::STATE _eState) const
{
	return m_pTransforms[m_eCurCoord]->Get_State(_eState);
}

_float CController_Transform::Get_SpeedPerSec() const
{
	return m_pTransforms[m_eCurCoord]->Get_SpeedPerSec();
}

_float CController_Transform::Get_RotationPerSec() const
{
	return m_pTransforms[m_eCurCoord]->Get_RotationPerSec();
}


void CController_Transform::Set_WorldMatrix(_fmatrix _WorldMatrix)
{
	m_pTransforms[m_eCurCoord]->Set_WorldMatrix(_WorldMatrix);
}

void CController_Transform::Set_WorldMatrix(const _float4x4& _WorldMatrix)
{
	m_pTransforms[m_eCurCoord]->Set_WorldMatrix(_WorldMatrix);
}

void CController_Transform::Set_Scale(_float _fX, _float _fY, _float _fZ)
{
	m_pTransforms[m_eCurCoord]->Set_Scale(_fX, _fY, _fZ);
}

void CController_Transform::Set_Scale(const _float3& _vScale)
{
	m_pTransforms[m_eCurCoord]->Set_Scale(_vScale);
}

void CController_Transform::Set_State(CTransform::STATE _eState, _fvector _vState)
{
	m_pTransforms[m_eCurCoord]->Set_State(_eState, _vState);
}

void CController_Transform::Set_State(CTransform::STATE _eState, const _float4& _vState)
{
	m_pTransforms[m_eCurCoord]->Set_State(_eState, _vState);
}

void CController_Transform::Set_SpeedPerSec(_float _fSpeedPerSec)
{
	m_pTransforms[m_eCurCoord]->Set_SpeedPerSec(_fSpeedPerSec);
}

void CController_Transform::Set_RotationPerSec(_float _fRotationPerSec)
{
	m_pTransforms[m_eCurCoord]->Set_RotationPerSec(_fRotationPerSec);
}

void CController_Transform::Set_AutoRotationYDirection(_fvector _vRotYTarget)
{

	static_cast<CTransform_3D*>(m_pTransforms[m_eCurCoord])->Set_AutoRotationYDirection(_vRotYTarget);
}

HRESULT CController_Transform::Ready_Transforms(CON_TRANSFORM_DESC* _pDesc)
{
	switch (m_eCurCoord)
	{
	case Engine::COORDINATE_2D:
	{
		CTransform* pTransform2D = CTransform_2D::Create(m_pDevice, m_pContext);
		if (nullptr == pTransform2D)
			return E_FAIL;

		if (FAILED(pTransform2D->Initialize(&(_pDesc->tTransform2DDesc))))
			return E_FAIL;

		m_pTransforms[COORDINATE_2D] = pTransform2D;

		if (true == m_isCoordChangeEnable)
		{
			CTransform* pTransform3D = CTransform_3D::Create(m_pDevice, m_pContext);
			if (nullptr == pTransform3D)
				return E_FAIL;

			if (FAILED(pTransform3D->Initialize(&(_pDesc->tTransform3DDesc))))
				return E_FAIL;

			m_pTransforms[COORDINATE_3D] = pTransform3D;
		}
	}
	break;
	case Engine::COORDINATE_3D:
	{
		CTransform* pTransform3D = CTransform_3D::Create(m_pDevice, m_pContext);
		if (nullptr == pTransform3D)
			return E_FAIL;

		if (FAILED(pTransform3D->Initialize(&(_pDesc->tTransform3DDesc))))
			return E_FAIL;

		m_pTransforms[COORDINATE_3D] = pTransform3D;

		if (true == m_isCoordChangeEnable)
		{
			CTransform* pTransform2D = CTransform_2D::Create(m_pDevice, m_pContext);
			if (nullptr == pTransform2D)
				return E_FAIL;

			if (FAILED(pTransform2D->Initialize(&(_pDesc->tTransform2DDesc))))
				return E_FAIL;

			m_pTransforms[COORDINATE_2D] = pTransform2D;
		}
	}
	break;
	default:
		break;
	}

	return S_OK;
}

CController_Transform* CController_Transform::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CON_TRANSFORM_DESC* _pDesc)
{
	CController_Transform* pInstance = new CController_Transform(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_pDesc)))
	{
		MSG_BOX("Failed to Created : CController_Transform");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CController_Transform::Free()
{
	for (_uint i = 0; i < COORDINATE_LAST; ++i)
	{
		Safe_Release(m_pTransforms[i]);
		m_pTransforms[i] = nullptr;
	}

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
	__super::Free();
}
