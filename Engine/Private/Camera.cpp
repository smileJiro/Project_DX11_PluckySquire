#include "Camera.h"
#include "Shader.h"
#include "GameInstance.h"
CCamera::CCamera(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

CCamera::CCamera(const CCamera& _Prototype)
	: CGameObject(_Prototype)

{
}

HRESULT CCamera::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CCamera::Initialize(void* _pArg)
{
	/* Zoom Level */
	m_ZoomLevels[(int)ZOOM_LEVEL::LEVEL_1] = XMConvertToRadians(8.f);
	m_ZoomLevels[(int)ZOOM_LEVEL::LEVEL_2] = XMConvertToRadians(25.f);
	m_ZoomLevels[(int)ZOOM_LEVEL::LEVEL_3] = XMConvertToRadians(35.f);
	m_ZoomLevels[(int)ZOOM_LEVEL::LEVEL_4] = XMConvertToRadians(40.f);
	m_ZoomLevels[(int)ZOOM_LEVEL::LEVEL_5] = XMConvertToRadians(47.f);
	m_ZoomLevels[(int)ZOOM_LEVEL::LEVEL_6] = XMConvertToRadians(62.f);
	m_ZoomLevels[(int)ZOOM_LEVEL::LEVEL_7] = XMConvertToRadians(74.f);
	m_ZoomLevels[(int)ZOOM_LEVEL::LEVEL_8] = XMConvertToRadians(84.f);
	m_ZoomLevels[(int)ZOOM_LEVEL::LEVEL_9] = XMConvertToRadians(100.f);
	m_ZoomLevels[(int)ZOOM_LEVEL::LEVEL_10] = XMConvertToRadians(120.f);

	if (nullptr == _pArg)
		return E_FAIL;

	// Transform 먼저 생성.
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	CAMERA_DESC* pDesc = static_cast<CAMERA_DESC*>(_pArg);

	m_iCurLevelID = pDesc->iCurLevelID;

	/* 뷰 행렬에 대한 트랜스폼 동기화 작업. */
	m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vEye), 1.f));
	m_pControllerTransform->LookAt_3D(XMVectorSetW(XMLoadFloat3(&pDesc->vAt), 1.f));
	
	/* 투영 행렬 구성 시 필요한 데이터를 저장. */
	m_eCameraType = pDesc->iCameraType;
	m_iCurZoomLevel = (_uint)pDesc->eZoomLevel;
	m_fFovy = m_ZoomLevels[m_iCurZoomLevel];
	m_fAspect = pDesc->fAspect;
	m_fNear = pDesc->fNear;
	m_fFar = pDesc->fFar;
	m_pGameInstance->Set_NearFarZ(_float2(m_fNear, m_fFar));

	Ready_DofConstData(pDesc);
	return S_OK;
}

void CCamera::Priority_Update(_float _fTimeDelta)
{
}

void CCamera::Update(_float _fTimeDelta)
{
}

void CCamera::Late_Update(_float _fTimeDelta)
{
	Action_PostProcessing_Fade(_fTimeDelta);

	if (true == m_isBindConstBuffer)
	{
		m_isBindConstBuffer = false;
		m_pGameInstance->UpdateConstBuffer(m_tDofData, m_pConstDofBuffer);
		Bind_DofConstBuffer();
	}

	Compute_PipeLineMatrices();
}

HRESULT CCamera::Render()
{
	return S_OK;
}

