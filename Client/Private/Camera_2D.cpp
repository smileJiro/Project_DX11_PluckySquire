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

	Action_SetUp_ByMode();
	Action_Mode(fTimeDelta);

	__super::Compute_PipeLineMatrices();
}

void CCamera_2D::Add_CurArm(CCameraArm* _pCameraArm)
{
	if (nullptr == _pCameraArm)
		return;

	m_pCurArm = _pCameraArm;
}

void CCamera_2D::Add_ArmData(_wstring _wszArmTag, ARM_DATA* _pArmData, SUB_DATA* _pSubData)
{
	if (nullptr != Find_ArmData(_wszArmTag))
		return;

	m_ArmDatas.emplace(_wszArmTag, make_pair(_pArmData, _pSubData));
}

_bool CCamera_2D::Set_NextArmData(_wstring _wszNextArmName, _int _iTriggerID)
{
	pair<ARM_DATA*, SUB_DATA*>* pData = Find_ArmData(_wszNextArmName);

	if (nullptr == pData)
		return false;

	if (nullptr == m_pCurArm)
		return false;

	m_pCurArm->Set_NextArmData(pData->first, _iTriggerID);
	m_szEventTag = _wszNextArmName;

	return _bool();
}

void CCamera_2D::Switch_CameraView(INITIAL_DATA* _pInitialData)
{
	if (nullptr == m_pCurArm)
		return;

	// Sketch Space 면에 따라서 Arm 바꿔 주기
	_vector vTargetPos = CSection_Manager::GetInstance()->Get_WorldPosition_FromWorldPosMap(m_strSectionName, { m_pTargetWorldMatrix->_41, m_pTargetWorldMatrix->_42 });
	
	m_eCurSpaceDir = (NORMAL_DIRECTION)((_int)roundf(XMVectorGetW(vTargetPos)));
	pair<ARM_DATA*, SUB_DATA*>* pData = nullptr;

	switch ((_int)m_eCurSpaceDir) {
	case (_int)NORMAL_DIRECTION::POSITIVE_X:
	{
		pData = Find_ArmData(TEXT("Default_Positive_X"));
		m_pCurArm->Set_ArmVector(XMLoadFloat3(&pData->first->vDesireArm));
	}
		break;
	case (_int)NORMAL_DIRECTION::NEGATIVE_X:
	{
		pData = Find_ArmData(TEXT("Default_Negative_X"));
		m_pCurArm->Set_ArmVector(XMLoadFloat3(&pData->first->vDesireArm));
	}
		break;
	case (_int)NORMAL_DIRECTION::POSITIVE_Y:
	{
		pData = Find_ArmData(TEXT("Default_Positive_Y"));
		m_pCurArm->Set_ArmVector(XMLoadFloat3(&pData->first->vDesireArm));
	}
		break;
	case (_int)NORMAL_DIRECTION::NEGATIVE_Y:
		break;
	case (_int)NORMAL_DIRECTION::POSITIVE_Z:
		break;
	case (_int)NORMAL_DIRECTION::NEGATIVE_Z:
	{
		pData = Find_ArmData(TEXT("Default_Negative_Z"));
		m_pCurArm->Set_ArmVector(XMLoadFloat3(&pData->first->vDesireArm));
	}
		break;
	}

	// Initial Data가 없어서 TargetPos + vArm * Length로 초기 위치를 바로 잡아주기
	if (nullptr == _pInitialData) {
		_vector vTargetPos = CSection_Manager::GetInstance()->Get_WorldPosition_FromWorldPosMap(m_strSectionName,{ m_pTargetWorldMatrix->_41, m_pTargetWorldMatrix->_42 });
		_vector vCameraPos = vTargetPos + (m_pCurArm->Get_Length() * m_pCurArm->Get_ArmVector());
		XMStoreFloat3(&m_v2DPreTargetWorldPos, vTargetPos);
		XMStoreFloat3(&m_v2DFixedPos, vTargetPos);

		m_fFixedY = XMVectorGetY(vTargetPos);

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

	_vector vTargetPos = CSection_Manager::GetInstance()->Get_WorldPosition_FromWorldPosMap(m_strSectionName,{ m_pTargetWorldMatrix->_41, m_pTargetWorldMatrix->_42 });
	_vector vAt = vTargetPos + XMLoadFloat3(&m_vAtOffset) + XMLoadFloat3(&m_vShakeOffset);
	XMStoreFloat3(&tData.vAt, vAt);

	tData.iZoomLevel = m_iCurZoomLevel;

	return tData;
}

void CCamera_2D::Action_Mode(_float _fTimeDelta)
{
	if (true == m_isInitialData)
		return;

	Find_TargetPos();

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
	case MOVE_TO_DESIREPOS:
		Move_To_DesirePos(_fTimeDelta);
		break;
	case MOVE_TO_SHOP:
		Move_To_Shop(_fTimeDelta);
		break;
	case RETURN_TO_DEFUALT:
		Return_To_Default(_fTimeDelta);
		break;
	case FLIPPING_UP:
		Flipping_Up(_fTimeDelta);
		break;
	case FLIPPING_PAUSE:
		//Flipping_Pause(_fTimeDelta);
		break;
	case FLIPPING_DOWN:
		Flipping_Down(_fTimeDelta);
		break;
	}
}

