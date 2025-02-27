#include "stdafx.h"
#include "Loader.h"

#include "GameInstance.h"
#include "CriticalSectionGuard.h"
#include "Sound_Manager.h"

/* For. Camera*/
#include "Camera_Free.h"
#include "Camera_Target.h"
#include "Camera_CutScene.h"
#include "Camera_2D.h"
#include "Ray.h"
#include "Cube.h"

/* For. etc Bulb, PlayerItem*/
#include "Blocker.h"
#include "CubeMap.h"
#include "MainTable.h"
#include "FallingRock.h"
#include "Spawner.h"
#include "CollapseBlock.h"
#include "Word_Container.h"
#include "JumpPad.h"


// Trigger
#include "TriggerObject.h"
#include "PlayerItem.h"
#include "Bulb.h"



/* For. UI*/
#include "Pick_Bulb.h"
#include "SettingPanelBG.h"
#include "StopStamp_UI.h"
#include "BombStamp_UI.h"
#include "ArrowForStamp.h"
#include "ESC_HeartPoint.h"
#include "UI_Interaction_Book.h"
#include "ShopPanel_BG.h"
#include "Logo_BG.h"
#include "ShopItemBG.h"
#include "Interaction_Heart.h"
#include "ESC_Goblin.h"
#include "Dialogue.h"
#include "Portrait.h"
#include "Logo_Props.h"
#include "Logo.h"
#include "ShopPanel_YesNo.h"
#include "FloorWord.h"
#include "PrintFloorWord.h"
#include "Npc_Logo.h"
#include "UI_JotMain.h"
#include "Narration.h"
#include "Narration_Anim.h"
/* For. UI*/

/* For. NPC*/
#include "Npc_Body.h"
#include "NPC_Store.h"
#include "NPC_Social.h"
#include "Npc_OnlySocial.h"
#include "Npc_Companion.h"
#include "NPC_Violet.h"
#include "NPC_Thrash.h"
#include "Npc_Rabbit.h"



#include "ModelObject.h"
#include "CarriableObject.h"
#include "DraggableObject.h"
#include "Player.h"
#include "PlayerBody.h"
#include "PlayerSword.h"
#include "StopStamp.h"
#include "BombStamp.h"
#include "PlayerBomb.h"
#include "Detonator.h"
#include "PalmMarker.h"
#include "PalmDecal.h"
#include "TestTerrain.h"
#include "RabbitLunch.h"
#include "Bomb.h"


#include "2DModel.h"
#include "3DModel.h"
#include "Controller_Model.h"
#include "GameEventExecuter.h"
#include "FSM.h"
#include "set"
#include "StateMachine.h"
#include "2DMapDefaultObject.h"
#include "2DMapActionObject.h"
#include "2DMapWordObject.h"
#include "3DMapDefaultObject.h"
#include "3DMapSkspObject.h"
#include "MapObjectFactory.h"
#include "DetectionField.h"
#include "Sneak_DetectionField.h"
#include "LightningBolt.h"
#include "LunchBox.h"

/* For. Monster */
#include "Beetle.h"
#include "BarfBug.h"
#include "Projectile_BarfBug.h"
#include "JumpBug.h"
#include "Goblin.h"
#include "Rat.h"
#include "Zippy.h"
#include "BirdMonster.h"
#include "Projectile_BirdMonster.h"
#include "Spear_Soldier.h"
#include "Soldier_Spear.h"
#include "Soldier_Shield.h"
#include "CrossBow_Soldier.h"
#include "Soldier_CrossBow.h"
#include "CrossBow_Arrow.h"
#include "Bomb_Soldier.h"
#include "Popuff.h"
#include "Monster_Body.h"
#include "Goblin_SideScroller.h"

/* For. Boss */
#include "ButterGrump.h"
#include "Boss_EnergyBall.h"
#include "Boss_HomingBall.h"
#include "Boss_YellowBall.h"
#include "Boss_PurpleBall.h"
#include "FSM_Boss.h"


// Sample
#include "SampleBook.h"

