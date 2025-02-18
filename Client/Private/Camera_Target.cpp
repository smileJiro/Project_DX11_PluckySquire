#include "stdafx.h"
#include "Camera_Target.h"

#include "GameInstance.h"

#include "Trigger_Manager.h"

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

	Switching(fTimeDelta);

	Action_SetUp_ByMode();
	Action_Mode(fTimeDelta);

	__super::Late_Update(fTimeDelta);
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

void CCamera_Target::Set_FreezeEnter(_uint _iFreezeMask, _fvector _vExitArm, _int _iTriggerID)
{
	m_iFreezeMask |= _iFreezeMask;
	//memcpy(&m_vFreezeEnterPos, m_pTargetWorldMatrix->m[3], sizeof(_float3));
	m_vFreezeEnterPos = m_vPreTargetPos;
	m_isFreezeExit = false;

	_float3 vExitArm;
	XMStoreFloat3(&vExitArm, _vExitArm);
	m_FreezeExitArms.push_back({ vExitArm , _iTriggerID });
}

void CCamera_Target::Set_FreezeExit(_uint _iFreezeMask, _int _iTriggerID)
{
	m_iFreezeMask ^= _iFreezeMask;
	m_fFreezeExitTime = { 1.f, 0.f };
	m_isFreezeExit = true;

	for (auto& iter = m_FreezeExitArms.begin(); iter != m_FreezeExitArms.end();) {
		if (_iTriggerID == (*iter).second) {
			m_vCurFreezeExitArm = (*iter).first;
			iter = m_FreezeExitArms.erase(iter);

			_vector vPos = m_pControllerTransform->Get_State(CTransform::STATE_POSITION);
			_vector vDir = vPos - XMLoadFloat3(&m_vPreTargetPos);
			_float fLength = XMVectorGetX(XMVector3Length(vDir));

			m_pCurArm->Set_ArmVector(XMVector3Normalize(vDir));
			m_pCurArm->Set_Length(fLength);
			m_pCurArm->Set_StartInfo(XMVector3Normalize(vDir), fLength);

			m_isFreezeExit = true;
			m_fFreezeExitTime.y = 0.f;
		}
		else
			++iter;
	}

	//m_pCurArm->Set_ArmVector(XMVector3Normalize(XMLoadFloat3(&m_vFreezeExitArm)));
}

void CCamera_Target::Set_EnableLookAt(_bool _isEnableLookAt)
{
	if (true == _isEnableLookAt && false == m_isEnableLookAt) {
		// false이다가 m_isExitLookAt으로 돌아간 것
		m_isExitLookAt = true;
		m_fLookTime.y = 0.f;
		XMStoreFloat3(&m_vStartLookVector, m_pControllerTransform->Get_State(CTransform::STATE_LOOK));
		//m_vStartLookVector = 
	}

	m_isEnableLookAt = _isEnableLookAt;
}

void CCamera_Target::Change_Target(const _float4x4* _pTargetWorldMatrix)
{
	m_pTargetWorldMatrix = _pTargetWorldMatrix;
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
	// 2D에서 3D로 갈 때 Section Tag? Portal에 따라서 나갈 때 3D Target 카메라의 초기값 정해 주기 
	// ㅈㄴ 하드코딩 나중에 수정

	m_isEnableLookAt = true;
	m_isFreezeExit = false;

	pair<ARM_DATA*, SUB_DATA*>* pData = nullptr;

	if (TEXT("Chapter2_SKSP_01") == _szSectionTag) {
		switch (_iPortalIndex) {
		case 0:
		{
			pData = Find_ArmData(TEXT("Chapter2_Portal1_Start"));
		}
			
			break;
		case 1:
		{
			pData = Find_ArmData(TEXT("Chapter2_Portal1_End"));
		}
			break;
		}
	}
	else if (TEXT("Chapter2_SKSP_02") == _szSectionTag) {
		switch (_iPortalIndex) {
		case 0:
		{
			pData = Find_ArmData(TEXT("Chapter2_Portal2_Start"));
		}
			break;
		case 1:
		{
			pData = Find_ArmData(TEXT("Chapter2_Portal2_End"));
		}
			break;
		}
	}
	else if (TEXT("Chapter2_SKSP_03") == _szSectionTag) {
		switch (_iPortalIndex) {
		case 0:
		{
			pData = Find_ArmData(TEXT("Chapter2_Portal3_Start"));
			m_vFreezeEnterPos = { 11.9279337f, 7.85533524f, 24.1609268f };
		}
			break;
		case 1:
		{
			pData = Find_ArmData(TEXT("Chapter2_Portal3_End"));
		}
			break;
		}
	}
	else if (TEXT("Chapter2_SKSP_04") == _szSectionTag) {
		switch (_iPortalIndex) {
		case 0:
		{
			pData = Find_ArmData(TEXT("Chapter2_Portal4_Start"));
		}
			break;
		case 1:
		{
			pData = Find_ArmData(TEXT("Chapter2_Portal4_End"));
		}
			break;
		}
	}
	else if (TEXT("Chapter2_SKSP_05") == _szSectionTag) {
		switch (_iPortalIndex) {
		case 0:
		{
			pData = Find_ArmData(TEXT("Chapter2_Portal5_Start"));
		}
			break;
		case 1:
		{
			pData = Find_ArmData(TEXT("Chapter2_Portal5_End"));
		}
			break;
		}
	}

	m_pCurArm->Set_ArmVector(XMLoadFloat3(&pData->first->vDesireArm));
	m_pCurArm->Set_Length(pData->first->fLength);
	m_iCurZoomLevel = { pData->second->iZoomLevel };
	m_fFovy = { m_ZoomLevels[pData->second->iZoomLevel] };
	m_vAtOffset = { pData->second->vAtOffset };
}