HRESULT CCamera::Load_DOF(const _wstring& _strJsonPath)
{
	const std::string filePathDOF = m_pGameInstance->WStringToString(_strJsonPath);
	std::ifstream inputFile(filePathDOF);
	if (!inputFile.is_open()) {
		throw std::runtime_error("파일을 열 수 없습니다: " + filePathDOF);
		return E_FAIL;
	}
	json jsonDOF;
	inputFile >> jsonDOF;
	m_tDofData.fAperture = jsonDOF["fAperture"];
	m_tDofData.fSensorHeight = jsonDOF["fSensorHeight"];
	m_tDofData.fFocusDistance = jsonDOF["fFocusDistance"];
	m_tDofData.fDofBrightness = jsonDOF["fDofBrightness"];
	m_tDofData.fBaseBlurPower = jsonDOF["fBaseBlurPower"];
	m_tDofData.vBlurColor.x = jsonDOF["vBlurColor"]["x"];
	m_tDofData.vBlurColor.y = jsonDOF["vBlurColor"]["y"];
	m_tDofData.vBlurColor.z = jsonDOF["vBlurColor"]["z"];
	inputFile.close();

	Compute_FocalLength();
	m_pGameInstance->UpdateConstBuffer(m_tDofData, m_pConstDofBuffer);
	m_pGameInstance->Bind_DofConstBuffer("DofConstData", m_pConstDofBuffer);
	return S_OK;
}

void CCamera::Compute_PipeLineMatrices()
{
	m_pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, XMMatrixInverse(nullptr, m_pControllerTransform->Get_WorldMatrix()));
	m_pGameInstance->Set_Transform(CPipeLine::D3DTS_PROJ, XMMatrixPerspectiveFovLH(m_fFovy,m_fAspect, m_fNear, m_fFar));
}

void CCamera::Compute_ViewMatrix()
{
	m_pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, XMMatrixInverse(nullptr, m_pControllerTransform->Get_WorldMatrix()));
}

void CCamera::Compute_ProjMatrix()
{
	m_pGameInstance->Set_Transform(CPipeLine::D3DTS_PROJ, XMMatrixPerspectiveFovLH(m_fFovy, m_fAspect, m_fNear, m_fFar));
}

void CCamera::Set_DofBufferData(const CONST_DOF& _tDofConstData, _bool _isUpdate)
{
	m_tDofData = _tDofConstData;

	if(true == _isUpdate)
	{
		Compute_FocalLength();
		m_pGameInstance->UpdateConstBuffer(_tDofConstData, m_pConstDofBuffer);
		m_pGameInstance->Bind_DofConstBuffer("DofConstData", m_pConstDofBuffer);
	}
}

void CCamera::Start_Zoom(_float _fZoomTime, ZOOM_LEVEL _eZoomLevel, RATIO_TYPE _eRatioType)
{
	//if (true == m_isZoomOn && 2 != m_eCameraType)
	//	return;

	m_isZoomOn = true;
	m_fZoomTime = { _fZoomTime, 0.f };
	m_iPreZoomLevel = m_iCurZoomLevel;
	m_iCurZoomLevel = _eZoomLevel;
	m_fStartFovy = m_fFovy;
	m_eRatioType = _eRatioType;
}

void CCamera::Start_Changing_AtOffset(_float _fAtOffsetTime, _vector _vNextAtOffset, _uint _iRatioType)
{
	//if (true == m_isChangingAtOffset && 2 != m_eCameraType)
	//	return;

	m_isChangingAtOffset = true;
	m_fAtOffsetTime = { _fAtOffsetTime, 0.f };
	m_iOffsetRatioType = _iRatioType;
	XMStoreFloat3(&m_vNextAtOffset, _vNextAtOffset);
	m_vStartAtOffset = m_vAtOffset;
}

void CCamera::Start_Shake_ByTime(_float _fShakeTime, _float _fShakeForce, _float _fShakeCycleTime, SHAKE_TYPE _ShakeType, _float _fDelayTime)
{
	if (true == m_isShake)
		return;

	m_isShake = true;
	m_eShakeType = _ShakeType;
	m_fShakeTime = { _fShakeTime, 0.f };
	m_fShakeForce = _fShakeForce;
	m_fShakeCycleTime = { _fShakeCycleTime, 0.f };
	m_fDelayTime = { _fDelayTime, 0.f };
}

