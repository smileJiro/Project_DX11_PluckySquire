#include "stdafx.h"
#include "Poolling_Manager.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CPoolling_Manager)
CPoolling_Manager::CPoolling_Manager()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CPoolling_Manager::Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	m_pDevice = _pDevice;
	m_pContext = _pContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);


	return S_OK;
}

void CPoolling_Manager::Update()
{
}

HRESULT CPoolling_Manager::Level_Enter(_int _iChangeLevelID)
{


	return S_OK;
}

HRESULT CPoolling_Manager::Level_Exit(_int _iChangeLevelID, _int _iNextChangeLevelID)
{
	if (_iChangeLevelID == LEVEL_LOADING)
	{
		m_iCurLevelID = _iNextChangeLevelID;
	}
		

	for (auto& iter : m_PoollingObjects)
	{
		for (CGameObject* pGameObject : iter.second)
			Safe_Release(pGameObject);

		iter.second.clear();
	}
	m_PoollingObjects.clear();

	for (auto& iter : m_ObjectsDescs)
	{
		delete iter.second.second;
	}
	m_ObjectsDescs.clear();

	return S_OK;
}

HRESULT CPoolling_Manager::Register_PoollingObject(const _wstring& _strPoollingTag, const POOLLING_DESC& _tPoollingDesc, CGameObject::GAMEOBJECT_DESC* _pDesc)
{
	if (nullptr != Find_PoollingObjects(_strPoollingTag))
		return E_FAIL;
	
	POOLLING_DESC tPoollingDesc = _tPoollingDesc;

	tPoollingDesc.iPrototypeLevelID;
	tPoollingDesc.strLayerTag;
	tPoollingDesc.strPrototypeTag;

	CGameObject* pGameObject = nullptr;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(tPoollingDesc.iPrototypeLevelID, tPoollingDesc.strPrototypeTag, m_iCurLevelID, tPoollingDesc.strLayerTag, &pGameObject, _pDesc)))
		return E_FAIL;

	vector<CGameObject*> PullingObjects;
	PullingObjects.push_back(pGameObject);
	Safe_AddRef(pGameObject);
	pGameObject->Set_Active(false);
	
	/* Pulling Objects */
	m_PoollingObjects.emplace(_strPoollingTag, PullingObjects);

	/* Object Descs  */
	
	m_ObjectsDescs.insert(std::make_pair(_strPoollingTag, std::make_pair(tPoollingDesc, _pDesc)));

	
	return S_OK;
}

HRESULT CPoolling_Manager::Create_Objects(const _wstring& _strPoollingTag, _uint iNumPoolingObjects, _float3* _pPosition, _float3* _pScaling)
{
	for(int i = 0; i < iNumPoolingObjects; ++i)
	{
		if (FAILED(Create_Object(_strPoollingTag, _pPosition, _pScaling)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CPoolling_Manager::Create_Object(const _wstring& _strPoollingTag, _float3* _pPosition, _float3* _pScaling)
{
	vector<CGameObject*>* pGameObjects = Find_PoollingObjects(_strPoollingTag);

	if (nullptr == pGameObjects)
		return E_FAIL;


	for (auto& pGameObject : *pGameObjects)
	{
		if (false == pGameObject->Is_Active())
		{
			if (nullptr != _pPosition)
				pGameObject->Set_Position(XMLoadFloat3(_pPosition));
			if (nullptr != _pScaling)
				pGameObject->Set_Scale(*_pScaling);

			pGameObject->Set_Active(true);

			return S_OK;
		}

	}

	if (FAILED(Pooling_Objects(_strPoollingTag, 5)))
		return E_FAIL;

	if (FAILED(Create_Object(_strPoollingTag, _pPosition, _pScaling)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPoolling_Manager::Pooling_Objects(const _wstring& _strPoollingTag, _uint _iNumPoolingObjects)
{
	pair<POOLLING_DESC, CGameObject::GAMEOBJECT_DESC*>* pPair = Find_PoollingDesc(_strPoollingTag);
	if (nullptr == pPair)
		return E_FAIL;

	CGameObject* pGameObject = nullptr;

	for (_uint i = 0; i < _iNumPoolingObjects; ++i)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(pPair->first.iPrototypeLevelID, pPair->first.strPrototypeTag, m_iCurLevelID, pPair->first.strLayerTag, &pGameObject, pPair->second)))
			return E_FAIL;

		m_PoollingObjects[_strPoollingTag].push_back(pGameObject);
		Safe_AddRef(pGameObject);
		pGameObject->Set_Active(false);
	}

	return S_OK;
}

vector<CGameObject*>* CPoolling_Manager::Find_PoollingObjects(const _wstring& _strPullingTag)
{
	auto iter = m_PoollingObjects.find(_strPullingTag);
	if (iter == m_PoollingObjects.end())
		return nullptr;

	return &(iter->second);
}

pair<POOLLING_DESC, CGameObject::GAMEOBJECT_DESC*>* CPoolling_Manager::Find_PoollingDesc(const _wstring& _strPoollingTag)
{
	auto iter = m_ObjectsDescs.find(_strPoollingTag);
	if (iter == m_ObjectsDescs.end())
		return nullptr;

	return &(iter->second);
}

void CPoolling_Manager::Free()
{
	for (auto& iter : m_PoollingObjects)
	{
		for(CGameObject* pGameObject : iter.second)
			Safe_Release(pGameObject);

		iter.second.clear();
	}
	m_PoollingObjects.clear();

	for (auto& iter : m_ObjectsDescs)
	{
		delete iter.second.second;
	}
	m_ObjectsDescs.clear();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	__super::Free();
}
