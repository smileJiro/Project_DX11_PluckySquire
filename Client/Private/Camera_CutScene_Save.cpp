#include "stdafx.h"
#include "Camera_CutScene_Save.h"
#include "GameInstance.h"
#include "Camera_Manager.h"

CCamera_CutScene_Save::CCamera_CutScene_Save(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CCamera{ _pDevice, _pContext }
{
}

CCamera_CutScene_Save::CCamera_CutScene_Save(const CCamera_CutScene_Save& _Prototype)
    : CCamera{ _Prototype }
{
}

HRESULT CCamera_CutScene_Save::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCamera_CutScene_Save::Initialize(void* _pArg)
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

void CCamera_CutScene_Save::Priority_Update(_float _fTimeDelta)
{
}

void CCamera_CutScene_Save::Update(_float _fTimeDelta)
{
}

void CCamera_CutScene_Save::Late_Update(_float _fTimeDelta)
{
	Action_Zoom(_fTimeDelta);
	Action_Shake(_fTimeDelta);
	Change_AtOffset(_fTimeDelta);
	Action_LookAt(_fTimeDelta);

	Before_CutScene(_fTimeDelta);
	Play_CutScene(_fTimeDelta);
	After_CutScene(_fTimeDelta);

	if (false == m_isSimulation)
		__super::Compute_PipeLineMatrices();
}

void CCamera_CutScene_Save::CutScene_Clear()
{
	if(!m_CutScenes.empty())
		m_CutScenes.clear();
	//if (!m_CutSceneSubDatas.empty())

	for (auto it = m_CutSceneSubDatas.begin(); it != m_CutSceneSubDatas.end(); ++it) {
		_wstring a = it->first;
	}

	cout << &m_CutSceneSubDatas << endl;

	map<std::wstring, CUTSCENE_SUB_DATA>().swap(m_CutSceneSubDatas);
}

void CCamera_CutScene_Save::Set_NextCutScene(_wstring _wszCutSceneName, INITIAL_DATA* _pTargetPos)
{
	m_pCurCutScene = Find_CutScene(_wszCutSceneName);

	if (nullptr == m_pCurCutScene)
		return;

	m_iSectorNum = (_uint)m_pCurCutScene->size();
	m_wszCurCutSceneTag = _wszCutSceneName;

	for (auto& Sector : *m_pCurCutScene) {
		Safe_AddRef(Sector);
	}

	m_isStartCutScene = true;
	Initialize_CameraInfo(_pTargetPos);

	// 있다면, 이전에 저장해 둔 CutScene Data 지우기
	pair<CUTSCENE_SUB_DATA, vector<CUTSCENE_DATA>>* pData = Find_CutSceneData(_wszCutSceneName);
	if (nullptr != pData)
		(*pData).second.clear();
	else {
		pair<CUTSCENE_SUB_DATA, vector<CUTSCENE_DATA>> Data;
		m_CutSceneDatas.emplace(_wszCutSceneName, Data);
	}

	// PrePosition 저장, Sector 이동할 떄도 해 줘야 할 것 같음
	m_vPrePosition = (*m_pCurCutScene)[m_iCurSectorIndex]->Get_KeyFrame(1).vPosition;

}

void CCamera_CutScene_Save::Add_CutScene(_wstring _wszCutSceneTag, vector<CCutScene_Sector*> _vecCutScene, CUTSCENE_SUB_DATA _tSubData)
{
	vector<CCutScene_Sector*>* pCutScene = Find_CutScene(_wszCutSceneTag);

	if (nullptr != pCutScene)
		return;

	for (auto& Sector : _vecCutScene)
		Sector->Sort_Sector();

	m_CutScenes.emplace(_wszCutSceneTag, _vecCutScene);

	_int i = _ITERATOR_DEBUG_LEVEL;

	cout << &m_CutSceneSubDatas << endl;
	_int a = (_int)m_CutSceneSubDatas.size();

	m_CutSceneSubDatas.try_emplace(_wszCutSceneTag, _tSubData);
	//m_CutSceneSubDatas.emplace(_wszCutSceneTag, _tSubData);
}

void CCamera_CutScene_Save::Play_CutScene(_float _fTimeDelta)
{
	if (nullptr == m_pCurCutScene || true == m_isStartCutScene || true == m_isFinishCutScene)
		return;

	_vector vPosition, vAt;

	_bool isSectorFinish = (*m_pCurCutScene)[m_iCurSectorIndex]->Play_Sector(_fTimeDelta, &vPosition, &vAt);
	if (true == isSectorFinish) {
		m_iCurSectorIndex++;

		if (m_iCurSectorIndex >= m_iSectorNum) {
			m_iCurSectorIndex = 0;
			m_isFinishCutScene = true;
			m_vAtOffset = { 0.f, 0.f, 0.f };

			_float fTotalTime = {};

			for (auto& Sector : *m_pCurCutScene) {
				fTotalTime += Sector->Get_LastTimeStamp();
				Safe_Release(Sector);
			}

			m_pCurCutScene = nullptr;

			pair<CUTSCENE_SUB_DATA, vector<CUTSCENE_DATA>>* pData = Find_CutSceneData(m_wszCurCutSceneTag);

			if (nullptr == pData)
				return;
			pData->first.fTotalTime = { fTotalTime, 0.f };
			
			auto Subiter = m_CutSceneSubDatas.find(m_wszCurCutSceneTag);

			pData->first.iNextCameraType = (*Subiter).second.iNextCameraType;
		}
		else {
			Change_Sector();
		}

		return;
	}

	m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vPosition, 1.f));
	
	vAt = vAt + XMLoadFloat3(&m_vAtOffset) + XMLoadFloat3(&m_vShakeOffset);

	m_pControllerTransform->LookAt_3D(XMVectorSetW(vAt, 1.f));
	XMStoreFloat3(&m_vAt, vAt);

	Process_Movement(_fTimeDelta);
	Save_Data();

	XMStoreFloat3(&m_vPrePosition, vPosition);

