#include "stdafx.h"
#include "Loader.h"

#include "GameInstance.h"
#include "CriticalSectionGuard.h"

//#include "Camera_Target.h"
#include "Test_Player.h"
#include "Test_Terrain.h"

#include "Camera_Free.h"
#include "Camera_Target.h"
#include "Camera_CutScene.h"
#include "2DModel.h"

#include "TriggerObject.h"

#include "StateMachine.h"

#include "MapObject.h"
#include "CubeMap.h"
#include "MainTable.h"
#include "Bulb.h"

#include "Book.h"

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
    case Camera_Tool::LEVEL_STATIC:
        hr = Loading_Level_Static();
        break;
    case Camera_Tool::LEVEL_CAMERA_TOOL:
        hr = Loading_Level_Camera_Tool();
        break;
    case Camera_Tool::LEVEL_TRIGGER_TOOL:
        hr = Loading_Level_Trigger_Tool();
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
    /* For. Prototype_Component_Texture_BRDF_Shilick */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BRDF_Shilick"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/CubeMap/HDRI/BRDF_Shilick.dds"), 1))))
        return E_FAIL;

    /* For. Prototype_Component_Texture_TestEnv */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_TestEnv"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/CubeMap/HDRI/TestEnv/TestEnv_%d.dds"), 3, true))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("Collider를 로딩중입니다."));
    
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Ray"),
        CRay::Create(m_pDevice, m_pContext))))
        return E_FAIL;

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

    lstrcpy(m_szLoadingText, TEXT("모델(을)를 로딩중입니다."));

    XMMATRIX matPretransform = XMMatrixScaling(1 / 150.0f, 1 / 150.0f, 1 / 150.0f);
    //matPretransform *= XMMatrixRotationAxis(_vector{ 0,1,0,0 }, XMConvertToRadians(180));

    //if (FAILED(Load_Models_FromJson(LEVEL_STATIC, MAP_3D_DEFAULT_PATH, L"Room_Enviroment.json", matPretransform)))
    //    return E_FAIL;

    /* For. Prototype_Component_VIBuffer_Rect */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_Component_VIBuffer_Cube */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"),
        CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("LightbulbPickup_01"),
        C3DModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/Models/NonAnim/LightbulbPickup_01/LightbulbPickup_01.model", matPretransform))))
        return E_FAIL;


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


    lstrcpy(m_szLoadingText, TEXT("객체원형(을)를 로딩중입니다."));
    
    /* For. Prototype_GameObject_CubeMap */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_CubeMap"),
        CCubeMap::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"),
        CModelObject::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_MapObject"),
        CMapObject::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_StateMachine"),
        CStateMachine::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    // Trigger
    /* For. Prototype_GameObject_Trigger */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_TriggerObject"),
        CTriggerObject::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_MainTable */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_MainTable"),
        CMainTable::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_Bulb */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Bulb"),
        CBulb::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
    m_isFinished = true;

    return S_OK;
}

