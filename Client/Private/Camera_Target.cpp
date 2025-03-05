#include "stdafx.h"
#include "Camera_Target.h"

#include "GameInstance.h"

#include "Trigger_Manager.h"
#include "Section_Manager.h"

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
	
	m_eTargetCoordinate = { COORDINATE_3D };

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

	Switching(fTimeDelta);

	Action_SetUp_ByMode();
	Action_Mode(fTimeDelta);

	__super::Late_Update(fTimeDelta);
}

#pragma region  Tool용
pair<ARM_DATA*, SUB_DATA*>* CCamera_Target::Get_ArmData(_wstring _wszArmName)
{
	pair<ARM_DATA*, SUB_DATA*>* pData = Find_ArmData(_wszArmName);

	if (nullptr == pData)
		return nullptr;

	return pData;
}

void CCamera_Target::Get_ArmNames(vector<_wstring>* _vecArmNames)
{
	_vecArmNames->clear();

	for (auto& ArmName : m_ArmDatas) {
		_vecArmNames->push_back(ArmName.first);
	}
}
#pragma endregion

void CCamera_Target::Add_CurArm(CCameraArm* _pCameraArm)
{
	if (nullptr == _pCameraArm)
		return;

	m_pCurArm = _pCameraArm;
}

void CCamera_Target::Add_ArmData(_wstring _wszArmTag, ARM_DATA* _pArmData, SUB_DATA* _pSubData)
{
	if (nullptr == Find_ArmData(_wszArmTag))
		m_ArmDatas.emplace(_wszArmTag, make_pair(_pArmData, _pSubData));
	else {
		Safe_Delete(_pArmData);
		Safe_Delete(_pSubData);
	}
}

void CCamera_Target::Add_CustomArm(ARM_DATA _tArmData)
{
	m_CustomArmData = _tArmData;
	m_pCurArm->Set_StartInfo();
}

void CCamera_Target::Set_FreezeEnter(_uint _iFreezeMask, _fvector _vExitArm, _int _iTriggerID)
{
	m_iFreezeMask |= _iFreezeMask;
	m_vFreezeEnterPos = m_vPreTargetPos;
	FREEZE_EXITDATA tExitData = {};
	XMStoreFloat3(&tExitData.vFreezeExitArm, _vExitArm);
	// 아직 Length는 가지고 오지 않았음
	// tExitData.fFreezeExitLength = ...;

	m_FreezeExitDatas.push_back({ tExitData , _iTriggerID });
}

void CCamera_Target::Set_FreezeExit(_uint _iFreezeMask, _int _iTriggerID)
{
	m_fFreezeOffsetTime.x = 1.2f;
	m_fFreezeExitReturnTime.x = 1.2f;
	m_iFreezeMask ^= _iFreezeMask;
	m_isFreezeExit = true;
	m_isFreezeOffsetReturn = true;
	m_isFreezeExitReturn = true;
	m_vStartFreezeOffset = m_vFreezeOffset;

    for (auto& iter = m_FreezeExitDatas.begin(); iter != m_FreezeExitDatas.end();) {
		if (_iTriggerID == (*iter).second) {
			m_vCurFreezeExitData = (*iter).first;
			iter = m_FreezeExitDatas.erase(iter);

			m_isFreezeExit = true;
			m_isFreezeExitReturn = true;
			m_fFreezeOffsetTime.y = 0.f;
			m_fFreezeExitReturnTime.y = 0.f;

			if(DEFAULT == m_eCameraMode )
				m_pCurArm->Set_StartInfo();
	/*		if (DEFAULT == m_eCameraMode) {
				_vector vPos = m_pControllerTransform->Get_State(CTransform::STATE_POSITION);
				_vector vDir = vPos - XMLoadFloat3(&m_vPreTargetPos);
				_float fLength = XMVectorGetX(XMVector3Length(vDir));

				m_pCurArm->Set_ArmVector(XMVector3Normalize(vDir));
				m_pCurArm->Set_Length(fLength);

				m_isFreezeExit = true;
				m_fFreezeExitTime.y = 0.f;
			}*/
		}
		else
			++iter;
	}
}

void CCamera_Target::Set_EnableLookAt(_bool _isEnableLookAt)
{
	if (true == _isEnableLookAt && false == m_isEnableLookAt) {
		// false이다가 m_isExitLookAt으로 돌아간 것
		m_isExitLookAt = true;
		m_fLookTime.y = 0.f;
		XMStoreFloat3(&m_vStartLookVector, m_pControllerTransform->Get_State(CTransform::STATE_LOOK));
	}

	m_isEnableLookAt = _isEnableLookAt;
}