#include "RayShape.h"
#include "Dice.h"
#include "Domino.h"
#include "Portal.h"
#include "Word.h"
#include "Magic_Hand.h"
#include "Magic_Hand_Body.h"
#include "Effect2D.h"
#include "Effect_Trail.h"
#include "Effect_Beam.h"



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
    case Client::LEVEL_CHAPTER_2:
        hr = Loading_Level_Chapter_2();
        break;
     case Client::LEVEL_CHAPTER_4:
        hr = Loading_Level_Chapter_4();
        break;
    case Client::LEVEL_CHAPTER_TEST:
        hr = Loading_Level_Chapter_TEST();
        break;

    case Client::LEVEL_CAMERA_TOOL:
        hr = Loading_Level_Camera_Tool();
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
    lstrcpy(m_szLoadingText, TEXT("컴포넌트를 로딩중입니다."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Gravity"),
        CGravity::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_PlayerAnimEvent"),
        CAnimEventGenerator::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/3DAnim/Latch_SkelMesh_NewRig/aaa.animevt"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Player2DAnimEvent"),
        CAnimEventGenerator::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/2DAnim/Static/Player/player2danimevts.animevt"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_PlayeEffectEvent"),
        CAnimEventGenerator::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/3DAnim/Latch_SkelMesh_NewRig/Player_Effect.animevt"))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("2D 콜라이더를 로딩중입니다."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
        CCollider_Circle::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
        CCollider_AABB::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Fan"),
        CCollider_Fan::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

    /* For. Prototype_Component_Texture_BRDF_Shilick */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BRDF_Shilick"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/CubeMap/HDRI/BRDF_Shilick.dds"), 1))))
        return E_FAIL;
    /* For. Prototype_Component_Texture_TestEnv */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_TestEnv"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/CubeMap/HDRI/TestEnv/TestEnv_%d.dds"), 3, true))))
        return E_FAIL;
    /* For. Prototype_Component_Texture_Chapter4Env */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Chapter4Env"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/CubeMap/HDRI/TestEnv/Chapter4Env_%d.dds"), 3, true))))
        return E_FAIL;

    
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_OptionBG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Static/T_Panel-Bottom1.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_OptionBG_Outline"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Logo/BG/T_bg_border.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_OptionBG_OutOutline"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Logo/BG/T_bg_border_outline.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ESCBack"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Static/KeyIcon/Keyboard/EscButton.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ESCBackArrow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Static/KeyIcon/Arrow/back_arrow_icon.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ESCEnter"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Static/KeyIcon/Keyboard/keyboard_Enter.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ESCBulb"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/shop_ui_icon_bulb.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_IconBG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Logo/BG/T_BG_TEST.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ESCGoblin"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Logo/BG/T_PauseGoblin.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ESCBookMark"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Logo/BG/T_bookmark.dds"), 1))))
		return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_OptionBG_Borderline"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Logo/BG/New_T_bg_border.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Trail"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effects/T_FX_CMN_Trail_03.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Grad04"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effects/T_FX_CMN_Grad_04.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Glow01"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effects/T_FX_CMN_Glow_01.dds"), 1))))
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
    /* For. Prototype_Component_Shader_VtxPointInstance */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPointInstance"),
        CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance.hlsl"), VTXPOINTPARTICLE::Elements, VTXPOINTPARTICLE::iNumElements))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMeshInstance"),
        CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMeshInstance.hlsl"), VTXMESHID::Elements, VTXMESHID::iNumElements))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMeshEffect"),
        CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Effect.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPoint"),
        CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPoint.hlsl"), VTXPOINT::Elements, VTXPOINT::iNumElements))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Trail"),
        CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Trail.hlsl"), VTXTRAIL::Elements, VTXTRAIL::iNumElements))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Beam"),
        CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxBeam.hlsl"), VTXBEAM::Elements, VTXBEAM::iNumElements))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Compute_Shader_MeshInstance"),
        CCompute_Shader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_CS_Mesh.hlsl")))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Compute_Shader_SpriteInstance"),
        CCompute_Shader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_CS_Sprite.hlsl")))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("모델(을)를 로딩중입니다."));
    if (FAILED(Load_Dirctory_2DModels_Recursive(LEVEL_STATIC,
        TEXT("../Bin/Resources/Models/2DAnim/Static/"))))
        return E_FAIL;
    lstrcpy(m_szLoadingText, TEXT("모델(을)를 로딩중입니다."));
    if (FAILED(Load_Dirctory_2DModels_Recursive(LEVEL_STATIC,
        TEXT("../Bin/Resources/Models/2DNonAnim/Static/"))))
        return E_FAIL;
    XMMATRIX matPretransform = XMMatrixScaling(1 / 150.0f, 1 / 150.0f, 1 / 150.0f);
    //matPretransform *= XMMatrixRotationAxis(_vector{ 0,1,0,0 }, XMConvertToRadians(180));

    if (FAILED(Load_Dirctory_Models_Recursive(LEVEL_STATIC,
        TEXT("../Bin/Resources/Models/3DPlayerPart/"), matPretransform)))
        return E_FAIL;
    //matPretransform *= XMMatrixRotationAxis(_vector{ 0,1,0,0 }, XMConvertToRadians(180));


    //if (FAILED(Load_Models_FromJson(LEVEL_STATIC, MAP_3D_DEFAULT_PATH, L"Room_Enviroment.json", matPretransform)))
    //    return E_FAIL;
    if (FAILED(Load_Models_FromJson(LEVEL_STATIC, MAP_3D_DEFAULT_PATH, L"Room_Enviroment_Small.json", matPretransform)))
        return E_FAIL;


    /* For. Prototype_Component_VIBuffer_Rect */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_Component_VIBuffer_Cube */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"),
        CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_Component_Trail*/
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Trail32"),
        CVIBuffer_Trail::Create(m_pDevice, m_pContext, 32))))
        return E_FAIL;

    /* For. Prototype_Component_Beam*/
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Beam16"),
        CVIBuffer_Beam::Create(m_pDevice, m_pContext, 16))))
        return E_FAIL;
    
    /* Bulb*/
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("LightbulbPickup_01"),
        C3DModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/Models/NonAnim/LightbulbPickup_01/LightbulbPickup_01.model", matPretransform))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Model2D_Bulb"),
        C2DModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/2DNonAnim/bulb/pickup_bulb_01.dds", true))))
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

    /* For. Prototype_GameObject_Effect2D */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Effect2D"),
        CEffect2D::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    

     /* For. Prototype_GameObject_Spawner */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Spawner"),
        CSpawner::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_CubeMap */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_CubeMap"),
        CCubeMap::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    
    /* For. Prototype_GameObject_CubeMap */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_GameEventExecuter"),
        CGameEventExecuter::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_MainTable */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_MainTable"),
        CMainTable::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    // ============ Camera
    /* For. Prototype_GameObject_Camera_Free */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_Free"),
        CCamera_Free::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_Camera_Target */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_Target"),
        CCamera_Target::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_Camera_CutScene */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_CutScene"),
        CCamera_CutScene::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_Camera_2D */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_2D"),
        CCamera_2D::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    // ============ Triger
    /* For. Prototype_GameObject_TriggerObject */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_TriggerObject"),
        CTriggerObject::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_RayShape"),
        CRayShape::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Portal"),
        CPortal::Create(m_pDevice, m_pContext))))
        return E_FAIL;
     
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Word_Container"),
        CWord_Container::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    // ============ etc Bulb, PlayerItem
     /* For. Prototype_GameObject_PlayerItem */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_PlayerItem"),
        CPlayerItem::Create(m_pDevice, m_pContext))))
        return E_FAIL;
 if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Bulb"),
        CBulb::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_Bulb */
   
    
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_UIObejct_ParentSettingPanel"), CSettingPanel::Create(m_pDevice, m_pContext))))
        return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_UIObejct_SettingPanel"), CSettingPanelBG::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_UIObejct_ESC_Goblin"), CESC_Goblin::Create(m_pDevice, m_pContext))))
		return E_FAIL;


    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"),
        CModelObject::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_CarrieableObject"),
        CCarriableObject::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_DraggableObject"),
        CDraggableObject::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_2DMapObject"),
        C2DMapDefaultObject::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_2DMap_ActionObject"),
        C2DMapActionObject::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_2DMap_WordObject"),
        C2DMapWordObject::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_3DMapObject"),
        C3DMapDefaultObject::Create(m_pDevice, m_pContext))))
        return E_FAIL;
            
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Word"),
        CWord::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Bomb"),
        CBomb::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_3DMap_SkspObject"),
        C3DMapSkspObject::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_StateMachine"),
        CStateMachine::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_TestPlayer"),
        CPlayer::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_PlayerSword"),
        CPlayerSword::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_StopStamp"),
        CStopStamp::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_BombStamp"),
        CBombStamp::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_PlayerBomb"),
        CPlayerBomb::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Detonator"),
        CDetonator::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_PalmMarker"),
        CPalmMarker::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_PalmDecal"),
        CPalmDecal::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_PlayerBody"),
        CPlayerBody::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    /* Monster */

    /* For. Prototype_GameObject_Monster_Body */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Monster_Body"),
        CMonster_Body::Create(m_pDevice, m_pContext))))
        return E_FAIL;

	/* For. Prototype_GameObject_NPC_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_NPC_Body"),
		CNpc_Body::Create(m_pDevice, m_pContext))))
		return E_FAIL;

     /* For. Prototype_GameObject_Beetle */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Beetle"),
        CBeetle::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_BarfBug */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_BarfBug"),
        CBarfBug::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_Projectile_BarfBug */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Projectile_BarfBug"),
        CProjectile_BarfBug::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_JumpBug */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_JumpBug"),
        CJumpBug::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_Goblin */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Goblin"),
        CGoblin::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_Rat */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Rat"),
        CRat::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_BirdMonster */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_BirdMonster"),
        CBirdMonster::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_Projectile_BirdMonster */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Projectile_BirdMonster"),
        CProjectile_BirdMonster::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_Spear_Soldier */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Spear_Soldier"),
        CSpear_Soldier::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_CrossBow_Soldier */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_CrossBow_Soldier"),
        CCrossBow_Soldier::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_Bomb_Soldier */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Bomb_Soldier"),
        CBomb_Soldier::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_Popuff */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Popuff"),
        CPopuff::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_Soldier_Spear */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Soldier_Spear"),
        CSoldier_Spear::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_Soldier_Shield */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Soldier_Shield"),
        CSoldier_Shield::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_Soldier_CrossBow */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Soldier_CrossBow"),
        CSoldier_CrossBow::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_CrossBow_Arrow */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_CrossBow_Arrow"),
        CCrossBow_Arrow::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_LightningBolt */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_LightningBolt"),
        CLightningBolt::Create(m_pDevice, m_pContext))))
        return E_FAIL;


    /* For. Prototype_GameObject_Blocker2D */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Blocker2D"),
        CBlocker::Create(m_pDevice, m_pContext, COORDINATE_2D))))
        return E_FAIL;

    /* For. Prototype_GameObject_SwordTrail */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_EffectTrail"),
        CEffect_Trail::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_EffectBeam"),
        CEffect_Beam::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    

    lstrcpy(m_szLoadingText, TEXT("이펙트(을)를 로딩중입니다."));

    if (FAILED(Load_Directory_Effects(LEVEL_STATIC, TEXT("../Bin/DataFiles/FX/Common/"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Load_Json_InLevel(TEXT("../Bin/DataFiles/FX/FX_StaticInfo.json"), TEXT("FX_Static"), LEVEL_STATIC)))
        return E_FAIL;

    if (FAILED(Load_Dirctory_2DModels_Recursive(LEVEL_STATIC,
        TEXT("../Bin/Resources/Models/2D_FX"))))
        return E_FAIL;

    if (FAILED(Loading_SFX_PathFind(TEXT("../Bin/Sounds/SFX/Common"))))
        return E_FAIL;

    if (FAILED(Loading_BGM_PathFind(TEXT("../Bin/Sounds/BGM"))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
    m_isFinished = true;

    return S_OK;
}

HRESULT CLoader::Loading_Level_Logo()
{
    lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo_BG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Logo/BG/_BACK_T_TitleBG.dds"), 1))))
		return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo_WhiteFlower0"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("..//Bin/Resources/Textures/Object/2DMap/mountain_white_flower_01.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo_WhiteFlower1"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("..//Bin/Resources/Textures/Object/2DMap/mountain_white_flower_02.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo_Tree0"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Object/2DMap/tree_green_01.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo_Tree1_ink0"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Object/2DMap/tree_green_ink_01.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo_Tree1_ink1"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Object/2DMap/tree_green_ink_02.dds"), 1))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("사운드를 로딩중입니다."));

    lstrcpy(m_szLoadingText, TEXT("쉐이더를 로딩중입니다."));

    lstrcpy(m_szLoadingText, TEXT("모델(을)를 로딩중입니다."));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_NPC_Pip_2DAnimation"),
		C2DModel::Create(m_pDevice, m_pContext, ("../Bin/Resources/Models/2DAnim/Logo/NPC/Pip/Pip.model2D")))))
		return E_FAIL;


    lstrcpy(m_szLoadingText, TEXT("객체원형(을)를 로딩중입니다."));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_UIObejct_LogoBG"), CLogo_BG::Create(m_pDevice, m_pContext))))
		return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_UIObejct_LogoProp"), CLogo_Props::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_UIObejct_Logo"), CLogo::Create(m_pDevice, m_pContext))))
        return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_UIObejct_NPC_Logo"), CNPC_Logo::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_UIObejct_JOT"), CUI_JotMain::Create(m_pDevice, m_pContext))))
		return E_FAIL;


    lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
    m_isFinished = true;

    return S_OK;
}


