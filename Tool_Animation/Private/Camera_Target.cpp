#include "stdafx.h"
#include "Camera_Target.h"

#include "GameInstance.h"

CCamera_Target::CCamera_Target(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext }
{
}

CCamera_Target::CCamera_Target(const CCamera_Target& Prototype)
	: CCamera{ Prototype }
{
}

HRESULT CCamera_Target::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Target::Initialize(void* pArg)
{
	CAMERA_TARGET_DESC* pDesc = static_cast<CAMERA_TARGET_DESC*>(pArg);

	pDesc->eStartCoord = COORDINATE_3D;
	pDesc->isCoordChangeEnable = false;
	pDesc->tTransform3DDesc.fSpeedPerSec = 10.f;
	pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(180.f);

	m_fSmoothSpeed = pDesc->fSmoothSpeed;
	m_eCameraMode = pDesc->eCameraMode;
	m_vAtOffset = pDesc->vAtOffset;
	m_pTargetWorldMatrix = pDesc->pTargetWorldMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CCamera_Target::Priority_Update(_float fTimeDelta)
{

}

void CCamera_Target::Update(_float fTimeDelta)
{

}

void CCamera_Target::Late_Update(_float fTimeDelta)
{
	Key_Input(fTimeDelta);

	Action_Mode(fTimeDelta);
	__super::Compute_PipeLineMatrices();
}

#ifdef _DEBUG
_float3 CCamera_Target::Get_ArmRotation()
{
	return m_pArm->Get_Rotation();
}
#endif


void CCamera_Target::Add_Arm(CCameraArm* _pCameraArm)
{
	if (nullptr == _pCameraArm)
		return;

	m_pArm = _pCameraArm;
}

void CCamera_Target::Change_Target(const _float4x4* _pTargetWorldMatrix)
{
	m_pTargetWorldMatrix = _pTargetWorldMatrix;
}

void CCamera_Target::Key_Input(_float _fTimeDelta)
{
#ifdef _DEBUG
	_long		MouseMove = {};
	_vector		fRotation = {};

	if (MOUSE_PRESSING(MOUSE_KEY::RB)) {
		if (MouseMove = MOUSE_MOVE(MOUSE_AXIS::X))
		{
			fRotation = XMVectorSetY(fRotation, MouseMove * _fTimeDelta * 0.3f);
			
		}

		if (MouseMove = MOUSE_MOVE(MOUSE_AXIS::Y))
		{
			fRotation = XMVectorSetX(fRotation, MouseMove * _fTimeDelta * 0.3f);
		}

		m_pArm->Set_Rotation(fRotation);
	}
#endif
}

void CCamera_Target::Action_Mode(_float fTimeDelta)
{
	switch (m_eCameraMode) {
	case DEFAULT:
		Defualt_Move(fTimeDelta);
		break;
	case TURN:
		break;
	}
}

void CCamera_Target::Defualt_Move(_float fTimeDelta)
{
	_vector vCameraPos = Calculate_CameraPos();
	Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, vCameraPos);

	Look_Target(fTimeDelta);
}

void CCamera_Target::Look_Target(_float fTimeDelta)
{
	_vector vTargetPos;

	memcpy(&vTargetPos, m_pTargetWorldMatrix->m[3], sizeof(_float4));
	m_pControllerTransform->LookAt_3D(XMVectorSetW(vTargetPos, 1.f));
}

_vector CCamera_Target::Calculate_CameraPos()
{
	_vector vTargetPos;

	memcpy(&vTargetPos, m_pTargetWorldMatrix->m[3], sizeof(_float4));
	vTargetPos = vTargetPos; // +XMLoadFloat3(&m_vPosOffset);

	_vector vCameraPos = vTargetPos + (m_pArm->Get_Length() * m_pArm->Get_ArmVector());

	return vCameraPos;
}

CCamera_Target* CCamera_Target::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Target* pInstance = new CCamera_Target(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera_Target");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_Target::Clone(void* pArg)
{
	CCamera_Target* pInstance = new CCamera_Target(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_Target");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Target::Free()
{
	Safe_Release(m_pArm);

	__super::Free();
}
