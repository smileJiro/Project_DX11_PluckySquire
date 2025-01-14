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
	m_arrZoomFovy[(int)ZOOM_LEVEL::IN_LEVEL2] = XMConvertToRadians(25.f);
	m_arrZoomFovy[(int)ZOOM_LEVEL::IN_LEVEL1] = XMConvertToRadians(45.f);
	m_arrZoomFovy[(int)ZOOM_LEVEL::NORMAL] = XMConvertToRadians(60.f);
	m_arrZoomFovy[(int)ZOOM_LEVEL::OUT_LEVEL1] = XMConvertToRadians(75.f);
	m_arrZoomFovy[(int)ZOOM_LEVEL::OUT_LEVEL2] = XMConvertToRadians(95.f);

	if (nullptr == _pArg)
		return E_FAIL;

	// Transform 먼저 생성.
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	CAMERA_DESC* pDesc = static_cast<CAMERA_DESC*>(_pArg);

	/* 뷰 행렬에 대한 트랜스폼 동기화 작업. */
	m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vEye), 1.f));
	m_pControllerTransform->LookAt_3D(XMVectorSetW(XMLoadFloat3(&pDesc->vAt), 1.f));
	
	/* 투영 행렬 구성 시 필요한 데이터를 저장. */
	m_eCameraType = pDesc->eCameraType;
	m_iCurZoomLevel = (_uint)pDesc->eZoomLevel;
	m_fFovy = m_arrZoomFovy[m_iCurZoomLevel];
	m_fAspect = pDesc->fAspect;
	m_fNear = pDesc->fNear;
	m_fFar = pDesc->fFar;


	m_pGameInstance->Set_NearFarZ(_float2(m_fNear, m_fFar));



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
}