HRESULT CLoader::Loading_Level_Chapter_2()
{
    lstrcpy(m_szLoadingText, TEXT("컴포넌트를 로딩중입니다."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Component_FSM"),
        CFSM::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Component_FSM_Boss"),
        CFSM_Boss::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Component_DetectionField"),
        CDetectionField::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Component_Sneak_DetectionField"),
        CSneak_DetectionField::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Component_BarfBugAttackAnimEvent"),
        CAnimEventGenerator::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/3DAnim/barfBug_Rig/BarfBug_Attack.animevt"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Component_BarfBug2DAttackAnimEvent"),
        CAnimEventGenerator::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/2DAnim/Chapter2/Monster/BarferBug/BarferBug2d_Attack.animevt"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Component_ZippyAttackAnimEvent"),
        CAnimEventGenerator::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/2DAnim/Chapter2/Monster/Zippy/Zippy_Attack.animevt"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Component_LightningBoltAnimEvent"),
        CAnimEventGenerator::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/2DAnim/Chapter2/MapObject/LightningBolt/LightningBolt.animevt"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Component_BookPageActionEvent"),
        CAnimEventGenerator::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/3DMapObject/book/book_Animation_Event.animevt"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Component_LunchBoxAnimEvent"),
        CAnimEventGenerator::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/3DAnim/Lunch_Box_Rig_GT/LunchBoxAnimEvt.animevt"))))
        return E_FAIL;
    #ifdef _DEBUG
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Component_DebugDraw_For_Client"),
        CDebugDraw_For_Client::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    #endif // _DEBUG

    lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Component_Texture_PickBulb"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Bulb_Pickup/pickup_counter_bulb_01.dds"), 1))))
		return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Component_Texture_BombStamp"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Object/Stamp/Resize_BombStamp.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Component_Texture_StopStamp"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Object/Stamp/Resize_StopStamp.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Component_Texture_ArrowForStamp"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Object/Stamp/Arrow.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Component_Texture_HeartPoint"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/HPBar/HUD_Heart_%d.dds"), 13))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Component_Texture_KEYQ"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Static/KeyIcon/Keyboard/keyboard_Q.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Component_Texture_ShopBG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/T_Panel_BG.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Component_Texture_ShopDialogue"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/shop_ui_panel_text.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Component_Texture_ShopChooseBG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/T_panel_YesNo.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Component_Texture_ShopBulb"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/shop_ui_panel_bulb.dds"), 1))))
		return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Component_Texture_DialogueBG"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Dialogue/Dialogue_BG/Dialogue/dialogue_%d.dds"), 27))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Component_Texture_DialoguePortrait"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Dialogue/Dialogue_BG/Character_Icon/dialogue_icon_%d.dds"), 17))))
        return E_FAIL;



    ///// 상점 관련
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Component_Texture_JumpAttack0"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_jump.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Component_Texture_SpinAttack0"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_spin.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Component_Texture_SpinAttack1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_spin_upgrade_1.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Component_Texture_SpinAttack2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_spin_upgrade_2.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Component_Texture_SpinAttack3"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_spin_upgrade_3.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Component_Texture_AttackPlus1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_sword_upgrade_1.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Component_Texture_AttackPlus2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_sword_upgrade_2.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Component_Texture_AttackPlus3"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_sword_upgrade_3.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Component_Texture_ThrowAttack0"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_throw.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Component_Texture_ThrowAttack1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_throw_upgrade_1.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Component_Texture_ThrowAttack2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_throw_upgrade_2.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Component_Texture_ThrowAttack3"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_throw_upgrade_3.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Component_Texture_ScrollItem"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_scroll.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Component_Texture_ItemSelectedBG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/shop_ui_panel_item_selected._testtga.dds"), 1))))
		return E_FAIL;


    ///// 상점 관련
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Component_Texture_BACK"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Static/KeyIcon/Keyboard/keyboard_backspace.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Component_Texture_Enter"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Static/KeyIcon/Keyboard/keyboard_Enter.dds"), 1))))
		return E_FAIL;




    lstrcpy(m_szLoadingText, TEXT("사운드를 로딩중입니다."));

    m_pGameInstance->Load_SFX(TEXT("P0102_01_01"), TEXT("../Bin/Resources/Audio/Narration/Chapter2/C0102/P0102_01_01.wav"));

    m_pGameInstance->Load_SFX(TEXT("P0102_02_01"), TEXT("../Bin/Resources/Audio/Narration/Chapter2/C0102/P0102_02_01.wav"));
    m_pGameInstance->Load_SFX(TEXT("P0102_02_02"), TEXT("../Bin/Resources/Audio/Narration/Chapter2/C0102/P0102_02_02.wav"));

    m_pGameInstance->Load_SFX(TEXT("P0304_01_01"), TEXT("../Bin/Resources/Audio/Narration/Chapter2/C0102/P0304_01_01.wav"));
    m_pGameInstance->Load_SFX(TEXT("P0304_01_02"), TEXT("../Bin/Resources/Audio/Narration/Chapter2/C0102/P0304_01_02.wav"));

    m_pGameInstance->Load_SFX(TEXT("P0304_02_01"), TEXT("../Bin/Resources/Audio/Narration/Chapter2/C0102/P0304_02_01.wav"));

    m_pGameInstance->Load_SFX(TEXT("P1112_01_01"), TEXT("../Bin/Resources/Audio/Narration/Chapter2/C1112/P1112_01_01.wav"));
    m_pGameInstance->Load_SFX(TEXT("P1112_01_02"), TEXT("../Bin/Resources/Audio/Narration/Chapter2/C1112/P1112_01_02.wav"));

    m_pGameInstance->Load_SFX(TEXT("P1920_01_01"), TEXT("../Bin/Resources/Audio/Narration/Chapter2/C1920/P1920_01_01.wav"));
    m_pGameInstance->Load_SFX(TEXT("P1920_01_02"), TEXT("../Bin/Resources/Audio/Narration/Chapter2/C1920/P1920_01_02.wav"));
    m_pGameInstance->Load_SFX(TEXT("P1920_02_01"), TEXT("../Bin/Resources/Audio/Narration/Chapter2/C1920/P1920_02_01.wav"));


    lstrcpy(m_szLoadingText, TEXT("쉐이더를 로딩중입니다."));

    lstrcpy(m_szLoadingText, TEXT("모델(을)를 로딩중입니다."));

    XMMATRIX matPretransform = XMMatrixScaling(1 / 150.0f, 1 / 150.0f, 1 / 150.0f);
    if (FAILED(Load_Dirctory_2DModels_Recursive(LEVEL_CHAPTER_2,
        TEXT("../Bin/Resources/Models/2DMapObject/Chapter2"))))
        return E_FAIL;
    if (FAILED(Load_Dirctory_2DModels_Recursive(LEVEL_CHAPTER_2,
        TEXT("../Bin/Resources/Models/2DMapObject/Static"))))
        return E_FAIL;

    /* 낱개 로딩 예시*/

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Model2D_FallingRock"),
        C2DModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/2DAnim/Chapter2/FallingRock/FallingRock.model2D", false))))
        return E_FAIL;

    matPretransform = XMMatrixScaling(1 / 160.0f, 1 / 160.0f, 1 / 160.0f);
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Model3D_FallingRock"),
        C3DModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NonAnim/Rock_03/Rock_03.model", matPretransform))))
        return E_FAIL;
    matPretransform = XMMatrixScaling(1 / 150.0f, 1 / 150.0f, 1 / 150.0f);
    //if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("player"),
    //    C2DModel::Create(m_pDevice, m_pContext, ("../Bin/Resources/Models/2DAnim/Chapter2/Player/player.model2D")))))
    //    return E_FAIL;
    //if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("player2dsword"),
    //    C2DModel::Create(m_pDevice, m_pContext, ("../Bin/Resources/Models/2DAnim/Chapter2/PlayerSword/player2dsword.model2D")))))
    //    return E_FAIL;
    // NPC 모델
    //if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("NPC_Store"),
    //    C2DModel::Create(m_pDevice, m_pContext, ("../Bin/Resources/Models/2DAnim/Chapter2/NPC/NPC_Shop/NPC_Store.model2D")))))
    //    return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("DJ_MoonBeard"),
	//	C2DModel::Create(m_pDevice, m_pContext, ("../Bin/Resources/Models/2DAnim/Chapter2/NPC/DJ_MoonBeard/DJ_MoonBeard.model2D")))))
	//	return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("0102_01_Narration_Model"),
	//	C2DModel::Create(m_pDevice, m_pContext, ("../Bin/Resources/Models/2DAnim/Chapter2/Narration/0102/0102_01_Narration_Model.model2D")))))
	//	return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("dice_pink_03"),
        C2DModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NonAnim/dice_01/dice_pink_03.dds", true))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("sketchspace_rabbit_carrot"),
        C2DModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NonAnim/Carrots_Carrot_01/sketchspace_rabbit_carrot.dds", true))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Grape_Green"),
        C2DModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NonAnim/Grapes_Grape_01/Grape_Green.dds", true))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Model2D_FallingRockShadow"),
        C2DModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/2DNonAnim/FallingRockShadow/FallingRockShadow.dds", true))))
        return E_FAIL;

    //if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("0102_02_Narration_Model_Jot"),
    //    C2DModel::Create(m_pDevice, m_pContext, ("../Bin/Resources/Models/2DAnim/Chapter2/Narration/0102/0102_02_Narration_Model_Jot.model2D")))))
    //    return E_FAIL;
    //if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("0102_02_Narration_Model_Humgrump"),
    //    C2DModel::Create(m_pDevice, m_pContext, ("../Bin/Resources/Models/2DAnim/Chapter2/Narration/0102/0102_02_Narration_Model_Humgrump.model2D")))))
    //    return E_FAIL;
    //if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("0102_02_Narration_Model_Villager"),
    //    C2DModel::Create(m_pDevice, m_pContext, ("../Bin/Resources/Models/2DAnim/Chapter2/Narration/0102/0102_02_Narration_Model_Villager.model2D")))))
    //    return E_FAIL;

   
    
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Component_Dice3D"),
        C3DModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NonAnim/dice_01/dice_01.model", matPretransform))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Model_Domino4"),
        C3DModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NonAnim/Domino_4/Domino_4.model", matPretransform))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Model_Domino3"),
        C3DModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NonAnim/Domino_3/Domino_3.model", matPretransform))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Model_Domino2"),
        C3DModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NonAnim/Domino_2/Domino_2.model", matPretransform))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Model_Domino1"),
        C3DModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NonAnim/Domino_1/Domino_1.model", matPretransform))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Model_Carrot_01"),
        C3DModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NonAnim/Carrots_Carrot_01/Carrots_Carrot_01.model", matPretransform))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Model_Carrot_02"),
        C3DModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NonAnim/Carrots_Carrot_02/Carrots_Carrot_02.model", matPretransform))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Model_Carrot_03"),
        C3DModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NonAnim/Carrots_Carrot_03/Carrots_Carrot_03.model", matPretransform))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Model_Grape_01"),
        C3DModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NonAnim/Grapes_Grape_01/Grapes_Grape_01.model", matPretransform))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Model_Grape_02"),
        C3DModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NonAnim/Grapes_Grape_02/Grapes_Grape_02.model", matPretransform))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Model_Grape_03"),
        C3DModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NonAnim/Grapes_Grape_03/Grapes_Grape_03.model", matPretransform))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Model_PlasticBlock"),
        C3DModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NonAnim/SM_Plastic_Block_04/SM_Plastic_Block_04.model", matPretransform))))
        return E_FAIL;
    ///* 중복 키값도 Prototype_Manager에서 쳐내개 변경했음. 이제 중복 키값 로드해도 멈추지않음. */
    //if (FAILED(Load_Models_FromJson(LEVEL_CHAPTER_2, MAP_3D_DEFAULT_PATH, L"Chapter_04_Default_Desk.json", matPretransform, true)))
    //    return E_FAIL;
    if (FAILED(Load_Models_FromJson(LEVEL_CHAPTER_2, MAP_3D_DEFAULT_PATH, L"Chapter_02_Play_Desk.json", matPretransform, true)))
        return E_FAIL;

    if (FAILED(Load_Dirctory_Models_Recursive(LEVEL_CHAPTER_2,
        TEXT("../Bin/Resources/Models/3DMapObject/"), matPretransform)))
        return E_FAIL;

    matPretransform *= XMMatrixRotationAxis(_vector{0,1,0,0},XMConvertToRadians(180));
    if (FAILED(Load_Dirctory_Models_Recursive(LEVEL_CHAPTER_2,
        TEXT("../Bin/Resources/Models/3DAnim/"), matPretransform)))
        return E_FAIL;
    
 


    if (FAILED(Load_Dirctory_2DModels_Recursive(LEVEL_CHAPTER_2,
        TEXT("../Bin/Resources/Models/2DAnim/Chapter2/"))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("객체원형(을)를 로딩중입니다."));

    /* For. Prototype_GameObject_SampleBook */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_GameObject_SampleBook"),
        CSampleBook::Create(m_pDevice, m_pContext))))
        return E_FAIL;


    ///////////////////////////////// UI /////////////////////////////////
    /* For. Prototype_UIObject_Pick_Bubble */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_UIObejct_PickBubble"),
        CPick_Bulb::Create(m_pDevice, m_pContext))))
        return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_GameObject_StopStamp"),
		CStopStamp_UI::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_GameObject_BombStamp"),
		CBombStamp_UI::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_GameObject_ArrowForStamp"),
		CArrowForStamp::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_GameObject_ESCHeartPoint"),
		ESC_HeartPoint::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_GameObject_Interaction_Book"),
		CUI_Interaction_Book::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_GameObject_ParentShopPannel"),
		CShopPanel::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_GameObject_ShopPannelBG"),
		CShopPanel_BG::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_GameObject_ShopPannelYesNo"),
		CShopPanel_YesNo::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_GameObject_ShopItem"),
		CShopItemBG::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_GameObject_Interaction_Heart"),
		CInteraction_Heart::Create(m_pDevice, m_pContext))))
		return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_GameObject_Dialogue"),
        CDialog::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_GameObject_Dialogue_Portrait"),
        CPortrait::Create(m_pDevice, m_pContext))))
        return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_GameObject_Narration"),
		CNarration::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_GameObject_Narration_Anim"),
		CNarration_Anim::Create(m_pDevice, m_pContext))))
		return E_FAIL;


    
    ///////////////////////////////// UI /////////////////////////////////
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_GameObject_StoreNPC"), 
        CNPC_Store::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_GameObject_NPC_OnlySocial"),
		CNPC_OnlySocial::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_GameObject_NPC_Social"),
		CNPC_Social::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_GameObject_NPC_Companion"),
		CNPC_Companion::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_GameObject_NPC_Companion_Violet"),
		CNPC_Violet::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_GameObject_NPC_Companion_Thrash"),
		CNPC_Thrash::Create(m_pDevice, m_pContext))))
		return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_GameObject_NPC_Rabbit"),
        CNpc_Rabbit::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    ///////////////////////////////// NPC /////////////////////////////////

    /* Monster */

    /* For. Prototype_GameObject_Goblin_SideScroller */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Goblin_SideScroller"),
        CGoblin_SideScroller::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_Zippy */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Zippy"),
        CZippy::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* Boss */

    /* For. Prototype_GameObject_ButterGrump */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_GameObject_ButterGrump"),
        CButterGrump::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_GameObject_Boss_HomingBall"),
        CBoss_HomingBall::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_GameObject_Boss_EnergyBall"),
        CBoss_EnergyBall::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_GameObject_Boss_YellowBall"),
        CBoss_YellowBall::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_GameObject_Boss_PurpleBall"),
        CBoss_PurpleBall::Create(m_pDevice, m_pContext))))
        return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_GameObject_FloorWord"),
		CFloorWord::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_GameObject_PrintFloorWord"),
		CPrintFloorWord::Create(m_pDevice, m_pContext))))
		return E_FAIL;

    /* 상호작용 오브젝트 */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_GameObject_Dice"),
        CDice::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_GameObject_Domino"),
        CDomino::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_GameObject_LunchBox"),
        CLunchBox::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_GameObject_RabbitLunch"),
        CRabbitLunch::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    /* Etc */

    /* For. Prototype_GameObject_FallingRock */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_GameObject_FallingRock"),
        CFallingRock::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_CollapseBlock */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_GameObject_CollapseBlock"),
        CCollapseBlock::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Magic_Hand, Magic_Hand_Body*/
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_GameObject_MagicHand"),
        CMagic_Hand::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_GameObject_MagicHandBody"),
        CMagic_Hand_Body::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    matPretransform = XMMatrixScaling(1 / 150.0f, 1 / 150.0f, 1 / 150.0f);

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Model_MagicHand"),
        C3DModel::Create(m_pDevice, m_pContext,
            ("../Bin/Resources/Models/FX/magic_hand_model/magic_hand_model.model"
                ), matPretransform))))
        return E_FAIL;

    //Map_Object_Create(LEVEL_STATIC, LEVEL_CHAPTER_2, L"Room_Enviroment.mchc");
    Map_Object_Create(LEVEL_STATIC, LEVEL_CHAPTER_2, L"Room_Enviroment_Small.mchc");

    lstrcpy(m_szLoadingText, TEXT("Level2 이펙트 로딩중입니다."));

    if (FAILED(Load_Directory_Effects(LEVEL_CHAPTER_2, TEXT("../Bin/DataFiles/FX/Level2/"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Load_Json_InLevel(TEXT("../Bin/DataFiles/FX/FX_Level2.json"), TEXT("FX_Level2"), LEVEL_CHAPTER_2)))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
    m_isFinished = true;

    return S_OK;
}

HRESULT CLoader::Loading_Level_Chapter_4()
{
    lstrcpy(m_szLoadingText, TEXT("컴포넌트를 로딩중입니다."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_Component_FSM"),
        CFSM::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_Component_FSM_Boss"),
        CFSM_Boss::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_Component_DetectionField"),
        CDetectionField::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_Component_Sneak_DetectionField"),
        CSneak_DetectionField::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_Component_BarfBugAttackAnimEvent"),
        CAnimEventGenerator::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/3DAnim/barfBug_Rig/BarfBug_Attack.animevt"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_Component_BarfBug2DAttackAnimEvent"),
        CAnimEventGenerator::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/2DAnim/Chapter2/Monster/BarferBug/BarferBug2d_Attack.animevt"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_2, TEXT("Prototype_Component_JumpBugJumpEvent"),
        CAnimEventGenerator::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/3DAnim/jumpBug_Rig/JumpBug_Jump.animevt"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_Component_BookPageActionEvent"),
        CAnimEventGenerator::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/3DMapObject/book/book_Animation_Event.animevt"))))
        return E_FAIL;

#ifdef _DEBUG
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_Component_DebugDraw_For_Client"),
        CDebugDraw_For_Client::Create(m_pDevice, m_pContext))))
        return E_FAIL;
