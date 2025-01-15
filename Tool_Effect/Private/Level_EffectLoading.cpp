#include "pch.h"
#include "Level_EffectLoading.h"
#include "GameInstance.h"
#include "EffectToolLoader.h"


CLevel_EffectLoading::CLevel_EffectLoading(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext) 
	: CLevel(_pDevice, _pContext)
{
}

HRESULT CLevel_EffectLoading::Initialize(LEVEL_ID _eNextLevelID)
{
	m_eNextLevelID = _eNextLevelID;

	m_pLoader = CEffectToolLoader::Create(m_pDevice, m_pContext, m_eNextLevelID);
	if (nullptr == m_pLoader)
		return E_FAIL;

	return S_OK;
}

void CLevel_EffectLoading::Update(_float _fTimeDelta)
{
	static _wstring strLoading = L"Loading";
}

HRESULT CLevel_EffectLoading::Render()
{
	return E_NOTIMPL;
}

CLevel_EffectLoading* CLevel_EffectLoading::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, LEVEL_ID _eNextLevelID)
{
	return nullptr;
}

void CLevel_EffectLoading::Free()
{
}