void CCamera_Target::Change_Target(const _float4x4* _pTargetWorldMatrix)
{
	m_pTargetWorldMatrix = _pTargetWorldMatrix;
}

void CCamera_Target::Change_Target(CGameObject* _pTarget)
{
	m_pTargetWorldMatrix = _pTarget->Get_ControllerTransform()->Get_WorldMatrix_Ptr();
	m_eTargetCoordinate = _pTarget->Get_CurCoord();
	m_isTargetChanged = true;

	_vector vStartPos = XMVectorSetW(XMLoadFloat3(&m_vPreTargetPos) + XMLoadFloat3(&m_vFreezeOffset), 1.f);
	XMStoreFloat3(&m_vStartPos, vStartPos);

	m_szTargetSectionTag = _pTarget->Get_Include_Section_Name();
}

void CCamera_Target::Turn_AxisY(_float _fTimeDelta)
{
	if (false == m_isTurnAxisY)
		return;

	if (true == m_pCurArm->Turn_AxisY(&m_CustomArmData, _fTimeDelta)) {
		m_isTurnAxisY = false;
	}
}

void CCamera_Target::Turn_AxisRight(_float _fTimeDelta)
{
	if (false == m_isTurnAxisRight)
		return;

	if (true == m_pCurArm->Turn_AxisRight(&m_CustomArmData, _fTimeDelta)) {
		m_isTurnAxisRight = false;
	}
}

void CCamera_Target::Change_Length(_float _fTimeDelta)
{
	if (false == m_isChangingLength)
		return;

	if (true == m_pCurArm->Change_Length(&m_CustomArmData, _fTimeDelta)) {
		m_isChangingLength = false;
	}
}

void CCamera_Target::Start_ResetArm_To_SettingPoint(_float _fResetTime)
{
	m_pCurArm->Set_StartInfo();
	Start_Changing_AtOffset(_fResetTime, XMLoadFloat3(&m_ResetArmData.vAtOffset), EASE_IN);
	Start_Zoom(_fResetTime, (ZOOM_LEVEL)m_ResetArmData.iZoomLevel, EASE_IN);
	m_eCameraMode = RESET_TO_SETTINGPOINT;
	m_fResetTime = { _fResetTime, 0.f };
}

void CCamera_Target::Switch_CameraView(INITIAL_DATA* _pInitialData)
{
	if (nullptr == m_pCurArm)
		return;

	// Initial Data가 없어서 TargetPos + vArm * Length로 초기 위치를 바로 잡아주기
	if (nullptr == _pInitialData) {
		memcpy(&m_vPreTargetPos, m_pTargetWorldMatrix->m[3], sizeof(_float3));
		_vector vCameraPos = XMLoadFloat3(&m_vPreTargetPos) + (m_pCurArm->Get_Length() * m_pCurArm->Get_ArmVector());

		Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vCameraPos, 1.f));

		Look_Target(XMLoadFloat3(&m_vPreTargetPos) + XMLoadFloat3(&m_vAtOffset), 0.f);

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
		//m_iCurZoomLevel = m_tInitialData.iZoomLevel;
		m_fFovy = m_ZoomLevels[m_tInitialData.iZoomLevel];
	}
}

void CCamera_Target::Set_InitialData(_wstring _szSectionTag, _uint _iPortalIndex)
{
	// 2D -> 3D, Portal에 따라서 나갈 때 SectionTag에 따라서 3D Target 카메라의 초기값 정해 주기 
	m_isEnableLookAt = true;
	m_isFreezeExit = false;

	pair<ARM_DATA*, SUB_DATA*>* pData = nullptr;

	auto& iter = m_SkspInitialTags.find(_szSectionTag);

	if (iter == m_SkspInitialTags.end()) {
		Set_InitialData(XMVectorSet(0.f, 0.67f, -0.74f, 0.f), 7.f, XMVectorZero(), ZOOM_LEVEL::LEVEL_6);
		return;
	}
		
	pData = Find_ArmData((*iter).second[_iPortalIndex]);

	if (nullptr == pData)
		return;

	Set_InitialData(pData);
}

void CCamera_Target::Set_InitialData(pair<ARM_DATA*, SUB_DATA*>* pData)
{
	m_pCurArm->Set_ArmVector(XMLoadFloat3(&pData->first->vDesireArm));
	m_pCurArm->Set_Length(pData->first->fLength);

	if (nullptr == pData->second)
		return;

	m_vAtOffset = pData->second->vAtOffset;
	m_iCurZoomLevel = pData->second->iZoomLevel;
	m_fFovy = m_ZoomLevels[m_iCurZoomLevel];
}