void CCamera::Start_Shake_ByCount(_float _fShakeTime, _float _fShakeForce, _int _iShakeCount, SHAKE_TYPE _ShakeType, _float _fDelayTime)
{
	if (true == m_isShake)
		return;

	m_isShake = true;
	m_eShakeType = _ShakeType;
	m_fShakeTime = { _fShakeTime, 0.f };
	m_fShakeForce = _fShakeForce;
	m_fShakeCycleTime = { m_fShakeTime.x / (_float)_iShakeCount, 0.f };
	m_fDelayTime = { _fDelayTime, 0.f };
}

void CCamera::Start_PostProcessing_Fade(FADE_TYPE _eFadeType, _float _fFadeTime)
{
	if (FADE_TYPE::FADE_LAST != m_eFadeType)
		return;

	if (FADE_TYPE::FADE_LAST <= _eFadeType)
		return;

	m_eFadeType = _eFadeType;
	m_vFadeTime.x = _fFadeTime;

}

void CCamera::Start_Turn_AxisY(_float _fTurnTime, _float _fMinRotationPerSec, _float _fMaxRotationPerSec)
{
	if (nullptr == m_pCurArm)
		return;

	m_isTurnAxisY = true;
	m_CustomArmData.fMoveTimeAxisY = { _fTurnTime, 0.f };
	m_CustomArmData.fRotationPerSecAxisY = { _fMinRotationPerSec, _fMaxRotationPerSec };
}

void CCamera::Start_Turn_AxisY(_float _fTurnTime, _float _fAngle, _uint _iRatioType)
{
	if (nullptr == m_pCurArm)
		return;

	m_isTurnAxisY_Angle = true;
	m_fTurnAngle = _fAngle;
	m_iTurnAngleRatioType = _iRatioType;
	m_pCurArm->Set_PreArmAngle(0.f);
	m_pCurArm->Set_ArmTurnTime({ _fTurnTime, 0.f });
}

void CCamera::Start_Turn_AxisRight(_float _fTurnTime, _float _fMinRotationPerSec, _float _fMaxRotationPerSec)
{
	if (nullptr == m_pCurArm)
		return;

	m_isTurnAxisRight = true;
	m_CustomArmData.fMoveTimeAxisRight = { _fTurnTime, 0.f };
	m_CustomArmData.fRotationPerSecAxisRight = { _fMinRotationPerSec, _fMaxRotationPerSec };
}

void CCamera::Start_Turn_AxisRight(_float _fTurnTime, _float _fAngle, _uint _iRatioType)
{
	if (nullptr == m_pCurArm)
		return;

	m_isTurnAxisRight_Angle = true;
	m_fTurnAngle = _fAngle;
	m_iTurnAngleRatioType = _iRatioType;
	m_pCurArm->Set_PreArmAngle(0.f);
	m_pCurArm->Set_ArmTurnTime({ _fTurnTime, 0.f });
}

void CCamera::Start_Changing_ArmLength(_float _fLengthTime, _float _fLength, RATIO_TYPE _eRatioType)
{
	if (nullptr == m_pCurArm)
		return;

	m_isChangingLength = true;
	m_CustomArmData.fLengthTime = { _fLengthTime, 0.f };
	m_CustomArmData.fLength = _fLength;
	m_CustomArmData.iLengthRatioType = _eRatioType;
	m_pCurArm->Set_StartInfo();
}

void CCamera::Start_Changing_ArmVector(_float _fChangingTime, _fvector _vNextArm, RATIO_TYPE _eRatioType)
{
	if (nullptr == m_pCurArm)
		return;

	m_isTurnVector = true;
	XMStoreFloat3(&m_CustomArmData.vDesireArm, _vNextArm);
	m_CustomArmData.fMoveTimeAxisY = { _fChangingTime, 0.f };
	m_CustomArmData.iRotationRatioType = _eRatioType;
	m_pCurArm->Set_StartInfo();
}

