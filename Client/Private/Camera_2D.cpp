#include "stdafx.h"
#include "Camera_2D.h"

#include "GameInstance.h"
#include "Section_Manager.h"

CCamera_2D::CCamera_2D(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext }
{
}

CCamera_2D::CCamera_2D(const CCamera_2D& Prototype)
	: CCamera{ Prototype }
{
}

HRESULT CCamera_2D::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_2D::Initialize(void* pArg)
{
	CAMERA_2D_DESC* pDesc = static_cast<CAMERA_2D_DESC*>(pArg);

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

void CCamera_2D::Priority_Update(_float fTimeDelta)
{
}

void CCamera_2D::Update(_float fTimeDelta)
{
}

void CCamera_2D::Late_Update(_float fTimeDelta)
{
#ifdef _DEBUG
	Key_Input(fTimeDelta);
#endif

	Switching(fTimeDelta);

	Action_Mode(fTimeDelta);
	__super::Compute_PipeLineMatrices();
}

void CCamera_2D::Add_CurArm(CCameraArm* _pCameraArm)
{
	if (nullptr == _pCameraArm)
		return;

	m_pCurArm = _pCameraArm;
}

void CCamera_2D::Switch_CameraView(INITIAL_DATA* _pInitialData)
{
	if (nullptr == m_pCurArm)
		return;

	// Initial Data가 없어서 TargetPos + vArm * Length로 초기 위치를 바로 잡아주기
	if (nullptr == _pInitialData) {
		_vector vTargetPos = CSection_Manager::GetInstance()->Get_WorldPosition_FromWorldPosMap({ m_pTargetWorldMatrix->_41, m_pTargetWorldMatrix->_42 });
		_vector vCameraPos = vTargetPos + (m_pCurArm->Get_Length() * m_pCurArm->Get_ArmVector());

		Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vCameraPos, 1.f));

		Look_Target(0.f);
	}
	// 초기 위치부터 다음위치까지 Lerp를 해야 함
	else {
		m_tInitialData = *_pInitialData;
		m_isInitialData = true;
		m_InitialTime = { _pInitialData->fInitialTime, 0.f };

		// 초기 위치 설정
		m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_tInitialData.vPosition), 1.f));

		// 초기 보는 곳 설정
		m_pControllerTransform->LookAt_3D(XMVectorSetW(XMLoadFloat3(&m_tInitialData.vAt), 1.f));

		// 초기 Zoom Level 설정
		m_iCurZoomLevel = m_tInitialData.iZoomLevel;
		m_fFovy = m_ZoomLevels[m_tInitialData.iZoomLevel];
	}
}

INITIAL_DATA CCamera_2D::Get_InitialData()
{
	INITIAL_DATA tData;

	XMStoreFloat3(&tData.vPosition, m_pControllerTransform->Get_State(CTransform::STATE_POSITION));

	_vector vTargetPos = CSection_Manager::GetInstance()->Get_WorldPosition_FromWorldPosMap({ m_pTargetWorldMatrix->_41, m_pTargetWorldMatrix->_42 });
	_vector vAt = vTargetPos + XMLoadFloat3(&m_vAtOffset) + XMLoadFloat3(&m_vShakeOffset);
	XMStoreFloat3(&tData.vAt, vAt);

	tData.iZoomLevel = m_iCurZoomLevel;

	return tData;
}

void CCamera_2D::Action_Mode(_float _fTimeDelta)
{
	if (true == m_isInitialData)
		return;

	Action_Zoom(_fTimeDelta);
	Action_Shake(_fTimeDelta);
	Change_AtOffset(_fTimeDelta);

	switch (m_eCameraMode) {
	case DEFAULT:
		Defualt_Move(_fTimeDelta);
		break;
	case MOVE_TO_DESIREPOS:
		Move_To_DesirePos(_fTimeDelta);
		break;
	case MOVE_TO_SHOP:
		Move_To_Shop(_fTimeDelta);
		break;
	case RETURN_TO_DEFUALT:
		Return_To_Default(_fTimeDelta);
		break;
	case FLIPPING:
		Fliping(_fTimeDelta);
		break;
	}
}

void CCamera_2D::Defualt_Move(_float _fTimeDelta)
{
	_vector vCameraPos = Calculate_CameraPos(_fTimeDelta);	// 목표 위치 + Arm -> 최종 결과물

	Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, vCameraPos);

	Look_Target(_fTimeDelta);
}

void CCamera_2D::Move_To_DesirePos(_float _fTimeDelta)
{
}

void CCamera_2D::Move_To_Shop(_float _fTimeDelta)
{
}

void CCamera_2D::Return_To_Default(_float _fTimeDelta)
{
}

void CCamera_2D::Fliping(_float _fTimeDelta)
{
}

void CCamera_2D::Look_Target(_float fTimeDelta)
{
	_vector vTargetPos = XMVectorSetW(XMLoadFloat3(&m_v2DTargetWorldPos), 1.f);

	_vector vAt = vTargetPos + XMLoadFloat3(&m_vAtOffset) + XMLoadFloat3(&m_vShakeOffset);
	m_pControllerTransform->LookAt_3D(XMVectorSetW(vAt, 1.f));
}

