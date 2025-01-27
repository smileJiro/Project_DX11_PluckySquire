#include "stdafx.h"
#include "Camera_CutScene.h"

#include "GameInstance.h"

#include "Camera_Manager_Tool.h"

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

	if(false == m_isSimulation)
		__super::Compute_PipeLineMatrices();
}

void CCamera_CutScene::CutScene_Clear()
{
	m_CutScenes.clear();
}

void CCamera_CutScene::Set_NextCutScene(_wstring _wszCutSceneName, CUTSCENE_INITIAL_DATA* _pTargetPos)
{
	m_pCurCutScene = Find_CutScene(_wszCutSceneName);

	if (nullptr == m_pCurCutScene)
		return;

	m_iSectorNum = m_pCurCutScene->size();

	for (auto& Sector : *m_pCurCutScene) {
		Safe_AddRef(Sector);
	}

	m_isStartCutScene = true;
	Initialize_CameraInfo(_pTargetPos);
}

void CCamera_CutScene::Add_CutScene(_wstring _wszCutSceneTag, vector<CCutScene_Sector*> _vecCutScene)
{
	vector<CCutScene_Sector*>* pCutScene = Find_CutScene(_wszCutSceneTag);

	if (nullptr != pCutScene)
		return;

	for (auto& Sector : _vecCutScene)
		Sector->Sort_Sector();
	
	m_CutScenes.emplace(_wszCutSceneTag, _vecCutScene);
}

void CCamera_CutScene::Play_CutScene(_float _fTimeDelta)
{
	if (nullptr == m_pCurCutScene || true == m_isStartCutScene || true == m_isFinishCutScene)
		return;

	_vector vPosition;

	_bool isSectorFinish = (*m_pCurCutScene)[m_iCurSectorIndex]->Play_Sector(_fTimeDelta, &vPosition);
	if (true == isSectorFinish) {
		m_iCurSectorIndex++;

		if (m_iCurSectorIndex >= m_iSectorNum) {
			m_iCurSectorIndex = 0;
			m_isFinishCutScene = true;
			m_vAtOffset = { 0.f, 0.f, 0.f };

			for (auto& Sector : *m_pCurCutScene) {
				Safe_Release(Sector);
			}

			m_pCurCutScene = nullptr;
		}
		else {
			Change_Sector();
		}

		return;
	}

	m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vPosition, 1.f));
	Process_Movement(_fTimeDelta);
	Save_Data();

#ifdef _DEBUG
	XMStoreFloat3(&m_vSimulationPos, XMVectorSetW(vPosition, 1.f));
#endif
}

void CCamera_CutScene::Change_Sector()
{
	// 이거 필요한가? 어차ㅠ피 바로 ++로 바꾸는데 아무튼

	// 초기 LookAt 설정
	Initialize_CameraInfo(nullptr);
}

vector<CCutScene_Sector*>* CCamera_CutScene::Find_CutScene(_wstring _wszCutSceneName)
{
	auto iter = m_CutScenes.find(_wszCutSceneName);

	if (iter == m_CutScenes.end())
		return nullptr;

	return &(iter->second);
}

void CCamera_CutScene::Before_CutScene(_float _fTimeDelta)
{
	if (true == m_isInitialData) {
		// Initial 데이터랑 KeyFrame 혹은 vector<m_CutSceneDatas> 첫 번째 값을 보간함
		CUTSCENE_KEYFRAME tKeyFrame = (*m_pCurCutScene)[m_iCurSectorIndex]->Get_KeyFrame(1);

		m_InitialTime.y += _fTimeDelta;
		_float fRatio = m_InitialTime.y / m_InitialTime.x;

		if (fRatio > 1.f) {
			m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&tKeyFrame.vPosition), 1.f));
			m_iCurZoomLevel = tKeyFrame.iZoomLevel;
			m_fFovy = m_ZoomLevels[tKeyFrame.iZoomLevel];
			m_InitialTime.y = 0.f;
			m_isInitialData = false;
			m_isStartCutScene = false;

			return;
		}

		_vector vPos = XMVectorLerp(XMLoadFloat3(&m_tInitialData.vPosition), XMLoadFloat3(&tKeyFrame.vPosition), fRatio);
		_float fFovy = m_pGameInstance->Lerp(m_ZoomLevels[m_tInitialData.iZoomLevel], m_ZoomLevels[tKeyFrame.iZoomLevel], fRatio);
		m_pControllerTransform->LookAt_3D(XMVectorSetW(XMLoadFloat3(&m_tInitialData.vAtOffset), 1.f));

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
	m_fSimulationTime.y += _fTimeDelta;

	if (m_fSimulationTime.y > m_fSimulationTime.x) {
		m_isFinishCutScene = false;
		m_fSimulationTime.y = 0.f;
		CCamera_Manager_Tool::GetInstance()->Change_CameraType(CCamera_Manager_Tool::FREE);
	}
}

