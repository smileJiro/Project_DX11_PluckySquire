#include "stdafx.h"
#include "Pooling_Manager.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CPooling_Manager)
CPooling_Manager::CPooling_Manager()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CPooling_Manager::Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	m_pDevice = _pDevice;
	m_pContext = _pContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);


	return S_OK;
}

void CPooling_Manager::Update()
{
}

HRESULT CPooling_Manager::Level_Enter(_int _iChangeLevelID)
{


	return S_OK;
}

HRESULT CPooling_Manager::Level_Exit(_int _iChangeLevelID, _int _iNextChangeLevelID)
{
	if (_iChangeLevelID == LEVEL_LOADING)
	{
		m_iCurLevelID = _iNextChangeLevelID;
	}
		

	for (auto& iter : m_PoolingObjects)
	{
		for (CGameObject* pGameObject : iter.second)
			Safe_Release(pGameObject);

		iter.second.clear();
	}
	m_PoolingObjects.clear();

	for (auto& iter : m_ObjectsDescs)
	{
		delete iter.second.second;
	}
	m_ObjectsDescs.clear();

	return S_OK;
}

HRESULT CPooling_Manager::Register_PoolingObject(const _wstring& _strPoolingTag, const Pooling_DESC& _tPoolingDesc, CGameObject::GAMEOBJECT_DESC* _pDesc)
{
	if (nullptr != Find_PoolingObjects(_strPoolingTag))
		return E_FAIL;
	
	Pooling_DESC tPoolingDesc = _tPoolingDesc;

	CGameObject* pGameObject = nullptr;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(tPoolingDesc.iPrototypeLevelID, tPoolingDesc.strPrototypeTag, m_iCurLevelID, tPoolingDesc.strLayerTag, &pGameObject, _pDesc)))
		return E_FAIL;

	vector<CGameObject*> PullingObjects;
	PullingObjects.push_back(pGameObject);
	Safe_AddRef(pGameObject);
	pGameObject->Set_Active(false);
	
	/* Pulling Objects */
	m_PoolingObjects.emplace(_strPoolingTag, PullingObjects);

	/* Object Descs  */
	
	m_ObjectsDescs.insert(std::make_pair(_strPoolingTag, std::make_pair(tPoolingDesc, _pDesc)));

	
	return S_OK;
}

HRESULT CPooling_Manager::Create_Objects(const _wstring& _strPoolingTag, _uint iNumPoolingObjects, _float3* _pPosition, _float4* _pRotation, _float3* _pScaling)
{
	for(int i = 0; i < iNumPoolingObjects; ++i)
	{
		if (FAILED(Create_Object(_strPoolingTag, _pPosition, _pRotation, _pScaling)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CPooling_Manager::Create_Object(const _wstring& _strPoolingTag, _float3* _pPosition, _float4* _pRotation, _float3* _pScaling)
{
	vector<CGameObject*>* pGameObjects = Find_PoolingObjects(_strPoolingTag);

	if (nullptr == pGameObjects)
		return E_FAIL;


	for (auto& pGameObject : *pGameObjects)
	{
		if (false == pGameObject->Is_Active())
		{
			if (nullptr != _pScaling)
				pGameObject->Set_Scale(*_pScaling);
			if (nullptr != _pRotation)
				pGameObject->Get_ControllerTransform()->RotationQuaternionW(*_pRotation);
			if (nullptr != _pPosition)
				pGameObject->Set_Position(XMLoadFloat3(_pPosition));



			pGameObject->Set_Active(true);
			pGameObject->Set_Alive();

			return S_OK;
		}

	}

	if (FAILED(Pooling_Objects(_strPoolingTag, 5)))
		return E_FAIL;

	if (FAILED(Create_Object(_strPoolingTag, _pPosition, _pRotation, _pScaling)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPooling_Manager::Pooling_Objects(const _wstring& _strPoolingTag, _uint _iNumPoolingObjects)
{
	pair<Pooling_DESC, CGameObject::GAMEOBJECT_DESC*>* pPair = Find_PoolingDesc(_strPoolingTag);
	if (nullptr == pPair)
		return E_FAIL;

	CGameObject* pGameObject = nullptr;

	for (_uint i = 0; i < _iNumPoolingObjects; ++i)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(pPair->first.iPrototypeLevelID, pPair->first.strPrototypeTag, m_iCurLevelID, pPair->first.strLayerTag, &pGameObject, pPair->second)))
			return E_FAIL;

		m_PoolingObjects[_strPoolingTag].push_back(pGameObject);
		Safe_AddRef(pGameObject);
		pGameObject->Set_Active(false);
		pGameObject->Set_Pooling(true);
	}

	return S_OK;
}

vector<CGameObject*>* CPooling_Manager::Find_PoolingObjects(const _wstring& _strPullingTag)
{
	auto iter = m_PoolingObjects.find(_strPullingTag);
	if (iter == m_PoolingObjects.end())
		return nullptr;

	return &(iter->second);
}

pair<Pooling_DESC, CGameObject::GAMEOBJECT_DESC*>* CPooling_Manager::Find_PoolingDesc(const _wstring& _strPoolingTag)
{
	auto iter = m_ObjectsDescs.find(_strPoolingTag);
	if (iter == m_ObjectsDescs.end())
		return nullptr;

	return &(iter->second);
}

void CPooling_Manager::Free()
{
	for (auto& iter : m_PoolingObjects)
	{
		for(CGameObject* pGameObject : iter.second)
			Safe_Release(pGameObject);

		iter.second.clear();
	}
	m_PoolingObjects.clear();

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
