#include "stdafx.h"
#include "Camera_2D.h"

#include "GameInstance.h"
#include "Section_Manager.h"
#include "Trigger_Manager.h"
#include "SampleBook.h"
#include "Effect_Manager.h"
#include "UI_Manager.h"
#include "Target_Manager.h"

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

	m_fBasicRatio[HORIZON_NON_SCALE] = { 0.25f, 0.f };
	m_fBasicRatio[HORIZON_SCALE] = { 0.1f, 0.1f };
	m_fBasicRatio[VERTICAL_NONE_SCALE] = { 0.f, 0.05f };
	m_fBasicRatio[VERTICAL_SCALE] = { 0.f, 0.1f };

	m_eMagnificationType = HORIZON_NON_SCALE;
	m_iFreezeMask |= FREEZE_Z;

	// TargetChangineTime이 Camera에 있는데 여기서는 TrackingTime으로 쫓아가는 게 나을 것 같기도


	if (FAILED(Create_NormalCopyTexture()))
		return E_FAIL;

	return S_OK;
}

void CCamera_2D::Priority_Update(_float fTimeDelta)
{
	//m_fA = { m_pTargetWorldMatrix->_41,  m_pTargetWorldMatrix->_42 };
}

void CCamera_2D::Update(_float fTimeDelta)
{
}