#ifdef _DEBUG
	XMStoreFloat3(&m_vSimulationPos, XMVectorSetW(vPosition, 1.f));
#endif
}

void CCamera_CutScene_Save::Change_Sector()
{
	// 이거 필요한가? 어차ㅠ피 바로 ++로 바꾸는데 아무튼

	// 초기 LookAt 설정
	Initialize_CameraInfo(nullptr);
}

vector<CCutScene_Sector*>* CCamera_CutScene_Save::Find_CutScene(_wstring _wszCutSceneName)
{
	auto iter = m_CutScenes.find(_wszCutSceneName);

	if (iter == m_CutScenes.end())
		return nullptr;

	return &(iter->second);
}

pair<CUTSCENE_SUB_DATA, vector<CUTSCENE_DATA>>* CCamera_CutScene_Save::Find_CutSceneData(_wstring _wszCutSceneName)
{
	auto iter = m_CutSceneDatas.find(_wszCutSceneName);

	if (iter == m_CutSceneDatas.end())
		return nullptr;

	return &(iter->second);
}
void CCamera_CutScene_Save::Before_CutScene(_float _fTimeDelta)
{
	if (false == m_isStartCutScene)
		return;

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
			m_isStartCutScene = false;

			return;
		}

		_vector vPos = XMVectorLerp(XMLoadFloat3(&m_tInitialData.vPosition), XMLoadFloat3(&tKeyFrame.vPosition), fRatio);
		_float fFovy = m_pGameInstance->Lerp(m_ZoomLevels[m_tInitialData.iZoomLevel], m_ZoomLevels[tKeyFrame.iZoomLevel], fRatio);
		m_pControllerTransform->LookAt_3D(XMVectorSetW(XMLoadFloat3(&m_tInitialData.vAt), 1.f));

		m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vPos, 1.f));
		m_fFovy = fFovy;
	}
	else {
		// 미리 만들어 놓음
		m_isStartCutScene = false;
	}
}

void CCamera_CutScene_Save::After_CutScene(_float _fTimeDelta)
{
	if (false == m_isFinishCutScene)
		return;

	// 해당 함수까지 끝난다면 카메라 전환 등 동작 수행
	m_fSimulationTime.y += _fTimeDelta;

	if (m_fSimulationTime.y > m_fSimulationTime.x) {
		m_isFinishCutScene = false;
		m_isInitialData = false;
		m_fSimulationTime.y = 0.f;
		CCamera_Manager::GetInstance()->Change_CameraType(CCamera_Manager::FREE);
	}
}