void CCamera::Set_ResetData()
{
	if (nullptr == m_pCurArm)
		return;

	XMStoreFloat3(&m_ResetArmData.vPreArm, m_pCurArm->Get_ArmVector());
	m_ResetArmData.fPreLength = m_pCurArm->Get_Length();
	m_ResetArmData.iZoomLevel = m_iCurZoomLevel;
	m_ResetArmData.vAtOffset = m_vAtOffset;
}


HRESULT CCamera::Bind_DofConstBuffer()
{
	return m_pGameInstance->Bind_DofConstBuffer("DofConstData", m_pConstDofBuffer);
}

void CCamera::End_Shake()
{
}

void CCamera::Action_Zoom(_float _fTimeDelta)
{
	if (false == m_isZoomOn)
		return;

	_float fRatio = m_pGameInstance->Calculate_Ratio(&m_fZoomTime, _fTimeDelta, m_eRatioType);

	if (fRatio >= (1.f - EPSILON)) {
		m_isZoomOn = false;
		m_fStartFovy = 0.f;
		m_fZoomTime.y = 0.f;
	}

	m_fFovy = m_pGameInstance->Lerp(m_fStartFovy, m_ZoomLevels[m_iCurZoomLevel], fRatio);
	Compute_FocalLength();
	Bind_DofConstBuffer();
}

void CCamera::Change_AtOffset(_float _fTimeDelta)
{
	if (false == m_isChangingAtOffset)
		return;

	_float fRatio = m_pGameInstance->Calculate_Ratio(&m_fAtOffsetTime, _fTimeDelta, m_iOffsetRatioType);

	if (fRatio >= (1.f - EPSILON)) {
		m_isChangingAtOffset = false;
		m_vStartAtOffset = { 0.f, 0.f, 0.f };
		m_fAtOffsetTime.y = 0.f;
	}

	_vector vAtOffset = XMVectorLerp(XMLoadFloat3(&m_vStartAtOffset), XMLoadFloat3(&m_vNextAtOffset), fRatio);

	XMStoreFloat3(&m_vAtOffset, vAtOffset);
}

void CCamera::Action_Shake(_float _fTimeDelta)
{
	if (false == m_isShake)
		return;

	m_fDelayTime.y += _fTimeDelta;
	m_fShakeTime.y += _fTimeDelta;
	m_fShakeCycleTime.y += _fTimeDelta;

	if (m_fDelayTime.y > m_fDelayTime.x) {

		// Shake 끝
		if (m_fShakeTime.y > m_fShakeTime.x) {
			m_fDelayTime.y = 0.f;
			m_fShakeTime.y = 0.f;
			m_fShakeCycleTime.y = 0.f;

			m_isShake = 0.f;
			m_vShakeOffset = { 0.f, 0.f, 0.f };
		}

		// Shake 주기
		if (m_fShakeCycleTime.y > m_fShakeCycleTime.x) {
			_float fRandomX = m_pGameInstance->Compute_Random(-1.f * m_fShakeForce, 1.f * m_fShakeForce);
			_float fRandomY = m_pGameInstance->Compute_Random(-1.f * m_fShakeForce, 1.f * m_fShakeForce);

			switch (m_eShakeType) {
			case SHAKE_XY:
				m_vShakeOffset = { fRandomX, fRandomY, 0.f };
				break;
			case SHAKE_X:
				m_vShakeOffset = { fRandomX, 0.f, 0.f };
				break;
			case SHAKE_Y:
				m_vShakeOffset = { 0.f, fRandomY, 0.f };
				break;
			}

			m_fShakeCycleTime.y = 0.f;
		}
	}
}