#endif // _DEBUG





    lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_Component_Texture_PickBulb"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Bulb_Pickup/pickup_counter_bulb_01.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_Component_Texture_BombStamp"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Object/Stamp/Resize_BombStamp.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_Component_Texture_StopStamp"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Object/Stamp/Resize_StopStamp.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_Component_Texture_ArrowForStamp"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Object/Stamp/Arrow.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_Component_Texture_HeartPoint"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/HPBar/HUD_Heart_%d.dds"), 13))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_Component_Texture_KEYQ"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Static/KeyIcon/Keyboard/keyboard_Q.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_Component_Texture_ShopBG"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/T_Panel_BG.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_Component_Texture_ShopDialogue"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/shop_ui_panel_text.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_Component_Texture_ShopChooseBG"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/T_panel_YesNo.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_Component_Texture_ShopBulb"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/shop_ui_panel_bulb.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_Component_Texture_DialogueBG"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Dialogue/Dialogue_BG/Dialogue/dialogue_%d.dds"), 27))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_Component_Texture_DialoguePortrait"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Dialogue/Dialogue_BG/Character_Icon/dialogue_icon_%d.dds"), 17))))
        return E_FAIL;



    ///// 상점 관련
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_Component_Texture_JumpAttack0"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_jump.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_Component_Texture_SpinAttack0"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_spin.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_Component_Texture_SpinAttack1"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_spin_upgrade_1.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_Component_Texture_SpinAttack2"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_spin_upgrade_2.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_Component_Texture_SpinAttack3"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_spin_upgrade_3.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_Component_Texture_AttackPlus1"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_sword_upgrade_1.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_Component_Texture_AttackPlus2"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_sword_upgrade_2.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_Component_Texture_AttackPlus3"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_sword_upgrade_3.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_Component_Texture_ThrowAttack0"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_throw.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_Component_Texture_ThrowAttack1"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_throw_upgrade_1.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_Component_Texture_ThrowAttack2"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_throw_upgrade_2.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_Component_Texture_ThrowAttack3"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_throw_upgrade_3.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_Component_Texture_ScrollItem"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_scroll.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_Component_Texture_ItemSelectedBG"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/shop_ui_panel_item_selected._testtga.dds"), 1))))
        return E_FAIL;


    ///// 상점 관련




    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_Component_Texture_BACK"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Static/KeyIcon/Keyboard/keyboard_backspace.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_Component_Texture_Enter"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Static/KeyIcon/Keyboard/keyboard_Enter.dds"), 1))))
        return E_FAIL;




    lstrcpy(m_szLoadingText, TEXT("사운드를 로딩중입니다."));

    lstrcpy(m_szLoadingText, TEXT("쉐이더를 로딩중입니다."));

    lstrcpy(m_szLoadingText, TEXT("모델(을)를 로딩중입니다."));


    if (FAILED(Load_Dirctory_2DModels_Recursive(LEVEL_CHAPTER_4,
        TEXT("../Bin/Resources/Models/2DMapObject/Chapter4/"))))
        return E_FAIL;
    if (FAILED(Load_Dirctory_2DModels_Recursive(LEVEL_CHAPTER_4,
        TEXT("../Bin/Resources/Models/2DAnim/Chapter4/"))))
        return E_FAIL;
    if (FAILED(Load_Dirctory_2DModels_Recursive(LEVEL_CHAPTER_4,
        TEXT("../Bin/Resources/Models/2DMapObject/Static"))))
        return E_FAIL;

    /* 낱개 로딩 예시*/

    // NPC 모델
    //if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("NPC_Store"),
    //    C2DModel::Create(m_pDevice, m_pContext, ("../Bin/Resources/Models/2DAnim/Chapter4/NPC/NPC_Shop/NPC_Store.model2D")))))
    //    return E_FAIL;


    XMMATRIX matPretransform = XMMatrixScaling(1 / 150.0f, 1 / 150.0f, 1 / 150.0f);


    if (FAILED(Load_Models_FromJson(LEVEL_CHAPTER_4, MAP_3D_DEFAULT_PATH, L"Chapter_04_Play_Desk.json", matPretransform, true)))
        return E_FAIL;

    if (FAILED(Load_Dirctory_Models_Recursive(LEVEL_CHAPTER_4,
        TEXT("../Bin/Resources/Models/3DMapObject/"), matPretransform)))
        return E_FAIL;

    matPretransform *= XMMatrixRotationAxis(_vector{ 0,1,0,0 }, XMConvertToRadians(180));

    if (FAILED(Load_Dirctory_Models_Recursive(LEVEL_CHAPTER_4,
        TEXT("../Bin/Resources/Models/3DAnim/"), matPretransform)))
        return E_FAIL;




    lstrcpy(m_szLoadingText, TEXT("객체원형(을)를 로딩중입니다."));

    /* For. Prototype_GameObject_SampleBook */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_GameObject_SampleBook"),
        CSampleBook::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    ///////////////////////////////// UI /////////////////////////////////
    /* For. Prototype_UIObject_Pick_Bubble */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_UIObejct_PickBubble"),
        CPick_Bulb::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_GameObject_StopStamp"),
        CStopStamp_UI::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_GameObject_BombStamp"),
        CBombStamp_UI::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_GameObject_ArrowForStamp"),
        CArrowForStamp::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_GameObject_ESCHeartPoint"),
        ESC_HeartPoint::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_GameObject_Interaction_Book"),
        CUI_Interaction_Book::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_GameObject_ParentShopPannel"),
        CShopPanel::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_GameObject_ShopPannelBG"),
        CShopPanel_BG::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_GameObject_ShopPannelYesNo"),
        CShopPanel_YesNo::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_GameObject_ShopItem"),
        CShopItemBG::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_GameObject_Interaction_Heart"),
        CInteraction_Heart::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_GameObject_Dialogue"),
        CDialog::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_GameObject_Dialogue_Portrait"),
        CPortrait::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_GameObject_JumpPad"),
        CJumpPad::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    ///////////////////////////////// UI /////////////////////////////////
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_GameObject_StoreNPC"),
        CNPC_Store::Create(m_pDevice, m_pContext))))
        return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_GameObject_NPC_OnlySocial"),
	//	CNPC_OnlySocial::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;
    //if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_GameObject_NPC_Social"),
    //    CNPC_Social::Create(m_pDevice, m_pContext))))
    //    return E_FAIL;
    ///////////////////////////////// NPC /////////////////////////////////
    /* Boss */

    /* For. Prototype_GameObject_ButterGrump */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_GameObject_ButterGrump"),
        CButterGrump::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_GameObject_Boss_HomingBall"),
        CBoss_HomingBall::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_GameObject_Boss_EnergyBall"),
        CBoss_EnergyBall::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_GameObject_Boss_YellowBall"),
        CBoss_YellowBall::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_GameObject_Boss_PurpleBall"),
        CBoss_PurpleBall::Create(m_pDevice, m_pContext))))
        return E_FAIL;


    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_GameObject_FloorWord"),
        CFloorWord::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_4, TEXT("Prototype_GameObject_PrintFloorWord"),
        CPrintFloorWord::Create(m_pDevice, m_pContext))))
        return E_FAIL;


    //Map_Object_Create(LEVEL_STATIC, LEVEL_CHAPTER_4, L"Room_Enviroment.mchc");
    Map_Object_Create(LEVEL_STATIC, LEVEL_CHAPTER_4, L"Room_Enviroment_Small.mchc");

    lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
    m_isFinished = true;

    return S_OK;
}