void CCamera_2D::Action_SetUp_ByMode()
{
	if (m_ePreCameraMode != m_eCameraMode) {
	
		switch (m_eCameraMode) {
		case DEFAULT:
			break;
		case MOVE_TO_NEXTARM:
			break;
		case MOVE_TO_DESIREPOS:
			break;
		case MOVE_TO_SHOP:
			break;
		case RETURN_TO_DEFUALT:
			break;
		case FLIPPING_UP:
		{	
			if (VERTICAL == m_eDirectionType) {
				m_eDirectionType = HORIZON;
			}
			
			Set_NextArmData(TEXT("BookFlipping_Horizon"), 0);
			
			CGameObject* pBook = m_pGameInstance->Get_GameObject_Ptr(m_pGameInstance->Get_CurLevelID(), TEXT("Layer_Book"), 0);
			m_pTargetWorldMatrix = pBook->Get_ControllerTransform()->Get_WorldMatrix_Ptr();
			m_eTargetCoordinate = COORDINATE_3D;
		}
			break;
		case FLIPPING_DOWN:
		{
			CSection* pSection = CSection_Manager::GetInstance()->Find_Section(m_strSectionName);
			
			if (true == static_cast<CSection_2D*>(pSection)->Is_Rotation()) {
				if (HORIZON == m_eDirectionType) {
					m_eDirectionType = VERTICAL;
					Set_NextArmData(TEXT("Book_Vertical"), 0);
				}
			}
			else {
				Set_NextArmData(TEXT("Book_Horizon"), 0);
			}

			CGameObject* pPlayer = m_pGameInstance->Get_GameObject_Ptr(m_pGameInstance->Get_CurLevelID(), TEXT("Layer_Player"), 0);
			m_pTargetWorldMatrix = pPlayer->Get_ControllerTransform()->Get_WorldMatrix_Ptr();
			m_eTargetCoordinate = COORDINATE_2D;

			// 어디 볼지는 지금은 무조건 player 위치인데 위치랑 카메라가 못 가는 곳에 따라서
			// 조정이 필요함
		/*	_vector vTargetPos = CSection_Manager::GetInstance()->Get_WorldPosition_FromWorldPosMap(m_strSectionName,{ m_pTargetWorldMatrix->_41, m_pTargetWorldMatrix->_42 });
			XMStoreFloat3(&m_v2DPreTargetWorldPos, vTargetPos);*/
		}
			break;
		}

		m_ePreCameraMode = m_eCameraMode;
	}
}

void CCamera_2D::Defualt_Move(_float _fTimeDelta)
{
	_vector vCamerPos = Calculate_CameraPos(_fTimeDelta);

	m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vCamerPos, 1.f));

	Look_Target(_fTimeDelta);
}

void CCamera_2D::Move_To_NextArm(_float _fTimeDelta)
{
	if (true == m_pCurArm->Move_To_NextArm_ByVector(_fTimeDelta, true)) {
		m_eCameraMode = DEFAULT;
	}

	_vector vCamerPos = Calculate_CameraPos(_fTimeDelta);

	m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vCamerPos, 1.f));

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

