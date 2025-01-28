#include "stdafx.h"
#include "Level_Loading.h"

#include "Loader.h"

#include "GameInstance.h"
#include "Event_Manager.h"
#include "BackGround.h"


CLevel_Loading::CLevel_Loading(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CLevel(_pDevice, _pContext)
{
}

HRESULT CLevel_Loading::Initialize(LEVEL_ID _eNextLevelID, CImguiLogger* _pLogger)
{
    m_eNextLevelID = _eNextLevelID;

    m_pGameInstance->Set_DebugRender(false);

    CBackGround::BACKGROUND_DESC Desc = {};

    _bool isLoadingBackground = false;
    switch (m_eNextLevelID)
    {
    case Map_Tool::LEVEL_TOOL_3D_MAP:
        Desc.strTextureComponentTag = L"Prototype_Component_Texture_MapTool_3D_Map";
        isLoadingBackground = true;
        break;
    case Map_Tool::LEVEL_TOOL_3D_MODEL:
        Desc.strTextureComponentTag = L"Prototype_Component_Texture_MapTool_3D_Model";
        isLoadingBackground = true;
        break;
    case Map_Tool::LEVEL_TOOL_2D_MAP:
        Desc.strTextureComponentTag = L"Prototype_Component_Texture_MapTool_2D_Map";
        isLoadingBackground = true;
        break;

    default:
        break;
    }

    if (isLoadingBackground)
    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_BackGround"),
        LEVEL_LOADING, L"Layer_Background", &Desc)))
        return E_FAIL;



    m_pLoader = CLoader::Create(m_pDevice, m_pContext, m_eNextLevelID, _pLogger);
    if (nullptr == m_pLoader)
        return E_FAIL;

    return S_OK;
}

void CLevel_Loading::Update(_float _fTimeDelta)
{
    if (true == m_pLoader->isFinished())
        Event_LevelChange(m_eNextLevelID);

    static _wstring strLoading = L"Loading";
    
}

HRESULT CLevel_Loading::Render()
{
#ifdef _DEBUG
    m_pLoader->Show_Debug();
#endif
    return S_OK;
}

CLevel_Loading* CLevel_Loading::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, LEVEL_ID _eNextLevelID, CImguiLogger* _pLogger)
{
    CLevel_Loading* pInstance = new CLevel_Loading(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize(_eNextLevelID, _pLogger)))
    {
        MSG_BOX("Failed to Created : CLevel_Loading");
        Safe_Release(pInstance);
    }


    return pInstance;
}

void CLevel_Loading::Free()
{
    Safe_Release(m_pLoader);
    __super::Free();
}
