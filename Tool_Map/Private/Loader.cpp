#include "stdafx.h"
#include "Loader.h"

#include "GameInstance.h"
#include "CriticalSectionGuard.h"

#include "Camera_Free.h"
//#include "CellContainor.h"
#include "MapObject.h"
#include "Ray.h"
#include "2DDefault_RenderObject.h"
#include "2DTile_RenderObject.h"
#include "2DMapObject.h"
#include "SampleBook.h"
#include "Sample_Skechspace.h"
#include "BackGround.h"
#include "2DModel.h"
#include "2DTrigger_Sample.h"


#include "CubeMap.h"

#include <filesystem>
#include <iostream>


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


HRESULT CLoader::Initialize(LEVEL_ID _eNextLevelID, CImguiLogger* _pLogger)
{
    m_eNextLevelID = _eNextLevelID;
    m_pLogger = _pLogger;
    Safe_AddRef(m_pLogger);
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
    case Map_Tool::LEVEL_TOOL_2D_MAP:
        hr = Loading_Level_2D_Map_Tool();
        break;
    case Map_Tool::LEVEL_TOOL_3D_MAP:
        hr = Loading_Level_3D_Map_Tool();
        break;
    case Map_Tool::LEVEL_TOOL_3D_MODEL:
        hr = Loading_Level_3D_Model_Tool();
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
    lstrcpy(m_szLoadingText, TEXT("액터를 로딩중입니다."));

    /* For. Prototype_Component_Actor_Dynamic */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Actor_Dynamic"),
        CActor_Dynamic::Create(m_pDevice, m_pContext, false))))
        return E_FAIL;

    /* For. Prototype_Component_Actor_Kinematic */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Actor_Kinematic"),
        CActor_Dynamic::Create(m_pDevice, m_pContext, true))))
        return E_FAIL;

    /* For. Prototype_Component_Actor_Static */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Actor_Static"),
        CActor_Static::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MapTool_Logo"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/MapTool_Logo.png")))))
        return E_FAIL;
    
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MapTool_2D_Map"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/MapTool_2D_Map.png")))))
        return E_FAIL;
    
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MapTool_3D_Map"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/MapTool_3D_Map.png")))))
        return E_FAIL;
        
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MapTool_3D_Model"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/MapTool_3D_Model.png")))))
        return E_FAIL;

    /* For. Prototype_Component_Texture_BRDF_Shilick */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BRDF_Shilick"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/CubeMap/HDRI/BRDF_Shilick.dds"), 1))))
        return E_FAIL;

    /* For. Prototype_Component_Texture_TestEnv */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_TestEnv"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/CubeMap/HDRI/TestEnv/TestEnv_%d.dds"), 3, true))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("사운드를 로딩중입니다."));

    lstrcpy(m_szLoadingText, TEXT("컴포넌트를 로딩중입니다."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Ray"),
        CRay::Create(m_pDevice, m_pContext))))
        return E_FAIL;

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
        CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
        return E_FAIL;
    /* For. Prototype_Component_Shader_VtxCube */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxCube"),
        CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
        return E_FAIL;
    /* For. Prototype_Component_Shader_VtxPointInstance */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPointInstance"),
        CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxPointInstance.hlsl"), VTXPOINTPARTICLE::Elements, VTXPOINTPARTICLE::iNumElements))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMeshInstance"),
        CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxMeshInstance.hlsl"), VTXMESHPARTICLE::Elements, VTXMESHPARTICLE::iNumElements))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("모델(을)를 로딩중입니다."));

    /* For. Prototype_Component_VIBuffer_Rect */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_Component_VIBuffer_Cube */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"),
        CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    lstrcpy(m_szLoadingText, TEXT("객체원형(을)를 로딩중입니다."));

    /* For. Prototype_GameObject_CubeMap */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_CubeMap"),
        CCubeMap::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_Camera_Target */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_Free"),
        CCamera_Free::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_MapObject */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_MapObject"),
        CMapObject::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    
    /* For. Prototype_GameObject_BackGround */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_BackGround"),
        CBackGround::Create(m_pDevice, m_pContext))))
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

HRESULT CLoader::Loading_Level_3D_Model_Tool()
{
    lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

    lstrcpy(m_szLoadingText, TEXT("사운드를 로딩중입니다."));

    lstrcpy(m_szLoadingText, TEXT("쉐이더를 로딩중입니다."));

    lstrcpy(m_szLoadingText, TEXT("모델(을)를 로딩중입니다."));

    XMMATRIX matPretransform = XMMatrixScaling(1 / 150.0f, 1 / 150.0f, 1 / 150.0f);

    if (FAILED(Load_Dirctory_Models_Recursive(LEVEL_TOOL_3D_MODEL,
        TEXT("../../Client/Bin/Resources/Models"), matPretransform)))
        return E_FAIL;


    lstrcpy(m_szLoadingText, TEXT("객체원형(을)를 로딩중입니다."));

    lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
    m_isFinished = true;

    return S_OK;
}


