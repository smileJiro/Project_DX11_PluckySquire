#include "stdafx.h"
#include "Effect_Manager.h"
#include "Effect_System.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CEffect_Manager)


CEffect_Manager::CEffect_Manager()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

void CEffect_Manager::Update_Manager()
{
#ifdef _DEBUG
	Update_DebugWindow();
#endif

	for (auto iter = m_UpdateSystems.begin(); iter != m_UpdateSystems.end(); )
	{
		if (false == (*iter)->Is_Active())
		{
			iter = m_UpdateSystems.erase(iter);
		}
		else
			++iter;
	}
}

void CEffect_Manager::Level_Exit()
{
	m_UpdateSystems.clear();

	for (auto& Pair : m_EffectSystems)
		Safe_Release(Pair.second);


	m_EffectSystems.clear();
}

HRESULT CEffect_Manager::Add_Effect(CEffect_System* _pEffectSystem, _bool _isInitialActive)
{
	if (nullptr == _pEffectSystem)
		return E_FAIL;

	m_EffectSystems.emplace(_pEffectSystem->Get_Name(), _pEffectSystem);
	Safe_AddRef(_pEffectSystem);
	
	if (false == _isInitialActive)
		_pEffectSystem->Set_Active(false);
	return S_OK;
}

void CEffect_Manager::Active_EffectPosition(const _tchar* _szEffectTag, _bool _isReset, _fvector _vPosition)
{
	_bool _isActive;
	CEffect_System* pEffect = Find_Effect(_szEffectTag, &_isActive);

	if (nullptr == pEffect)
		return;

	pEffect->Set_Position(_vPosition);

	//pEffect->Set_Active(true);
	pEffect->Active_All(_isReset);

	if (false == _isActive)
		m_UpdateSystems.push_back(pEffect);
}

void CEffect_Manager::Active_EffectPositionID(const _tchar* _szEffectTag, _bool _isReset, _fvector _vPosition, _uint _iEffectID)
{
	_bool _isActive;
	CEffect_System* pEffect = Find_Effect(_szEffectTag, &_isActive);

	if (nullptr == pEffect)
		return;

	pEffect->Set_Position(_vPosition);

	//pEffect->Set_Active(true);
	pEffect->Active_Effect(_isReset, _iEffectID);

	if (false == _isActive)
		m_UpdateSystems.push_back(pEffect);
}


void CEffect_Manager::Active_Effect(const _tchar* _szEffectTag, _bool _isReset, const _float4x4* _pWorldMatrix)
{
	_bool _isActive;
 	CEffect_System* pEffect = Find_Effect(_szEffectTag, &_isActive);

	if (nullptr == pEffect)
		return;

	pEffect->Set_SpawnMatrix(_pWorldMatrix);

	pEffect->Set_Active(true);
	pEffect->Active_All(_isReset);
	
	if (false == _isActive)
		m_UpdateSystems.push_back(pEffect);
}

void CEffect_Manager::Active_EffectID(const _tchar* _szEffectTag, _bool _isReset, const _float4x4* _pWorldMatrix, _uint _iEffectID)
{
	_bool _isActive;
	CEffect_System* pEffect = Find_Effect(_szEffectTag, &_isActive);

	if (nullptr == pEffect)
		return;

	pEffect->Set_SpawnMatrix(_pWorldMatrix);


	//pEffect->Set_Active(true);
	pEffect->Active_Effect(_isReset, _iEffectID);

	if (false == _isActive)
		m_UpdateSystems.push_back(pEffect);
}

void CEffect_Manager::InActive_Effect(const _tchar* _szEffectTag)
{
	for (auto iter = m_UpdateSystems.begin(); iter != m_UpdateSystems.end(); )
	{
		if (0 == lstrcmp((*iter)->Get_Name().c_str(), _szEffectTag))
		{
			(*iter)->Inactive_All();
			m_UpdateSystems.erase(iter);
			break;
		}
		else
			++iter;
	}

}

