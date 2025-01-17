#include "pch.h"
#include "EffectToolLoader.h"

#include "GameInstance.h"

#include "MainEffectTool.h"


CEffectToolLoader::CEffectToolLoader(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : m_pDevice(_pDevice)
    , m_pContext(_pContext)
    , m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

_uint APIENTRY LoadingMain(void* pArg)
{
    CEffectToolLoader* pLoader = static_cast<CEffectToolLoader*>(pArg);

    if (FAILED(pLoader->Loading()))
        return 1;

    return 0;
}

HRESULT CEffectToolLoader::Initialize(LEVEL_ID _eNextLevelID)
{
    m_eNextLevelID = _eNextLevelID;

    InitializeCriticalSection(&m_Critical_Section);

    m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);
    if (0 == m_hThread)
        return E_FAIL;

    return S_OK;
}

HRESULT CEffectToolLoader::Loading()
{
	EnterCriticalSection(&m_Critical_Section);

	CoInitializeEx(nullptr, 0);

	HRESULT hr = {};

	switch (m_eNextLevelID)
	{
    case EffectTool::LEVEL_STATIC:
        break;
    case EffectTool::LEVEL_TOOL:
        hr = Loading_Level_Tool();
        break;
	}


	LeaveCriticalSection(&m_Critical_Section);


	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}

void CEffectToolLoader::Show_Debug()
{
    SetWindowText(g_hWnd, m_szLoadingText);

}


HRESULT CEffectToolLoader::Loading_Level_Tool()
{
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Effect_Temp"),
        CSprite_Particle::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Effects/TestEffect.json")))))
        return E_FAIL;


    lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
    m_isFinished = true;

    return S_OK;
}

CEffectToolLoader* CEffectToolLoader::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, LEVEL_ID _eNextLevelID)
{
    CEffectToolLoader* pInstance = new CEffectToolLoader(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize(_eNextLevelID)))
    {
        MSG_BOX("Failed to Created : CEffectToolLoader");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEffectToolLoader::Free()
{
    __super::Free();

    // 해당 쓰레드의 동작이 완전히 완료될 때 까지 무한히 기다린다.
    WaitForSingleObject(m_hThread, INFINITE);
    // 쓰레드를 삭제한다.
    DeleteObject(m_hThread);
    // 크리티컬 섹션을 삭제한다.
    DeleteCriticalSection(&m_Critical_Section);

    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);
    Safe_Release(m_pGameInstance);
}
