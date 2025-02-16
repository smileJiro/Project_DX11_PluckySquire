#include "Transform_2D.h"

CTransform_2D::CTransform_2D(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CTransform(_pDevice, _pContext)
{
}

CTransform_2D::CTransform_2D(const CTransform& _Prototype)
	:CTransform(_Prototype)
{
}

HRESULT CTransform_2D::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTransform_2D::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;
	TRANSFORM_2D_DESC* pDesc = static_cast<TRANSFORM_2D_DESC*>(_pArg);
	RotationZ(pDesc->fInitialRotation);
	return S_OK;
}

void CTransform_2D::LookDir(_fvector _vDir)
{
	_float fRadian = atan2f(XMVectorGetY(_vDir), XMVectorGetX(_vDir));
	Rotation(fRadian, {0,0,1});
}

_bool CTransform_2D::Go_Straight(_float _fTimeDelta)
{
	_vector vPos = Get_State(STATE::STATE_POSITION);
	_vector vLook = Get_State(STATE::STATE_UP);
	_vector vFinalPos = vPos + XMVector3Normalize(vLook) * m_fSpeedPerSec * _fTimeDelta;

	Set_State(STATE::STATE_POSITION, vFinalPos);

	return true;
}

_bool CTransform_2D::Go_Backward(_float _fTimeDelta)
{
	_vector vPos = Get_State(STATE::STATE_POSITION);
	_vector vLook = Get_State(STATE::STATE_UP);
	_vector vFinalPos = vPos - XMVector3Normalize(vLook) * m_fSpeedPerSec * _fTimeDelta;

	Set_State(STATE::STATE_POSITION, vFinalPos);

	return true;
}

CTransform_2D* CTransform_2D::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CTransform_2D* pInstance = new CTransform_2D(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTransform_2D");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CTransform_2D::Clone(void* pArg)
{
	CTransform_2D* pInstance = new CTransform_2D(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTransform_2D");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTransform_2D::Free()
{
	__super::Free();
}