void CEffect_Manager::InActive_EffectID(const _tchar* _szEffectTag, _uint _iEffectID)
{
	for (auto iter = m_UpdateSystems.begin(); iter != m_UpdateSystems.end(); )
	{
		if (0 == lstrcmp((*iter)->Get_Name().c_str(), _szEffectTag))
		{
			(*iter)->Inactive_Effect(_iEffectID);
			break;
		}
		else
			++iter;
	}
}

void CEffect_Manager::Stop_Spawn(const _tchar* _szEffectTag, _float _fDelayTime)
{
	for (auto iter = m_UpdateSystems.begin(); iter != m_UpdateSystems.end(); )
	{
		if (0 == lstrcmp((*iter)->Get_Name().c_str(), _szEffectTag))
		{
			(*iter)->Stop_SpawnAll(_fDelayTime);
			break;
		}
		else
			++iter;
	}
}

void CEffect_Manager::Stop_SpawnID(const _tchar* _szEffectTag, _float _fDelayTime, _uint _iEffectID)
{
	for (auto iter = m_UpdateSystems.begin(); iter != m_UpdateSystems.end(); )
	{
		if (0 == lstrcmp((*iter)->Get_Name().c_str(), _szEffectTag))
		{
			(*iter)->Stop_Spawn(_fDelayTime, _iEffectID);
			break;
		}
		else
			++iter;
	}
}

CEffect_System* CEffect_Manager::Find_Effect(const _tchar* _szEffectTag, _Out_ _bool* _pActive)
{
	auto iter = m_EffectSystems.find(_szEffectTag);
	if (iter == m_EffectSystems.end())
	{
		return nullptr;
	}

	auto iterFirst = iter;
	while (iter != m_EffectSystems.end() && iter->first == _szEffectTag)
	{
		if (false == iter->second->Is_Active())
		{
			if (_pActive)
				*_pActive = false;
			return iter->second;
		}
		++iter;
	}

	if (_pActive)
		*_pActive = true;
	return iterFirst->second;
	//return nullptr;
}

void CEffect_Manager::Free()
{
	for (auto& Pair : m_EffectSystems)
		Safe_Release(Pair.second);
	m_EffectSystems.clear();

	Safe_Release(m_pGameInstance);

	__super::Free();
}


#ifdef _DEBUG
void CEffect_Manager::Update_DebugWindow()
{
	//ImGui::Begin("Debug_Effect");
	//std::vector<std::string> Keys;

	//static _int iSelected = -1;
	//_wstring wstrName = L"Start";

	//_int i = 0;
	//for (const auto& Pair : m_EffectSystems) 
	//{
	//	
	//	if (Pair.first == wstrName)
	//	{
	//		++i;
	//	}
	//	else
	//	{
	//		i = 0;
	//		wstrName = Pair.first;
	//	}

	//	_string strName = WSTRINGTOSTRING(Pair.first) + to_string(i);
	//	Keys.push_back(strName);
	//}

	//std::vector<const char*> itemCStrs;
	//for (const auto& str : Keys) {
	//	itemCStrs.push_back(str.c_str());
	//}

	////if (ImGui::ListBox("Items", &iSelected,
	////	[](void* data, _int idx, const _char** out_text) {
	////		auto& names = *static_cast<std::vector<std::string>*>(data);
	////		*out_text = names[idx].c_str();
	////		return true;
	////	},
	////	& Keys, static_cast<int>(Keys.size()), 5))

	//if (ImGui::ListBox("Effects", &iSelected, itemCStrs.data(), static_cast<int>(itemCStrs.size()))) 
	//{
	//	/*int selectedKey = std::next(items.begin(), selectedIndex)->first;
	//	ImGui::Text("Selected Key: %d, Value: %s", selectedKey, items[selectedKey].c_str());*/
	//}

	//ImGui::End();
}
#endif