HRESULT CLoader::Loading_Level_2D_Map_Tool()
{
    lstrcpy(m_szLoadingText, TEXT("컴포넌트를 로딩중입니다."));

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
        CCollider_Circle::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
        CCollider_AABB::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_2D_MAP, TEXT("Prototype_Component_Texture_None_Model"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/None_Model.png")))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("사운드를 로딩중입니다."));

    lstrcpy(m_szLoadingText, TEXT("쉐이더를 로딩중입니다."));

    lstrcpy(m_szLoadingText, TEXT("모델(을)를 로딩중입니다."));

    XMMATRIX matPretransform = XMMatrixScaling(1 / 150.0f, 1 / 150.0f, 1 / 150.0f);

    //if (FAILED(Load_Models_FromJson(LEVEL_TOOL_2D_MAP, MAP_3D_DEFAULT_PATH, L"Room_Enviroment.json", matPretransform)))
    //    return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_2D_MAP, TEXT("topboard"),
        C3DModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/Models/NonAnim/SM_desk_split_topboard_02/SM_desk_split_topboard_02.model", matPretransform))))
        return E_FAIL;

    if (FAILED(Load_Dirctory_Models_Recursive(LEVEL_TOOL_2D_MAP,  L"../../Client/Bin/Resources/Models/3DMapObject/", matPretransform)))
        return E_FAIL;
    
    //if (FAILED(Load_Models_FromJson(LEVEL_TOOL_2D_MAP, MAP_3D_DEFAULT_PATH, L"Chapter_04_Default_Desk.json", matPretransform, true)))
    ////    return E_FAIL;

    if (FAILED(Load_Dirctory_2DModels_Recursive(LEVEL_TOOL_2D_MAP,
        TEXT("../../Client/Bin/Resources/Models/2DMapObject/"))))
        return E_FAIL;


    lstrcpy(m_szLoadingText, TEXT("객체원형(을)를 로딩중입니다."));

    /* For. Prototype_GameObject_MapObject */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_2D_MAP, TEXT("Prototype_GameObject_2DDefaultRenderObject"),
        C2DDefault_RenderObject::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_2D_MAP, TEXT("Prototype_GameObject_2DTile_RenderObject"),
        C2DTile_RenderObject::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_2D_MAP, TEXT("Prototype_GameObject_2DMapObject"),
        C2DMapObject::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_2D_MAP, TEXT("Prototype_GameObject_SampleBook"),
        CSampleBook::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_2D_MAP, TEXT("Prototype_GameObject_Sample_Skechspace"),
        CSample_Skechspace::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_2D_MAP, TEXT("Prototype_GameObject_2DTrigger"),
        C2DTrigger_Sample::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
    m_isFinished = true;

    return S_OK;
}


HRESULT CLoader::Loading_Level_3D_Map_Tool()
{
    lstrcpy(m_szLoadingText, TEXT("컴포넌트를 로딩중입니다."));

    lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

    lstrcpy(m_szLoadingText, TEXT("사운드를 로딩중입니다."));

    lstrcpy(m_szLoadingText, TEXT("쉐이더를 로딩중입니다."));

    lstrcpy(m_szLoadingText, TEXT("모델(을)를 로딩중입니다."));

    XMMATRIX matPretransform = XMMatrixScaling(1 / 150.0f, 1 / 150.0f, 1 / 150.0f);
    
    if (FAILED(Load_Dirctory_Models_Recursive(LEVEL_TOOL_3D_MAP,
        TEXT("../../Client/Bin/Resources/Models/NonAnim"), matPretransform)))
        return E_FAIL;

    if (FAILED(Load_Dirctory_Models_Recursive(LEVEL_TOOL_3D_MAP,
        L"../../Client/Bin/Resources/Models/3DMapObject/", matPretransform)))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("객체원형(을)를 로딩중입니다."));

    lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
    m_isFinished = true;

    return S_OK;
}