void CCamera_CutScene_Save::Process_Movement(_float _fTimeDelta)
{
	if (false == (*m_pCurCutScene)[m_iCurSectorIndex]->Get_IsLookAt())
		return;

	if (true == (*m_pCurCutScene)[m_iCurSectorIndex]->Get_IsChangeKeyFrame()) {

		_float fTimeOffset = (*m_pCurCutScene)[m_iCurSectorIndex]->Get_TimeOffset();
		_int iCurKeyFrameIdx = (*m_pCurCutScene)[m_iCurSectorIndex]->Get_CurKeyFrameIndex();
		vector<CUTSCENE_KEYFRAME>* pKeyFrames = (*m_pCurCutScene)[m_iCurSectorIndex]->Get_KeyFrames();

		if (iCurKeyFrameIdx >= (_int)((*pKeyFrames).size() - 2))
			return;

		Start_Changing_LookAt(fTimeOffset, XMLoadFloat3(&(*pKeyFrames)[iCurKeyFrameIdx + 1].vAt), (*pKeyFrames)[iCurKeyFrameIdx + 1].iAtRatioType);
		Start_Zoom(fTimeOffset, (CCamera::ZOOM_LEVEL)(*pKeyFrames)[iCurKeyFrameIdx + 1].iZoomLevel, (RATIO_TYPE)(*pKeyFrames)[iCurKeyFrameIdx + 1].iZoomRatioType);

		//Slerp_At(iCurKeyFrameIdx, pKeyFrames);
		//Action_LookAt(_fTimeDelta);
	}

	//_vector vAt = XMLoadFloat3(&m_vAt) + XMLoadFloat3(&m_vAtOffset) + XMLoadFloat3(&m_vShakeOffset);

	//m_pControllerTransform->LookAt_3D(XMVectorSetW(vAt, 1.f));
}

void CCamera_CutScene_Save::Initialize_CameraInfo(INITIAL_DATA* _pTargetPos)
{
	if (nullptr == m_pCurCutScene)
		return;

	if (nullptr == _pTargetPos) {
		// 초기 보는 곳 설정(첫 dummy Frame)
		CUTSCENE_KEYFRAME tKeyFrame = (*m_pCurCutScene)[m_iCurSectorIndex]->Get_KeyFrame(1);

		m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&tKeyFrame.vPosition), 1.f));
		m_pControllerTransform->LookAt_3D(XMVectorSetW(XMLoadFloat3(&tKeyFrame.vAt), 1.f));
		m_vAtOffset = { 0.f, 0.f, 0.f };
		m_vAt = tKeyFrame.vAt;

		// 초기 Zoom Level 설정
		m_iCurZoomLevel = tKeyFrame.iZoomLevel;
		m_fFovy = m_ZoomLevels[m_iCurZoomLevel];
	}
	else {
		m_tInitialData = *_pTargetPos;
		m_isInitialData = true;
		m_InitialTime.x = 0.3f;
		// 초기 위치 설정
		m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_tInitialData.vPosition), 1.f));

		// 초기 보는 곳 설정
		m_pControllerTransform->LookAt_3D(XMVectorSetW(XMLoadFloat3(&m_tInitialData.vAt), 1.f));
		m_vAt = m_tInitialData.vAt;
		m_vAtOffset = { 0.f, 0.f, 0.f };

		// 초기 Zoom Level 설정
		m_iCurZoomLevel = m_tInitialData.iZoomLevel;
		m_fFovy = m_ZoomLevels[m_tInitialData.iZoomLevel];
	}
}

void CCamera_CutScene_Save::Start_Changing_LookAt(_float _fLookAtTime, _fvector _vNextLookAt, _uint _iRatioType)
{
	m_isChangingLookAt = true;
	m_fLookAtTime = { _fLookAtTime, 0.f };
	m_iLookAtRatioType = _iRatioType;
	XMStoreFloat3(&m_vNextLookAt, _vNextLookAt);
	m_vStartLookAt = m_vAt;
}

