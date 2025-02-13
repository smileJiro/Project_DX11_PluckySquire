#include "stdafx.h"
#include "Camera_Target.h"

#include "GameInstance.h"

CCamera_Target::CCamera_Target(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCamera{ _pDevice, _pContext }
{
}

CCamera_Target::CCamera_Target(const CCamera_Target& _Prototype)
	: CCamera{ _Prototype }
{
}

HRESULT CCamera_Target::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Target::Initialize(void* _pArg)
{
	CAMERA_TARGET_DESC* pDesc = static_cast<CAMERA_TARGET_DESC*>(_pArg);

	pDesc->eStartCoord = COORDINATE_3D;
	pDesc->isCoordChangeEnable = false;
	pDesc->tTransform3DDesc.fSpeedPerSec = 10.f;
	pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(180.f);

	m_fSmoothSpeed = pDesc->fSmoothSpeed;
	m_eCameraMode = pDesc->eCameraMode;
	m_vAtOffset = pDesc->vAtOffset;
	m_pTargetWorldMatrix = pDesc->pTargetWorldMatrix;

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	return S_OK;
}

void CCamera_Target::Priority_Update(_float _fTimeDelta)
{
	
}

void CCamera_Target::Update(_float _fTimeDelta)
{
	
}

void CCamera_Target::Late_Update(_float _fTimeDelta)
{
	Key_Input(_fTimeDelta);

	Action_Mode(_fTimeDelta);

	m_pArm->Get_ArmVector();

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
			fRotation = XMVectorSetX(fRotation, MouseMove * _fTimeDelta * -0.3f);
		}

		m_pArm->Set_Rotation(fRotation);
	}
#endif
}

void CCamera_Target::Set_NextArmData(ARM_DATA* _pArmData, SUB_DATA* _pSubData)
{
	if (nullptr == _pArmData)
		return;

	if (nullptr == m_pArm)
		return;

	m_pArm->Set_NextArmData(_pArmData, 0);

	// Sub Data ผ๖วเ
	if (nullptr != _pSubData) {
		Start_Zoom(_pSubData->fZoomTime, (CCamera::ZOOM_LEVEL)_pSubData->iZoomLevel, (RATIO_TYPE)_pSubData->iZoomRatioType);
		Start_Changing_AtOffset(_pSubData->fAtOffsetTime, XMLoadFloat3(&_pSubData->vAtOffset), _pSubData->iAtRatioType);
	}
}

void CCamera_Target::Action_Mode(_float _fTimeDelta)
{
	Action_Zoom(_fTimeDelta);
	Action_Shake(_fTimeDelta);
	Change_AtOffset(_fTimeDelta);

	switch (m_eCameraMode) {
	case DEFAULT:
		Defualt_Move(_fTimeDelta);
		break;
	case MOVE_TO_NEXTARM:
		Move_To_NextArm(_fTimeDelta);
		break;
	}
}

void CCamera_Target::Defualt_Move(_float _fTimeDelta)
{
	_vector vCameraPos = Calculate_CameraPos();
	Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, vCameraPos);

	Look_Target(_fTimeDelta);
}

void CCamera_Target::Move_To_NextArm(_float _fTimeDelta)
{
	if (true == m_pArm->Move_To_NextArm_ByVector(_fTimeDelta)) {
		//m_pArm->Set_DesireVector();
		m_eCameraMode = DEFAULT;
		return;
	}

	_vector vCameraPos = Calculate_CameraPos();
	Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, vCameraPos);

	Look_Target(_fTimeDelta);
}

void CCamera_Target::Look_Target(_float _fTimeDelta)
{
	_vector vTargetPos = XMLoadFloat4x4(m_pTargetWorldMatrix).r[TARGET_POS];
	
	_vector vAt = vTargetPos + XMLoadFloat3(&m_vAtOffset) + XMLoadFloat3(&m_vShakeOffset);
	m_pControllerTransform->LookAt_3D(XMVectorSetW(vAt, 1.f));

	XMStoreFloat3(&m_vTargetPos, vAt);
}

_vector CCamera_Target::Calculate_CameraPos()
{
	_vector vTargetPos;

	memcpy(&vTargetPos, m_pTargetWorldMatrix->m[3], sizeof(_float4));
	vTargetPos = vTargetPos; // +XMLoadFloat3(&m_vPosOffset);

	_vector vCameraPos = vTargetPos + (m_pArm->Get_Length() * m_pArm->Get_ArmVector());

	return vCameraPos;
}

CCamera_Target* CCamera_Target::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CCamera_Target* pInstance = new CCamera_Target(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera_Target");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_Target::Clone(void* _pArg)
{
	CCamera_Target* pInstance = new CCamera_Target(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
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
