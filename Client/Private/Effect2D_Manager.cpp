#include "stdafx.h"
#include "Effect2D_Manager.h"
#include "GameInstance.h"
#include "Effect2D.h"

IMPLEMENT_SINGLETON(CEffect2D_Manager)
CEffect2D_Manager::CEffect2D_Manager()
	:m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CEffect2D_Manager::Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	m_pDevice = _pDevice;
	m_pContext = _pContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	return S_OK;
}

HRESULT CEffect2D_Manager::Level_Exit(_int _iChangeLevelID, _int _iNextChangeLevelID)
{

	if(LEVEL_LOADING == _iChangeLevelID)
	{
		Clear();

		m_iCurLevelID = _iNextChangeLevelID;
	}

	
	return S_OK;
}

HRESULT CEffect2D_Manager::Level_Enter(_int _iChangeLevelID)
{
	return S_OK;
}

HRESULT CEffect2D_Manager::Register_EffectPool(const _wstring& _strEffectModelTag, LEVEL_ID _ePrototypeModelLevelID, _uint _iNumInitialPoolingEffects)
{
	if (nullptr != Find_EffectPool(_strEffectModelTag))
		return E_FAIL;

	CEffect2D::EFFECT2D_DESC EffectDesc = {};
	EffectDesc.iCurLevelID = m_iCurLevelID;
	EffectDesc.iModelPrototypeLevelID_2D = _ePrototypeModelLevelID;
	EffectDesc.strModelPrototypeTag_2D = _strEffectModelTag;

	/* 1. 초기 풀링 객체 수 지정 */
	vector<CEffect2D*> EffectPool;
	EffectPool.resize(_iNumInitialPoolingEffects);
	for (_uint i = 0; i < _iNumInitialPoolingEffects; ++i)
	{
		/* 2. 루프 돌며 지정한 수만큼 객체 클론. */
		CEffect2D* pEffect = static_cast<CEffect2D*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_Effect2D"), &EffectDesc));
		if (nullptr == pEffect)
			return E_FAIL;
		EffectPool[i]  = pEffect;	// 원본 객체

		/* 3. 생성 된 객체를 Object_Manager Layer에 삽입. */
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_iCurLevelID, TEXT("Layer_PoolingEffect2D"), pEffect)))
			return E_FAIL;
		Safe_AddRef(pEffect);			// Layer에 추가하고 AddRef
		pEffect->Set_Active(false);
	}

	m_EffectPools.emplace(_strEffectModelTag, EffectPool);
	return S_OK;
}

HRESULT CEffect2D_Manager::Remove_EffectPool(const _wstring& _strEffectModelTag)
{
	vector<CEffect2D*>* pEffectPool = Find_EffectPool(_strEffectModelTag);
	if (nullptr == pEffectPool)
		return E_FAIL;

	for (auto& pEffect : (*pEffectPool))
	{
		Safe_Release(pEffect);
	}
	(*pEffectPool).clear();

	return S_OK;
}

vector<CEffect2D*>* CEffect2D_Manager::Find_EffectPool(const _wstring& _strEffectModelTag)
{
	auto iter = m_EffectPools.find(_strEffectModelTag);
	if (iter == m_EffectPools.end())
		return nullptr;

	return &(iter->second);
}

HRESULT CEffect2D_Manager::Pooling_Effects(const _wstring& _strEffectModelTag, _uint _iNumInitialPoolingEffects)
{
	vector<CEffect2D*>* pEffectPool = Find_EffectPool(_strEffectModelTag);
	CEffect2D::EFFECT2D_DESC EffectDesc = {};
	EffectDesc.iCurLevelID = m_iCurLevelID;
	EffectDesc.iModelPrototypeLevelID_2D = (*pEffectPool)[0]->Get_EffectModelPrototypeLevelID();
	EffectDesc.strModelPrototypeTag_2D = (*pEffectPool)[0]->Get_EffectModelPrototypeTag();

	/* 1. 초기 풀링 객체 수 지정 */
	for (_uint i = 0; i < _iNumInitialPoolingEffects; ++i)
	{
		/* 2. 루프 돌며 지정한 수만큼 객체 클론. */
		CEffect2D* pEffect = static_cast<CEffect2D*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_Effect2D"), &EffectDesc));
		if (nullptr == pEffect)
			return E_FAIL;
		(*pEffectPool).push_back(pEffect);	// 원본 객체

		/* 3. 생성 된 객체를 Object_Manager Layer에 삽입. */
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_iCurLevelID, TEXT("Layer_PoolingEffect2D"), pEffect)))
			return E_FAIL;
		Safe_AddRef(pEffect);			// Layer에 추가하고 AddRef
		pEffect->Set_Active(false);
	}

	return S_OK;
}

HRESULT CEffect2D_Manager::Play_Effect(const _wstring& _strEffectModelTag, const _wstring& _strSectionKey, _fmatrix _2DWorldMatrix, _float _fDelayTime, _uint _iAnimIndex, _bool _isLoop, _float _fLifeTime, _uint _iSectionLayerGroup, CGameObject** _ppOut)
{
	vector<CEffect2D*>* pEffectPool = Find_EffectPool(_strEffectModelTag);
	if (nullptr == pEffectPool)
		return E_FAIL;

	for (auto& pEffect : *pEffectPool)
	{
		if (false == pEffect->Is_Active())
		{
			if (nullptr != _ppOut)
				*_ppOut = pEffect;
			pEffect->Play_Effect(_strSectionKey, _2DWorldMatrix, _fDelayTime, _iAnimIndex, _isLoop, _fLifeTime, _iSectionLayerGroup);
			return S_OK;
		}
	}

	/* 만약 모두 Active True 였다면, Pulling을 추가적으로 수행한다. */
	if (FAILED(Pooling_Effects(_strEffectModelTag, 5)))
		return E_FAIL;

	/* 다시한번 호출하여 재귀적으로 새로 생성된 이펙트를 만든다. */
	Play_Effect(_strEffectModelTag, _strSectionKey, _2DWorldMatrix, _fDelayTime, _iAnimIndex, _isLoop, _fLifeTime, _iSectionLayerGroup, _ppOut);

	return S_OK;
}

void CEffect2D_Manager::Clear()
{
	for (auto& Pair : m_EffectPools)
	{
		for (auto& EffectPool : Pair.second)
		{
			Safe_Release(EffectPool);
		}

		Pair.second.clear();
	}
	m_EffectPools.clear();
}

void CEffect2D_Manager::Free()
{
	Clear();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	__super::Free();
}
