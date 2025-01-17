#include "stdafx.h"
#include "Loader.h"

#include "GameInstance.h"
#include "CriticalSectionGuard.h"

#include "Camera_Free.h"
#include "Camera_Target.h"

/* For. UI*/
#include "Pick_Bulb.h"
#include "SettingPanel.h"
#include "StopStamp.h"
#include "BombStamp.h"
#include "ArrowForStamp.h"
/* For. UI*/

#include "ModelObject.h"
#include "TestPlayer.h"
#include "TestTerrain.h"

#include "Beetle.h"
#include "BarfBug.h"
#include "2DModel.h"
#include "3DModel.h"
#include "Controller_Model.h"
#include "FSM.h"

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
    case Client::LEVEL_STATIC:
        hr = Loading_Level_Static();
         break;
    case Client::LEVEL_LOGO:
        hr = Loading_Level_Logo();
        break;
    case Client::LEVEL_GAMEPLAY:
        hr = Loading_Level_GamePlay();
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
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_OptionBG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Static/T_Panel-Bottom.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_OptionBG_Outline"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Logo/BG/T_bg_border.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_OptionBG_OutOutline"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Logo/BG/T_bg_border_outline.dds"), 1))))
		return E_FAIL;


              

    lstrcpy(m_szLoadingText, TEXT("사운드를 로딩중입니다."));

    lstrcpy(m_szLoadingText, TEXT("쉐이더를 로딩중입니다."));

    /* Shader */
    /* For. Prototype_Component_Shader_VtxPosTex */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
        CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
        return E_FAIL;
    /* For. Prototype_Component_Shader_VtxNorTex */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
        CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
        return E_FAIL;
    /* For. Prototype_Component_Shader_VtxMesh */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
        return E_FAIL;
    /* For. Prototype_Component_Shader_VtxAnimMesh */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
        return E_FAIL;
    /* For. Prototype_Component_Shader_VtxCube */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxCube"),
        CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
        return E_FAIL;

    ///* For. Prototype_Component_Shader_VtxRectInstance */
    //if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxRectInstance"),
    //    CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxRectInstance.hlsl"), VTXRECTINSTANCE::Elements, VTXRECTINSTANCE::iNumElements))))
    //    return E_FAIL;

    /* For. Prototype_Component_Shader_VtxPointInstance */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPointInstance"),
        CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance.hlsl"), VTXPOINTPARTICLE::Elements, VTXPOINTPARTICLE::iNumElements))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("모델(을)를 로딩중입니다."));

    /* For. Prototype_Component_VIBuffer_Rect */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("객체원형(을)를 로딩중입니다."));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_UIObejct_SettingPanel"), CSettingPanel::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"),
        CModelObject::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    
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


HRESULT CLoader::Loading_Level_GamePlay()
{
    lstrcpy(m_szLoadingText, TEXT("컴포넌트를 로딩중입니다."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_FSM"),
        CFSM::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_PickBulb"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Bulb_Pickup/pickup_counter_bulb_01.dds"), 1))))
		return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BombStamp"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Object/Stamp/Resize_BombStamp.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_StopStamp"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Object/Stamp/Resize_StopStamp.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ArrowForStamp"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Object/Stamp/Arrow.dds"), 1))))
		return E_FAIL;



    lstrcpy(m_szLoadingText, TEXT("사운드를 로딩중입니다."));

    lstrcpy(m_szLoadingText, TEXT("쉐이더를 로딩중입니다."));

    lstrcpy(m_szLoadingText, TEXT("모델(을)를 로딩중입니다."));

    /* For. Prototype_Component_VIBuffer_Rect */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect"),
        CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
        return E_FAIL;


    /* 낱개 로딩 예시*/
    //if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_WoodenPlatform_01"),
    //    C3DModel::Create(m_pDevice, m_pContext,  "../Bin/Resources/Models/WoodenPlatform_01/WoodenPlatform_01.model", XMMatrixScaling(1.0f / 150.f, 1.0f / 150.f, 1.0f / 150.f)))))
    //    return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_player2DAnimation"),
        C2DModel::Create(m_pDevice, m_pContext, ("../Bin/Resources/TestModels/2DAnim/Player/player2DAnimation.json")))))
        return E_FAIL;
    XMMATRIX matPretransform = XMMatrixScaling(1 / 150.0f, 1 / 150.0f, 1 / 150.0f);
    matPretransform *= XMMatrixRotationAxis(_vector{0,1,0,0},XMConvertToRadians(180));
    if (FAILED(Load_Dirctory_Models_Recursive(LEVEL_GAMEPLAY,
        TEXT("../Bin/Resources/TestModels/"), matPretransform)))
        return E_FAIL;


    lstrcpy(m_szLoadingText, TEXT("객체원형(을)를 로딩중입니다."));


    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_TestPlayer"),
        CTestPlayer::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    /* For. Prototype_GameObject_TestTerrain */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_TestTerrain"),
        CTestTerrain::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_Camera_Free */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Camera_Free"),
        CCamera_Free::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_Camera_Target */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Camera_Target"),
        CCamera_Target::Create(m_pDevice, m_pContext))))
        return E_FAIL;


    ///////////////////////////////// UI /////////////////////////////////
    /* For. Prototype_UIObject_Pick_Bubble */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_UIObejct_PickBubble"),
        CPick_Bulb::Create(m_pDevice, m_pContext))))
        return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_StopStamp"),
		CStopStamp::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_BombStamp"),
		CBombStamp::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_ArrowForStamp"),
		CArrowForStamp::Create(m_pDevice, m_pContext))))
		return E_FAIL;


    /* Monster */

    /* For. Prototype_GameObject_Beetle */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Beetle"),
        CBeetle::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_BarfBug */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_BarfBug"),
        CBarfBug::Create(m_pDevice, m_pContext))))
        return E_FAIL;



    lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
    m_isFinished = true;

    return S_OK;
}