void CCamera_Target::Set_InitialData(_fvector _vArm, _float _fLength, _fvector _vOffset, _uint _iZoomLevel)
{
	m_pCurArm->Set_ArmVector(_vArm);
	m_pCurArm->Set_Length(_fLength);
	XMStoreFloat3(&m_vAtOffset, _vOffset);
	m_iCurZoomLevel = _iZoomLevel;
	m_fFovy = m_ZoomLevels[m_iCurZoomLevel];
}

INITIAL_DATA CCamera_Target::Get_InitialData()
{
	INITIAL_DATA tData;

	XMStoreFloat3(&tData.vPosition, m_pControllerTransform->Get_State(CTransform::STATE_POSITION));

#pragma region Pos + Look
	//_vector vLook = m_pControllerTransform->Get_State(CTransform::STATE_LOOK);
	//_vector vPos = m_pControllerTransform->Get_State(CTransform::STATE_POSITION);
	//_vector vAt = vPos + vLook;
	//XMStoreFloat3(&tData.vAt, vAt);

	//tData.iZoomLevel = m_iCurZoomLevel;
#pragma endregion
	_vector vFreezeOffset = -XMLoadFloat3(&m_vPreFreezeOffset);
	_vector vAt = XMVectorSetW(XMLoadFloat3(&m_vPreTargetPos), 1.f) + vFreezeOffset + XMLoadFloat3(&m_vAtOffset) + XMLoadFloat3(&m_vShakeOffset);
	XMStoreFloat3(&tData.vAt, vAt);

	tData.iZoomLevel = m_iCurZoomLevel;

	if (false == m_isEnableLookAt) {
		_vector vLook = m_pControllerTransform->Get_State(CTransform::STATE_LOOK);
		_vector vPos = m_pControllerTransform->Get_State(CTransform::STATE_POSITION);
		_vector vResultAt = vPos + vLook;
		XMStoreFloat3(&tData.vAt, vResultAt);

	}

	m_vFreezeOffset = { 0.f, 0.f, 0.f };

	return tData;
}

_bool CCamera_Target::Set_NextArmData(_wstring _wszNextArmName, _int _iTriggerID)
{
	pair<ARM_DATA*, SUB_DATA*>* pData = Find_ArmData(_wszNextArmName);

	if (nullptr == pData)
		return false;

	if (nullptr == m_pCurArm)
		return false;

	// 만약 Switching 중에 NextArm Trigger에 닿았다면
	if (true == m_isInitialData) {
		m_pCurArm->Set_ArmVector(XMLoadFloat3(&pData->first->vDesireArm));
		m_pCurArm->Set_Length(pData->first->fLength);
		m_iCurZoomLevel = pData->second->iZoomLevel;
		m_fFovy = m_ZoomLevels[m_iCurZoomLevel];
		m_vAtOffset = pData->second->vAtOffset;

		m_eCameraMode = DEFAULT;
		return false;
	}

	// Swithcing 중이 아니라면 평소대로 함
	m_pCurArm->Set_NextArmData(pData->first, _iTriggerID);
	m_szEventTag = _wszNextArmName;

	if (nullptr != pData->second) {
		Start_Zoom(pData->second->fZoomTime, (CCamera::ZOOM_LEVEL)pData->second->iZoomLevel, (RATIO_TYPE)pData->second->iZoomRatioType);
		Start_Changing_AtOffset(pData->second->fAtOffsetTime, XMLoadFloat3(&pData->second->vAtOffset), pData->second->iAtRatioType);

		for (auto& PreArm : m_PreSubArms) {
			if (_iTriggerID == PreArm.first.iTriggerID) {
				if (true == PreArm.second) {    // Return 중이라면?
					PreArm.second = false;
					return true;
				}

				return true;
			}
		}

		RETURN_SUBDATA tSubData;

		tSubData.iZoomLevel = m_iPreZoomLevel;
		tSubData.vAtOffset = m_vAtOffset;
		tSubData.iTriggerID = _iTriggerID;

		m_PreSubArms.push_back(make_pair(tSubData, false));
	}

	return true;
}

void CCamera_Target::Set_PreArmDataState(_int _iTriggerID, _bool _isReturn)
{

	if (nullptr == m_pCurArm)
		return;
	int a = 0;
 	m_pCurArm->Set_PreArmDataState(_iTriggerID, _isReturn);

	if (true == _isReturn) {
		for (auto& PreArm : m_PreSubArms) {
			if (_iTriggerID == PreArm.first.iTriggerID) {
				Start_Zoom(m_pCurArm->Get_ReturnTime(), (ZOOM_LEVEL)(PreArm.first.iZoomLevel), EASE_IN);
				Start_Changing_AtOffset(m_pCurArm->Get_ReturnTime(), XMLoadFloat3(&PreArm.first.vAtOffset), EASE_IN);

				PreArm.second = true; // Return 중
			}
		}
	}
	else {
		// 안 빼고 남긴다
		// 값을 바꾸지도 않는다
	}

	m_iCurTriggerID = _iTriggerID;
}