HRESULT CLoader::Loading_Level_Camera_Tool()
{
    lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

    lstrcpy(m_szLoadingText, TEXT("사운드를 로딩중입니다."));

    lstrcpy(m_szLoadingText, TEXT("쉐이더를 로딩중입니다."));


    lstrcpy(m_szLoadingText, TEXT("모델(을)를 로딩중입니다."));


    /* For. Prototype_Component_VIBuffer_Rect */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CAMERA_TOOL, TEXT("Prototype_Component_VIBuffer_Rect"),
        CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CAMERA_TOOL, TEXT("Prototype_Component_player2DAnimation"),
        C2DModel::Create(m_pDevice, m_pContext, ("../../Client/Bin/Resources/Models/2DAnim/Chapter2/Player/player.model2d"), (_uint)LEVEL_CAMERA_TOOL))))
        return E_FAIL;

    XMMATRIX matPretransform = XMMatrixScaling(1 / 150.0f, 1 / 150.0f, 1 / 150.0f);

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CAMERA_TOOL, TEXT("SM_desk_split_topboard_02"),
        C3DModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/Models/NonAnim/SM_desk_split_topboard_02/SM_desk_split_topboard_02.model", matPretransform))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CAMERA_TOOL, TEXT("Prototype_Model_Book"),
        C3DModel::Create(m_pDevice, m_pContext,
            ("../../Tool_Effect/Bin/Resources/Models/3DAnim/book/book.model"
                ), matPretransform))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CAMERA_TOOL, TEXT("Prototype_Model_MagicHand"),
        C3DModel::Create(m_pDevice, m_pContext,
            ("../../Tool_Effect/Bin/Resources/Models/3DAnim/magic_hand_model/magic_hand_model.model"
                ), matPretransform))))
        return E_FAIL;

    if (FAILED(Load_Models_FromJson(LEVEL_CAMERA_TOOL, MAP_3D_DEFAULT_PATH, L"Chapter_02_Play_Desk.json", matPretransform)))
        return E_FAIL;

    if (FAILED(Load_Dirctory_Models_Recursive(LEVEL_CAMERA_TOOL,
        TEXT("../../Client/Bin/Resources/Models/3DMapObject/"), matPretransform)))
        return E_FAIL;

    matPretransform *= XMMatrixRotationAxis(_vector{ 0,1,0,0 }, XMConvertToRadians(180));

    if (FAILED(Load_Dirctory_Models_Recursive(LEVEL_CAMERA_TOOL,
        TEXT("../../Client/Bin/Resources/Models/3DAnim/"), matPretransform)))
        return E_FAIL;

    if (FAILED(Load_Dirctory_Models_Recursive(LEVEL_CAMERA_TOOL,
        TEXT("../../Client/Bin/Resources/Models/3DObject/"), matPretransform)))
        return E_FAIL;
    if (FAILED(Load_Dirctory_2DModels_Recursive(LEVEL_CAMERA_TOOL,
        TEXT("../../Client/Bin/Resources/Models/2DAnim/Chapter2/"))))
        return E_FAIL;


    lstrcpy(m_szLoadingText, TEXT("객체원형(을)를 로딩중입니다."));

    /* For. Prototype_GameObject_TestPlayer */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CAMERA_TOOL, TEXT("Prototype_GameObject_Test_Player"),
        CTest_Player::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_TestBody */
    //if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CAMERA_TOOL, TEXT("Prototype_GameObject_Test_Body_Player"),
    //    CTest_Body_Player::Create(m_pDevice, m_pContext))))
    //    return E_FAIL;

    /* For. Prototype_GameObject_TestTerrain */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CAMERA_TOOL, TEXT("Prototype_GameObject_Test_Terrain"),
        CTest_Terrain::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_Camera_Free */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CAMERA_TOOL, TEXT("Prototype_GameObject_Camera_Free"),
        CCamera_Free::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_Camera_Target */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CAMERA_TOOL, TEXT("Prototype_GameObject_Camera_Target"),
        CCamera_Target::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_Camera_CutScene */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CAMERA_TOOL, TEXT("Prototype_GameObject_Camera_CutScene"),
        CCamera_CutScene::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_Book */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CAMERA_TOOL, TEXT("Prototype_GameObject_Book"),
        CBook::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    Map_Object_Create(LEVEL_CAMERA_TOOL, LEVEL_CAMERA_TOOL, L"Chapter_02_Play_Desk.mchc");

    lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
    m_isFinished = true;

    return S_OK;
}