void CCamera_2D::Late_Update(_float fTimeDelta)
{
	Key_Input(fTimeDelta);
#ifdef _DEBUG
	//Imgui(fTimeDelta);
#endif

	Switching(fTimeDelta);

	Action_SetUp_ByMode();
	Action_Mode(fTimeDelta);

	__super::Late_Update(fTimeDelta);
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

void CCamera_2D::Add_CustomArm(ARM_DATA _tArmData)
{
	m_CustomArmData = _tArmData;
	m_pCurArm->Set_StartInfo();
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
	if (CSection_Manager::GetInstance()->is_WordPos_Capcher()) 
	{
		CSection* pSection = CSection_Manager::GetInstance()->Find_Section(m_strSectionName);
		m_iPlayType = static_cast<CSection_2D*>(pSection)->Get_Section_2D_PlayType();

#pragma region Narration || Book
		if (CSection_2D::NARRAION == m_iPlayType || CSection_2D::PLAYMAP == m_iPlayType) { // 일단 Narration이랑 Book이랑 같이 처리
			pair<ARM_DATA*, SUB_DATA*>* pData = nullptr;
			pData = Find_ArmData(TEXT("Book_Horizon"));

			m_pCurArm->Set_ArmVector(XMLoadFloat3(&pData->first->vDesireArm));
		}
#pragma endregion

#pragma region Sketch Space
		else if (CSection_2D::SKSP == m_iPlayType) {

			_uint iNormalType = static_cast<CSection_2D*>(pSection)->Get_Override_Normal();

			// Sketch Space의 Normal Type에 따라 Arm 결정하기
			/* 기존 Normal*/
			if (ARM_NORMAL_TYPE::DEFAULT_NORMAL == iNormalType) {
				_vector vTargetPos = CSection_Manager::GetInstance()->Get_WorldPosition_FromWorldPosMap(m_strSectionName, { m_pTargetWorldMatrix->_41, m_pTargetWorldMatrix->_42 });

				m_eCurSpaceDir = (NORMAL_DIRECTION)((_int)roundf(XMVectorGetW(vTargetPos)));
				pair<ARM_DATA*, SUB_DATA*>* pData = nullptr;

				switch ((_int)m_eCurSpaceDir) {
				case (_int)NORMAL_DIRECTION::NONEWRITE_NORMAL:
				{
					int a = 0;
				}
				break;
				case (_int)NORMAL_DIRECTION::POSITIVE_X:
				{
					pData = Find_ArmData(TEXT("Default_Positive_X"));
				}
				break;
				case (_int)NORMAL_DIRECTION::NEGATIVE_X:
				{
					pData = Find_ArmData(TEXT("Default_Negative_X"));
				}
				break;
				case (_int)NORMAL_DIRECTION::POSITIVE_Y:
				{
					pData = Find_ArmData(TEXT("Default_Positive_Y"));
				}
				break;
				case (_int)NORMAL_DIRECTION::NEGATIVE_Y:
					break;
				case (_int)NORMAL_DIRECTION::POSITIVE_Z:
					break;
				case (_int)NORMAL_DIRECTION::NEGATIVE_Z:
				{
					pData = Find_ArmData(TEXT("Default_Negative_Z"));
				}
				break;
				}
				if (nullptr != pData)
					Set_InitialData(pData);
			}
			else if (ARM_NORMAL_TYPE::CUSTOM_NORMAL == iNormalType){
				Set_InitialData(m_strSectionName);
			}
			else if (ARM_NORMAL_TYPE::NORMAL_MAP == iNormalType) {	
				if(FAILED(m_pGameInstance->Copy_RT_Resource(TEXT("Target_Normal"), m_pCopyNormalMap)))
					MSG_BOX("Copy Failed");
				_vector vTargetPos = CSection_Manager::GetInstance()->Get_WorldPosition_FromWorldPosMap(m_strSectionName, { m_pTargetWorldMatrix->_41, m_pTargetWorldMatrix->_42 });
				_vector vNormal = {};
				
				_matrix matResult = m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);
				matResult = matResult * m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);

				vTargetPos = XMVector3TransformCoord(vTargetPos, matResult);

				_int iPixelX = (_int)((XMVectorGetX(vTargetPos) + 1.f) * (g_iWinSizeX * 0.5f));
				_int iPixelY = (_int)((1.f - XMVectorGetY(vTargetPos)) * (g_iWinSizeY * 0.5f));

				D3D11_MAPPED_SUBRESOURCE mappedResource;
				if (FAILED(m_pContext->Map(m_pCopyNormalMap, 0, D3D11_MAP_READ, 0, &mappedResource)))
					MSG_BOX("Normal Map Copy Failed");

				//_int iWidth = mappedResource.RowPitch / sizeof(uint16_t) / 4;
				//_int iHeight = mappedResource.DepthPitch / mappedResource.RowPitch;

				// RowPitch는 한 줄의 바이트 수를 나타냄
				uint16_t* fData = static_cast<uint16_t*>(mappedResource.pData);

				// 픽셀 위치 계산 (4 floats per pixel)
				_int rowPitchInPixels = mappedResource.RowPitch / (sizeof(uint16_t) * 4);
				_int iIndex = iPixelY * rowPitchInPixels + iPixelX;

			/*	if (iWidth * iHeight <= iIndex || 0 > iIndex)
					vNormal = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);*/

				_uint iDefaultIndex = iIndex * 4;

				// float4 데이터 읽기
				_float x = fData[iDefaultIndex] / 65535.0f;; // Red 채널
				_float y = fData[iDefaultIndex + 1] / 65535.0f;; // Green 채널
				_float z = fData[iDefaultIndex + 2] / 65535.0f;; // Blue 채널
				_float w = fData[iDefaultIndex + 3] / 65535.0f;; // Alpha 채널

				m_pContext->Unmap(m_pCopyNormalMap, 0);

				vNormal = XMVectorSet(x, y, z, w);

				Set_InitialData(vNormal, 12.5f, XMVectorZero(), 5);
			}
		}
#pragma endregion

		// Initial Data가 없어서 TargetPos + vArm * Length로 초기 위치를 바로 잡아주기
		if (nullptr == _pInitialData) {
			_vector vTargetPos = CSection_Manager::GetInstance()->Get_WorldPosition_FromWorldPosMap(m_strSectionName,{ m_pTargetWorldMatrix->_41, m_pTargetWorldMatrix->_42 });
			_vector vCameraPos = vTargetPos + (m_pCurArm->Get_Length() * m_pCurArm->Get_ArmVector());
			XMStoreFloat3(&m_v2DPreTargetWorldPos, vTargetPos);
			XMStoreFloat3(&m_v2DFixedPos, vTargetPos);

			m_fFixedY = XMVectorGetY(vTargetPos);

			Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vCameraPos, 1.f));

			Look_Target(0.f);

			m_fFovy = m_ZoomLevels[m_iCurZoomLevel];

			m_isInitialData = false;
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
			// ㄹㅇ 3D의 초기 ZoomLevel을 그냥 아예 설정하고 있었음... 근데 이렇게 하면 내가 각 Sksp마다 fov를 조절할 수는 있지만
			// 같은 Positive Y인데 다른 경우엔 어캄? .. 걍 저렇게 하는 게 나을 수도
			//m_iCurZoomLevel = m_tInitialData.iZoomLevel;
			m_fFovy = m_ZoomLevels[m_tInitialData.iZoomLevel];
		}

		Check_MagnificationType();
	}
}

void CCamera_2D::Change_Target(CGameObject* _pTarget)
{
	m_pTargetWorldMatrix = _pTarget->Get_ControllerTransform()->Get_WorldMatrix_Ptr();
	m_eTargetCoordinate = _pTarget->Get_CurCoord();
	m_isTargetChanged = true;

	m_vStartPos = m_v2DPreTargetWorldPos;
}

