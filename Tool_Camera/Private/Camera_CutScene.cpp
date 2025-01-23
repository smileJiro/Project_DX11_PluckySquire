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
	return S_OK;
}

void CCamera_CutScene::Priority_Update(_float _fTimeDelta)
{
}

void CCamera_CutScene::Update(_float _fTimeDelta)
{
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

	m_isStartCutScene = true;
}

void CCamera_CutScene::Add_CutScene(_wstring _wszCutSceneTag, vector<CCutScene_Sector*> _vecCutScene)
{
	vector<CCutScene_Sector*>* pCutScene = Find_CutScene(_wszCutSceneTag);

	if (nullptr == pCutScene)
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

	_bool isSectorFinish = (*m_pCurCutScene)[m_iCurSectorCount]->Play_Sector(_fTimeDelta, &vPosition);

	if (true == isSectorFinish) {
		m_iCurSectorCount++;

		if (m_iCurSectorCount >= m_iCurSectorNum) {
			m_iCurSectorCount = 0;
			m_isFinishCutScene = true;
		}
		else {
			Change_Sector();
		}

		return;
	}

	m_pControllerTransform->Set_State(CTransform::STATE_POSITION, vPosition);

}

void CCamera_CutScene::Change_Sector()
{
	// 이거 필요한가? 어차ㅠ피 바로 ++로 바꾸는데 아무튼
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