HRESULT CLoader::Load_Dirctory_2DModels_Recursive(_uint _iLevId, const _tchar* _szDirPath)
{
    std::filesystem::path path;
    path = _szDirPath;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
        if (entry.path().extension() == ".model2d") {
            LOG_TYPE("2D Model Loading(" + entry.path().stem().string() + ".model)...", LOG_LOADING);
            if (FAILED(m_pGameInstance->Add_Prototype(_iLevId, entry.path().filename().replace_extension(),
                C2DModel::Create(m_pDevice, m_pContext, entry.path().string().c_str()))))
            {
                string str = "Failed to Create 2DModel";
                str += entry.path().filename().replace_extension().string();
                MessageBoxA(NULL, str.c_str(), "에러", MB_OK);
                return E_FAIL;
            }

        }
    }
    return S_OK;
}

HRESULT CLoader::Load_Models_FromJson(LEVEL_ID _iLevId, const _tchar* _szJsonFilePath, _fmatrix _PreTransformMatrix, _bool _isCollider)
{
    std::ifstream input_file(_szJsonFilePath);

    json jFileData;
    if (!input_file.is_open())
    {
        MSG_BOX("Failed to Open Json File ");
        return E_FAIL;
    }
    input_file >> jFileData;
    input_file.close();

    json& jModelList = jFileData["data"];
    set<string> strModelNames;
    for (auto& j : jModelList)
        strModelNames.insert(j.get<string>());


    std::filesystem::path path;
    path = "../../Client/Bin/Resources/Models/NonAnim/";
    string strFileName;
    _uint iLoadedCount = 0;
    _uint iLoadCount = (_uint)strModelNames.size();
    for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
        if (entry.path().extension() != ".model") continue;
        strFileName = entry.path().filename().replace_extension().string();
        if (strModelNames.find(strFileName) != strModelNames.end())
        {
            LOG_TYPE("Model Loading(" + strFileName + ".model)...", LOG_LOADING);
            auto pModel = C3DModel::Create(m_pDevice, m_pContext, entry.path().string().c_str(), _PreTransformMatrix, _isCollider);
            if (FAILED(m_pGameInstance->Add_Prototype(_iLevId, StringToWstring(strFileName),
                pModel)))
            {
                Safe_Release(pModel);
                iLoadedCount++;

                continue;
            }
            iLoadedCount++;
            if (iLoadedCount >= iLoadCount)
            {
                break;
            }
        }


    }
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
        LOG_TYPE("Model Loading(" + str + ".model)...", LOG_LOADING);

        if (FAILED(m_pGameInstance->Add_Prototype(_iLevId, filename.c_str(),
            C3DModel::Create(m_pDevice, m_pContext, str.c_str(), _PreTransformMatrix))))
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
            LOG_TYPE("Model Loading(" + entry.path().stem().string() + ".model)...", LOG_LOADING);

            if (FAILED(m_pGameInstance->Add_Prototype(_iLevId, entry.path().filename().replace_extension(),
                C3DModel::Create(m_pDevice, m_pContext, entry.path().string().c_str(), _PreTransformMatrix))))
                return E_FAIL;
        }
    }
    return S_OK;
}

HRESULT CLoader::Load_Dirctory_2DModels(_uint _iLevId, const _tchar* _szDirPath)
{

    WIN32_FIND_DATA		FindFileData = {};
    HANDLE				hFind = INVALID_HANDLE_VALUE;

    _tchar				szFilePath[MAX_PATH] = TEXT("");
    _tchar				szFullPath[MAX_PATH] = TEXT("");
    _tchar				szProtoTag[MAX_PATH] = TEXT("");
    _tchar				szExtension[MAX_PATH] = TEXT(".model2d");

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

        LOG_TYPE("2DModel Loading(" + str + ".2Dmodel)...", LOG_LOADING);

        wstring filename = wstring(FindFileData.cFileName);
        size_t lastDot = filename.find_last_of('.');
        filename = filename.substr(0, lastDot); // 확장자 제거

        if (FAILED(m_pGameInstance->Add_Prototype(_iLevId, filename.c_str(),
            C2DModel::Create(m_pDevice, m_pContext, str.c_str()))))
        {
            wstring str = TEXT("Failed to Create 2DModel");
            str += filename;
            MessageBoxW(NULL, str.c_str(), TEXT("에러"), MB_OK);
            return E_FAIL;
        }

    } while (FindNextFile(hFind, &FindFileData));

    FindClose(hFind);

    return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, LEVEL_ID _eNextLevelID, CImguiLogger* _pLogger)
{
    CLoader* pInstance = new CLoader(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize(_eNextLevelID, _pLogger)))
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
    Safe_Release(m_pLogger);
    Safe_Release(m_pGameInstance);
}
