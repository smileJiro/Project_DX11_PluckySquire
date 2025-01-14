#include "stdafx.h"
#include "Loader.h"

#include "GameInstance.h"
#include "CriticalSectionGuard.h"

#include "Camera_Free.h"

CLoader::CLoader(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
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
    CLoader* pLoader = static_cast<CLoader*>(pArg);

    if (FAILED(pLoader->Loading()))
        return 1;

    return 0;
}


HRESULT CLoader::Initialize(LEVEL_ID _eNextLevelID)
{
    m_eNextLevelID = _eNextLevelID;

    InitializeCriticalSection(&m_Critical_Section);
    m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);
    if (0 == m_hThread)
        return E_FAIL;

    return S_OK;
}

HRESULT CLoader::Loading()
{
    CCriticalSectionGuard csGuard(&m_Critical_Section);

    HRESULT coInitiResult = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    if (FAILED(coInitiResult))
        return coInitiResult;

    HRESULT hr = {};

    switch (m_eNextLevelID)
    {
    case Map_Tool::LEVEL_STATIC:
        hr = Loading_Level_Static();
         break;
    case Map_Tool::LEVEL_TOOL_MAP:
        hr = Loading_Level_Logo();
        break;
    case Map_Tool::LEVEL_TOOL_TRIGGER:
        hr = S_OK;
        break;
    }
    
    CoUninitialize();

    if (FAILED(hr))
    {
        return hr;
    }

    return S_OK;
}

#ifdef _DEBUG
void CLoader::Show_Debug()
{
    SetWindowText(g_hWnd, m_szLoadingText);
}
#endif // DEBUG

HRESULT CLoader::Loading_Level_Static()
{
    lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

    lstrcpy(m_szLoadingText, TEXT("사운드를 로딩중입니다."));

    lstrcpy(m_szLoadingText, TEXT("쉐이더를 로딩중입니다."));

    /* Shader */
    /* For. Prototype_Component_Shader_VtxPosTex */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
        CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
        return E_FAIL;
    /* For. Prototype_Component_Shader_VtxNorTex */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
        CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
        return E_FAIL;
    /* For. Prototype_Component_Shader_VtxMesh */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
        return E_FAIL;
    /* For. Prototype_Component_Shader_VtxAnimMesh */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
        return E_FAIL;
    /* For. Prototype_Component_Shader_VtxCube */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxCube"),
        CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
        return E_FAIL;

    /* For. Prototype_Component_Shader_VtxRectInstance */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxRectInstance"),
        CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxRectInstance.hlsl"), VTXRECTINSTANCE::Elements, VTXRECTINSTANCE::iNumElements))))
        return E_FAIL;

    /* For. Prototype_Component_Shader_VtxPointInstance */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPointInstance"),
        CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxPointInstance.hlsl"), VTXPOINTINSTANCE::Elements, VTXPOINTINSTANCE::iNumElements))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("모델(을)를 로딩중입니다."));

    /* For. Prototype_Component_VIBuffer_Rect */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("객체원형(을)를 로딩중입니다."));


    lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
    m_isFinished = true;

    return S_OK;
}

HRESULT CLoader::Loading_Level_Logo()
{
    lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

    lstrcpy(m_szLoadingText, TEXT("사운드를 로딩중입니다."));

    lstrcpy(m_szLoadingText, TEXT("쉐이더를 로딩중입니다."));

    lstrcpy(m_szLoadingText, TEXT("모델(을)를 로딩중입니다."));

    lstrcpy(m_szLoadingText, TEXT("객체원형(을)를 로딩중입니다."));

    lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
    m_isFinished = true;

    return S_OK;
}

HRESULT CLoader::Loading_Level_Trigger()
{
    return E_NOTIMPL;
}


HRESULT CLoader::Loading_Level_Map_Tool()
{
    lstrcpy(m_szLoadingText, TEXT("컴포넌트를 로딩중입니다."));

    lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

    lstrcpy(m_szLoadingText, TEXT("사운드를 로딩중입니다."));

    lstrcpy(m_szLoadingText, TEXT("쉐이더를 로딩중입니다."));

    lstrcpy(m_szLoadingText, TEXT("모델(을)를 로딩중입니다."));

    /* For. Prototype_Component_VIBuffer_Rect */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_MAP, TEXT("Prototype_Component_VIBuffer_Rect"),
        CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_Component_Model_Test */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_MAP, TEXT("Prototype_Component_Model_Test"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/Models/Test/Tree_Mod_03.model", XMMatrixScaling(1.0f / 150.f, 1.0f / 150.f, 1.0f / 150.f)))))
        return E_FAIL;

    /* For. Prototype_Component_Model_WoodenPlatform_01 */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_MAP, TEXT("Prototype_Component_Model_WoodenPlatform_01"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/Models/WoodenPlatform_01/WoodenPlatform_01.model", XMMatrixScaling(1.0f / 150.f, 1.0f / 150.f, 1.0f / 150.f)))))
        return E_FAIL;
    

    lstrcpy(m_szLoadingText, TEXT("객체원형(을)를 로딩중입니다."));

    /* For. Prototype_GameObject_Camera_Target */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_MAP, TEXT("Prototype_GameObject_Camera_Free"),
        CCamera_Free::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
    m_isFinished = true;

    return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, LEVEL_ID _eNextLevelID)
{
    CLoader* pInstance = new CLoader(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize(_eNextLevelID)))
    {
        MSG_BOX("Failed to Created : CLoader");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLoader::Free()
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