HRESULT CLoader::Loading_Level_Camera_Tool()
{
    lstrcpy(m_szLoadingText, TEXT("컴포넌트를 로딩중입니다."));

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CAMERA_TOOL, TEXT("Prototype_Component_Ray"),
        CRay::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CAMERA_TOOL, TEXT("Prototype_Component_FSM"),
        CFSM::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CAMERA_TOOL, TEXT("Prototype_Component_FSM_Boss"),
        CFSM_Boss::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CAMERA_TOOL, TEXT("Prototype_Component_DetectionField"),
        CDetectionField::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CAMERA_TOOL, TEXT("Prototype_Component_Sneak_DetectionField"),
        CSneak_DetectionField::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CAMERA_TOOL, TEXT("Prototype_Component_BarfBugAttackAnimEvent"),
        CAnimEventGenerator::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/3DAnim/barfBug_Rig/BarfBug_Attack.animevt"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CAMERA_TOOL, TEXT("Prototype_Component_BarfBug2DAttackAnimEvent"),
        CAnimEventGenerator::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/2DAnim/Chapter2/Monster/BarferBug/BarferBug2d_Attack.animevt"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CAMERA_TOOL, TEXT("Prototype_Component_BookPageActionEvent"),
        CAnimEventGenerator::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/3DMapObject/book/book_Animation_Event.animevt"))))
        return E_FAIL;

#ifdef _DEBUG
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CAMERA_TOOL, TEXT("Prototype_Component_DebugDraw_For_Client"),
        CDebugDraw_For_Client::Create(m_pDevice, m_pContext))))
        return E_FAIL;