void CCamera_Target::Key_Input(_float _fTimeDelta)
{
#ifdef _DEBUG

#pragma region RB 카메라 Arm 회전
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
#pragma endregion

#pragma region 예시 코드
	/*if (KEY_DOWN(KEY::K)) {
		Start_Changing_ArmLength(2.f, 20.f, EASE_IN);
		Start_Turn_AxisY(2.f, XMConvertToRadians(-5.f), XMConvertToRadians(-10.f));
		Start_Turn_AxisRight(2.f, XMConvertToRadians(-10.f), XMConvertToRadians(-2.f));
	}

	if (KEY_DOWN(KEY::J)) {
		Set_ResetData();
	}

	if (KEY_DOWN(KEY::R)) {
		Start_ResetArm_To_SettingPoint(2.f);
	}*/


	// CuttomData 예시 코드
	// Right축, Y축 기준으로 회전 시간과 회전 정도 설정
	// Length 시간과 Length 시간 설정을 한 번에 할 수 있음(Length는 EASE_IN, EASE_OUT 등등을 설정해 줘야 하지만 회전은 RPS로 설정함)
	/*CCamera_2D* pCamera = static_cast<CCamera_2D*>(CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET_2D));
	pCamera->Set_CameraMode(CCamera_2D::MOVE_TO_CUSTOMARM);

	ARM_DATA tData = {};
	tData.fMoveTimeAxisRight = { 5.f, 0.f };
	tData.fRotationPerSecAxisRight = { XMConvertToRadians(-10.f), XMConvertToRadians(-1.f) };
	tData.fMoveTimeAxisY = { 5.f, 0.f };
	tData.fRotationPerSecAxisY = { XMConvertToRadians(-10.f), XMConvertToRadians(-1.f) };
	tData.fLength = 20.f;
	tData.fLengthTime = { 5.f, 0.f };
	tData.iLengthRatioType = EASE_OUT;

	pCamera->Add_CustomArm(tData);

	pCamera->Start_Shake_ByCount(0.2f, 0.1f, 10, CCamera::SHAKE_XY);
	pCamera->Start_Changing_AtOffset(3.f, XMVectorSet(-0.7f, 2.f, 0.f, 0.f), EASE_IN_OUT);*/
#pragma endregion

	//Imgui(_fTimeDelta);

#endif
}

void CCamera_Target::Action_Mode(_float _fTimeDelta)
{
	if (true == m_isInitialData)
		return;

	Action_Zoom(_fTimeDelta);
	Action_Shake(_fTimeDelta);
	Change_AtOffset(_fTimeDelta);

	Turn_AxisY(_fTimeDelta);
	Turn_AxisRight(_fTimeDelta);
	Change_Length(_fTimeDelta);

	Change_FreezeOffset(_fTimeDelta);
	Move_To_ExitArm(_fTimeDelta);

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
	case MOVE_TO_CUSTOMARM:
		Move_To_CustomArm(_fTimeDelta);
		break;
	case RETURN_TO_DEFUALT:
		break;
	case RESET_TO_SETTINGPOINT:
		Reset_To_SettingPoint(_fTimeDelta);
		break;
	}
}

void CCamera_Target::Action_SetUp_ByMode()
{
	if (m_ePreCameraMode != m_eCameraMode) {

		switch (m_eCameraMode) {
		case DEFAULT:
			break;
		case MOVE_TO_NEXTARM:
		{

		}
		break;
		case RETURN_TO_PREARM:
		{

		}
		break;
		}

		m_ePreCameraMode = m_eCameraMode;
	}
}

void CCamera_Target::Defualt_Move(_float _fTimeDelta)
{
	_vector vTargetPos;
	_vector vCameraPos = Calculate_CameraPos(&vTargetPos, _fTimeDelta);	// 목표 위치 + Arm -> 최종 결과물
	_vector vCurPos = m_pControllerTransform->Get_State(CTransform::STATE_POSITION);

	Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, vCameraPos);

	Look_Target(vTargetPos, _fTimeDelta);
}

void CCamera_Target::Move_To_NextArm(_float _fTimeDelta)
{
	if (true == m_isFreezeExitReturn) {
		m_isFreezeExitReturn = false;
	}

	if (true == m_pCurArm->Move_To_NextArm_ByVector(_fTimeDelta)) {
		m_eCameraMode = DEFAULT;
		CTrigger_Manager::GetInstance()->On_End(m_szEventTag);

		return;
	}

	_vector vTargetPos;
	_vector vCameraPos = Calculate_CameraPos(&vTargetPos, _fTimeDelta);
	_vector vCurPos = m_pControllerTransform->Get_State(CTransform::STATE_POSITION);

	Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, vCameraPos);

	Look_Target(vTargetPos, _fTimeDelta);
}