void CCamera_2D::Flipping_Up(_float _fTimeDelta)
{	
	if (true == m_pCurArm->Move_To_NextArm_ByVector(_fTimeDelta, true)) {
		m_eCameraMode = FLIPPING_PAUSE;

		//return;
	}

	_vector vCamerPos = Calculate_CameraPos(_fTimeDelta);
	m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vCamerPos, 1.f));

	Look_Target(_fTimeDelta);
}

void CCamera_2D::Flipping_Pause(_float _fTimeDelta)
{
}

void CCamera_2D::Flipping_Down(_float _fTimeDelta)
{
	if (true == m_pCurArm->Move_To_NextArm_ByVector(_fTimeDelta, true)) {
		m_eCameraMode = DEFAULT;

		//return;
	}

	_vector vCamerPos = Calculate_CameraPos(_fTimeDelta);
	m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vCamerPos, 1.f));

	Look_Target(_fTimeDelta);
}

void CCamera_2D::Look_Target(_float fTimeDelta)
{
	_vector vTargetPos = XMVectorSetW(XMLoadFloat3(&m_v2DPreTargetWorldPos), 1.f);

	_vector vAt = vTargetPos + XMLoadFloat3(&m_vAtOffset) + XMLoadFloat3(&m_vShakeOffset);

	m_pControllerTransform->LookAt_3D(XMVectorSetW(vAt, 1.f));
}

_vector CCamera_2D::Calculate_CameraPos(_float _fTimeDelta)
{	
	if(true == XMVector3Equal(XMVectorSet(0.f, 0.f, 0.f, 0.f), XMLoadFloat3(&m_v2DTargetWorldPos))) {
		// 0 0 0이면 WorldPos에 넣기
		m_v2DTargetWorldPos = m_v2DFixedPos;
	}

	_vector vDistance = XMLoadFloat3(&m_v2DTargetWorldPos) - XMLoadFloat3(&m_v2DPreTargetWorldPos);
	_float fSpeed = XMVectorGetX(XMVector3Length(vDistance)) / m_fTrackingTime;

	_vector vCurPos = XMLoadFloat3(&m_v2DPreTargetWorldPos) + (XMVector3Normalize(vDistance) * fSpeed * _fTimeDelta);

	_vector vCameraPos = vCurPos + (m_pCurArm->Get_Length() * m_pCurArm->Get_ArmVector());

	XMStoreFloat3(&m_v2DPreTargetWorldPos, vCurPos);
	m_v2DFixedPos = m_v2DTargetWorldPos;

	return vCameraPos;
	
	/*if (true == m_isBook) {
		vTargetPos = XMVectorSetY(vTargetPos, m_fFixedY);
	}*/

	//NORMAL_DIRECTION::POSITIVE_X >>> normal 판정 enum >>> 사용시 주의 점은 반올림함수를 사용하면 안정성이 오르니 반올림 함수 사용하삼.

	//if(true == XMVector3Equal(XMVectorSet(0.f, 0.f, 0.f, 0.f), XMLoadFloat3(&m_v2DTargetWorldPos))) {
	//	_vector vCurPos = XMVectorLerp(XMLoadFloat3(&m_v2DPreTargetWorldPos), XMLoadFloat3(&m_v2DTargetWorldPos), 0.05f);

	//	_vector vCameraPos = vCurPos + (m_pCurArm->Get_Length() * m_pCurArm->Get_ArmVector());
	//	XMStoreFloat3(&m_v2DPreTargetWorldPos, vCurPos);
	//	
	//	return vCameraPos;
	//}

	//_vector vCurPos = XMVectorLerp(XMVectorSetW(XMLoadFloat3(&m_v2DPreTargetWorldPos), 1.f), vTargetPos, 0.05f);

	//_vector vCameraPos = vCurPos + (m_pCurArm->Get_Length() * m_pCurArm->Get_ArmVector());
	//XMStoreFloat3(&m_v2DPreTargetWorldPos, vCurPos);
	//XMStoreFloat3(&m_v2DTargetWorldPos, vTargetPos);
}