#endif // _DEBUG



    lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

    lstrcpy(m_szLoadingText, TEXT("사운드를 로딩중입니다."));

    lstrcpy(m_szLoadingText, TEXT("쉐이더를 로딩중입니다."));

    lstrcpy(m_szLoadingText, TEXT("모델(을)를 로딩중입니다."));

    XMMATRIX matPretransform = XMMatrixScaling(1 / 150.0f, 1 / 150.0f, 1 / 150.0f);
    if (FAILED(Load_Dirctory_2DModels_Recursive(LEVEL_CAMERA_TOOL,
        TEXT("../Bin/Resources/Models/2DMapObject/Chapter2"))))
        return E_FAIL;
    if (FAILED(Load_Dirctory_2DModels_Recursive(LEVEL_CAMERA_TOOL,
        TEXT("../Bin/Resources/Models/2DMapObject/Static"))))
        return E_FAIL;

    if (FAILED(Load_Dirctory_2DModels_Recursive(LEVEL_CAMERA_TOOL,
        TEXT("../Bin/Resources/Models/2D_FX"))))
        return E_FAIL;

    if (FAILED(Load_Models_FromJson(LEVEL_CAMERA_TOOL, MAP_3D_DEFAULT_PATH, L"Chapter_02_Play_Desk.json", matPretransform, true)))
        return E_FAIL;
    //if (FAILED(Load_Models_FromJson(LEVEL_CAMERA_TOOL, MAP_3D_DEFAULT_PATH, L"Chapter_04_Play_Desk.json", matPretransform, true)))
    //    return E_FAIL;

    if (FAILED(Load_Dirctory_Models_Recursive(LEVEL_CAMERA_TOOL,
        TEXT("../Bin/Resources/Models/3DMapObject/"), matPretransform)))
        return E_FAIL;

    matPretransform *= XMMatrixRotationAxis(_vector{ 0,1,0,0 }, XMConvertToRadians(180));
    if (FAILED(Load_Dirctory_Models_Recursive(LEVEL_CAMERA_TOOL,
        TEXT("../Bin/Resources/Models/3DAnim/"), matPretransform)))
        return E_FAIL;




    if (FAILED(Load_Dirctory_2DModels_Recursive(LEVEL_CAMERA_TOOL,
        TEXT("../Bin/Resources/Models/2DAnim/Chapter2/"))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("객체원형(을)를 로딩중입니다."));

    /* For. Prototype_GameObject_Cube */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CAMERA_TOOL, TEXT("Prototype_GameObject_Cube"),
        CCube::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_SampleBook */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CAMERA_TOOL, TEXT("Prototype_GameObject_SampleBook"),
        CSampleBook::Create(m_pDevice, m_pContext))))
        return E_FAIL;


    /* For. Magic_Hand, Magic_Hand_Body*/
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CAMERA_TOOL, TEXT("Prototype_GameObject_MagicHand"),
        CMagic_Hand::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CAMERA_TOOL, TEXT("Prototype_GameObject_MagicHandBody"),
        CMagic_Hand_Body::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_Camera_CutScene */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CAMERA_TOOL, TEXT("Prototype_GameObject_Camera_CutScene_Save"),
        CCamera_CutScene::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    matPretransform = XMMatrixScaling(1 / 150.0f, 1 / 150.0f, 1 / 150.0f);


    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CAMERA_TOOL, TEXT("Prototype_Model_Book"),
        C3DModel::Create(m_pDevice, m_pContext,
            ("../../Tool_Effect/Bin/Resources/Models/3DAnim/book/book.model"
                ), matPretransform))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CAMERA_TOOL, TEXT("Prototype_Model_MagicHand"),
        C3DModel::Create(m_pDevice, m_pContext,
            ("../Bin/Resources/Models/FX/magic_hand_model/magic_hand_model.model"
                ), matPretransform))))
        return E_FAIL;

    //Map_Object_Create(LEVEL_STATIC, LEVEL_CHAPTER_2, L"Room_Enviroment.mchc");
    Map_Object_Create(LEVEL_STATIC, LEVEL_CAMERA_TOOL, L"Room_Enviroment_Small.mchc");

    lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
    m_isFinished = true;

    return S_OK;
}