INITIAL_DATA CCamera_Target::Get_InitialData()
{
	INITIAL_DATA tData;

	XMStoreFloat3(&tData.vPosition, m_pControllerTransform->Get_State(CTransform::STATE_POSITION));

	_vector vTargetPos;
	memcpy(&vTargetPos, m_pTargetWorldMatrix->m[3], sizeof(_float4));
	_vector vAt = vTargetPos + XMLoadFloat3(&m_vAtOffset) + XMLoadFloat3(&m_vShakeOffset);
	XMStoreFloat3(&tData.vAt, vAt);

	tData.iZoomLevel = m_iCurZoomLevel;

	if (false == m_isEnableLookAt) {
		_vector vLook = m_pControllerTransform->Get_State(CTransform::STATE_LOOK);
		_vector vPos = m_pControllerTransform->Get_State(CTransform::STATE_POSITION);
		_vector vResultAt = vPos + vLook;
		XMStoreFloat3(&tData.vAt, vResultAt);
		
	}

	return tData;
}

_bool CCamera_Target::Set_NextArmData(_wstring _wszNextArmName, _int _iTriggerID)
{
	pair<ARM_DATA*, SUB_DATA*>* pData = Find_ArmData(_wszNextArmName);

	if (nullptr == pData)
		return false;

	if (nullptr == m_pCurArm)
		return false;

	Set_Arm_From_Freeze();

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

	Set_Arm_From_Freeze();

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
	if (true == m_isInitialData)
		return;

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
	_float fRatio;
	
	if (true == m_isFreezeExit) {

		m_fFreezeExitTime.y += _fTimeDelta;
		fRatio = m_fFreezeExitTime.y / m_fFreezeExitTime.x;

		if (m_pCurArm->Move_To_FreezeExitArm(fRatio, XMLoadFloat3(&m_vCurFreezeExitArm))) {
			m_fFreezeExitTime.y = 0.f;
			m_isFreezeExit = false;
		}
	}

	_vector vTargetPos;
	_vector vCameraPos = Calculate_CameraPos(&vTargetPos, _fTimeDelta);	// 목표 위치 + Arm -> 최종 결과물
	_vector vCurPos = m_pControllerTransform->Get_State(CTransform::STATE_POSITION);

	if (true == m_isFreezeExit) {
		// Freeze X를 나왔는데 Freeze Z에 있을 때
		if (FREEZE_Z == (m_iFreezeMask & FREEZE_Z)) {
			vCameraPos = XMVectorLerp(vCurPos, XMVectorSetZ(vCameraPos, XMVectorGetZ(vCurPos)), fRatio);
		}
		// Freeze Z를 나왔는데 Freeze X에 있을 때
		else if (FREEZE_X == (m_iFreezeMask & FREEZE_X)) {
			vCameraPos = XMVectorLerp(vCurPos, XMVectorSetX(vCameraPos, XMVectorGetX(vCurPos)), fRatio);
		}
		else if (RESET == m_iFreezeMask) {
			//vCameraPos = XMVectorLerp(vCurPos, vCameraPos, fRatio);

			_float fZ = m_pGameInstance->Lerp(XMVectorGetZ(vCurPos), XMVectorGetZ(vCameraPos), fRatio);
			vCameraPos = XMVectorSetZ(vCameraPos, fZ);
		}
	}
	else {
		if (FREEZE_X == (m_iFreezeMask & FREEZE_X)) {
			vCameraPos = XMVectorSetX(vCameraPos, XMVectorGetX(vCurPos));
		}
		if (FREEZE_Z == (m_iFreezeMask & FREEZE_Z)) {
			vCameraPos = XMVectorSetZ(vCameraPos, XMVectorGetZ(vCurPos));
		}
	}

	Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, vCameraPos);

	Look_Target(vTargetPos, _fTimeDelta);
}

