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
	return m_pCurArm->Get_Rotation();
}
#endif


void CCamera_Target::Add_CurArm(CCameraArm* _pCameraArm)
{
	if (nullptr == _pCameraArm)
		return;

	m_pCurArm = _pCameraArm;
}

void CCamera_Target::Add_ArmData(_wstring _wszArmTag, ARM_DATA _pData)
{
	if (nullptr != Find_ArmData(_wszArmTag))
		return;

	ARM_DATA* pArmData = new ARM_DATA();

	pArmData->fLength = _pData.fLength;
	pArmData->fMoveTimeAxisY = _pData.fMoveTimeAxisY;
	pArmData->fMoveTimeAxisRight = _pData.fMoveTimeAxisRight;
	pArmData->fLengthTime = _pData.fLengthTime;
	pArmData->fRotationPerSecAxisY = _pData.fRotationPerSecAxisY;
	pArmData->fRotationPerSecAxisRight = _pData.fRotationPerSecAxisRight;

	m_ArmDatas.emplace(_wszArmTag, pArmData);
}

void CCamera_Target::Change_Target(const _float4x4* _pTargetWorldMatrix)
{
	m_pCurArm->Change_Target(_pTargetWorldMatrix);
}

void CCamera_Target::Set_NextArmData(_wstring _wszNextArmName)
{
	ARM_DATA* pData = Find_ArmData(_wszNextArmName);

	if (nullptr == pData)
		return;

	if (nullptr == m_pCurArm)
		return;

	m_pCurArm->Set_NextArmData(pData);
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

		m_pCurArm->Set_Rotation(fRotation);
	}
#endif
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
	case TURN:
		break;
	}
}

void CCamera_Target::Defualt_Move(_float fTimeDelta)
{
	_vector vCameraPos = m_pCurArm->Calculate_CameraPos(fTimeDelta);
	Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, vCameraPos);

	_vector vTargetPos = m_pCurArm->Get_TargetState(CCameraArm::POS);

	_vector vAt = vTargetPos + XMLoadFloat3(&m_vAtOffset);
	m_pControllerTransform->LookAt_3D(XMVectorSetW(vAt, 1.f));
}

void CCamera_Target::Look_Target(_float fTimeDelta)
{

}

ARM_DATA* CCamera_Target::Find_ArmData(_wstring _wszArmTag)
{
	auto iter = m_ArmDatas.find(_wszArmTag);

	if (iter == m_ArmDatas.end())
		return nullptr;

	return iter->second;
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
	for (auto& ArmData : m_ArmDatas)
		Safe_Delete(ArmData.second);
	m_ArmDatas.clear();

	Safe_Release(m_pCurArm);

	__super::Free();
}