INITIAL_DATA CCamera_2D::Get_InitialData()
{
	INITIAL_DATA tData;

	XMStoreFloat3(&tData.vPosition, m_pControllerTransform->Get_State(CTransform::STATE_POSITION));

	_vector vAt = XMVectorSetW(XMLoadFloat3(&m_v2DPreTargetWorldPos), 1.f) + XMLoadFloat3(&m_vAtOffset) + XMLoadFloat3(&m_vShakeOffset);
	XMStoreFloat3(&tData.vAt, vAt);

	tData.iZoomLevel = m_iCurZoomLevel;

	return tData;
}

void CCamera_2D::Set_InitialData(_fvector _vArm, _float _fLength, _fvector _vOffset, _uint _iZoomLevel)
{
	m_pCurArm->Set_ArmVector(_vArm);
	m_pCurArm->Set_Length(_fLength);
	XMStoreFloat3(&m_vAtOffset, _vOffset);
	m_iCurZoomLevel = { _iZoomLevel };
	m_fFovy = m_ZoomLevels[m_iCurZoomLevel];
}

void CCamera_2D::Set_InitialData(pair<ARM_DATA*, SUB_DATA*>* pData)
{
	m_pCurArm->Set_ArmVector(XMLoadFloat3(&pData->first->vDesireArm));
	m_pCurArm->Set_Length(pData->first->fLength);

	if (nullptr == pData->second)
		return;

	m_vAtOffset = pData->second->vAtOffset;
	m_iCurZoomLevel = pData->second->iZoomLevel;
	m_fFovy = m_ZoomLevels[m_iCurZoomLevel];
}

void CCamera_2D::Set_InitialData(_wstring _szSectionTag)
{
	// SectionTag에 따라서 미리 저장해 둔 Arm 불러오기
	pair<ARM_DATA*, SUB_DATA*>* pData = nullptr;

	if (TEXT("Chapter4_SKSP_02") == _szSectionTag) {
		pData = Find_ArmData(TEXT("Custom_Flag"));
	}
	else if (TEXT("Chapter4_SKSP_07") == _szSectionTag) {
		pData = Find_ArmData(TEXT("Custom_Stair"));
	}
	else if (TEXT("Chapter6_SKSP_05") == _szSectionTag) {
		switch (m_iPortalID) {
		case 0:
			break;
		case 1:
			break;
		case 2:
		{
			pData = Find_ArmData(TEXT("Custom_Box_2"));
		}
			break;
		}
	}

	if (nullptr != pData) 
		Set_InitialData(pData);
}

void CCamera_2D::Action_Mode(_float _fTimeDelta)
{
	if (true == m_isInitialData)
		return;

#pragma region Arm Debuging 코드
	//static _bool isDebug = true;
	//int a = 0;
	////isDebug = false;
	//if (false == isDebug) {
	//	ARM_DATA tData = {};
	//	tData.fMoveTimeAxisRight = { 1.5f, 0.f };
	//	tData.fRotationPerSecAxisRight = { XMConvertToRadians(-11.f), XMConvertToRadians(-1.f) };
	//	tData.iRotationRatioType = EASE_IN_OUT;
	//	tData.fLength = 9.f;
	//	tData.fLengthTime = { 1.5f, 0.f };
	//	tData.iLengthRatioType = EASE_IN_OUT;
	
	//	Add_CustomArm(tData);
	//	m_eCameraMode = MOVE_TO_CUSTOMARM;
	//	Start_Changing_AtOffset(1.5f, XMVectorSet(0.f, 0.f, -3.f, 0.f), EASE_IN_OUT);
	//	Start_Zoom(1.5f, (ZOOM_LEVEL)LEVEL_5, EASE_IN_OUT);
	//	//Start_Changing_AtOffset(1.5f, XMVectorSet(0.f, 0.f, 0.f, 0.f), EASE_IN_OUT);
	
	//	isDebug = true;
	//}
	
	/*Start_Zoom(1.5f, (ZOOM_LEVEL)LEVEL_6, EASE_IN_OUT);
	Start_Changing_AtOffset(1.5f, XMVectorSet(0.f, 0.f, 0.f, 0.f), EASE_IN_OUT);*/
#pragma endregion

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
	case MOVE_TO_CUSTOMARM:
		Move_To_CustomArm(_fTimeDelta);
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
	case NARRATION:
		Play_Narration(_fTimeDelta);
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
		case MOVE_TO_CUSTOMARM:
			break;
		case RETURN_TO_DEFUALT:
			break;
		case FLIPPING_UP:
		{	
			if (VERTICAL == m_eDirectionType) {
				m_eDirectionType = HORIZON;
				Start_Zoom(0.5f, (ZOOM_LEVEL)5, EASE_IN_OUT);
			}
			
			Set_NextArmData(TEXT("BookFlipping_Horizon"), 0);
			
			CGameObject* pBook = m_pGameInstance->Get_GameObject_Ptr(m_pGameInstance->Get_CurLevelID(), TEXT("Layer_Book"), 0);
			Change_Target(pBook);		
		}
			break;
		case FLIPPING_DOWN:
		{
			Check_MagnificationType();

			if (VERTICAL == m_eDirectionType) {
				Set_NextArmData(TEXT("Book_Vertical"), 0);
				// 지금은 종모드면 무조건 줌
				Start_Zoom(0.5f, (ZOOM_LEVEL)3, EASE_IN_OUT);
			}
			else {
				Set_NextArmData(TEXT("Book_Horizon"), 0);
			}

			// 객체 가지고 오기
			switch (m_iPlayType) {
			case CSection_2D::PLAYMAP:
			{
				CGameObject* pPlayer = m_pGameInstance->Get_GameObject_Ptr(m_pGameInstance->Get_CurLevelID(), TEXT("Layer_Player"), 0);
				Change_Target(pPlayer);
			}
				break;
			case CSection_2D::NARRAION:
			{
				m_eTargetCoordinate = COORDINATE_2D;
			}
				break;
			case CSection_2D::SKSP:
				break;
			}

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
		m_eCameraMode = CAMERA_2D_MODE::DEFAULT;
	}

	_vector vCamerPos = Calculate_CameraPos(_fTimeDelta);

	m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vCamerPos, 1.f));

	Look_Target(_fTimeDelta);
}