void CCamera_2D::Switching(_float _fTimeDelta)
{
	if (false == m_isInitialData)
		return;

	_float fRatio = Calculate_Ratio(&m_InitialTime, _fTimeDelta, EASE_IN);

	if (fRatio >= (1.f - EPSILON)) {
		_vector vTargetPos = CSection_Manager::GetInstance()->Get_WorldPosition_FromWorldPosMap(m_strSectionName,{ m_pTargetWorldMatrix->_41, m_pTargetWorldMatrix->_42 });
		_vector vCameraPos = vTargetPos + (m_pCurArm->Get_Length() * m_pCurArm->Get_ArmVector());
		m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vCameraPos, 1.f));

		m_fFixedY = XMVectorGetY(vTargetPos);

		_vector vLookAt = vTargetPos + XMLoadFloat3(&m_vAtOffset) + XMLoadFloat3(&m_vShakeOffset);
		m_pControllerTransform->LookAt_3D(XMVectorSetW(vLookAt, 1.f));

		m_fFovy = m_ZoomLevels[m_iCurZoomLevel];

		XMStoreFloat3(&m_v2DPreTargetWorldPos, vTargetPos);
		XMStoreFloat3(&m_v2DFixedPos, vTargetPos);

		m_InitialTime.y = 0.f;
		m_isInitialData = false;

		m_eCameraMode = DEFAULT;
		return;
	}

	// Pos
	_vector vTargetPos = CSection_Manager::GetInstance()->Get_WorldPosition_FromWorldPosMap(m_strSectionName,{ m_pTargetWorldMatrix->_41, m_pTargetWorldMatrix->_42 });

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

void CCamera_2D::Find_TargetPos()
{
	// Target 위치 구하기
	switch (m_eTargetCoordinate) {
	case (_uint)COORDINATE_2D:
	{
		_vector vTargetPos = CSection_Manager::GetInstance()->Get_WorldPosition_FromWorldPosMap(m_strSectionName, { m_pTargetWorldMatrix->_41, m_pTargetWorldMatrix->_42 });
		XMStoreFloat3(&m_v2DTargetWorldPos, vTargetPos);

		if (m_eCurSpaceDir != (NORMAL_DIRECTION)((_int)roundf(XMVectorGetW(vTargetPos)))) {

			switch (((_int)roundf(XMVectorGetW(vTargetPos)))) {
			case (_int)NORMAL_DIRECTION::POSITIVE_X:
			{
				Set_NextArmData(TEXT("Default_Positive_X"), 0);
			}
			break;
			case (_int)NORMAL_DIRECTION::NEGATIVE_X:
			{
				Set_NextArmData(TEXT("Default_Negative_X"), 0);
			}
			break;
			case (_int)NORMAL_DIRECTION::POSITIVE_Y:
			{
				Set_NextArmData(TEXT("Default_Positive_Y"), 0);
			}
			break;
			case (_int)NORMAL_DIRECTION::NEGATIVE_Y:
				break;
			case (_int)NORMAL_DIRECTION::POSITIVE_Z:
				break;
			case (_int)NORMAL_DIRECTION::NEGATIVE_Z:
			{
				Set_NextArmData(TEXT("Default_Negative_Z"), 0);
			}
			break;
			}

			m_eCameraMode = MOVE_TO_NEXTARM;
			m_eCurSpaceDir = (NORMAL_DIRECTION)((_int)roundf(XMVectorGetW(vTargetPos)));
		}
	}
	break;
	case (_uint)COORDINATE_3D:
	{
		memcpy(&m_v2DTargetWorldPos, m_pTargetWorldMatrix->m[3], sizeof(_float3));
	}
	break;
	}
}

pair<ARM_DATA*, SUB_DATA*>* CCamera_2D::Find_ArmData(_wstring _wszArmTag)
{
	auto iter = m_ArmDatas.find(_wszArmTag);

	if (iter == m_ArmDatas.end())
		return nullptr;

	return &(iter->second);
}

#ifdef _DEBUG
void CCamera_2D::Key_Input(_float _fTimeDelta)
{
	//_float fLength = m_pCurArm->Get_Length();
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
	for (auto& ArmData : m_ArmDatas) {
		Safe_Delete(ArmData.second.first);
		Safe_Delete(ArmData.second.second);
	}
	m_ArmDatas.clear();

	Safe_Release(m_pCurArm);

	__super::Free();
}
