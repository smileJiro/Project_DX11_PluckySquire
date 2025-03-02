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

//void CCamera::ZoomIn(_float _fZoomTime)
//{
//
//	if (!m_bZoomOn)
//	{
//		if (m_iCurZoomLevel > (_int)ZOOM_LEVEL::IN_LEVEL2)
//		{
//			m_iPreZoomLevel = m_iCurZoomLevel;
//			m_iCurZoomLevel -= 1;
//			m_bZoomOn = true;
//			m_eCamState = STATE_ZOOM_IN;
//			m_fStartFovy = m_fFovy;
//			m_vZoomTime.x = _fZoomTime;
//			m_vZoomTime.y = 0.0f;
//		}
//
//	}
//}
//
//void CCamera::ZoomIn(_float _fZoomTime, ZOOM_LEVEL _eLevel)
//{
//	if (m_iCurZoomLevel == _eLevel)
//		return;
//
//	if (!m_bZoomOn)
//	{
//		if (m_iCurZoomLevel > (_int)ZOOM_LEVEL::IN_LEVEL2)
//		{
//			m_iPreZoomLevel = m_iCurZoomLevel;
//			m_iCurZoomLevel = (int)_eLevel; // 줌레벨 지정용 줌인
//			m_bZoomOn = true;
//			m_eCamState = STATE_ZOOM_IN;
//			m_fStartFovy = m_fFovy;
//			m_vZoomTime.x = _fZoomTime;
//			m_vZoomTime.y = 0.0f;
//		}
//	}
//}
//
//void CCamera::ZoomOut(_float _fZoomTime)
//{
//	if (!m_bZoomOn)
//	{
//		if (m_iCurZoomLevel < (_int)ZOOM_LEVEL::OUT_LEVEL2)
//		{
//			m_iPreZoomLevel = m_iCurZoomLevel;
//			m_iCurZoomLevel += 1;
//			m_bZoomOn = true;
//			m_eCamState = STATE_ZOOM_OUT;
//			m_vZoomTime.x = _fZoomTime;
//			m_vZoomTime.y = 0.0f;
//		}
//
//	}
//}
//
//void CCamera::ZoomOut(_float _fZoomTime, ZOOM_LEVEL _eLevel)
//{
//	if (m_iCurZoomLevel == _eLevel)
//		return;
//
//	if (!m_bZoomOn)
//	{
//		if (m_iCurZoomLevel < (_int)ZOOM_LEVEL::OUT_LEVEL2)
//		{
//			m_iPreZoomLevel = m_iCurZoomLevel;
//			m_iCurZoomLevel = (int)_eLevel;
//			m_bZoomOn = true;
//			m_eCamState = STATE_ZOOM_OUT;
//			m_vZoomTime.x = _fZoomTime;
//			m_vZoomTime.y = 0.0f;
//		}
//
//	}
//}

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

//void CCamera::Start_Shake(SHAKE_TYPE _eType, _float _fShakeTime, _int _iShakeCount, _float _fPower)
//{
//	if (true == m_isShake)
//		return;
//
//	m_isShake = true;
//	m_eShakeType = _eType;
//	m_iShakeCount = _iShakeCount;
//	m_fShakePower = _fPower;
//	m_vShakeTime.x = _fShakeTime;
//	m_vShakeTime.y = 0.0f;
//
//	m_vShakeCycleTime.x = _fShakeTime / (float)_iShakeCount;
//	m_vShakeCycleTime.y = 0.0f;
//}

HRESULT CCamera::Bind_DofConstBuffer()
{
	return m_pGameInstance->Bind_DofConstBuffer("DofConstData", m_pConstDofBuffer);
}

void CCamera::End_Shake()
{
}