void CCamera::Action_PostProcessing_Fade(_float _fTimeDelta)
{
	switch (m_eFadeType)
	{
	case Engine::CCamera::FADE_IN:
	{
		/*	FadeIn : 밝아지는 거*/
		/* fFadeRatio = m_vFadeTime.y / m_vFadeTime.x; */
		m_vFadeTime.y += _fTimeDelta;

		if (m_vFadeTime.x < m_vFadeTime.y)
		{
			m_vFadeTime.y = 0.0f;
			m_eFadeType = FADE_LAST;
			m_tDofData.fFadeRatio = 1.0f;
		}
		else
		{
			m_tDofData.fFadeRatio = m_vFadeTime.y / m_vFadeTime.x;
		}
		m_isBindConstBuffer = true; // 해당 데이터가 true이면, 이번프레임 dof를 업데이트 
	}
		break;
	case Engine::CCamera::FADE_OUT:
	{
		m_vFadeTime.y += _fTimeDelta;

		if (m_vFadeTime.x < m_vFadeTime.y)
		{
			m_vFadeTime.y = 0.0f;
			m_tDofData.fFadeRatio = 0.0f;
			m_eFadeType = FADE_LAST;
		}
		else
		{
			m_tDofData.fFadeRatio = 1.0f - (m_vFadeTime.y / m_vFadeTime.x);
		}
		m_isBindConstBuffer = true; // 해당 데이터가 true이면, 이번프레임 dof를 업데이트
	}
		break;
	case Engine::CCamera::FADE_LAST:

		break;
	default:
		break;
	}
}

void CCamera::Turn_AxisY(_float _fTimeDelta)
{
	if (nullptr == m_pCurArm)
		return;

	if (false == m_isTurnAxisY)
		return;

	if (true == m_pCurArm->Turn_AxisY(&m_CustomArmData, _fTimeDelta)) {
		m_isTurnAxisY = false;
	}
}

void CCamera::Turn_AxisY_Angle(_float _fTimeDelta)
{
	if (nullptr == m_pCurArm)
		return;

	if (false == m_isTurnAxisY_Angle)
		return;


	if (true == m_pCurArm->Turn_AxisY(m_fTurnAngle, _fTimeDelta, m_iTurnAngleRatioType)) {
		m_isTurnAxisY_Angle = false;
	}
}

void CCamera::Turn_AxisRight(_float _fTimeDelta)
{
	if (nullptr == m_pCurArm)
		return;

	if (false == m_isTurnAxisRight)
		return;

	if (true == m_pCurArm->Turn_AxisRight(&m_CustomArmData, _fTimeDelta)) {
		m_isTurnAxisRight = false;
	}
}

void CCamera::Turn_AxisRight_Angle(_float _fTimeDelta)
{
	if (nullptr == m_pCurArm)
		return;

	if (false == m_isTurnAxisRight_Angle)
		return;

	if (true == m_pCurArm->Turn_AxisRight(m_fTurnAngle, _fTimeDelta, m_iTurnAngleRatioType)) {
		m_isTurnAxisRight_Angle = false;
	}
}

void CCamera::Turn_Vector(_float _fTimeDelta)
{
	if (nullptr == m_pCurArm)
		return;

	if (false == m_isTurnVector)
		return;

	if (true == m_pCurArm->Turn_Vector(&m_CustomArmData, _fTimeDelta)) {
		m_isTurnVector = false;
	}
}


void CCamera::Change_Length(_float _fTimeDelta)
{
	if (nullptr == m_pCurArm)
		return;

	if (false == m_isChangingLength)
		return;

	if (true == m_pCurArm->Change_Length(&m_CustomArmData, _fTimeDelta)) {
		m_isChangingLength = false;
	}
}

// _isUpdate: 바로 적용할 것인지(true면 바로 적용)
// _fFadeRatio: 1이면 밝게, 0이면 어둡게
void CCamera::Set_FadeRatio(_float _fFadeRatio, _bool _isUpdate)
{
	_fFadeRatio = std::clamp(_fFadeRatio, 0.0f, 1.0f);
	m_tDofData.fFadeRatio = _fFadeRatio;

	if (true == _isUpdate)
	{
		m_pGameInstance->UpdateConstBuffer(m_tDofData, m_pConstDofBuffer);
		Bind_DofConstBuffer();
	}
}

