#include "stdafx.h"
#include "Event_Manager.h"
#include "GameInstance.h"

#include "Level_Static.h"
#include "Level_Loading.h"
#include "Level_Camera_Tool.h"
#include "Layer.h"
#include "GameObject.h"


IMPLEMENT_SINGLETON(CEvent_Manager)

CEvent_Manager::CEvent_Manager()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

void CEvent_Manager::Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	m_pDevice = _pDevice;
	m_pContext = _pContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

void CEvent_Manager::Update(_float _fTimeDelta)
{
	for (auto& Pair : m_DelayActiveList)
	{
		Pair.first->Set_Active(Pair.second);
		Safe_Release(Pair.first);
	}
	m_DelayActiveList.clear();

	for (auto& pDeadObject : m_DeadObjectsList)
	{
		pDeadObject->Set_Dead();
		Safe_Release(pDeadObject);
	}
	m_DeadObjectsList.clear();

	for (size_t i = 0; i < m_Events.size(); ++i)
	{
		Excute(m_Events[i]);
	}
	m_Events.clear();

}

HRESULT CEvent_Manager::Excute(const EVENT& _tEvent)
{
	switch (_tEvent.eType)
	{
	case Camera_Tool::EVENT_TYPE::CREATE_OBJECT:
	{
		Excute_CreateObject(_tEvent);
	}
	break;
	case Camera_Tool::EVENT_TYPE::DELETE_OBJECT:
	{
		Excute_DeleteObject(_tEvent);
	}
	break;
	case Camera_Tool::EVENT_TYPE::LEVEL_CHANGE:
	{
		Excute_LevelChange(_tEvent);
	}
	break;
	case Camera_Tool::EVENT_TYPE::SET_ACTIVE:
	{
		Excute_SetActive(_tEvent);
	}
	break;
	default:
		break;
	}


	return S_OK;
}

HRESULT CEvent_Manager::Excute_CreateObject(const EVENT& _tEvent)
{
	/* Parameter_0 : LevelD; Parameter_1 : LayerTag; Parameter_2 : GameObject Adress */
	_uint iCurLevelID = (_uint)_tEvent.Parameters[0];

	if (iCurLevelID >= LEVEL_ID::LEVEL_END)
		return E_FAIL;

	_wstring strLayerTag = reinterpret_cast<const _tchar*>(_tEvent.Parameters[1]);
	CGameObject* pCreateObject = reinterpret_cast<CGameObject*>(_tEvent.Parameters[2]);

	Safe_AddRef(pCreateObject);
	if (nullptr == pCreateObject)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(iCurLevelID, strLayerTag, pCreateObject)))
	{
		Safe_Release(pCreateObject);
		return E_FAIL;
	}

	Safe_Release(pCreateObject);

	return S_OK;
}

HRESULT CEvent_Manager::Excute_DeleteObject(const EVENT& _tEvent)
{
	/* Parameter_0 : Object Address */
	CGameObject* pDeleteObject = reinterpret_cast<CGameObject*>(_tEvent.Parameters[0]);
	if (nullptr == pDeleteObject)
		return E_FAIL;

	m_DeadObjectsList.push_back(pDeleteObject);
	Safe_AddRef(pDeleteObject);

	return S_OK;
}

HRESULT CEvent_Manager::Excute_LevelChange(const EVENT& _tEvent)
{
	// par
	_uint iChangeLevelID = (_uint)(_tEvent.Parameters[0]);
	_int iNextChangeLevelID = (_int)(_tEvent.Parameters[1]);

	if (iChangeLevelID >= (_uint)LEVEL_ID::LEVEL_END)
		return E_FAIL;

	/* Client Exit */
	if (FAILED(Client_Level_Exit(iChangeLevelID, iNextChangeLevelID)))
		return E_FAIL;

	/* Engine Exit */
	if (FAILED(m_pGameInstance->Engine_Level_Exit(iChangeLevelID, iNextChangeLevelID)))
		return E_FAIL;

	/* Level_Manager Exit */
	if (FAILED(m_pGameInstance->Level_Manager_Exit(iChangeLevelID, iNextChangeLevelID)))
		return E_FAIL;

	CLevel* pChangeLevel = nullptr;
	switch ((LEVEL_ID)iChangeLevelID)
	{
	case Camera_Tool::LEVEL_STATIC:
		pChangeLevel = CLevel_Static::Create(m_pDevice, m_pContext);
		break;
	case Camera_Tool::LEVEL_LOADING:
		pChangeLevel = CLevel_Loading::Create(m_pDevice, m_pContext, (LEVEL_ID)iNextChangeLevelID);
		break;
	case Camera_Tool::LEVEL_CAMERA_TOOL:
		pChangeLevel = CLevel_Camera_Tool::Create(m_pDevice, m_pContext);
		break;
	default:
		break;
	}

	if (nullptr == pChangeLevel)
		return E_FAIL;

	/* Level_Manager Enter */
	if (FAILED(m_pGameInstance->Level_Manager_Enter(iChangeLevelID, pChangeLevel)))
	{
		Safe_Release(pChangeLevel);
		return E_FAIL;
	}

	/* Engine Level Enter */
	if (FAILED(m_pGameInstance->Engine_Level_Enter(iChangeLevelID)))
		return E_FAIL;

	/* Client Enter */
	if (FAILED(Client_Level_Enter(iChangeLevelID)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEvent_Manager::Excute_SetActive(const EVENT& _tEvent)
{
	if (_tEvent.Parameters[0] == 0)
		return E_FAIL;

	CBase* pBase = (CBase*)_tEvent.Parameters[0];
	if (nullptr == pBase)
		return E_FAIL;

	_bool isActive = (_bool)(_tEvent.Parameters[1]);
	_bool isDelay = _tEvent.Parameters[2];

	if (true == isDelay)
	{
		m_DelayActiveList.push_back(make_pair(pBase, isActive));
		Safe_AddRef(pBase);
	}
	else
		pBase->Set_Active(isActive);

	return S_OK;
}

HRESULT CEvent_Manager::Client_Level_Enter(_uint _iNextLevelID)
{

	return S_OK;
}

HRESULT CEvent_Manager::Client_Level_Exit(_int _iChangeLevelID, _int _iNextChangeLevelID)
{
	_int iCurLevelID = m_pGameInstance->Get_CurLevelID();

	return S_OK;
}

void CEvent_Manager::Free()
{
	for (auto& pDeadObject : m_DeadObjectsList)
		Safe_Release(pDeadObject);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	__super::Free();
}
