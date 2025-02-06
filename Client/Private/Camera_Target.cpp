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
	return m_pCurArm->Get_Rotation();
}
#endif


void CCamera_Target::Add_CurArm(CCameraArm* _pCameraArm)
{
	if (nullptr == _pCameraArm)
		return;

	m_pCurArm = _pCameraArm;
}

void CCamera_Target::Add_ArmData(_wstring _wszArmTag, ARM_DATA* _pArmData, SUB_DATA* _pSubData)
{
	if (nullptr != Find_ArmData(_wszArmTag))
		return;

	m_ArmDatas.emplace(_wszArmTag, make_pair(_pArmData, _pSubData));
}

void CCamera_Target::Set_Freeze(_uint _iFreezeMask)
{
	m_iFreezeMask |= _iFreezeMask;
}

void CCamera_Target::Change_Target(const _float4x4* _pTargetWorldMatrix)
{
	m_pTargetWorldMatrix = _pTargetWorldMatrix;
}

void CCamera_Target::Switch_CameraView()
{
	if (nullptr == m_pCurArm)
		return;

	memcpy(&m_vPreTargetPos, m_pTargetWorldMatrix->m[3], sizeof(_float3));
	_vector vCameraPos = XMLoadFloat3(&m_vPreTargetPos) + (m_pCurArm->Get_Length() * m_pCurArm->Get_ArmVector());

	Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, vCameraPos);
	
	Look_Target(XMLoadFloat3(&m_vPreTargetPos), 0.f);
}

_bool CCamera_Target::Set_NextArmData(_wstring _wszNextArmName, _int _iTriggerID)
{
	pair<ARM_DATA*, SUB_DATA*>* pData = Find_ArmData(_wszNextArmName);

	if (nullptr == pData)
		return false;

	if (nullptr == m_pCurArm)
		return false;

	m_pCurArm->Set_NextArmData(pData->first, _iTriggerID);

	if (nullptr != pData->second) {

		Start_Zoom(pData->second->fZoomTime, (CCamera::ZOOM_LEVEL)pData->second->iZoomLevel, (CCamera::RATIO_TYPE)pData->second->iZoomRatioType);
		Start_Changing_AtOffset(pData->second->fAtOffsetTime, XMLoadFloat3(&pData->second->vAtOffset), pData->second->iAtRatioType);
	}

	return true;
}

void CCamera_Target::Set_PreArmDataState(_int _iTriggerID, _bool _isReturn)
{
	if (nullptr == m_pCurArm)
		return;

	m_pCurArm->Set_PreArmDataState(_iTriggerID, _isReturn);
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
	case MOVE_TO_NEXTARM:
		Move_To_NextArm(_fTimeDelta);
		break;
	case RETURN_TO_PREARM:
		Move_To_PreArm(_fTimeDelta);
		break;
	case RETURN_TO_DEFUALT:
		break;
	}
}

void CCamera_Target::Defualt_Move(_float _fTimeDelta)
{
	_vector vTargetPos;
	_vector vCameraPos = Calculate_CameraPos(&vTargetPos, _fTimeDelta);	// 목표 위치 + Arm -> 최종 결과물
	_vector vCurPos = m_pControllerTransform->Get_State(CTransform::STATE_POSITION);

	if (FREEZE_X == (m_iFreezeMask & FREEZE_X)) {
		vCameraPos = XMVectorSetX(vCameraPos, XMVectorGetX(vCurPos));
	}
	if (FREEZE_Z == (m_iFreezeMask & FREEZE_Z)) {
		vCameraPos = XMVectorSetZ(vCameraPos, XMVectorGetZ(vCurPos));
	}

	Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, vCameraPos);

	//if (RESET == (m_iFreezeMask | RESET)) 
	Look_Target(vTargetPos, _fTimeDelta);
}

void CCamera_Target::Move_To_NextArm(_float _fTimeDelta)
{
	if (true == m_pCurArm->Move_To_NextArm(_fTimeDelta)) {
		m_eCameraMode = DEFAULT;
		//return;
	}

	_vector vTargetPos;
	_vector vCameraPos = Calculate_CameraPos(&vTargetPos, _fTimeDelta);
	Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, vCameraPos);

	Look_Target(vTargetPos, _fTimeDelta);
}

void CCamera_Target::Look_Target(_fvector _vTargetPos, _float fTimeDelta)
{
	//_vector vTargetPos = XMLoadFloat4x4(m_pTargetWorldMatrix).r[TARGET_POS];

	_vector vAt = _vTargetPos + XMLoadFloat3(&m_vAtOffset) + XMLoadFloat3(&m_vShakeOffset);
	m_pControllerTransform->LookAt_3D(XMVectorSetW(vAt, 1.f));
}

void CCamera_Target::Move_To_PreArm(_float _fTimeDelta)
{
	if (true == m_pCurArm->Move_To_PreArm(_fTimeDelta)) {
		m_eCameraMode = DEFAULT;
		//return;
	}

	_vector vTargetPos;
	_vector vCameraPos = Calculate_CameraPos(&vTargetPos, _fTimeDelta);
	Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, vCameraPos);

	Look_Target(vTargetPos, _fTimeDelta);
}

_vector CCamera_Target::Calculate_CameraPos(_vector* _pLerpTargetPos, _float _fTimeDelta)
{
	_vector vTargetPos;
	memcpy(&vTargetPos, m_pTargetWorldMatrix->m[3], sizeof(_float4));
   
	_vector vCurPos = XMVectorLerp(XMVectorSetW(XMLoadFloat3(&m_vPreTargetPos), 1.f), vTargetPos, m_fSmoothSpeed * _fTimeDelta);
    
	if (nullptr != _pLerpTargetPos)
		*_pLerpTargetPos = vCurPos;

	_vector vCameraPos = vCurPos + (m_pCurArm->Get_Length() * m_pCurArm->Get_ArmVector());
	XMStoreFloat3(&m_vPreTargetPos, vCurPos);

	return vCameraPos;
}

pair<ARM_DATA*, SUB_DATA*>* CCamera_Target::Find_ArmData(_wstring _wszArmTag)
{
	auto iter = m_ArmDatas.find(_wszArmTag);

	if (iter == m_ArmDatas.end())
		return nullptr;

	return &(iter->second);
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
	for (auto& ArmData : m_ArmDatas) {
		Safe_Delete(ArmData.second.first);
		Safe_Delete(ArmData.second.second);
	}
	m_ArmDatas.clear();

	Safe_Release(m_pCurArm);

	__super::Free();
}