HRESULT CLoader::Loading_Level_Trigger_Tool()
{
    lstrcpy(m_szLoadingText, TEXT("컴포넌트를 로딩중입니다."));

    lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

    lstrcpy(m_szLoadingText, TEXT("사운드를 로딩중입니다."));

    lstrcpy(m_szLoadingText, TEXT("쉐이더를 로딩중입니다."));

    lstrcpy(m_szLoadingText, TEXT("모델(을)를 로딩중입니다."));

    XMMATRIX matPretransform = XMMatrixScaling(1 / 150.0f, 1 / 150.0f, 1 / 150.0f);

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TRIGGER_TOOL, TEXT("SM_desk_split_topboard_02"),
        C3DModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/Models/NonAnim/SM_desk_split_topboard_02/SM_desk_split_topboard_02.model", matPretransform))))
        return E_FAIL;

    if (FAILED(Load_Models_FromJson(LEVEL_TRIGGER_TOOL, MAP_3D_DEFAULT_PATH, L"Chapter_04_Play_Desk.json", matPretransform)))
        return E_FAIL;

    if (FAILED(Load_Dirctory_Models_Recursive(LEVEL_TRIGGER_TOOL,
        TEXT("../../Client/Bin/Resources/Models/3DObject/"), matPretransform)))
        return E_FAIL;

    matPretransform *= XMMatrixRotationAxis(_vector{ 0,1,0,0 }, XMConvertToRadians(180));

    if (FAILED(Load_Dirctory_Models_Recursive(LEVEL_TRIGGER_TOOL,
        TEXT("../../Client/Bin/Resources/Models/3DAnim/"), matPretransform)))
        return E_FAIL;

    if (FAILED(Load_Dirctory_Models_Recursive(LEVEL_TRIGGER_TOOL,
        TEXT("../../Client/Bin/Resources/Models/3DObject/Static/3DPlayerPart/"), matPretransform)))
        return E_FAIL;
    //if (FAILED(Load_Dirctory_2DModels_Recursive(LEVEL_TRIGGER_TOOL,
    //    TEXT("../../Client/Bin/Resources/Models/2DAnim/"))))
    //    return E_FAIL;


   // matPretransform = XMMatrixScaling(1 / 150.0f, 1 / 150.0f, 1 / 150.0f);

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TRIGGER_TOOL, TEXT("Prototype_Model_Book"),
        C3DModel::Create(m_pDevice, m_pContext,
            ("../Bin/Resources/Models/book/book.model"
                ), matPretransform))))
        return E_FAIL;


    lstrcpy(m_szLoadingText, TEXT("객체원형(을)를 로딩중입니다."));

    /* For. Prototype_GameObject_Camera_Free */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TRIGGER_TOOL, TEXT("Prototype_GameObject_Camera_Free"),
        CCamera_Free::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_MapObject */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TRIGGER_TOOL, TEXT("Prototype_GameObject_MapObject"),
        CModelObject::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_TestTerrain */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TRIGGER_TOOL, TEXT("Prototype_GameObject_Test_Terrain"),
        CTest_Terrain::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    //if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TRIGGER_TOOL, TEXT("alphabet_blocks_d_mesh"),
    //    C3DModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/Models/NonAnim/alphabet_blocks_d_mesh/alphabet_blocks_d_mesh.model", matPretransform))))
    //    return E_FAIL;
    //if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TRIGGER_TOOL, TEXT("alphabet_blocks_a_mesh"),
    //    C3DModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/Models/NonAnim/alphabet_blocks_a_mesh/alphabet_blocks_a_mesh.model", matPretransform))))
    //    return E_FAIL;


    /* For. Prototype_GameObject_MapObject */
    //if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TRIGGER_TOOL, TEXT("Prototype_GameObject_Book"),
    //    CBook::Create(m_pDevice, m_pContext))))
    //    return E_FAIL;

    Map_Object_Create(LEVEL_TRIGGER_TOOL, LEVEL_TRIGGER_TOOL, L"Chapter_04_Play_Desk.mchc");

    //Map_Object_Create(LEVEL_STATIC, LEVEL_TRIGGER_TOOL, L"Room_Enviroment.mchc");

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
        {
            wstring str = TEXT("Failed to Create 3DModel");
            str += filename;
            MessageBoxW(NULL, str.c_str(), TEXT("에러"), MB_OK);
            return E_FAIL;
        }

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
            C2DModel::Create(m_pDevice, m_pContext, str.c_str(), _iLevId))))
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

HRESULT CLoader::Load_Dirctory_Models_Recursive(_uint _iLevId, const _tchar* _szDirPath, _fmatrix _PreTransformMatrix)
{
    std::filesystem::path path;
    path = _szDirPath;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
        if (entry.path().extension() == ".model") {
            //cout << entry.path().string() << endl;

            if (FAILED(m_pGameInstance->Add_Prototype(_iLevId, entry.path().filename().replace_extension(),
                C3DModel::Create(m_pDevice, m_pContext, entry.path().string().c_str(), _PreTransformMatrix))))
            {
                string str = "Failed to Create 3DModel";
                str += entry.path().filename().replace_extension().string();
                MessageBoxA(NULL, str.c_str(), "에러", MB_OK);
                return E_FAIL;
            }
              
        }
    }
    return S_OK;
}

HRESULT CLoader::Load_Dirctory_2DModels_Recursive(_uint _iLevId, const _tchar* _szDirPath)
{
    std::filesystem::path path;
    path = _szDirPath;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
        if (entry.path().extension() == ".model2d") {
            //cout << entry.path().string() << endl;

            if (FAILED(m_pGameInstance->Add_Prototype(_iLevId, entry.path().filename().replace_extension(),
                C2DModel::Create(m_pDevice, m_pContext, entry.path().string().c_str(), _iLevId))))
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

HRESULT CLoader::Load_Models_FromJson(LEVEL_ID _iLevId, const _tchar* _szJsonFilePath, _fmatrix _PreTransformMatrix)
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
            if (FAILED(m_pGameInstance->Add_Prototype(_iLevId, StringToWstring(strFileName),
                C3DModel::Create(m_pDevice, m_pContext, entry.path().string().c_str(), _PreTransformMatrix))))
                return E_FAIL;
            iLoadedCount++;
            if (iLoadedCount >= iLoadCount)
            {
                break;
            }
        }


    }
    return S_OK;
}

