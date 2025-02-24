#include "stdafx.h"
#include "PlayerData_Manager.h"

#include "GameInstance.h"

#include "PlayerItem.h"
#include "Bulb.h"

IMPLEMENT_SINGLETON(CPlayerData_Manager)

CPlayerData_Manager::CPlayerData_Manager()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CPlayerData_Manager::Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	m_pDevice = _pDevice;
	m_pContext = _pContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	Set_Tags();

	return S_OK;
}

HRESULT CPlayerData_Manager::Level_Enter(_int _iChangeLevelID)
{
	return S_OK;
}

HRESULT CPlayerData_Manager::Level_Exit(_int _iChangeLevelID, _int _iNextChangeLevelID)
{
	for (auto& PlayerItem : m_ItemState)
		Safe_Release(PlayerItem.second.second);

	return S_OK;
}

_bool CPlayerData_Manager::Is_Own(PLAYERITEM_TYPE _eItemType)
{
	_wstring wszItemTag = m_ItemTags[_eItemType].first;

	return m_ItemState[wszItemTag].first;
}

HRESULT CPlayerData_Manager::Spawn_PlayerItem(_uint _iPrototypeLevelID, _uint _iLevelID, _wstring _szItemTag, _float3 _vPos)
{
	// ItemTag
	// first: Flipping_Glove						second: latch_glove

	// ItemState
	// ItemState[Flipping_Glove].first: bool		ItemState[Filiping_Glove].second: GameObject*

	CPlayerItem::PLAYERITEM_DESC Desc = {};

	Desc.iCurLevelID = _iLevelID;
	Desc.tTransform3DDesc.vInitialPosition = _vPos;

	for (auto& ItemTag : m_ItemTags) {
		if (ItemTag.first == _szItemTag) {
			Desc.szModelTag = ItemTag.second;
			Desc.szItemTag = ItemTag.first;
			Desc.szEventTag = TEXT("Get_") + ItemTag.first;
			break;
		}
	}

	CGameObject* pPlayerItem;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(_iPrototypeLevelID, TEXT("Prototype_GameObject_PlayerItem"),
		_iLevelID, TEXT("Layer_Terrain"), &pPlayerItem, &Desc)))
		return E_FAIL;

	m_ItemState[_szItemTag].second = dynamic_cast<CPlayerItem*>(pPlayerItem);
	Safe_AddRef(m_ItemState[_szItemTag].second);

	return S_OK;
}

HRESULT CPlayerData_Manager::Spawn_Bulb(_uint _iPrototypeLevelID, _uint _iLevelID)
{
	ifstream file(TEXT("../Bin/DataFiles/Trigger/Bulb/Bulb_Position.json"));

	if (!file.is_open())
	{
		MSG_BOX("파일을 열 수 없습니다.");
		file.close();
		return E_FAIL;
	}

	json Result;
	file >> Result;
	file.close();

	json Bulb_json;

	for (auto& Bulb_json : Result) {
		_float3 vPosition = { Bulb_json["Bulb_Position"][0].get<_float>(), Bulb_json["Bulb_Position"][1].get<_float>(), Bulb_json["Bulb_Position"][2].get<_float>() };

		// Create
		CGameObject* pBulb;
		CBulb::BULB_DESC Desc = {};
		Desc.eStartCoord = COORDINATE_3D;
		Desc.tTransform3DDesc.vInitialPosition = vPosition;

		m_pGameInstance->Add_GameObject_ToLayer(_iPrototypeLevelID, TEXT("Prototype_GameObject_Bulb"),
			_iLevelID, TEXT("Layer_Bulb"), &pBulb, &Desc);
	}

	return S_OK;
}

void CPlayerData_Manager::Get_PlayerItem(_wstring _szItemTag)
{
	auto& iterator = m_ItemState.find(_szItemTag);

	if (iterator == m_ItemState.end())
		return;

	if (nullptr == m_ItemState[_szItemTag].second)
		return;

	m_ItemState[_szItemTag].first = true;
	m_ItemState[_szItemTag].second->Change_Mode(CPlayerItem::GETTING);

	CGameObject* pPlayer = m_pGameInstance->Get_GameObject_Ptr(m_pGameInstance->Get_CurLevelID(), TEXT("Layer_Player"), 0);
	if (nullptr == pPlayer)
		return;

	// Player에게 이거 가지고 있다고 넘겨주기
}

void CPlayerData_Manager::Set_Tags()
{
	m_ItemTags[FLIPPING_GLOVE] = { TEXT("Flipping_Glove"), TEXT("latch_glove") };
	m_ItemTags[TILTING_GLOVE] = { TEXT("Tilting_Glove"), TEXT("latch_glove") };
	m_ItemTags[STOP_STAMP] = { TEXT("Stop_Stamp"), TEXT("Stop_Stamp") };
	m_ItemTags[BOMB_STAMP] = { TEXT("Bomb_Stamp"), TEXT("Bomb_Stamp") };

	for (_uint i = 0; i < ITEM_END; ++i) {
		m_ItemState[m_ItemTags[i].first] = { false, nullptr };
	}
}

void CPlayerData_Manager::Free()
{
	for (auto& PlayerItem : m_ItemState)
		Safe_Release(PlayerItem.second.second);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	__super::Free();
}