void CCamera_Target::Look_Target(_fvector _vTargetPos, _float fTimeDelta)
{
	// Tool용
	if (false == m_isLookAt)
		return;

	if ((true == m_isEnableLookAt) && (false == m_isExitLookAt)) {
		_vector vFreezeOffset = -XMLoadFloat3(&m_vPreFreezeOffset);
		_vector vAt = _vTargetPos + vFreezeOffset + XMLoadFloat3(&m_vAtOffset) + XMLoadFloat3(&m_vShakeOffset);
		m_pControllerTransform->LookAt_3D(XMVectorSetW(vAt, 1.f));
	}
	else if ((true == m_isEnableLookAt) && (true == m_isExitLookAt)) {
		// LookAt 보간

		_float fRatio = m_pGameInstance->Calculate_Ratio(&m_fLookTime, fTimeDelta, EASE_IN_OUT);

		if (fRatio >= (1.f - EPSILON)) {
			_vector vFreezeOffset = -XMLoadFloat3(&m_vPreFreezeOffset);
			_vector vAt = _vTargetPos + vFreezeOffset + XMLoadFloat3(&m_vAtOffset) + XMLoadFloat3(&m_vShakeOffset);
			m_pControllerTransform->LookAt_3D(XMVectorSetW(vAt, 1.f));

			m_isExitLookAt = false;
			m_fLookTime.y = 0.f;
			return;
		}

		_vector vFreezeOffset = -XMLoadFloat3(&m_vPreFreezeOffset);
		_vector vPos = m_pControllerTransform->Get_State(CTransform::STATE_POSITION);
		_vector vAt = _vTargetPos + vFreezeOffset + XMLoadFloat3(&m_vAtOffset) + XMLoadFloat3(&m_vShakeOffset);
		_vector vDir = XMVector3Normalize(vAt - vPos);

		_vector vLook = XMVectorLerp(XMLoadFloat3(&m_vStartLookVector), vDir, fRatio);

		m_pControllerTransform->Get_Transform()->Set_Look(vLook);
	}
	//else {
	//	if (DEFAULT != m_eCameraMode) {
	//		_vector vFreezeOffset = -XMLoadFloat3(&m_vPreFreezeOffset);
	//		_vector vAt = _vTargetPos + XMLoadFloat3(&m_vAtOffset) + XMLoadFloat3(&m_vShakeOffset);
	//		m_pControllerTransform->LookAt_3D(XMVectorSetW(vAt, 1.f));
	//	}
	//}
}

void CCamera_Target::Move_To_PreArm(_float _fTimeDelta)
{
	if (true == m_isFreezeExitReturn) {
		m_isFreezeExitReturn = false;
	}

	if (true == m_pCurArm->Move_To_PreArm(_fTimeDelta)) {
		m_eCameraMode = DEFAULT;
		//return;

		for (auto& iter = m_PreSubArms.begin(); iter != m_PreSubArms.end();) {
			if (m_iCurTriggerID == (*iter).first.iTriggerID) {
				m_fFovy = m_ZoomLevels[(*iter).first.iZoomLevel];
				m_iCurZoomLevel = (*iter).first.iZoomLevel;
				m_vAtOffset = (*iter).first.vAtOffset;

				iter = m_PreSubArms.erase(iter);
			}
			else
				++iter;
		}
	}

	_vector vTargetPos;
	_vector vCameraPos = Calculate_CameraPos(&vTargetPos, _fTimeDelta);

	Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, vCameraPos);

	Look_Target(vTargetPos, _fTimeDelta);
}

void CCamera_Target::Move_To_CustomArm(_float _fTimeDelta)
{
	if (true == m_isFreezeExitReturn) {
		m_isFreezeExitReturn = false;
	}

	if (true == m_pCurArm->Move_To_CustomArm(&m_CustomArmData, _fTimeDelta)) {
		m_eCameraMode = DEFAULT;
	}

	_vector vTargetPos;
	_vector vCamerPos = Calculate_CameraPos(&vTargetPos, _fTimeDelta);

	m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vCamerPos, 1.f));

	Look_Target(vTargetPos, _fTimeDelta);
}

