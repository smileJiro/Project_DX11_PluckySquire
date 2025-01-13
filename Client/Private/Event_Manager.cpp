#include "stdafx.h"
#include "Event_Manager.h"
#include "GameInstance.h"

#include "Level_Static.h"
#include "Level_Loading.h"
#include "Level_Logo.h"
#include "Level_GamePlay.h"
#include "Layer.h"

#include "Cam_Manager.h"




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
	for (auto& Pair : m_vecDelayActiveList)
	{
		Pair.first->Set_Active(Pair.second);
		Safe_Release(Pair.first);
	}
	m_vecDelayActiveList.clear();
	
	for (auto& pDeadObject : m_vecDeadList)
	{
		pDeadObject->Set_Dead();
		Safe_Release(pDeadObject);
	}
	m_vecDeadList.clear();

	for (size_t i = 0; i < m_vecEvent.size(); ++i)
	{
		Excute(m_vecEvent[i]);
	}
	m_vecEvent.clear();

}

HRESULT CEvent_Manager::Excute(const EVENT& _tEvent)
{
	switch (_tEvent.eType)
	{
	case Client::EVENT_TYPE::CREATE_OBJECT:
	{
		Excute_CreateObject(_tEvent);
	}
		break;
	case Client::EVENT_TYPE::DELETE_OBJECT:
	{
		Excute_DeleteObject(_tEvent);
	}
		break;
	case Client::EVENT_TYPE::LEVEL_CHANGE:
	{
		Excute_LevelChange(_tEvent);
	}
		break;
	case Client::EVENT_TYPE::SET_ACTIVE:
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

	m_vecDeadList.push_back(pDeleteObject);
	Safe_AddRef(pDeleteObject);

	return S_OK;
}

HRESULT CEvent_Manager::Excute_LevelChange(const EVENT& _tEvent)
{
	// Parameter[0] : NextLevelID(클라에선 로딩만 오고, 로딩에선 실제 전환 인덱스가 옴);
	// Parameter[1] : Next CLevel Class Address
	_uint iNextLevelID = (_uint)(_tEvent.Parameters[0]);

	if (iNextLevelID >= (_uint)LEVEL_ID::LEVEL_END)
		return E_FAIL;

	/* Client Exit */
	if (FAILED(Client_Level_Exit()))
		return E_FAIL;

	/* Engine Exit */
	if (FAILED(m_pGameInstance->Engine_Level_Exit()))
		return E_FAIL;

	/* Level_Manager Exit */
	if (FAILED(m_pGameInstance->Level_Manager_Exit()))
		return E_FAIL;

	CLevel* pNextLevel = nullptr;
	switch ((LEVEL_ID)_tEvent.Parameters[0])
	{
	case Client::LEVEL_STATIC:
		pNextLevel = CLevel_Static::Create(m_pDevice, m_pContext);
		break;
	case Client::LEVEL_LOADING:
		pNextLevel = CLevel_Loading::Create(m_pDevice, m_pContext, (LEVEL_ID)_tEvent.Parameters[1]);
		break;
	case Client::LEVEL_LOGO:
		pNextLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
		break;
	case Client::LEVEL_GAMEPLAY:
		pNextLevel = CLevel_GamePlay::Create(m_pDevice, m_pContext);
		break;
	default:
		break;
	}

	if (nullptr == pNextLevel)
		return E_FAIL;

	/* Level_Manager Enter */
	if (FAILED(m_pGameInstance->Level_Manager_Enter(iNextLevelID, pNextLevel)))
	{
		Safe_Release(pNextLevel);
		return E_FAIL;
	}

	/* Engine Level Enter */
	if (FAILED(m_pGameInstance->Engine_Level_Enter(iNextLevelID)))
		return E_FAIL;

	/* Client Enter */
	if (FAILED(Client_Level_Enter(iNextLevelID)))
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
		m_vecDelayActiveList.push_back(make_pair(pBase, isActive));
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

HRESULT CEvent_Manager::Client_Level_Exit()
{
	_int iCurLevelID = m_pGameInstance->Get_CurLevelID();

	return S_OK;
}

void CEvent_Manager::Free()
{
	for (auto& pDeadObject : m_vecDeadList)
		Safe_Release(pDeadObject);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	__super::Free();
}
