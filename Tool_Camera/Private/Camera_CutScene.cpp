#include "stdafx.h"
#include "Camera_CutScene.h"

#include "GameInstance.h"

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
	Action_Zoom(_fTimeDelta);
	Action_Shake(_fTimeDelta);
	Change_AtOffset(_fTimeDelta);

	Before_CutScene(_fTimeDelta);
	Play_CutScene(_fTimeDelta);
	After_CutScene(_fTimeDelta);
}

void CCamera_CutScene::Late_Update(_float _fTimeDelta)
{
}

void CCamera_CutScene::Set_NextCutScene(_wstring _wszCutSceneName)
{
	m_pCurCutScene = Find_CutScene(_wszCutSceneName);

	if (nullptr == m_pCurCutScene)
		return;

	for (auto& Sector : *m_pCurCutScene) {
		Safe_AddRef(Sector);
	}

	m_isStartCutScene = true;
	Initialize_CameraInfo();
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
	Look_Target(_fTimeDelta);

#ifdef _DEBUG
	XMStoreFloat3(&m_vSimulationPos, XMVectorSetW(vPosition, 1.f));
#endif
}

void CCamera_CutScene::Change_Sector()
{
	// 이거 필요한가? 어차ㅠ피 바로 ++로 바꾸는데 아무튼

	// 초기 LookAt 설정
	Initialize_CameraInfo();
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
	// 미리 만들어 놓음
	m_isStartCutScene = false;
}

void CCamera_CutScene::After_CutScene(_float _fTimeDelta)
{
	if (false == m_isFinishCutScene)
		return;

	// 해당 함수까지 끝난다면 카메라 전환 등 동작 수행
	m_isFinishCutScene = false;
}

void CCamera_CutScene::Look_Target(_float _fTimeDelta)
{
	if (false == (*m_pCurCutScene)[m_iCurSectorIndex]->Get_IsLookAt())
		return;

	if (true == (*m_pCurCutScene)[m_iCurSectorIndex]->Get_IsChangeKeyFrame()) {
		_float fTimeOffset = (*m_pCurCutScene)[m_iCurSectorIndex]->Get_TimeOffset();
		_int iCurKeyFrameIdx = (*m_pCurCutScene)[m_iCurSectorIndex]->Get_CurKeyFrameIndex();
		vector<CUTSCENE_KEYFRAME>* pKeyFrames = (*m_pCurCutScene)[m_iCurSectorIndex]->Get_KeyFrames();
		
		Start_Changing_AtOffset(fTimeOffset, XMLoadFloat3(&(*pKeyFrames)[iCurKeyFrameIdx + 1].vAtOffset), (*pKeyFrames)[iCurKeyFrameIdx + 1].iAtRatioType);
	}

	_vector vAt = XMLoadFloat3(&m_vTargetPos) + XMLoadFloat3(&m_vAtOffset) + XMLoadFloat3(&m_vShakeOffset);

	m_pControllerTransform->LookAt_3D(XMVectorSetW(vAt, 1.f));
}

void CCamera_CutScene::Initialize_CameraInfo()
{
	if (nullptr == m_pCurCutScene)
		return;

	CUTSCENE_KEYFRAME tKeyFrame = (*m_pCurCutScene)[m_iCurSectorIndex]->Get_KeyFrame(0);
	
	m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&tKeyFrame.vPosition));
	m_pControllerTransform->LookAt_3D(XMVectorSetW(XMLoadFloat3(&tKeyFrame.vAtOffset), 1.f));
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