_vector CCamera_Target::Calculate_CameraPos(_vector* _pLerpTargetPos, _float _fTimeDelta)
{
	_vector vTargetPos;

	// 현재 타겟 위치와 Freeze했을 당시 Target 위치 사이의 Offset을 계산한다 
	Calculate_FreezeOffset(&vTargetPos);

	_vector vCurPos, vCurFreezeOffset = {};

	// Target Change가 안 됐을 때는 시간 제한 없이 Smooth를 한다
	if (false == m_isTargetChanged) {
		vCurPos = XMVectorLerp(XMVectorSetW(XMLoadFloat3(&m_vPreTargetPos), 1.f), vTargetPos + XMLoadFloat3(&m_vFreezeOffset), m_fSmoothSpeed * _fTimeDelta);
		vCurFreezeOffset = XMVectorLerp(XMLoadFloat3(&m_vPreFreezeOffset), XMLoadFloat3(&m_vFreezeOffset), m_fSmoothSpeed * _fTimeDelta);

		XMStoreFloat3(&m_vPreFreezeOffset, vCurFreezeOffset);
	}
	// Target Change일 땐 지정된 시간만큼 바뀐 Target으로 이동한다
	else {
		_float fRatio = m_pGameInstance->Calculate_Ratio(&m_fTargetChangingTime, _fTimeDelta, EASE_IN_OUT);

		if (fRatio >= (1.f - EPSILON)) {
			vCurPos = vTargetPos;
			m_isTargetChanged = false;
		}

		vCurPos = XMVectorLerp(XMVectorSetW(XMLoadFloat3(&m_vStartPos), 1.f), vTargetPos, fRatio);
	}

	vCurPos = XMVectorSetW(vCurPos, 1.f);
	if (nullptr != _pLerpTargetPos)
		*_pLerpTargetPos = vCurPos;

	_vector vCameraPos = vCurPos + (m_pCurArm->Get_Length() * m_pCurArm->Get_ArmVector());
	XMStoreFloat3(&m_vPreTargetPos, vCurPos);

	return vCameraPos;
}

void CCamera_Target::Calculate_FreezeOffset(_vector* _pTargetPos)
{
	switch (m_eTargetCoordinate) {
	case (_uint)COORDINATE_2D:
	{
		_vector vTargetPos = CSection_Manager::GetInstance()->Get_WorldPosition_FromWorldPosMap(m_szTargetSectionTag, { m_pTargetWorldMatrix->_41, m_pTargetWorldMatrix->_42 });
		*_pTargetPos = vTargetPos;
	}
		break;
	case (_uint)COORDINATE_3D:
	{
		memcpy(_pTargetPos, m_pTargetWorldMatrix->m[3], sizeof(_float4));
	}
		break;
	}
	
	if (FREEZE_X == (m_iFreezeMask & FREEZE_X)) {
		m_vFreezeOffset.x = m_vFreezeEnterPos.x - XMVectorGetX(*_pTargetPos);
	}
	if (FREEZE_Z == (m_iFreezeMask & FREEZE_Z)) {
		m_vFreezeOffset.z = m_vFreezeEnterPos.z - XMVectorGetZ(*_pTargetPos);
	}
}

void CCamera_Target::Switching(_float _fTimeDelta)
{
	if (false == m_isInitialData)
		return;

	//m_InitialTime.y += _fTimeDelta;
	//_float fRatio = m_InitialTime.y / m_InitialTime.x;

	_float fRatio = Calculate_Ratio(&m_InitialTime, _fTimeDelta, EASE_IN_OUT);

	if (fRatio >= (1.f - EPSILON)) {
		_vector vTargetPos;
		memcpy(&vTargetPos, m_pTargetWorldMatrix->m[3], sizeof(_float4));
		_vector vCameraPos = vTargetPos + (m_pCurArm->Get_Length() * m_pCurArm->Get_ArmVector());
		m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vCameraPos, 1.f));

		_vector vLookAt = vTargetPos + XMLoadFloat3(&m_vAtOffset) + XMLoadFloat3(&m_vShakeOffset);
		m_pControllerTransform->LookAt_3D(XMVectorSetW(vLookAt, 1.f));

		m_fFovy = m_ZoomLevels[m_iCurZoomLevel];

		memcpy(&m_vPreTargetPos, m_pTargetWorldMatrix->m[3], sizeof(_float3));

		m_InitialTime.y = 0.f;
		m_isInitialData = false;

		// ㅋㅋ 이거 나중에 어떻게든 해 봐 고쳐
		m_isFreezeExit = false;
		XMStoreFloat3(&m_vFreezeEnterPos, vTargetPos);
		m_vPreFreezeOffset = { 0.f, 0.f, 0.f };
		return;
	}

	// Pos
	_vector vTargetPos;
	memcpy(&vTargetPos, m_pTargetWorldMatrix->m[3], sizeof(_float4));
	_vector vCameraPos = vTargetPos + (m_pCurArm->Get_Length() * m_pCurArm->Get_ArmVector());
	_vector vResultPos = XMVectorLerp(XMLoadFloat3(&m_tInitialData.vPosition), vCameraPos, fRatio);

	Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vResultPos, 1.f));

	// LookAt
	_vector vLookAt = vTargetPos + XMLoadFloat3(&m_vAtOffset) + XMLoadFloat3(&m_vShakeOffset);
	_vector vResultLookAt = XMVectorLerp(XMLoadFloat3(&m_tInitialData.vAt), vLookAt, fRatio);

	m_pControllerTransform->LookAt_3D(XMVectorSetW(vResultLookAt, 1.f));

	// Zoom Level
	_float fFovy = m_pGameInstance->Lerp(m_ZoomLevels[m_tInitialData.iZoomLevel], m_ZoomLevels[m_iCurZoomLevel], fRatio);
	m_fFovy = fFovy;
}