void CCamera_CutScene::Process_Movement(_float _fTimeDelta)
{
	if (false == (*m_pCurCutScene)[m_iCurSectorIndex]->Get_IsLookAt())
		return;

	if (true == (*m_pCurCutScene)[m_iCurSectorIndex]->Get_IsChangeKeyFrame()) {
		_float fTimeOffset = (*m_pCurCutScene)[m_iCurSectorIndex]->Get_TimeOffset();
		_int iCurKeyFrameIdx = (*m_pCurCutScene)[m_iCurSectorIndex]->Get_CurKeyFrameIndex();
		vector<CUTSCENE_KEYFRAME>* pKeyFrames = (*m_pCurCutScene)[m_iCurSectorIndex]->Get_KeyFrames();

		if (iCurKeyFrameIdx >= (_int)((*pKeyFrames).size() - 2))
			return;

		Start_Changing_AtOffset(fTimeOffset, XMLoadFloat3(&(*pKeyFrames)[iCurKeyFrameIdx + 1].vAtOffset), (*pKeyFrames)[iCurKeyFrameIdx + 1].iAtRatioType);
		Start_Zoom(fTimeOffset, (CCamera::ZOOM_LEVEL)(*pKeyFrames)[iCurKeyFrameIdx + 1].iZoomLevel, (CCamera::RATIO_TYPE)(*pKeyFrames)[iCurKeyFrameIdx + 1].iZoomRatioType);
	}

	_vector vAt = XMLoadFloat3(&m_vTargetPos) + XMLoadFloat3(&m_vAtOffset) + XMLoadFloat3(&m_vShakeOffset);
	
	cout << "Offset: " << m_vAtOffset.x << "   " << m_vAtOffset.y << "   " << m_vAtOffset.z << endl;

	m_pControllerTransform->LookAt_3D(XMVectorSetW(vAt, 1.f));
}

void CCamera_CutScene::Initialize_CameraInfo(CUTSCENE_INITIAL_DATA* _pTargetPos)
{
	if (nullptr == m_pCurCutScene)
		return;

	if (nullptr == _pTargetPos) {
		// 초기 보는 곳 설정(첫 dummy Frame)
		CUTSCENE_KEYFRAME tKeyFrame = (*m_pCurCutScene)[m_iCurSectorIndex]->Get_KeyFrame(1);

		m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&tKeyFrame.vPosition));
		m_pControllerTransform->LookAt_3D(XMVectorSetW(XMLoadFloat3(&tKeyFrame.vAtOffset), 1.f));
		m_vAtOffset = { 0.f, 0.f, 0.f };
		m_vTargetPos = tKeyFrame.vAtOffset;

		// 초기 Zoom Level 설정
		m_iCurZoomLevel = tKeyFrame.iZoomLevel;
		m_fFovy = m_ZoomLevels[m_iCurZoomLevel];
	}
	else {
		m_tInitialData = *_pTargetPos;
		m_isInitialData = true;
		m_InitialTime.x = 0.3f;
		// 초기 위치 설정
		m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_tInitialData.vPosition));
		
		// 초기 보는 곳 설정
		m_pControllerTransform->LookAt_3D(XMVectorSetW(XMLoadFloat3(&m_tInitialData.vAtOffset), 1.f));
		m_vTargetPos = m_tInitialData.vAtOffset;
		m_vAtOffset = { 0.f, 0.f, 0.f };
		
		// 초기 Zoom Level 설정
		m_iCurZoomLevel = m_tInitialData.iZoomLevel;
		m_fFovy = m_ZoomLevels[m_tInitialData.iZoomLevel];
	}
}

void CCamera_CutScene::Save_Data()
{
	CUTSCENE_DATA tData = {};
	XMStoreFloat3(&tData.vPosition, m_pControllerTransform->Get_State(CTransform::STATE_POSITION));
	tData.vRotation = {};
	
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
	if (nullptr != m_pCurCutScene) {
		for (auto& Sector : *m_pCurCutScene) {
			Safe_Release(Sector);
		}
	}

	for (auto& CutScene : m_CutScenes) {
		for (auto& Sector : CutScene.second) {
			Safe_Release(Sector);
		}
	}

	__super::Free();
}