void CCamera_2D::Move_To_CustomArm(_float _fTimeDelta)
{
	if (true == m_pCurArm->Move_To_CustomArm(&m_CustomArmData, _fTimeDelta)) {
		m_eCameraMode = CAMERA_2D_MODE::DEFAULT;
	}

	_vector vCamerPos = Calculate_CameraPos(_fTimeDelta);

	m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vCamerPos, 1.f));

	Look_Target(_fTimeDelta);
}


void CCamera_2D::Return_To_Default(_float _fTimeDelta)
{
}

void CCamera_2D::Flipping_Up(_float _fTimeDelta)
{	
	if (true == m_pCurArm->Move_To_NextArm_ByVector(_fTimeDelta, true)) {
		m_eCameraMode = FLIPPING_PAUSE;
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
		
		if (CSection_2D::NARRAION == m_iPlayType)
			m_eCameraMode = NARRATION;
		else
			m_eCameraMode = CAMERA_2D_MODE::DEFAULT;
	}

	_vector vCamerPos = Calculate_CameraPos(_fTimeDelta);
	m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vCamerPos, 1.f));

	Look_Target(_fTimeDelta);
}

void CCamera_2D::Play_Narration(_float _fTimeDelta)
{
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

	// Book Scroll 계산
	Calculate_Book_Scroll();
	_vector vCurPos = {};

	// Target Change가 안 됐을 때는 시간 제한 없이 Smooth를 한다
	if (false == m_isTargetChanged) {
		// 좌표 계산
		_vector vDistance = XMLoadFloat3(&m_v2DTargetWorldPos) - XMLoadFloat3(&m_v2DPreTargetWorldPos);
		_float fSpeed = XMVectorGetX(XMVector3Length(vDistance)) / (m_fTrackingTime.x);
		vCurPos = XMLoadFloat3(&m_v2DPreTargetWorldPos) + (XMVector3Normalize(vDistance) * fSpeed * _fTimeDelta);
	}
	// Target Change일 땐 지정된 시간만큼 바뀐 Target으로 이동한다
	else {

		_float fRatio = m_pGameInstance->Calculate_Ratio(&m_fTrackingTime, _fTimeDelta, EASE_IN_OUT);

		if (fRatio >= (1.f - EPSILON)) {
			m_fTrackingTime.y = 0.f;
			m_isTargetChanged = false;
		}

		vCurPos = XMVectorLerp(XMLoadFloat3(&m_vStartPos), XMLoadFloat3(&m_v2DTargetWorldPos), fRatio);
		vCurPos = XMVectorSetW(vCurPos, 1.f);
	}
	
	_vector vCameraPos = vCurPos + (m_pCurArm->Get_Length() * m_pCurArm->Get_ArmVector());

	XMStoreFloat3(&m_v2DPreTargetWorldPos, vCurPos);
	m_v2DFixedPos = m_v2DTargetWorldPos;

	return vCameraPos;
	
	/*if (true == m_isBook) {
		vTargetPos = XMVectorSetY(vTargetPos, m_fFixedY);
	}*/
}