void CCamera_Target::Change_FreezeOffset(_float _fTimeDelta)
{
	if (false == m_isFreezeExit || false == m_isFreezeOffsetReturn)
		return;

	_float fRatio = m_pGameInstance->Calculate_Ratio(&m_fFreezeOffsetTime, _fTimeDelta, EASE_IN_OUT);

	if (fRatio >= (1.f - EPSILON)) {
		m_fFreezeOffsetTime.y = 0.f;
		//m_isFreezeExit = false;
		m_vFreezeOffset = { 0.f, 0.f, 0.f };
		m_isFreezeOffsetReturn = false;

		return;
	}

	_vector vFreezeOffset = XMVectorLerp(XMLoadFloat3(&m_vStartFreezeOffset), XMVectorZero(), fRatio);

	XMStoreFloat3(&m_vFreezeOffset, vFreezeOffset);

#pragma region offset 갔다가 회전
	/*if (true == m_isFreezeOffsetReturn) {
		_float fRatio = m_pGameInstance->Calculate_Ratio(&m_fFreezeExitTime, _fTimeDelta, EASE_IN_OUT);

		if (fRatio >= (1.f - EPSILON)) {
			m_fFreezeExitTime.y = 0.f;
			m_isFreezeOffsetReturn = false;
			m_vFreezeOffset = { 0.f, 0.f, 0.f };
			m_vStartFreezeOffset = { 0.f, 0.f, 0.f };

			return;
		}

		_vector vFreezeOffset = XMVectorLerp(XMLoadFloat3(&m_vStartFreezeOffset), XMVectorZero(), fRatio);

		XMStoreFloat3(&m_vFreezeOffset, vFreezeOffset);
	}
	else if (false == m_isFreezeOffsetReturn) {

		if (true == m_isFreezeExitReturn) {

			_float fRatio = m_pGameInstance->Calculate_Ratio(&m_fFreezeExitTime, _fTimeDelta, EASE_IN_OUT);

			if (fRatio >= (1.f - EPSILON)) {
				m_fFreezeExitTime.y = 0.f;
				m_isFreezeExit = false;
				m_isFreezeExitReturn = false;
				m_vFreezeOffset = { 0.f, 0.f, 0.f };

				m_pCurArm->Set_ArmVector(XMVector3Normalize(XMLoadFloat3(&m_vCurFreezeExitData.vFreezeExitArm)));

				return;
			}

			m_pCurArm->Move_To_FreezeExitArm(fRatio, XMLoadFloat3(&m_vCurFreezeExitData.vFreezeExitArm), 0.f);
		}
		else {
			m_isFreezeExit = false;
			m_fFreezeExitTime.y = 0.f;
			m_vFreezeOffset = { 0.f, 0.f, 0.f };
		}
	}*/
#pragma endregion

#pragma region Offset과 동시 회전(시간 동일)
	//_float fRatio = m_pGameInstance->Calculate_Ratio(&m_fFreezeExitTime, _fTimeDelta, EASE_IN_OUT);

	//if (fRatio >= (1.f - EPSILON)) {
	//	m_fFreezeExitTime.y = 0.f;
	//	m_isFreezeExit = false;
	//	m_vFreezeOffset = { 0.f, 0.f, 0.f };

	//	if (m_eCameraMode == DEFAULT && true == m_isFreezeExitReturn)
	//		m_pCurArm->Set_ArmVector(XMVector3Normalize(XMLoadFloat3(&m_vCurFreezeExitData.vFreezeExitArm)));

	//	return;
	//}

	//_vector vFreezeOffset = XMVectorLerp(XMLoadFloat3(&m_vFreezeOffset), XMVectorZero(), fRatio);

	//if(m_eCameraMode == DEFAULT && true == m_isFreezeExitReturn)
	//	m_pCurArm->Move_To_FreezeExitArm(fRatio, XMLoadFloat3(&m_vCurFreezeExitData.vFreezeExitArm), 0.f);

	//XMStoreFloat3(&m_vFreezeOffset, vFreezeOffset);
#pragma endregion
}

