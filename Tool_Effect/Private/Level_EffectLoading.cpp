#include "pch.h"
#include "Level_EffectLoading.h"
#include "GameInstance.h"
#include "EffectToolLoader.h"

#include "Event_Manager.h"


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
	if (true == m_pLoader->isFinished() && KEY_DOWN(KEY::ENTER))
	{
		EVENT tEvent;
		tEvent.eType = EVENT_TYPE::LEVEL_CHANGE;
		tEvent.Parameters.resize(2); // NumParameters

		tEvent.Parameters[0] = (DWORD_PTR)m_eNextLevelID;

		CEvent_Manager::GetInstance()->AddEvent(tEvent);
	}

	static _wstring strLoading = L"Loading";
}

HRESULT CLevel_EffectLoading::Render()
{
#ifdef _DEBUG
	m_pLoader->Show_Debug();
#endif
	return S_OK;

	return S_OK;
}

CLevel_EffectLoading* CLevel_EffectLoading::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, LEVEL_ID _eNextLevelID)
{
	CLevel_EffectLoading* pInstance = new CLevel_EffectLoading(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_eNextLevelID)))
	{
		MSG_BOX("Failed to Created : CLevel_EffectLoading");
		Safe_Release(pInstance);
	}


	return pInstance;
}

void CLevel_EffectLoading::Free()
{
	Safe_Release(m_pLoader);
	__super::Free();
}