void CCamera_CutScene_Save::Action_LookAt(_float _fTimeDelta)
{
	if (false == m_isChangingLookAt)
		return;
	
	_float fRatio = m_pGameInstance->Calculate_Ratio(&m_fLookAtTime, _fTimeDelta, m_iLookAtRatioType);

	if (fRatio >= (1.f - EPSILON)) {
		m_isChangingLookAt = false;
		m_vStartLookAt = { 0.f, 0.f, 0.f };
		m_fLookAtTime.y = 0.f;
		m_vAt = m_vNextLookAt;

		return;
	}

	_vector vLookAt = XMVectorLerp(XMLoadFloat3(&m_vStartLookAt), XMLoadFloat3(&m_vNextLookAt), fRatio);

	XMStoreFloat3(&m_vAt, vLookAt);
}

void CCamera_CutScene_Save::Slerp_At(_int iCurKeyFrameIdx, vector<CUTSCENE_KEYFRAME>* pKeyFrames)
{
	/*_vector vOldAtDir = XMVector3Normalize(XMLoadFloat3(&m_vAt) - XMLoadFloat3(&m_vPrePosition));
	_vector vNewAtDir = XMVector3Normalize(XMLoadFloat3(&(*pKeyFrames)[iCurKeyFrameIdx].vAt) - XMLoadFloat3(&m_vPrePosition));

	_vector qOld = XMQuaternionRotationMatrix(XMMatrixLookAtLH(XMVectorZero(), vOldAtDir, XMVectorSet(0, 1, 0, 0)));
	_vector qNew = XMQuaternionRotationMatrix(XMMatrixLookAtLH(XMVectorZero(), vNewAtDir, XMVectorSet(0, 1, 0, 0)));

	_vector qlerped = XMQuaternionSlerp(qOld, qNew, 0.5f);

	_matrix matRot = XMMatrixRotationQuaternion(qlerped);
	_vector vFinalLookDir = XMVector3Normalize(matRot.r[2]);

	_vector vFinalAt = XMLoadFloat3(&m_vPrePosition) + (vFinalLookDir * XMVectorGetX(XMVector3Length(XMLoadFloat3(&(*pKeyFrames)[iCurKeyFrameIdx].vAt) - XMLoadFloat3(&m_vPrePosition))));
	XMStoreFloat3(&m_vAt, vFinalAt);*/

	_vector vAt = XMVectorLerp(XMLoadFloat3(&m_vAt), XMLoadFloat3(&(*pKeyFrames)[iCurKeyFrameIdx].vAt), 0.5f);

	XMStoreFloat3(&m_vAt, vAt);
	m_vStartLookAt = m_vAt;
}

void CCamera_CutScene_Save::Save_Data()
{
	pair<CUTSCENE_SUB_DATA, vector<CUTSCENE_DATA>>* pData = Find_CutSceneData(m_wszCurCutSceneTag);

	CUTSCENE_DATA tData = {};

	XMStoreFloat3(&tData.vPosition, m_pControllerTransform->Get_State(CTransform::STATE_POSITION));
	tData.vRotation = {};	// 나중에
	tData.fFovy = m_fFovy;

	// 초기 입력 데이터가 있을 때
	if (true == m_isInitialData) {
		_vector vAtOffset = XMLoadFloat3(&m_vAt) + XMLoadFloat3(&m_vShakeOffset);
		XMStoreFloat3(&tData.vAt, vAtOffset);
	}
	else {
		_vector vAtOffset = XMLoadFloat3(&m_vAt) + XMLoadFloat3(&m_vAtOffset) + XMLoadFloat3(&m_vShakeOffset);
		XMStoreFloat3(&tData.vAt, vAtOffset);
	}

	(*pData).second.push_back(tData);
}

CCamera_CutScene_Save* CCamera_CutScene_Save::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CCamera_CutScene_Save* pInstance = new CCamera_CutScene_Save(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera_CutScene_Save");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_CutScene_Save::Clone(void* _pArg)
{
	CCamera_CutScene_Save* pInstance = new CCamera_CutScene_Save(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_CutScene_Save");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_CutScene_Save::Free()
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