void CCamera_2D::Switching(_float _fTimeDelta)
{
	if (false == m_isInitialData)
		return;

	_float fRatio = Calculate_Ratio(&m_InitialTime, _fTimeDelta, EASE_IN_OUT);

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

		m_eCameraMode = CAMERA_2D_MODE::DEFAULT;
		return;
	}

	// Pos
	_vector vTargetPos = CSection_Manager::GetInstance()->Get_WorldPosition_FromWorldPosMap(m_strSectionName,{ m_pTargetWorldMatrix->_41, m_pTargetWorldMatrix->_42 });
	if (true == XMVector3Equal(vTargetPos, XMVectorZero())) {
		vTargetPos = XMLoadFloat3(&m_v2DPreTargetWorldPos);
	}

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

	XMStoreFloat3(&m_v2DPreTargetWorldPos, vTargetPos);
}

void CCamera_2D::Find_TargetPos()
{
	// Target 위치 구하기
	switch (m_eTargetCoordinate) {
	case (_uint)COORDINATE_2D:
	{
#pragma region Book
		if (CSection_2D::PLAYMAP == m_iPlayType) {

			if (false == Is_Target_In_SketchSpace())
				break;
			
			_vector vTargetPos = CSection_Manager::GetInstance()->Get_WorldPosition_FromWorldPosMap(m_strSectionName, { m_pTargetWorldMatrix->_41, m_pTargetWorldMatrix->_42 });

			if (!XMVector3Equal(vTargetPos, XMVectorZero())) {
				XMStoreFloat3(&m_v2DTargetWorldPos, vTargetPos);
				m_v2DdMatrixPos = { m_pTargetWorldMatrix->_41, m_pTargetWorldMatrix->_42 };
			}
			else
				break;
		}
#pragma endregion

#pragma region Sketch Space
		else if (CSection_2D::SKSP == m_iPlayType) {

			if (false == Is_Target_In_SketchSpace())
				break;

			_vector vTargetPos = CSection_Manager::GetInstance()->Get_WorldPosition_FromWorldPosMap(m_strSectionName, { m_pTargetWorldMatrix->_41, m_pTargetWorldMatrix->_42 });

			if (!XMVector3Equal(vTargetPos, XMVectorZero())) {
				XMStoreFloat3(&m_v2DTargetWorldPos, vTargetPos);
				m_v2DdMatrixPos = { m_pTargetWorldMatrix->_41, m_pTargetWorldMatrix->_42 };
			}
			else
				break;

			// Sketch Space의 Normal Type에 따라 Arm 결정하기
			CSection* pSection = CSection_Manager::GetInstance()->Find_Section(m_strSectionName);
			_uint iNormalType = static_cast<CSection_2D*>(pSection)->Get_Override_Normal();

			/* 기존 Normal*/
			if (ARM_NORMAL_TYPE::DEFAULT_NORMAL == iNormalType) {
				if (m_eCurSpaceDir != (NORMAL_DIRECTION)((_int)roundf(XMVectorGetW(vTargetPos)))) {

					switch (((_int)roundf(XMVectorGetW(vTargetPos)))) {
					case (_int)NORMAL_DIRECTION::NONEWRITE_NORMAL:
					{
						//Set_NextArmData(TEXT("Default_Positive_X"), 0);
					}
					break;
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
						if (NORMAL_DIRECTION::NEGATIVE_Z == m_eCurSpaceDir)
							Set_NextArmData(TEXT("Default_Positive_Y"), 0);
						else if (NORMAL_DIRECTION::NEGATIVE_X == m_eCurSpaceDir)
							Set_NextArmData(TEXT("Default_Positive_Y_Left"), 0);
						else if (NORMAL_DIRECTION::POSITIVE_X == m_eCurSpaceDir)
							Set_NextArmData(TEXT("Default_Positive_Y_Right"), 0);
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

			/* Custom Normal*/
			else if (ARM_NORMAL_TYPE::CUSTOM_NORMAL == iNormalType) {
			}

			/* Normal Map */
			else if (ARM_NORMAL_TYPE::NORMAL_MAP == iNormalType) {
			}
		}
#pragma endregion

#pragma region Narration
		else if (CSection_2D::NARRAION == m_iPlayType) {
			_float2 fSectionSize = CSection_Manager::GetInstance()->Get_Section_RenderTarget_Size(m_strSectionName);
			_float2 vPos = { };

			if (true != m_iNarrationPosType) {// 임시 처리.....
				m_isTargetChanged = true;
				m_vStartPos = m_v2DPreTargetWorldPos;
			}

			m_iNarrationPosType = false; //Uimgr->isLeft_Right();	// true면 left

			if (true == m_iNarrationPosType)
				vPos = { fSectionSize.x * 0.25f, fSectionSize.y * 0.5f };
			else
				vPos = { fSectionSize.x * (1.f - 0.25f), fSectionSize.y * 0.5f };

			vPos = { (vPos.x - (fSectionSize.x * 0.5f)), -vPos.y + (fSectionSize.y * 0.5f) };
			_vector vTargetPos = CSection_Manager::GetInstance()->Get_WorldPosition_FromWorldPosMap(m_strSectionName, { vPos.x, vPos.y });

			XMStoreFloat3(&m_v2DTargetWorldPos, vTargetPos);
		}
#pragma endregion
	}
	break;
	case (_uint)COORDINATE_3D:
	{
		memcpy(&m_v2DTargetWorldPos, m_pTargetWorldMatrix->m[3], sizeof(_float3));
		m_iNarrationPosType = false; // 임시 처리.....
	}
	break;
	}
}

void CCamera_2D::Calculate_Book_Scroll()
{
	if (CSection_2D::PLAYMAP != m_iPlayType || COORDINATE_3D == m_eTargetCoordinate)
		return;

	_float2 fSectionSize = CSection_Manager::GetInstance()->Get_Section_RenderTarget_Size(m_strSectionName);

#pragma region Freeze 처리 1
	// Freeze 처리
	//_float2 vTargetUV = { m_pTargetWorldMatrix->_41 + (fSectionSize.x * 0.5f), -(m_pTargetWorldMatrix->_42 - (fSectionSize.y * 0.5f)) };

	//if (vTargetUV.x <= (fSectionSize.x * m_fBasicRatio[m_eMagnificationType].x) ||
	//	vTargetUV.x >= (fSectionSize.x * (1.f - m_fBasicRatio[m_eMagnificationType].x))) {
	//	m_v2DTargetWorldPos.x = m_v2DFixedPos.x;
	//}

	//if (vTargetUV.y <= (fSectionSize.y * m_fBasicRatio[m_eMagnificationType].y) ||
	//	vTargetUV.y >= (fSectionSize.y * (1.f - m_fBasicRatio[m_eMagnificationType].y))) {
	//	m_v2DTargetWorldPos.z = m_v2DFixedPos.z;
	//}

	//if (FREEZE_X == (m_iFreezeMask & FREEZE_X)) {
	//	m_v2DTargetWorldPos.x = m_v2DFixedPos.x;
	//}
	//if (FREEZE_Z == (m_iFreezeMask & FREEZE_Z)) {
	//	m_v2DTargetWorldPos.z = m_v2DFixedPos.z;
	//}
#pragma endregion

#pragma region 처리2

	//_float2 vSectionWorld = { fSectionSize.x - (fSectionSize.x * 0.5f), -fSectionSize.y + (fSectionSize.y * 0.5f) };

	//if (m_v2DdMatrixPos.x <= (vSectionWorld.x * m_fBasicRatio[m_eMagnificationType].x)) {
	//	m_v2DdMatrixPos.x = vSectionWorld.x * m_fBasicRatio[m_eMagnificationType].x;
	//}
	//else if (m_v2DdMatrixPos.x >= (vSectionWorld.x * (1.f - m_fBasicRatio[m_eMagnificationType].x))) {
	//	m_v2DdMatrixPos.x = vSectionWorld.x * (1.f - m_fBasicRatio[m_eMagnificationType].x);
	//}

	//if (m_v2DdMatrixPos.y <= (vSectionWorld.y * m_fBasicRatio[m_eMagnificationType].y)) {
	//	m_v2DdMatrixPos.y = vSectionWorld.y * m_fBasicRatio[m_eMagnificationType].y;
	//}
	//else if (m_v2DdMatrixPos.y >= (vSectionWorld.y * (1.f - m_fBasicRatio[m_eMagnificationType].y))) {
	//	m_v2DdMatrixPos.y = vSectionWorld.y * (1.f - m_fBasicRatio[m_eMagnificationType].y);
	//}


	//if (FREEZE_X == (m_iFreezeMask & FREEZE_X)) {
	//	m_v2DdMatrixPos.x = fSectionSize.x * 0.5f;
	//}
	//if (FREEZE_Z == (m_iFreezeMask & FREEZE_Z)) {
	//	m_v2DdMatrixPos.y = fSectionSize.y * 0.5f;
	//}
#pragma endregion
	_float2 vTargetPixelPos = { m_v2DdMatrixPos.x + (fSectionSize.x * 0.5f), -m_v2DdMatrixPos.y + (fSectionSize.y * 0.5f) };


	if (vTargetPixelPos.x <= (fSectionSize.x * m_fBasicRatio[m_eMagnificationType].x)) {
		vTargetPixelPos.x = fSectionSize.x * m_fBasicRatio[m_eMagnificationType].x;
	}
	else if (vTargetPixelPos.x >= (fSectionSize.x * (1.f - m_fBasicRatio[m_eMagnificationType].x))) {
		vTargetPixelPos.x = fSectionSize.x * (1.f - m_fBasicRatio[m_eMagnificationType].x);
	}
	
	if (vTargetPixelPos.y <= (fSectionSize.y * m_fBasicRatio[m_eMagnificationType].y)) {
		vTargetPixelPos.y = fSectionSize.y * m_fBasicRatio[m_eMagnificationType].y;
	}
	else if (vTargetPixelPos.y >= (fSectionSize.y * (1.f - m_fBasicRatio[m_eMagnificationType].y))) {
		vTargetPixelPos.y = fSectionSize.y * (1.f - m_fBasicRatio[m_eMagnificationType].y);
	}

	if (FREEZE_X == (m_iFreezeMask & FREEZE_X)) {
		vTargetPixelPos.x = fSectionSize.x * 0.5f;
	}
	if (FREEZE_Z == (m_iFreezeMask & FREEZE_Z)) {
		vTargetPixelPos.y = fSectionSize.y * 0.5f;
	}

	_float2 v2DTargetWorld = { vTargetPixelPos.x - (fSectionSize.x * 0.5f), (fSectionSize.y * 0.5f) - vTargetPixelPos.y };
	_vector vTargetPos = CSection_Manager::GetInstance()->Get_WorldPosition_FromWorldPosMap(m_strSectionName, { v2DTargetWorld.x, v2DTargetWorld.y });
	
	if (XMVector3Equal(vTargetPos, XMVectorZero())) {
		int a = 0;
	}

	XMStoreFloat3(&m_v2DTargetWorldPos, vTargetPos);
}

void CCamera_2D::Check_MagnificationType()
{
	if (CSection_2D::PLAYMAP != m_iPlayType)
		return;

	CSection* pSection = CSection_Manager::GetInstance()->Find_Section(m_strSectionName);

	// 종
	if (true == static_cast<CSection_2D*>(pSection)->Is_Rotation()) {
		// 배율
		if (1.f == 1.f) {
			m_eMagnificationType = VERTICAL_NONE_SCALE;
			m_iFreezeMask = FREEZE_X;
		}
		else {
			m_eMagnificationType = VERTICAL_SCALE;
			m_iFreezeMask = NONE;
		}
		m_eDirectionType = VERTICAL;
	}
	// 횡
	else {
		// 배율

		_float fRatio = static_cast<CSection_2D*>(pSection)->Get_CameraRatio();

		if (1.f == 1.f) {
			m_eMagnificationType = HORIZON_NON_SCALE;
			m_iFreezeMask = FREEZE_Z;
		}
		else {
			m_eMagnificationType = HORIZON_SCALE;
			m_iFreezeMask = NONE;
		}
		m_eDirectionType = HORIZON;
	}
}

_bool CCamera_2D::Is_Target_In_SketchSpace()
{
	_float2 fSectionSize = CSection_Manager::GetInstance()->Get_Section_RenderTarget_Size(m_strSectionName);
	_float2 fTargetPos = {};

	memcpy(&fTargetPos, m_pTargetWorldMatrix->m[3], sizeof(_float2));

	fTargetPos = { fTargetPos.x + (fSectionSize.x * 0.5f), -(fTargetPos.y - (fSectionSize.y * 0.5f)) };

	if (fTargetPos.x <= 0.f || fSectionSize.x <= fTargetPos.x ||
		fTargetPos.y <= 0.f || fSectionSize.y <= fTargetPos.y) {
		return false;
	}

	return true;
}

HRESULT CCamera_2D::Create_NormalCopyTexture()
{
	m_pCopyNormalMap = nullptr;

	D3D11_TEXTURE2D_DESC	desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));

	desc.Width = (_uint)g_iWinSizeX; // 원본 텍스처 너비
	desc.Height = (_uint)g_iWinSizeY; // 원본 텍스처 높이
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R16G16B16A16_UNORM; // 원본 텍스처와 동일한 포맷
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_STAGING; // CPU 읽기 전용
	desc.BindFlags = 0; // 바인딩 없음
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	m_pDevice->CreateTexture2D(&desc, nullptr, &m_pCopyNormalMap);

	return S_OK;
}

pair<ARM_DATA*, SUB_DATA*>* CCamera_2D::Find_ArmData(_wstring _wszArmTag)
{
	auto iter = m_ArmDatas.find(_wszArmTag);

	if (iter == m_ArmDatas.end())
		return nullptr;

	return &(iter->second);
}


void CCamera_2D::Key_Input(_float _fTimeDelta)
{
	_long		MouseMove = {};
	_vector		fRotation = {};

	if (KEY_PRESSING(KEY::CTRL)) {
		if (MOUSE_PRESSING(MOUSE_KEY::RB)) {
			if (MouseMove = MOUSE_MOVE(MOUSE_AXIS::Y))
			{
				fRotation = XMVectorSetX(fRotation, MouseMove * _fTimeDelta * -0.3f);
			}

			m_pCurArm->Set_Rotation(fRotation);
		}
	}
	else if (KEY_PRESSING(KEY::TAB)) {

		//if (KEY_PRESSING(KEY::LSHIFT))
		//	return;
		if (MOUSE_PRESSING(MOUSE_KEY::RB)) {
			if (MouseMove = MOUSE_MOVE(MOUSE_AXIS::X))
			{
				fRotation = XMVectorSetY(fRotation, MouseMove * _fTimeDelta * 0.3f);

			}
		}

		m_pCurArm->Set_Rotation(fRotation);
	}
	else if (MOUSE_PRESSING(MOUSE_KEY::RB)) {

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

	_float fArmLength = {};
	fArmLength = m_pCurArm->Get_Length();

	if (KEY_PRESSING(KEY::CTRL)) {
		if (KEY_DOWN(KEY::K)) {
			fArmLength += 0.1;
			m_pCurArm->Set_Length(fArmLength);
		}
		else if (KEY_DOWN(KEY::J)) {
			fArmLength -= 0.1;
			m_pCurArm->Set_Length(fArmLength);
		}
	}

	if (KEY_PRESSING(KEY::TAB)) {
		if (KEY_DOWN(KEY::K)) {
			m_vAtOffset.y += 0.1f;
		}
		else if (KEY_DOWN(KEY::J)) {
			m_vAtOffset.y -= 0.1f;
		}
	}
}
#ifdef _DEBUG
void CCamera_2D::Imgui(_float _fTimeDelta)
{
	ImGui::Begin("Arm");

	_float3 vCurArm = {};
	XMStoreFloat3(&vCurArm, m_pCurArm->Get_ArmVector());
	_float fArmLength = m_pCurArm->Get_Length();

	ImGui::SameLine();
	if (ImGui::Button("- Length") || ImGui::IsItemActive()) {// 누르고 있는 동안 계속 동작
		fArmLength -= 0.1;
		m_pCurArm->Set_Length(fArmLength);
	}
	ImGui::SameLine();
	if (ImGui::Button("+ Length") || ImGui::IsItemActive()) {
		fArmLength += 0.1;
		m_pCurArm->Set_Length(fArmLength);
	}

	fArmLength = m_pCurArm->Get_Length();
	ImGui::Text("Cur Arm: %.4f, %.4f, %.4f", vCurArm.x, vCurArm.y, vCurArm.z);

	ImGui::Text("Cur Length: %.2f", fArmLength);
	ImGui::Text("Cur ZoomLevel: %d", m_iCurZoomLevel);
	ImGui::Text("Cur AtOffset: %.4f, %.4f, %.4f", m_vAtOffset.x, m_vAtOffset.y, m_vAtOffset.z);

	ImGui::NewLine();

	static _float3 vInputArm = {};
	static _float fInputLength = {};
	static _float3 vInputAtOffset = {};

	ImGui::Text("Desire Arm: %.2f, %.2f, %.2f", vInputArm.x, vInputArm.y, vInputArm.z);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##DesireX", &vInputArm.x);
	ImGui::SameLine(0, 10.0f);

	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##DesireY", &vInputArm.y);
	ImGui::SameLine(0, 10.0f);

	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##DesireZ", &vInputArm.z);

	ImGui::SameLine();

	if (ImGui::Button("Set Arm")) {
		m_pCurArm->Set_ArmVector(XMLoadFloat3(&vInputArm));
	}

	ImGui::Text("Desire Length: %.2f         ", fInputLength);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##DesireLength", &fInputLength);

	ImGui::SameLine();

	if (ImGui::Button("Set Length")) {
		m_pCurArm->Set_Length(fInputLength);
	}

	ImGui::Text("Set AtOffset: %.2f, %.2f, %.2f", vInputAtOffset.x, vInputAtOffset.y, vInputAtOffset.z);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##ResetAtOffsetX", &vInputAtOffset.x);
	ImGui::SameLine(0, 10.0f);

	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##ResetAtOffsetY", &vInputAtOffset.y);
	ImGui::SameLine(0, 10.0f);

	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##ResetAtOffsetZ", &vInputAtOffset.z);

	ImGui::SameLine();

	if (ImGui::Button("Set AtOffset")) {
		m_vAtOffset = vInputAtOffset;
	}

	if (ImGui::Button("Set CurArm To InputArm")) {
		vInputArm = vCurArm;
	}

	ImGui::End();
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
	Safe_Release(m_pCopyNormalMap);

	__super::Free();
}