#ifdef _DEBUG

void CCamera_Target::Imgui(_float _fTimeDelta)
{
	// ImGui
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

	_float3 vTargetPos = {};
	_vector vPos = m_pControllerTransform->Get_State(CTransform::STATE_POSITION);
	_float3 vDebugArm = {};

	memcpy(&vTargetPos, m_pTargetWorldMatrix->m[3], sizeof(_float3));
	XMStoreFloat3(&vDebugArm, XMVector3Normalize(XMLoadFloat3(&vTargetPos) - vPos));

	fArmLength = m_pCurArm->Get_Length();
	ImGui::Text("Cur Arm: %.4f, %.4f, %.4f", vCurArm.x, vCurArm.y, vCurArm.z);
	ImGui::Text("Debug Arm(Pos - TargetPos): %.4f, %.4f, %.4f", vDebugArm.x, vDebugArm.y, vDebugArm.z);
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
#endif // DEBUG

void CCamera_Target::Load_InitialArmTag()
{
	_wstring szFileName;

	switch (m_iCurLevelID) {
	case LEVEL_CHAPTER_2:
		szFileName = TEXT("Chapter2/Chapter2_SketchSpace_InitialTag.json");
		break;
	case LEVEL_CHAPTER_4:
		szFileName = TEXT("Chapter4/Chapter4_SketchSpace_InitialTag.json");
		break;
	case LEVEL_CHAPTER_6:
		szFileName = TEXT("Chapter6/Chapter6_SketchSpace_InitialTag.json");
		break;
	case LEVEL_CHAPTER_8:
		szFileName = TEXT("Chapter6/Chapter6_SketchSpace_InitialTag.json");
		break;
	}

	_wstring wszLoadPath = L"../Bin/DataFiles/Camera/ArmData/" + szFileName;

	ifstream file(wszLoadPath);

	if (!file.is_open())
	{
		MSG_BOX("Initial Arm 파일을 열 수 없습니다.");
		file.close();
		return;
	}

	json Result;
	file >> Result;
	file.close();

	if (Result.is_array()) {
		for (auto& InitialTag_json : Result) {
			_wstring szSectionTag = m_pGameInstance->StringToWString(InitialTag_json["Section_Tag"]);
			vector<_wstring> ArmTags;

			for (auto& ArmTag : InitialTag_json["Arm_Tag"]) {
				ArmTags.push_back(m_pGameInstance->StringToWString(ArmTag));
			}

			m_SkspInitialTags.emplace(szSectionTag, ArmTags);
		}
	}
}

void CCamera_Target::Move_To_ExitArm(_float _fTimeDelta)
{
	if (false == m_isFreezeExit || false == m_isFreezeExitReturn)
		return;

	_float fRatio = m_pGameInstance->Calculate_Ratio(&m_fFreezeExitReturnTime, _fTimeDelta, EASE_IN_OUT);

	if (fRatio >= (1.f - EPSILON)) {
		m_fFreezeExitReturnTime.y = 0.f;
		m_isFreezeExit = false;
		m_isFreezeOffsetReturn = false;

		m_pCurArm->Set_ArmVector(XMVector3Normalize(XMLoadFloat3(&m_vCurFreezeExitData.vFreezeExitArm)));

		return;
	}

	m_pCurArm->Move_To_FreezeExitArm(fRatio, XMLoadFloat3(&m_vCurFreezeExitData.vFreezeExitArm), 0.f);
}

void CCamera_Target::Reset_To_SettingPoint(_float _fTimeDelta)
{
	_float fRatio = m_pGameInstance->Calculate_Ratio(&m_fResetTime, _fTimeDelta, EASE_IN_OUT);

	if (fRatio >= (1.f - EPSILON)) {
		m_fResetTime.y = 0.f;
		m_eCameraMode = DEFAULT;

		m_pCurArm->Set_ArmVector(XMVector3Normalize(XMLoadFloat3(&m_ResetArmData.vPreArm)));

		return;
	}

	m_pCurArm->Reset_To_SettingPoint(fRatio, XMLoadFloat3(&m_ResetArmData.vPreArm), m_ResetArmData.fPreLength);

	_vector vTargetPos;
	_vector vCameraPos = Calculate_CameraPos(&vTargetPos, _fTimeDelta);	// 목표 위치 + Arm -> 최종 결과물

	Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, vCameraPos);

	Look_Target(vTargetPos, _fTimeDelta);
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
