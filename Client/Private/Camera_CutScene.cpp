#include "stdafx.h"
#include "Camera_CutScene.h"

#include "GameInstance.h"

#include "Camera_Manager.h"

CCamera_CutScene::CCamera_CutScene(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCamera{ _pDevice, _pContext }
{
}

CCamera_CutScene::CCamera_CutScene(const CCamera_CutScene& _Prototype)
	: CCamera{ _Prototype }
{
}

HRESULT CCamera_CutScene::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_CutScene::Initialize(void* _pArg)
{
	CAMERA_DESC* pDesc = static_cast<CAMERA_DESC*>(_pArg);

	pDesc->eStartCoord = COORDINATE_3D;
	pDesc->isCoordChangeEnable = false;
	pDesc->tTransform3DDesc.fSpeedPerSec = 10.f;
	pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(180.f);

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	return S_OK;
}

void CCamera_CutScene::Priority_Update(_float _fTimeDelta)
{
}

void CCamera_CutScene::Update(_float _fTimeDelta)
{

}

void CCamera_CutScene::Late_Update(_float _fTimeDelta)
{
	Before_CutScene(_fTimeDelta);
	Play_CutScene(_fTimeDelta);
	After_CutScene(_fTimeDelta);

	Action_Zoom(_fTimeDelta);
	Action_Shake(_fTimeDelta);
	Change_AtOffset(_fTimeDelta);

	__super::Compute_PipeLineMatrices();
}

_bool CCamera_CutScene::Set_NextCutScene(_wstring _wszCutSceneName)
{
	m_pCurCutScene = Find_CutScene(_wszCutSceneName);

	if (nullptr == m_pCurCutScene)
		return false;

	m_isStartCutScene = true;
	

	return true;
}

void CCamera_CutScene::Add_CutScene(_wstring _wszCutSceneTag, pair<_float2, vector<CUTSCENE_DATA>> _CutSceneData)
{
	pair<_float2, vector<CUTSCENE_DATA>>* pData = Find_CutScene(_wszCutSceneTag);

	if (nullptr != pData)
		return;

	m_CutSceneDatas.emplace(_wszCutSceneTag, _CutSceneData);
}

void CCamera_CutScene::Switch_CameraView(INITIAL_DATA* _pInitialData)
{
	if (nullptr == m_pCurCutScene)
		return;

	if (nullptr == _pInitialData) {
		// 초기 보는 곳 설정(첫 dummy Frame)

		CUTSCENE_DATA tCutSceneData = m_pCurCutScene->second[0];

		m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&tCutSceneData.vPosition));
		m_pControllerTransform->LookAt_3D(XMVectorSetW(XMLoadFloat3(&tCutSceneData.vAtOffset), 1.f));
		m_vTargetPos = { 0.f, 0.f, 0.f };
		m_vAtOffset = tCutSceneData.vAtOffset;

		// 초기 Zoom Level 설정
		//m_iCurZoomLevel = tCutSceneData.iZoomLevel;
		m_fFovy = tCutSceneData.fFovy;
	}
	else {
		m_tInitialData = *_pInitialData;
		m_isInitialData = true;
		m_InitialTime.x = 0.3f;
		// 초기 위치 설정
		m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_tInitialData.vPosition));

		// 초기 보는 곳 설정
		m_pControllerTransform->LookAt_3D(XMVectorSetW(XMLoadFloat3(&m_tInitialData.vAt), 1.f));
		m_vTargetPos = m_tInitialData.vAt;
		m_vAtOffset = { 0.f, 0.f, 0.f };

		// 초기 Zoom Level 설정
		m_iCurZoomLevel = m_tInitialData.iZoomLevel;
		m_fFovy = m_ZoomLevels[m_tInitialData.iZoomLevel];
	}
}

INITIAL_DATA CCamera_CutScene::Get_InitialData()
{
	INITIAL_DATA tData;

	XMStoreFloat3(&tData.vPosition, m_pControllerTransform->Get_State(CTransform::STATE_POSITION));

	_vector vAt = XMLoadFloat3(&m_vTargetPos) + XMLoadFloat3(&m_vAtOffset) + XMLoadFloat3(&m_vShakeOffset);
	XMStoreFloat3(&tData.vAt, vAt);

	tData.iZoomLevel = m_iCurZoomLevel;

	return tData;
}