//void CCamera::Action_Zoom(_float _fTimeDelta)
//{
//	if (!m_bZoomOn)
//		return;
//
//	switch (m_eCamState)
//	{
//	case Engine::CCamera::STATE_NONE:
//		break;
//	case Engine::CCamera::STATE_ZOOM_IN:
//	{
//		m_vZoomTime.y += _fTimeDelta;
//		m_fFovy = m_arrZoomFovy[m_iPreZoomLevel] + (m_arrZoomFovy[m_iCurZoomLevel] - m_arrZoomFovy[m_iPreZoomLevel]) * (m_vZoomTime.y / m_vZoomTime.x);
//		if (m_vZoomTime.x <= m_vZoomTime.y)
//		{
//			m_vZoomTime.y = 0.0f;
//			m_fFovy = m_arrZoomFovy[m_iCurZoomLevel];
//			m_bZoomOn = false;
//			m_eCamState = STATE_NONE;
//		}
//	}
//		break;
//	case Engine::CCamera::STATE_ZOOM_OUT:
//	{
//		m_vZoomTime.y += _fTimeDelta;
//		m_fFovy = m_arrZoomFovy[m_iPreZoomLevel] + (m_arrZoomFovy[m_iCurZoomLevel] - m_arrZoomFovy[m_iPreZoomLevel]) * (m_vZoomTime.y / m_vZoomTime.x);
//		if (m_vZoomTime.x <= m_vZoomTime.y)
//		{
//			m_vZoomTime.y = 0.0f;
//			m_fFovy = m_arrZoomFovy[m_iCurZoomLevel];
//			m_bZoomOn = false;
//			m_eCamState = STATE_NONE;
//		}
//	}
//		break;
//
//	}
//
//}

void CCamera::Action_Zoom(_float _fTimeDelta)
{
	if (false == m_isZoomOn)
		return;

	_float fRatio = Calculate_Ratio(&m_fZoomTime, _fTimeDelta, m_eRatioType);

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

	_float fRatio = Calculate_Ratio(&m_fAtOffsetTime, _fTimeDelta, m_iOffsetRatioType);

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
			m_tDofData.fFadeRatio = (1.0f - m_vFadeTime.y) / m_vFadeTime.x;
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

//void CCamera::Action_Shake(_float _fTimeDelta)
//{
//	if (false == m_isShake)
//		return;
//
//	m_vShakeTime.y += _fTimeDelta;
//	m_vShakeCycleTime.y += _fTimeDelta;
//
//	if (m_vShakeCycleTime.x <= m_vShakeCycleTime.y)
//		m_vShakeCycleTime.y = 0.0f;
//	else
//	{
//		return;
//	}
//
//
//	if (m_vShakeTime.x <= m_vShakeTime.y)
//	{
//		m_vShakeTime.y = 0.0f;
//		m_vShakeCycleTime.y = 0.0f;
//		m_isShake = false;
//		m_iShakeCount = 0;
//		m_iShakeCountAcc = 0;
//		m_vShakeMovement = { 0.0f ,0.0f ,0.0f };
//		return;
//	}
//	
//	m_vShakeMovement.x = m_pGameInstance->Compute_Random(m_fShakePower * -1.0f, m_fShakePower);
//	m_vShakeMovement.y = m_pGameInstance->Compute_Random(m_fShakePower * -1.0f, m_fShakePower);
//
//}

_float CCamera::Calculate_Ratio(_float2* _fTime, _float _fTimeDelta, _uint _iRatioType)
{
	_float fRatio = {};

	_fTime->y += _fTimeDelta;
	fRatio = _fTime->y / _fTime->x;
	fRatio = clamp(fRatio, 0.f, 1.f);

	switch (_iRatioType) {
	case (_uint)RATIO_TYPE::EASE_IN:
		//fRatio = (fRatio + (_float)pow((_double)fRatio, (_double)2.f)) * 0.5f;
		fRatio = fRatio * fRatio;
		break;
	case (_uint)RATIO_TYPE::EASE_OUT:
		//fRatio = 1.0f - ((1.0f - fRatio) + (_float)pow((_double)(1.0f - fRatio), 2.f)) * 0.5f;
		fRatio = 1.0f - (1.0f - fRatio) * (1.0f - fRatio);
		break;
	case (_uint)RATIO_TYPE::LERP:
		break;
	case (_uint)RATIO_TYPE::EASE_IN_OUT:
		fRatio = fRatio * fRatio * (3.f - 2.f * fRatio);
		break;
	}
	return fRatio;
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