_bool CCamera::Turn_Camera_AxisY(_float _fAngle, _float _fTurnTime, _float _fTimeDelta, _uint _iRatioType)
{
	if (false == m_isStartTurn) {
		m_fCameraTurnTime = { _fTurnTime, 0.f };
		m_fPreLookAngle = 0.f;
		m_isStartTurn = true;
	}

	_float fRatio = m_pGameInstance->Calculate_Ratio(&m_fCameraTurnTime, _fTimeDelta, _iRatioType);

	if (fRatio >= (1.f - EPSILON)) {
		m_pControllerTransform->TurnAngle(_fAngle - m_fPreLookAngle);

		m_fCameraTurnTime.y = 0.f;
		m_fPreLookAngle = 0.f;
		m_isStartTurn = false;

		return true;
	}

	_float fAngle = m_pGameInstance->Lerp(0.f, _fAngle, fRatio);
	_float fResultAngle = fAngle - m_fPreLookAngle;
	m_fPreLookAngle = fAngle;

	// Y축
	m_pControllerTransform->TurnAngle(fResultAngle);

	return false;
}
_bool CCamera::Turn_Camera_AxisRight(_float _fAngle, _float _fTurnTime, _float _fTimeDelta, _uint _iRatioType)
{
	if (false == m_isStartTurn) {
		m_fCameraTurnTime = { _fTurnTime, 0.f };
		m_fPreLookAngle = 0.f;
		m_isStartTurn = true;
	}

	_float fRatio = m_pGameInstance->Calculate_Ratio(&m_fCameraTurnTime, _fTimeDelta, _iRatioType);

	if (fRatio >= (1.f - EPSILON)) {
		m_pControllerTransform->TurnAngle(_fAngle - m_fPreLookAngle);

		m_fCameraTurnTime.y = 0.f;
		m_fPreLookAngle = 0.f;
		m_isStartTurn = false;

		return true;
	}

	_float fAngle = m_pGameInstance->Lerp(0.f, _fAngle, fRatio);
	_float fResultAngle = fAngle - m_fPreLookAngle;
	m_fPreLookAngle = fAngle;

	// Right
	_vector vLook = m_pControllerTransform->Get_State(CTransform::STATE_LOOK);
	_vector vCrossX = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	m_pControllerTransform->TurnAngle(fResultAngle, vCrossX);

	return false;
}

HRESULT CCamera::Ready_DofConstData(CAMERA_DESC* _pDesc)
{
	m_tDofData.fAperture = _pDesc->fAperture;
	m_tDofData.fFocusDistance = _pDesc->fFocusDistance;
	m_tDofData.fSensorHeight = _pDesc->fSensorHeight;
	m_tDofData.fDofBrightness = _pDesc->fDofBrightness;
	m_tDofData.fBaseBlurPower = _pDesc->fBaseBlurPower;
	m_tDofData.vBlurColor = _pDesc->vBlurColor;
	m_tDofData.fFadeRatio = 1.0f;
	Compute_FocalLength();

	if (FAILED(m_pGameInstance->CreateConstBuffer(m_tDofData, D3D11_USAGE_DYNAMIC, &m_pConstDofBuffer)))
		return E_FAIL;

	m_pGameInstance->Bind_DofConstBuffer("DofConstData", m_pConstDofBuffer);
	return S_OK;
}

HRESULT CCamera::Compute_FocalLength()
{
	_float fFocalLength = m_tDofData.fSensorHeight / 2 * tanf(m_fFovy / 2);
	m_tDofData.fFocalLength = fFocalLength;

	return S_OK;
}

void CCamera::Free()
{
	Safe_Release(m_pConstDofBuffer);
	__super::Free();
}

HRESULT CCamera::Cleanup_DeadReferences()
{
	return S_OK;
}