void CCamera_CutScene::Play_CutScene(_float _fTimeDelta)
{
	if (nullptr == m_pCurCutScene || true == m_isStartCutScene || true == m_isFinishCutScene)
		return;

	//_vector vPosition;

	m_pCurCutScene->first.y += _fTimeDelta;
	_float fRatio = m_pCurCutScene->first.y / m_pCurCutScene->first.x;

	if (fRatio > 1.f) {
		_uint iLastIndex = (_uint)m_pCurCutScene->second.size() - 1;
		
		m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_pCurCutScene->second[iLastIndex].vPosition), 1.f));
		_vector vAt = XMLoadFloat3(&m_vTargetPos) + XMLoadFloat3(&m_pCurCutScene->second[iLastIndex].vAtOffset);

		m_pControllerTransform->LookAt_3D(XMVectorSetW(vAt, 1.f));
		m_fFovy = m_pCurCutScene->second[iLastIndex].fFovy;

		// Reset
		m_pCurCutScene->first.y = 0.f;
		m_isFinishCutScene = true;
		m_vAtOffset = { 0.f, 0.f, 0.f };

		m_pCurCutScene = nullptr;
		return;
	}
	
	_uint iIndex = (_uint)m_pGameInstance->Lerp(0, (_float)(m_pCurCutScene->second.size()), fRatio);
	
	m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_pCurCutScene->second[iIndex].vPosition), 1.f));
	_vector vAt = XMLoadFloat3(&m_vTargetPos) + XMLoadFloat3(&m_pCurCutScene->second[iIndex].vAtOffset);

	m_pControllerTransform->LookAt_3D(XMVectorSetW(vAt, 1.f));
	m_fFovy = m_pCurCutScene->second[iIndex].fFovy;
}

void CCamera_CutScene::Change_Sector()
{
	// 이거 필요한가? 어차ㅠ피 바로 ++로 바꾸는데 아무튼

	// 초기 LookAt 설정
	Switch_CameraView(nullptr);
}

pair<_float2, vector<CUTSCENE_DATA>>* CCamera_CutScene::Find_CutScene(_wstring _wszCutSceneName)
{
	auto iter = m_CutSceneDatas.find(_wszCutSceneName);

	if (iter == m_CutSceneDatas.end())
		return nullptr;

	return &(iter->second);
}

void CCamera_CutScene::Before_CutScene(_float _fTimeDelta)
{
	if (true == m_isInitialData) {
		// Initial 데이터랑 KeyFrame 혹은 vector<m_CutSceneDatas> 첫 번째 값을 보간함
		CUTSCENE_DATA tData = m_pCurCutScene->second[0];

		m_InitialTime.y += _fTimeDelta;
		_float fRatio = m_InitialTime.y / m_InitialTime.x;

		if (fRatio > 1.f) {
			m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&tData.vPosition), 1.f));
			m_fFovy = tData.fFovy;
			m_InitialTime.y = 0.f;
			m_isInitialData = false;
			m_isStartCutScene = false;

			return;
		}

		_vector vPos = XMVectorLerp(XMLoadFloat3(&m_tInitialData.vPosition), XMLoadFloat3(&tData.vPosition), fRatio);
		_float fFovy = m_pGameInstance->Lerp(m_ZoomLevels[m_tInitialData.iZoomLevel], tData.fFovy, fRatio);
		// Data의 첫번째 vAtOffset은 보는 곳임
		// 근데 At은 왜 Lerp를 안 했지? 나중에 이상하면 고치기
		m_pControllerTransform->LookAt_3D(XMVectorSetW(XMLoadFloat3(&tData.vAtOffset), 1.f));

		m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vPos, 1.f));
		m_fFovy = fFovy;
	}
	else {
		// 미리 만들어 놓음
		m_isStartCutScene = false;
	}
}

void CCamera_CutScene::After_CutScene(_float _fTimeDelta)
{
	if (false == m_isFinishCutScene)
		return;

	// 해당 함수까지 끝난다면 카메라 전환 등 동작 수행
	CCamera_Manager::GetInstance()->Change_CameraType(CCamera_Manager::TARGET);
	m_isFinishCutScene = false;
	
	/*m_fSimulationTime.y += _fTimeDelta;

	if (m_fSimulationTime.y > m_fSimulationTime.x) {
		m_isFinishCutScene = false;
		m_fSimulationTime.y = 0.f;
		
	}*/
}

CCamera_CutScene* CCamera_CutScene::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CCamera_CutScene* pInstance = new CCamera_CutScene(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera_CutScene");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_CutScene::Clone(void* _pArg)
{
	CCamera_CutScene* pInstance = new CCamera_CutScene(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_CutScene");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_CutScene::Free()
{
	__super::Free();
}