HRESULT CLoader::Load_Dirctory_Models(_uint _iLevId, const _tchar* _szDirPath, _fmatrix _PreTransformMatrix)
{
    WIN32_FIND_DATA		FindFileData = {};
    HANDLE				hFind = INVALID_HANDLE_VALUE;

    _tchar				szFilePath[MAX_PATH] = TEXT("");
    _tchar				szFullPath[MAX_PATH] = TEXT("");
    _tchar				szProtoTag[MAX_PATH] = TEXT("");
    _tchar				szExtension[MAX_PATH] = TEXT(".model");

    lstrcpy(szFilePath, _szDirPath);
    lstrcat(szFilePath, TEXT("*"));
    lstrcat(szFilePath, szExtension);

    hFind = FindFirstFile(szFilePath, &FindFileData);

    if (INVALID_HANDLE_VALUE == hFind)
        return E_FAIL;

    do
    {
        if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            continue;

        lstrcpy(szFullPath, _szDirPath);
        lstrcat(szFullPath, FindFileData.cFileName);

        wstring wstr = szFullPath;
        string str = m_pGameInstance->WStringToString(wstr);

        //string str{ wstr.begin(), wstr.end() };

        wstring filename = wstring(FindFileData.cFileName);
        size_t lastDot = filename.find_last_of('.');
        filename = filename.substr(0, lastDot); // 확장자 제거

        if (FAILED(m_pGameInstance->Add_Prototype(_iLevId, filename.c_str(),
            C3DModel::Create(m_pDevice, m_pContext, str.c_str(), _PreTransformMatrix))))
            return E_FAIL;

    } while (FindNextFile(hFind, &FindFileData));

    FindClose(hFind);

    return S_OK;
}

HRESULT CLoader::Load_Dirctory_2DModels(_uint _iLevId, const _tchar* _szDirPath)
{

    WIN32_FIND_DATA		FindFileData = {};
    HANDLE				hFind = INVALID_HANDLE_VALUE;

    _tchar				szFilePath[MAX_PATH] = TEXT("");
    _tchar				szFullPath[MAX_PATH] = TEXT("");
    _tchar				szProtoTag[MAX_PATH] = TEXT("");
    _tchar				szExtension[MAX_PATH] = TEXT(".json");

    lstrcpy(szFilePath, _szDirPath);
    lstrcat(szFilePath, TEXT("*"));
    lstrcat(szFilePath, szExtension);

    hFind = FindFirstFile(szFilePath, &FindFileData);

    if (INVALID_HANDLE_VALUE == hFind)
        return E_FAIL;

    do
    {
        if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            continue;

        lstrcpy(szFullPath, _szDirPath);
        lstrcat(szFullPath, FindFileData.cFileName);

        wstring wstr = szFullPath;
        string str = m_pGameInstance->WStringToString(wstr);

        //string str{ wstr.begin(), wstr.end() };

        wstring filename = wstring(FindFileData.cFileName);
        size_t lastDot = filename.find_last_of('.');
        filename = filename.substr(0, lastDot); // 확장자 제거

        if (FAILED(m_pGameInstance->Add_Prototype(_iLevId, filename.c_str(),
            C2DModel::Create(m_pDevice, m_pContext, str.c_str()))))
            return E_FAIL;

    } while (FindNextFile(hFind, &FindFileData));

    FindClose(hFind);

    return S_OK;
}

HRESULT CLoader::Load_Dirctory_Models_Recursive(_uint _iLevId, const _tchar* _szDirPath, _fmatrix _PreTransformMatrix)
{
	std::filesystem::path path;
    path = _szDirPath;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
        if (entry.path().extension() == ".model") {
            //cout << entry.path().string() << endl;

            if (FAILED(m_pGameInstance->Add_Prototype(_iLevId, entry.path().filename().replace_extension(),
                C3DModel::Create(m_pDevice, m_pContext, entry.path().string().c_str(), _PreTransformMatrix))))
                return E_FAIL;
        }
    }
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