HRESULT CLoader::Loading_BGM_PathFind(const _wstring& strDirectoryPath)
{
    for (const auto& entry : std::filesystem::directory_iterator(strDirectoryPath))
    {
        if (entry.is_regular_file())
        {
            // 전체 경로
            _string strFullPath = entry.path().generic_string();
            _string strPrototypeTag;
            _string strFileName = entry.path().stem().generic_string();
            strPrototypeTag = strFileName;
            _wstring wstrPrototypeTag = m_pGameInstance->StringToWString(strPrototypeTag);
            _wstring wstrFullPath = m_pGameInstance->StringToWString(strFullPath);

            if (FAILED(LOAD_BGM(wstrPrototypeTag, wstrFullPath)))
                return E_FAIL;
        }
    }

    return S_OK;
}

HRESULT CLoader::Loading_SFX_PathFind(const _wstring& strDirectoryPath)
{
    for (const auto& entry : std::filesystem::directory_iterator(strDirectoryPath))
    {
        if (entry.is_regular_file())
        {
            // 전체 경로
            _string strFullPath = entry.path().generic_string();
            _string strPrototypeTag;
            _string strFileName = entry.path().stem().generic_string();
            strPrototypeTag = strFileName;
            _wstring wstrPrototypeTag = m_pGameInstance->StringToWString(strPrototypeTag);
            _wstring wstrFullPath = m_pGameInstance->StringToWString(strFullPath);

            if (FAILED(LOAD_SFX(wstrPrototypeTag, wstrFullPath)))
                return E_FAIL;
        }
    }

    return S_OK;
}

HRESULT CLoader::Loading_Level_Chapter_TEST()
{
    lstrcpy(m_szLoadingText, TEXT("컴포넌트를 로딩중입니다."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Component_FSM"),
        CFSM::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Component_FSM_Boss"),
        CFSM_Boss::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Component_DetectionField"),
        CDetectionField::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Component_Sneak_DetectionField"),
        CSneak_DetectionField::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Component_BarfBugAttackAnimEvent"),
        CAnimEventGenerator::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/3DAnim/barfBug_Rig/BarfBug_Attack.animevt"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Component_BarfBug2DAttackAnimEvent"),
        CAnimEventGenerator::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/2DAnim/Chapter2/Monster/BarferBug/BarferBug2d_Attack.animevt"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Component_ZippyAttackAnimEvent"),
        CAnimEventGenerator::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/2DAnim/Chapter2/Monster/Zippy/Zippy_Attack.animevt"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Component_BirdMonsterAttackEvent"),
        CAnimEventGenerator::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/3DAnim/Namastarling_Rig_01/BirdMonster_Attack.animevt"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Component_JumpBugJumpEvent"),
        CAnimEventGenerator::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/3DAnim/jumpBug_Rig/JumpBug_Jump.animevt"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Component_SoldierAttackEvent"),
        CAnimEventGenerator::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/3DAnim/humgrump_troop_Rig_GT/SoldierAttack.animevt"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Component_BookPageActionEvent"),
        CAnimEventGenerator::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/3DMapObject/book/book_Animation_Event.animevt"))))
        return E_FAIL;

#ifdef _DEBUG
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Component_DebugDraw_For_Client"),
        CDebugDraw_For_Client::Create(m_pDevice, m_pContext))))
        return E_FAIL;