HRESULT CLoader::Map_Object_Create(LEVEL_ID _eProtoLevelId, LEVEL_ID _eObjectLevelId, _wstring _strFileName)
{
    wstring strFileName = _strFileName;

    _wstring strFullFilePath = MAP_3D_DEFAULT_PATH + strFileName;

    HANDLE	hFile = CreateFile(strFullFilePath.c_str(),
        GENERIC_READ,
        NULL,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    if (INVALID_HANDLE_VALUE == hFile)
    {
        return E_FAIL;
    }
    _uint iCount = 0;

    DWORD	dwByte(0);
    _uint iLayerCount = 0;
    _int isTempReturn = 0;
    isTempReturn = ReadFile(hFile, &iLayerCount, sizeof(_uint), &dwByte, nullptr);

    for (_uint i = 0; i < iLayerCount; i++)
    {
        _uint		iObjectCnt = 0;
        _char		szLayerTag[MAX_PATH];
        wstring		strLayerTag;

        isTempReturn = ReadFile(hFile, &szLayerTag, (DWORD)(sizeof(_char) * MAX_PATH), &dwByte, nullptr);
        isTempReturn = ReadFile(hFile, &iObjectCnt, sizeof(_uint), &dwByte, nullptr);

        strLayerTag = m_pGameInstance->StringToWString(szLayerTag);
        for (size_t i = 0; i < iObjectCnt; i++)
        {
            cout << i << endl;
            _char		szSaveMeshName[MAX_PATH];
            _float4x4	vWorld = {};


            isTempReturn = ReadFile(hFile, &szSaveMeshName, (DWORD)(sizeof(_char) * MAX_PATH), &dwByte, nullptr);
            isTempReturn = ReadFile(hFile, &vWorld, sizeof(_float4x4), &dwByte, nullptr);


            CModelObject::MODELOBJECT_DESC NormalDesc = {};
            NormalDesc.strModelPrototypeTag_3D = m_pGameInstance->StringToWString(szSaveMeshName).c_str();
            NormalDesc.strShaderPrototypeTag_3D = L"Prototype_Component_Shader_VtxMesh";
            NormalDesc.isCoordChangeEnable = false;
            NormalDesc.iModelPrototypeLevelID_3D = _eProtoLevelId;
            NormalDesc.eStartCoord = COORDINATE_3D;
            CGameObject* pGameObject = nullptr;
            m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_MapObject"),
                _eObjectLevelId,
                strLayerTag,
                &pGameObject,
                (void*)&NormalDesc);

            if (pGameObject)
            {
                _uint issksp = 0;
                _char		szSaveskspName[MAX_PATH];


                isTempReturn = ReadFile(hFile, &issksp, sizeof(_uint), &dwByte, nullptr);

                if (0 < issksp)
                {
                    isTempReturn = ReadFile(hFile, &szSaveskspName, (DWORD)(sizeof(_char) * MAX_PATH), &dwByte, nullptr);
                }


                _uint iMaterialCount = 0;
                _uint iOverrideCount = 0;

                _uint iMaterialIndex;
                _uint eColorMode;
                _float4 fDiffuseColor;


                ReadFile(hFile, &iMaterialCount, sizeof(_uint), &dwByte, nullptr);
                if (0 < iMaterialCount)
                {
                    for (_uint i = 0; i < iMaterialCount; i++)
                    {
                        ReadFile(hFile, &iMaterialIndex, sizeof(_uint), &dwByte, nullptr);
                        ReadFile(hFile, &eColorMode, sizeof(C3DModel::COLOR_SHADER_MODE), &dwByte, nullptr);
                        ReadFile(hFile, &fDiffuseColor, sizeof(_float4), &dwByte, nullptr);
                    }
                }

                isTempReturn = ReadFile(hFile, &iOverrideCount, sizeof(_uint), &dwByte, nullptr);
                if (0 < iOverrideCount)
                {
                    CModelObject* pModelObject = static_cast<CModelObject*>(pGameObject);
                    for (_uint i = 0; i < iOverrideCount; i++)
                    {
                        _uint iMaterialIndex, iTexTypeIndex, iTexIndex;
                        isTempReturn = ReadFile(hFile, &iMaterialIndex, sizeof(_uint), &dwByte, nullptr);
                        isTempReturn = ReadFile(hFile, &iTexTypeIndex, sizeof(_uint), &dwByte, nullptr);
                        isTempReturn = ReadFile(hFile, &iTexIndex, sizeof(_uint), &dwByte, nullptr);

                        pModelObject->Change_TextureIdx(iTexIndex, iTexTypeIndex, iMaterialIndex);
                    }
                }
                pGameObject->Set_WorldMatrix(vWorld);
            }
        }
    }
    CloseHandle(hFile);
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
