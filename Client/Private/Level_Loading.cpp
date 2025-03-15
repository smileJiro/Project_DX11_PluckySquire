#include "stdafx.h"
#include "Level_Loading.h"

#include "Loader.h"

#include "GameInstance.h"
#include "Event_Manager.h"

#include "Level_Logo.h"

#include "Logo_BackGround.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CLevel(_pDevice, _pContext)
{
}

HRESULT CLevel_Loading::Initialize(LEVEL_ID _eNextLevelID)
{
    m_eNextLevelID = _eNextLevelID;
    Ready_DeafultCameraSet();

    m_pLoader = CLoader::Create(m_pDevice, m_pContext, m_eNextLevelID);
    if (nullptr == m_pLoader)
        return E_FAIL;

    if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
        return E_FAIL;

    return S_OK;
}

void CLevel_Loading::Update(_float _fTimeDelta)
{
    if (true == m_pLoader->isFinished())
    {
        if (LEVEL_CHAPTER_8 == m_eNextLevelID) {
            if (KEY_DOWN(KEY::ENTER)) {
                Event_LevelChange(m_eNextLevelID);
            }
        }
        else {
            Event_LevelChange(m_eNextLevelID);
        }
    }

    static _wstring strLoading = L"Loading";
}

HRESULT CLevel_Loading::Render()
{
    
#ifdef _DEBUG
    m_pLoader->Show_Debug();
#endif
    return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
    if (LEVEL_CHAPTER_8 == m_eNextLevelID) {
        CLogo_BackGround::MAIN_LOGO_DESC		Desc{};

        Desc.vColor = { 1.f, 47.f, 47.f, 1.f };
        Desc.iBackGroundMainType = CLogo_BackGround::MAIN_HUMGRUMP;

        if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Logo_BackGround"),
            LEVEL_LOADING, strLayerTag, &Desc)))
            return E_FAIL;
    }

    return S_OK;
}

CLevel_Loading* CLevel_Loading::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, LEVEL_ID _eNextLevelID)
{
    CLevel_Loading* pInstance = new CLevel_Loading(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize(_eNextLevelID)))
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