#endif // _DEBUG





    lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Component_Texture_PickBulb"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Bulb_Pickup/pickup_counter_bulb_01.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Component_Texture_BombStamp"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Object/Stamp/Resize_BombStamp.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Component_Texture_StopStamp"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Object/Stamp/Resize_StopStamp.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Component_Texture_ArrowForStamp"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Object/Stamp/Arrow.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Component_Texture_HeartPoint"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/HPBar/HUD_Heart_%d.dds"), 13))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Component_Texture_KEYQ"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Static/KeyIcon/Keyboard/keyboard_Q.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Component_Texture_ShopBG"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/T_Panel_BG.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Component_Texture_ShopDialogue"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/shop_ui_panel_text.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Component_Texture_ShopChooseBG"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/T_panel_YesNo.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Component_Texture_ShopBulb"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/shop_ui_panel_bulb.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Component_Texture_DialogueBG"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Dialogue/Dialogue_BG/Dialogue/dialogue_%d.dds"), 27))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Component_Texture_DialoguePortrait"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Dialogue/Dialogue_BG/Character_Icon/dialogue_icon_%d.dds"), 17))))
        return E_FAIL;



    ///// 상점 관련
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Component_Texture_JumpAttack0"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_jump.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Component_Texture_SpinAttack0"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_spin.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Component_Texture_SpinAttack1"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_spin_upgrade_1.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Component_Texture_SpinAttack2"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_spin_upgrade_2.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Component_Texture_SpinAttack3"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_spin_upgrade_3.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Component_Texture_AttackPlus1"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_sword_upgrade_1.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Component_Texture_AttackPlus2"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_sword_upgrade_2.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Component_Texture_AttackPlus3"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_sword_upgrade_3.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Component_Texture_ThrowAttack0"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_throw.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Component_Texture_ThrowAttack1"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_throw_upgrade_1.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Component_Texture_ThrowAttack2"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_throw_upgrade_2.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Component_Texture_ThrowAttack3"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_throw_upgrade_3.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Component_Texture_ScrollItem"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_scroll.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Component_Texture_ItemSelectedBG"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/shop_ui_panel_item_selected._testtga.dds"), 1))))
        return E_FAIL;


    ///// 상점 관련
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Component_Texture_BACK"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Static/KeyIcon/Keyboard/keyboard_backspace.dds"), 1))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Component_Texture_Enter"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Static/KeyIcon/Keyboard/keyboard_Enter.dds"), 1))))
        return E_FAIL;




    lstrcpy(m_szLoadingText, TEXT("사운드를 로딩중입니다."));


    lstrcpy(m_szLoadingText, TEXT("쉐이더를 로딩중입니다."));

    lstrcpy(m_szLoadingText, TEXT("모델(을)를 로딩중입니다."));

    XMMATRIX matPretransform = XMMatrixScaling(1 / 150.0f, 1 / 150.0f, 1 / 150.0f);
    if (FAILED(Load_Dirctory_2DModels_Recursive(LEVEL_CHAPTER_TEST,
        TEXT("../Bin/Resources/Models/2DMapObject/Chapter4"))))
        return E_FAIL;

    if (FAILED(Load_Dirctory_2DModels_Recursive(LEVEL_CHAPTER_TEST,
        TEXT("../Bin/Resources/Models/2DMapObject/Static"))))
        return E_FAIL;

    /* 낱개 로딩 예시*/

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Model2D_FallingRock"),
        C2DModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/2DAnim/Chapter2/FallingRock/FallingRock.model2D", false))))
        return E_FAIL;

    matPretransform = XMMatrixScaling(1 / 160.0f, 1 / 160.0f, 1 / 160.0f);
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Model3D_FallingRock"),
        C3DModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NonAnim/Rock_03/Rock_03.model", matPretransform))))
        return E_FAIL;
    matPretransform = XMMatrixScaling(1 / 150.0f, 1 / 150.0f, 1 / 150.0f);

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("dice_pink_03"),
        C2DModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NonAnim/dice_01/dice_pink_03.dds", true))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Model2D_FallingRockShadow"),
        C2DModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/2DNonAnim/FallingRockShadow/FallingRockShadow.dds", true))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("JumpBug"),
        C2DModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/2DAnim/Chapter4/Monster/JumpBug/JumpBug.model2d"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Rat"),
        C2DModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/2DAnim/Monster/Rat/Rat.model2d"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Bomb2D"),
        C2DModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/2DAnim/Static/Bomb/Bomb2D.model2d"))))
        return E_FAIL;


    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Component_Dice3D"),
        C3DModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NonAnim/dice_01/dice_01.model", matPretransform))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Model_Domino4"),
        C3DModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NonAnim/Domino_4/Domino_4.model", matPretransform))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Model_Domino3"),
        C3DModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NonAnim/Domino_3/Domino_3.model", matPretransform))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Model_Domino2"),
        C3DModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NonAnim/Domino_2/Domino_2.model", matPretransform))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_Model_Domino1"),
        C3DModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NonAnim/Domino_1/Domino_1.model", matPretransform))))
        return E_FAIL;
    //if (FAILED(Load_Models_FromJson(LEVEL_CHAPTER_TEST, MAP_3D_DEFAULT_PATH, L"Chapter_04_Default_Desk.json", matPretransform, true)))
    //    return E_FAIL;
    if (FAILED(Load_Models_FromJson(LEVEL_CHAPTER_TEST, MAP_3D_DEFAULT_PATH, L"Chapter_04_Default_Desk.json", matPretransform, true)))
        return E_FAIL;

    if (FAILED(Load_Dirctory_Models_Recursive(LEVEL_CHAPTER_TEST,
        TEXT("../Bin/Resources/Models/3DMapObject/"), matPretransform)))
        return E_FAIL;

    matPretransform *= XMMatrixRotationAxis(_vector{ 0,1,0,0 }, XMConvertToRadians(180));

    if (FAILED(Load_Dirctory_Models_Recursive(LEVEL_CHAPTER_TEST,
        TEXT("../Bin/Resources/Models/3DAnim/"), matPretransform)))
        return E_FAIL;

    if (FAILED(Load_Dirctory_2DModels_Recursive(LEVEL_CHAPTER_TEST,
        TEXT("../Bin/Resources/Models/2DAnim/Chapter2/"))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("객체원형(을)를 로딩중입니다."));

    /* For. Prototype_GameObject_SampleBook */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_GameObject_SampleBook"),
        CSampleBook::Create(m_pDevice, m_pContext))))
        return E_FAIL;


    ///////////////////////////////// UI /////////////////////////////////
    /* For. Prototype_UIObject_Pick_Bubble */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_UIObejct_PickBubble"),
        CPick_Bulb::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_GameObject_StopStamp"),
        CStopStamp_UI::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_GameObject_BombStamp"),
        CBombStamp_UI::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_GameObject_ArrowForStamp"),
        CArrowForStamp::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_GameObject_ESCHeartPoint"),
        ESC_HeartPoint::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_GameObject_Interaction_Book"),
        CUI_Interaction_Book::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_GameObject_ParentShopPannel"),
        CShopPanel::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_GameObject_ShopPannelBG"),
        CShopPanel_BG::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_GameObject_ShopPannelYesNo"),
        CShopPanel_YesNo::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_GameObject_ShopItem"),
        CShopItemBG::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_GameObject_Interaction_Heart"),
        CInteraction_Heart::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_GameObject_Dialogue"),
        CDialog::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_GameObject_Dialogue_Portrait"),
        CPortrait::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_GameObject_Narration"),
        CNarration::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_GameObject_Narration_Anim"),
        CNarration_Anim::Create(m_pDevice, m_pContext))))
        return E_FAIL;



    ///////////////////////////////// UI /////////////////////////////////
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_GameObject_StoreNPC"),
        CNPC_Store::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_GameObject_NPC_OnlySocial"),
        CNPC_OnlySocial::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_GameObject_NPC_Social"),
        CNPC_Social::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_GameObject_NPC_Companion"),
        CNPC_Companion::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_GameObject_NPC_Companion_Violet"),
        CNPC_Violet::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_GameObject_NPC_Companion_Thrash"),
        CNPC_Thrash::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    ///////////////////////////////// NPC /////////////////////////////////


    /* Monster */

    /* For. Prototype_GameObject_Goblin_SideScroller */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Goblin_SideScroller"),
        CGoblin_SideScroller::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_Zippy */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Zippy"),
        CZippy::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* Boss */

    /* For. Prototype_GameObject_ButterGrump */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_GameObject_ButterGrump"),
        CButterGrump::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_GameObject_Boss_HomingBall"),
        CBoss_HomingBall::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_GameObject_Boss_EnergyBall"),
        CBoss_EnergyBall::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_GameObject_Boss_YellowBall"),
        CBoss_YellowBall::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_GameObject_Boss_PurpleBall"),
        CBoss_PurpleBall::Create(m_pDevice, m_pContext))))
        return E_FAIL;


    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_GameObject_FloorWord"),
        CFloorWord::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_GameObject_PrintFloorWord"),
        CPrintFloorWord::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* 상호작용 오브젝트 */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_GameObject_Dice"),
        CDice::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_GameObject_Domino"),
        CDomino::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* Etc */

    /* For. Prototype_GameObject_FallingRock */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_GameObject_FallingRock"),
        CFallingRock::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_CollapseBlock */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAPTER_TEST, TEXT("Prototype_GameObject_CollapseBlock"),
        CCollapseBlock::Create(m_pDevice, m_pContext))))
        return E_FAIL;



    //Map_Object_Create(LEVEL_STATIC, LEVEL_CHAPTER_TEST, L"Room_Enviroment.mchc");
    Map_Object_Create(LEVEL_STATIC, LEVEL_CHAPTER_TEST, L"Room_Enviroment_Small.mchc");

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
                C2DModel::Create(m_pDevice, m_pContext, entry.path().string().c_str()))))
            {
                string str = "Failed to Create 2DModel : ";
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
    _string arrLevelTexts[] = {
        "",
        "STATIC",
        "LOGO",
        "CHAPTER_2",
        "CHAPTER_4",
        "CHATER_TEST",
    };
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
    path = "../Bin/Resources/Models/NonAnim/";
    string strFileName;
	_uint iLoadedCount = 0;
    _uint iLoadCount = (_uint)strModelNames.size();
    for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
        if (entry.path().extension() != ".model") continue; 
        strFileName = entry.path().filename().replace_extension().string();
        auto iter = strModelNames.find(strFileName);
        if (iter != strModelNames.end())
        {
            strModelNames.erase(iter);
            if (FAILED(m_pGameInstance->Add_Prototype(_iLevId, StringToWstring(strFileName),
                C3DModel::Create(m_pDevice, m_pContext, entry.path().string().c_str(), _PreTransformMatrix, _isCollider))))
                return E_FAIL;
			iLoadedCount++;
            cout <<""<< arrLevelTexts[_iLevId] << " Map Load, " << iLoadedCount << " / " << iLoadCount << "("<< (((_float)iLoadedCount / (_float)iLoadCount) * 100.f)  << "%)... " << endl;
			
            if (iLoadedCount >= iLoadCount)
                break;
        }
    }
    cout << "" << arrLevelTexts[_iLevId] << " Map Load  Complete! ";
    
    if (0 < (_uint)strModelNames.size())
    {
        cout << (_uint)strModelNames.size() << " File Missing..." << endl;
        wcout << "확인용, 스케치스페이스는 나중에 수정" << endl;
    }

    for (_string strFIle : strModelNames)
    {
        cout << "Missing File : " << strFIle << endl;

    }

    return S_OK;
}


HRESULT CLoader::Load_Directory_Effects(LEVEL_ID _iLevID, const _tchar* _szJsonFilePath)
{
    std::filesystem::path path;
    path = _szJsonFilePath;

    for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
        if (entry.path().extension() == ".json") {
            
            CEffect_System* pParticleSystem = CEffect_System::Create(m_pDevice, m_pContext, entry.path().c_str());

            if (FAILED(m_pGameInstance->Add_Prototype(_iLevID, entry.path().filename(), pParticleSystem)))
            {
                string str = "Failed to Create Effects";
                str += entry.path().filename().replace_extension().string();
                MessageBoxA(NULL, str.c_str(), "에러", MB_OK);
                return E_FAIL;
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
            C3DMapObject* pGameObject =
                CMapObjectFactory::Bulid_3DObject<C3DMapObject>(
                    (LEVEL_ID)_eObjectLevelId,
                    m_pGameInstance,
                    hFile, false, true , true);
            if (nullptr != pGameObject)
                m_pGameInstance->Add_GameObject_ToLayer(_eObjectLevelId, strLayerTag.c_str(), pGameObject);
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

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);
}