_vector CCamera_2D::Calculate_CameraPos(_float _fTimeDelta)
{
	_vector vTargetPos = CSection_Manager::GetInstance()->Get_WorldPosition_FromWorldPosMap({ m_pTargetWorldMatrix->_41, m_pTargetWorldMatrix->_42 });
	XMStoreFloat3(&m_v2DTargetWorldPos, vTargetPos);

	_vector vCameraPos = vTargetPos + (m_pCurArm->Get_Length() * m_pCurArm->Get_ArmVector());

	return vCameraPos;
}

void CCamera_2D::Switching(_float _fTimeDelta)
{
	if (false == m_isInitialData)
		return;

	_float fRatio = Calculate_Ratio(&m_InitialTime, _fTimeDelta, EASE_IN);

	if (fRatio > 1.f) {
		_vector vTargetPos = CSection_Manager::GetInstance()->Get_WorldPosition_FromWorldPosMap({ m_pTargetWorldMatrix->_41, m_pTargetWorldMatrix->_42 });
		_vector vCameraPos = vTargetPos + (m_pCurArm->Get_Length() * m_pCurArm->Get_ArmVector());
		m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vCameraPos, 1.f));

		_vector vLookAt = vTargetPos + XMLoadFloat3(&m_vAtOffset) + XMLoadFloat3(&m_vShakeOffset);
		m_pControllerTransform->LookAt_3D(XMVectorSetW(vLookAt, 1.f));

		m_fFovy = m_ZoomLevels[m_iCurZoomLevel];

		m_InitialTime.y = 0.f;
		m_isInitialData = false;
		return;
	}

	// Pos
	_vector vTargetPos = CSection_Manager::GetInstance()->Get_WorldPosition_FromWorldPosMap({ m_pTargetWorldMatrix->_41, m_pTargetWorldMatrix->_42 });
	_vector vCameraPos = vTargetPos + (m_pCurArm->Get_Length() * m_pCurArm->Get_ArmVector());
	_vector vResulPos = XMVectorLerp(XMLoadFloat3(&m_tInitialData.vPosition), vCameraPos, fRatio);

	Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vResulPos, 1.f));

	// LookAt
	_vector vLookAt = vTargetPos + XMLoadFloat3(&m_vAtOffset) + XMLoadFloat3(&m_vShakeOffset);
	_vector vResultLookAt = XMVectorLerp(XMLoadFloat3(&m_tInitialData.vAt), vLookAt, fRatio);

	m_pControllerTransform->LookAt_3D(XMVectorSetW(vResultLookAt, 1.f));

	// Zoom Level
	_float fFovy = m_pGameInstance->Lerp(m_ZoomLevels[m_tInitialData.iZoomLevel], m_ZoomLevels[m_iCurZoomLevel], fRatio);
	m_fFovy = fFovy;
}

#ifdef _DEBUG
void CCamera_2D::Key_Input(_float _fTimeDelta)
{
	//_long		MouseMove = {};
	//_vector		fRotation = {};

	//if (MOUSE_PRESSING(MOUSE_KEY::RB)) {
	///*	if (MouseMove = MOUSE_MOVE(MOUSE_AXIS::X))
	//	{
	//		fRotation = XMVectorSetY(fRotation, MouseMove * _fTimeDelta * 0.3f);

	//	}*/

	//	if (MouseMove = MOUSE_MOVE(MOUSE_AXIS::Y))
	//	{
	//		fRotation = XMVectorSetX(fRotation, MouseMove * _fTimeDelta * -0.3f);
	//	}

	//	m_pCurArm->Set_Rotation(fRotation);
	//}



	//_float fLength = m_pCurArm->Get_Length();

	//if (KEY_DOWN(KEY::F)) {
	//	m_pCurArm->Set_Length(fLength - 0.5f);
	//}
	//if (KEY_DOWN(KEY::G)){
	//	m_pCurArm->Set_Length(fLength + 0.5f);
	//}

	//if (KEY_DOWN(KEY::LEFT)) {  // y
	//	m_vAtOffset.y -= 0.5f;
	//}
	//if (KEY_DOWN(KEY::RIGHT)) {
	//	m_vAtOffset.y += 0.5f;
	//}
	//if (KEY_DOWN(KEY::DOWN)) {  // z
	//	m_vAtOffset.z -= 0.5f;
	//}
	//if (KEY_DOWN(KEY::UP)) {
	//	m_vAtOffset.z += 0.5f;
	//}

}
#endif

CCamera_2D* CCamera_2D::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_2D* pInstance = new CCamera_2D(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera_2D");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_2D::Clone(void* pArg)
{
	CCamera_2D* pInstance = new CCamera_2D(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_2D");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_2D::Free()
{
	Safe_Release(m_pCurArm);

	__super::Free();
}
