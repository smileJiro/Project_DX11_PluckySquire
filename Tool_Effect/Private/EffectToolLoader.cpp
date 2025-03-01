#include "pch.h"
#include "EffectToolLoader.h"
#include "GameInstance.h"

#include "MainEffectTool.h"

#include "Effect_Reference.h"
#include "ModelObject.h"
#include "Magic_Hand_Body.h"
#include "Magic_Hand.h"
#include "TestBeam.h"

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
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Effect_ForNew"),
        CEffect_System::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_GameObject_EffectReference"),
        CEffect_Reference::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Reference"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Particles/Baodian024.dds")))))
        return E_FAIL;


    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_TestBeam"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effects/T_FX_CMN_Mask_07.dds")))))
        return E_FAIL;

    

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_GameObject_MagicHandBody"),
        CMagic_Hand_Body::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_GameObject_MagicHand"),
        CMagic_Hand::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_GameObject_TestBeam"),
        CTestBeam::Create(m_pDevice, m_pContext))))
        return E_FAIL;


    XMMATRIX matPretransform = XMMatrixScaling(1 / 150.0f, 1 / 150.0f, 1 / 150.0f);
    matPretransform *= XMMatrixRotationAxis(_vector{ 0,1,0,0 }, XMConvertToRadians(180));

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Model_Player"),
        C3DModel::Create(m_pDevice, m_pContext, 
            ("../Bin/Resources/Models/3DAnim/Latch_SkelMesh_NewRig/Latch_SkelMesh_NewRig.model"
            ), matPretransform))))
        return E_FAIL;

    matPretransform = XMMatrixScaling(1 / 150.0f, 1 / 150.0f, 1 / 150.0f);

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Model_Book"),
        C3DModel::Create(m_pDevice, m_pContext,
            ("../Bin/Resources/Models/3DAnim/book/book.model"
                ), matPretransform))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Model_MagicHand"),
        C3DModel::Create(m_pDevice, m_pContext,
            ("../Bin/Resources/Models/3DAnim/magic_hand_model/magic_hand_model.model"
                ), matPretransform))))
        return E_FAIL;

    matPretransform = XMMatrixScaling(1 / 150.0f, 1 / 150.0f, 1 / 150.0f);
    matPretransform *= XMMatrixRotationAxis(_vector{ 0,1,0,0 }, XMConvertToRadians(180));
    
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Model_Bomb"),
        C3DModel::Create(m_pDevice, m_pContext,
            ("../Bin/Resources/Models/Nonanim/bomb/bomb.model"
                ), matPretransform))))
        return E_FAIL;


    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Effect_MagicHand"),
        CEffect_System::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Effects/BookOut.json")))))
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