void CCamera_Target::Move_To_NextArm(_float _fTimeDelta)
{
	// 만약 방금 Freeze Trigger를 나갔다면 Arm을 그냥 지금으로 설정, Length도 
	Set_Arm_From_Freeze();

	if (true == m_pCurArm->Move_To_NextArm_ByVector(_fTimeDelta)) {
		m_eCameraMode = DEFAULT;
		CTrigger_Manager::GetInstance()->On_End(m_szEventTag);

		//if(RESET != m_iFreezeMask)
		//	m_pCurArm->Set_ArmVector(XMLoadFloat3(&m_vCurFreezeExitArm));

		return;
	}

	_vector vTargetPos;
	_vector vCameraPos = Calculate_CameraPos(&vTargetPos, _fTimeDelta);
	_vector vCurPos = m_pControllerTransform->Get_State(CTransform::STATE_POSITION);
	if (RESET != m_iFreezeMask) {
		if (FREEZE_X == (m_iFreezeMask & FREEZE_X)) {
			_vector vFixedPos = XMVectorSetX(vTargetPos, m_vFreezeEnterPos.x);
			vCameraPos = vFixedPos + (m_pCurArm->Get_Length() * m_pCurArm->Get_ArmVector());
		}
		if (FREEZE_Z == (m_iFreezeMask & FREEZE_Z)) {
			_vector vFixedPos = XMVectorSetY(vTargetPos, m_vFreezeEnterPos.y);
			vCameraPos = vFixedPos + (m_pCurArm->Get_Length() * m_pCurArm->Get_ArmVector());
		}
	}

	Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, vCameraPos);

	Look_Target(vTargetPos, _fTimeDelta);
}

void CCamera_Target::Look_Target(_fvector _vTargetPos, _float fTimeDelta)
{
	//_vector vTargetPos = XMLoadFloat4x4(m_pTargetWorldMatrix).r[TARGET_POS];

	if ((true == m_isEnableLookAt) && (false == m_isExitLookAt)) {
		_vector vAt = _vTargetPos + XMLoadFloat3(&m_vAtOffset) + XMLoadFloat3(&m_vShakeOffset);
		m_pControllerTransform->LookAt_3D(XMVectorSetW(vAt, 1.f));
	}
	else if ((true == m_isEnableLookAt) && (true == m_isExitLookAt)) {
		// LookAt 보간

		_float fRatio = m_pGameInstance->Calculate_Ratio(&m_fLookTime, fTimeDelta, EASE_IN_OUT);

		if (fRatio >= (1.f - EPSILON)) {
			_vector vAt = _vTargetPos + XMLoadFloat3(&m_vAtOffset) + XMLoadFloat3(&m_vShakeOffset);
			m_pControllerTransform->LookAt_3D(XMVectorSetW(vAt, 1.f));

			m_isExitLookAt = false;
			m_fLookTime.y = 0.f;
			return;
		}

		_vector vPos = m_pControllerTransform->Get_State(CTransform::STATE_POSITION);
		_vector vAt = _vTargetPos + XMLoadFloat3(&m_vAtOffset) + XMLoadFloat3(&m_vShakeOffset);
		_vector vDir = XMVector3Normalize(vAt - vPos);

		_vector vLook = XMVectorLerp(XMLoadFloat3(&m_vStartLookVector), vDir, fRatio);

		m_pControllerTransform->Get_Transform()->Set_Look(vLook);
	}
}

void CCamera_Target::Move_To_PreArm(_float _fTimeDelta)
{
	Set_Arm_From_Freeze();

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

	if (RESET != m_iFreezeMask) {
		if (FREEZE_X == (m_iFreezeMask & FREEZE_X)) {
			_vector vFixedPos = XMVectorSetX(vTargetPos, m_vFreezeEnterPos.x);
			vCameraPos = vFixedPos + (m_pCurArm->Get_Length() * m_pCurArm->Get_ArmVector());
		}
		if (FREEZE_Z == (m_iFreezeMask & FREEZE_Z)) {
			_vector vFixedPos = XMVectorSetY(vTargetPos, m_vFreezeEnterPos.y);
			vCameraPos = vFixedPos + (m_pCurArm->Get_Length() * m_pCurArm->Get_ArmVector());
		}
	}

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

void CCamera_Target::Switching(_float _fTimeDelta)
{
	if (false == m_isInitialData)
		return;

	//m_InitialTime.y += _fTimeDelta;
	//_float fRatio = m_InitialTime.y / m_InitialTime.x;

	_float fRatio = Calculate_Ratio(&m_InitialTime, _fTimeDelta, EASE_IN);

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

		// ㅋㅋ 시발 이거 나중에 어떻게든 해 봐 고쳐
		m_isFreezeExit = false;
		return;
	}

	// Pos
	_vector vTargetPos;
	memcpy(&vTargetPos, m_pTargetWorldMatrix->m[3], sizeof(_float4));
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

void CCamera_Target::Set_Arm_From_Freeze()
{
	// 만약 방금 Freeze Trigger를 나갔다면 Arm을 그냥 지금으로 설정, Length도 
	if ((true == m_isFreezeExit)) {
		_vector vPos = m_pControllerTransform->Get_State(CTransform::STATE_POSITION);
		_vector vDir = vPos - XMLoadFloat3(&m_vPreTargetPos);
		_float fLength = XMVectorGetX(XMVector3Length(vDir));

		m_pCurArm->Set_ArmVector(XMVector3Normalize(vDir));
		m_pCurArm->Set_Length(fLength);
		m_pCurArm->Set_StartInfo(XMVector3Normalize(vDir), fLength);

		m_isFreezeExit = false;
		m_fFreezeExitTime.y = 0.f;
	}
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