HRESULT CCamera::Render()
{
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


void CCamera::ZoomIn(_float _fZoomTime)
{

	if (!m_bZoomOn)
	{
		if (m_iCurZoomLevel > (_int)ZOOM_LEVEL::IN_LEVEL2)
		{
			m_iPreZoomLevel = m_iCurZoomLevel;
			m_iCurZoomLevel -= 1;
			m_bZoomOn = true;
			m_eCamState = STATE_ZOOM_IN;
			m_fStartFovy = m_fFovy;
			m_vZoomTime.x = _fZoomTime;
			m_vZoomTime.y = 0.0f;
		}

	}
}

void CCamera::ZoomIn(_float _fZoomTime, ZOOM_LEVEL _eLevel)
{
	if (m_iCurZoomLevel == _eLevel)
		return;

	if (!m_bZoomOn)
	{
		if (m_iCurZoomLevel > (_int)ZOOM_LEVEL::IN_LEVEL2)
		{
			m_iPreZoomLevel = m_iCurZoomLevel;
			m_iCurZoomLevel = (int)_eLevel; // 줌레벨 지정용 줌인
			m_bZoomOn = true;
			m_eCamState = STATE_ZOOM_IN;
			m_fStartFovy = m_fFovy;
			m_vZoomTime.x = _fZoomTime;
			m_vZoomTime.y = 0.0f;
		}
	}
}

void CCamera::ZoomOut(_float _fZoomTime)
{
	if (!m_bZoomOn)
	{
		if (m_iCurZoomLevel < (_int)ZOOM_LEVEL::OUT_LEVEL2)
		{
			m_iPreZoomLevel = m_iCurZoomLevel;
			m_iCurZoomLevel += 1;
			m_bZoomOn = true;
			m_eCamState = STATE_ZOOM_OUT;
			m_vZoomTime.x = _fZoomTime;
			m_vZoomTime.y = 0.0f;
		}

	}
}

void CCamera::ZoomOut(_float _fZoomTime, ZOOM_LEVEL _eLevel)
{
	if (m_iCurZoomLevel == _eLevel)
		return;

	if (!m_bZoomOn)
	{
		if (m_iCurZoomLevel < (_int)ZOOM_LEVEL::OUT_LEVEL2)
		{
			m_iPreZoomLevel = m_iCurZoomLevel;
			m_iCurZoomLevel = (int)_eLevel;
			m_bZoomOn = true;
			m_eCamState = STATE_ZOOM_OUT;
			m_vZoomTime.x = _fZoomTime;
			m_vZoomTime.y = 0.0f;
		}

	}
}

void CCamera::Start_Shake(SHAKE_TYPE _eType, _float _fShakeTime, _int _iShakeCount, _float _fPower)
{
	if (true == m_isShake)
		return;

	m_isShake = true;
	m_eShakeType = _eType;
	m_iShakeCount = _iShakeCount;
	m_fShakePower = _fPower;
	m_vShakeTime.x = _fShakeTime;
	m_vShakeTime.y = 0.0f;

	m_vShakeCycleTime.x = _fShakeTime / (float)_iShakeCount;
	m_vShakeCycleTime.y = 0.0f;
}

void CCamera::End_Shake()
{
}

void CCamera::Action_Zoom(_float _fTimeDelta)
{
	if (!m_bZoomOn)
		return;

	switch (m_eCamState)
	{
	case Engine::CCamera::STATE_NONE:
		break;
	case Engine::CCamera::STATE_ZOOM_IN:
	{
		m_vZoomTime.y += _fTimeDelta;
		m_fFovy = m_arrZoomFovy[m_iPreZoomLevel] + (m_arrZoomFovy[m_iCurZoomLevel] - m_arrZoomFovy[m_iPreZoomLevel]) * (m_vZoomTime.y / m_vZoomTime.x);
		if (m_vZoomTime.x <= m_vZoomTime.y)
		{
			m_vZoomTime.y = 0.0f;
			m_fFovy = m_arrZoomFovy[m_iCurZoomLevel];
			m_bZoomOn = false;
			m_eCamState = STATE_NONE;
		}
	}
		break;
	case Engine::CCamera::STATE_ZOOM_OUT:
	{
		m_vZoomTime.y += _fTimeDelta;
		m_fFovy = m_arrZoomFovy[m_iPreZoomLevel] + (m_arrZoomFovy[m_iCurZoomLevel] - m_arrZoomFovy[m_iPreZoomLevel]) * (m_vZoomTime.y / m_vZoomTime.x);
		if (m_vZoomTime.x <= m_vZoomTime.y)
		{
			m_vZoomTime.y = 0.0f;
			m_fFovy = m_arrZoomFovy[m_iCurZoomLevel];
			m_bZoomOn = false;
			m_eCamState = STATE_NONE;
		}
	}
		break;

	}

}

void CCamera::Action_Shake(_float _fTimeDelta)
{
	if (false == m_isShake)
		return;

	m_vShakeTime.y += _fTimeDelta;
	m_vShakeCycleTime.y += _fTimeDelta;

	if (m_vShakeCycleTime.x <= m_vShakeCycleTime.y)
		m_vShakeCycleTime.y = 0.0f;
	else
	{
		return;
	}


	if (m_vShakeTime.x <= m_vShakeTime.y)
	{
		m_vShakeTime.y = 0.0f;
		m_vShakeCycleTime.y = 0.0f;
		m_isShake = false;
		m_iShakeCount = 0;
		m_iShakeCountAcc = 0;
		m_vShakeMovement = { 0.0f ,0.0f ,0.0f };
		return;
	}
	
	m_vShakeMovement.x = m_pGameInstance->Compute_Random(m_fShakePower * -1.0f, m_fShakePower);
	m_vShakeMovement.y = m_pGameInstance->Compute_Random(m_fShakePower * -1.0f, m_fShakePower);

}

void CCamera::Free()
{
	__super::Free();
}

HRESULT CCamera::Safe_Release_DeadObjects()
{
	return S_OK;
}
