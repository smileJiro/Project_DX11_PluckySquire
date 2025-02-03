#include "stdafx.h"
#include "Loader.h"

#include "GameInstance.h"
#include "CriticalSectionGuard.h"

#include "Camera_Free.h"
#include "Camera_Target.h"
#include "Camera_CutScene.h"

/* For. Trigger*/
#include "Camera_Trigger.h"

/* For. UI*/
#include "Pick_Bulb.h"
#include "SettingPanelBG.h"
#include "StopStamp.h"
#include "BombStamp.h"
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
/* For. UI*/

/* For. NPC*/
#include "NPC_Store.h"


#include "ModelObject.h"
#include "Player.h"
#include "PlayerBody.h"
#include "PlayerSword.h"
#include "TestTerrain.h"

#include "2DModel.h"
#include "3DModel.h"
#include "Controller_Model.h"
#include "FSM.h"
#include "set"
#include "StateMachine.h"
#include "MapObject.h"
#include "DetectionField.h"

/* For. Monster */
#include "Beetle.h"
#include "BarfBug.h"
#include "Projectile_BarfBug.h"
#include "JumpBug.h"
#include "Goblin.h"
#include "Rat.h"
#include "BirdMonster.h"
#include "Soldier_Spear.h"
#include "Soldier_CrossBow.h"
#include "Soldier_Bomb.h"
#include "Popuff.h"
#include "Monster_Body.h"

/* For. Boss */
#include "ButterGrump.h"
#include "Boss_EnergyBall.h"
#include "Boss_HomingBall.h"
#include "Boss_YellowBall.h"
#include "Boss_PurpleBall.h"
#include "FSM_Boss.h"


// Sample
#include "SampleBook.h"
#include "2DDefault_RenderObject.h"

#include "DebugDraw_For_Client.h"




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

    lstrcpy(m_szLoadingText, TEXT("컴포넌트를 로딩중입니다."));

    lstrcpy(m_szLoadingText, TEXT("2D 콜라이더를 로딩중입니다."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
        CCollider_Circle::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
        CCollider_AABB::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));
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
        CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMeshInstance.hlsl"), VTXMESHPARTICLE::Elements, VTXMESHPARTICLE::iNumElements))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("모델(을)를 로딩중입니다."));
    XMMATRIX matPretransform = XMMatrixScaling(1 / 150.0f, 1 / 150.0f, 1 / 150.0f);
    //matPretransform *= XMMatrixRotationAxis(_vector{ 0,1,0,0 }, XMConvertToRadians(180));


    //if (FAILED(Load_Models_FromJson(LEVEL_STATIC, MAP_3D_DEFAULT_PATH, L"Room_Free_Enviroment.json", matPretransform)))
    //    return E_FAIL;
    //
    if (FAILED(Load_Models_FromJson(LEVEL_STATIC, MAP_3D_DEFAULT_PATH, L"Room_Enviroment.json", matPretransform)))
        return E_FAIL;


    /* For. Prototype_Component_VIBuffer_Rect */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
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
    // ============ Triger
    /* For. Prototype_GameObject_Camera_Trigger */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_Trigger"),
        CCamera_Trigger::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_UIObejct_ParentSettingPanel"), CSettingPanel::Create(m_pDevice, m_pContext))))
        return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_UIObejct_SettingPanel"), CSettingPanelBG::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_UIObejct_ESC_Goblin"), CESC_Goblin::Create(m_pDevice, m_pContext))))
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
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_TestPlayer"),
        CPlayer::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_PlayerSword"),
        CPlayerSword::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_PlayerBody"),
        CPlayerBody::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    /* Monster */

    /* For. Prototype_GameObject_Monster_Body */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Monster_Body"),
        CMonster_Body::Create(m_pDevice, m_pContext))))
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

    /* For. Prototype_GameObject_Soldier_Spear */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Soldier_Spear"),
        CSoldier_Spear::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_Soldier_CrossBow */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Soldier_CrossBow"),
        CSoldier_CrossBow::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_Soldier_Bomb */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Soldier_Bomb"),
        CSoldier_Bomb::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_Popuff */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Popuff"),
        CPopuff::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("이펙트(을)를 로딩중입니다."));

    if (FAILED(Load_Directory_Effects(LEVEL_STATIC, TEXT("../Bin/DataFiles/FX/Common/"))))
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

    lstrcpy(m_szLoadingText, TEXT("객체원형(을)를 로딩중입니다."));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_UIObejct_LogoBG"), CLogo_BG::Create(m_pDevice, m_pContext))))
		return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_UIObejct_LogoProp"), CLogo_Props::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_UIObejct_Logo"), CLogo::Create(m_pDevice, m_pContext))))
        return E_FAIL;


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

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_FSM_Boss"),
        CFSM_Boss::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_PlayerAnimEvent"),
        CAnimEventGenerator::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/3DAnim/Latch_SkelMesh_NewRig/aaa.animevt"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_DetectionField"),
        CDetectionField::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_BarfBugAnimEvent"),
        CAnimEventGenerator::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/3DAnim/barfBug_Rig/BarfBug_Attack.animevt"))))
        return E_FAIL;

    #ifdef _DEBUG
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_DebugDraw_For_Client"),
        CDebugDraw_For_Client::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    #endif // _DEBUG

    



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
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HeartPoint"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/HPBar/HUD_Heart_%d.dds"), 13))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_KEYQ"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Static/KeyIcon/Keyboard/keyboard_Q.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ShopBG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/T_Panel_BG.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ShopDialogue"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/shop_ui_panel_text.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ShopChooseBG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/T_panel_YesNo.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ShopBulb"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/shop_ui_panel_bulb.dds"), 1))))
		return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_DialogueBG"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Dialogue/Dialogue_BG/Dialogue/dialogue_%d.dds"), 27))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_DialoguePortrait"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Dialogue/Dialogue_BG/Character_Icon/dialogue_icon_%d.dds"), 17))))
        return E_FAIL;


    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_SampleMap"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Map/SampleMap.dds"), 1))))
		return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_2DDefaultRenderObject"),
        C2DDefault_RenderObject::Create(m_pDevice, m_pContext))))
        return E_FAIL;



    ///// 상점 관련
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_JumpAttack0"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_jump.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_SpinAttack0"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_spin.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_SpinAttack1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_spin_upgrade_1.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_SpinAttack2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_spin_upgrade_2.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_SpinAttack3"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_spin_upgrade_3.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_AttackPlus1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_sword_upgrade_1.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_AttackPlus2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_sword_upgrade_2.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_AttackPlus3"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_sword_upgrade_3.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ThrowAttack0"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_throw.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ThrowAttack1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_throw_upgrade_1.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ThrowAttack2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_throw_upgrade_2.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ThrowAttack3"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_throw_upgrade_3.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ScrollItem"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/ItemIcon/shop_ui_icon_item_scroll.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ItemSelectedBG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Menu/Shop/shop_ui_panel_item_selected._testtga.dds"), 1))))
		return E_FAIL;


    ///// 상점 관련




	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BACK"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Static/KeyIcon/Keyboard/keyboard_backspace.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Enter"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Static/KeyIcon/Keyboard/keyboard_Enter.dds"), 1))))
		return E_FAIL;




    lstrcpy(m_szLoadingText, TEXT("사운드를 로딩중입니다."));

    lstrcpy(m_szLoadingText, TEXT("쉐이더를 로딩중입니다."));

    lstrcpy(m_szLoadingText, TEXT("모델(을)를 로딩중입니다."));


    if (FAILED(Load_Dirctory_2DModels_Recursive(LEVEL_GAMEPLAY,
        TEXT("../Bin/Resources/Models/2DMapObject/"))))
        return E_FAIL;

    /* 낱개 로딩 예시*/

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_player2DAnimation"),
        C2DModel::Create(m_pDevice, m_pContext, ("../Bin/Resources/Models/2DAnim/Player/player.model2D")))))
        return E_FAIL;

    // NPC 모델
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_NPC_SHOP_2DAnimation"),
        C2DModel::Create(m_pDevice, m_pContext, ("../Bin/Resources/Models/2DAnim/NPC/NPC_Shop/NPC_Store.model2D")))))
        return E_FAIL;


    XMMATRIX matPretransform = XMMatrixScaling(1 / 150.0f, 1 / 150.0f, 1 / 150.0f);
    
    //if (FAILED(Load_Models_FromJson(LEVEL_GAMEPLAY, MAP_3D_DEFAULT_PATH, L"Room_Free_Enviroment.json", matPretransform)))
    //    return E_FAIL;
    if (FAILED(Load_Models_FromJson(LEVEL_GAMEPLAY, MAP_3D_DEFAULT_PATH, L"Chapter_04_Default_Desk.json", matPretransform, true)))
        return E_FAIL;

    if (FAILED(Load_Dirctory_Models_Recursive(LEVEL_GAMEPLAY,
        TEXT("../Bin/Resources/Models/3DMapObject/"), matPretransform)))
        return E_FAIL;

    matPretransform *= XMMatrixRotationAxis(_vector{0,1,0,0},XMConvertToRadians(180));

    if (FAILED(Load_Dirctory_Models_Recursive(LEVEL_GAMEPLAY,
        TEXT("../Bin/Resources/Models/3DAnim/"), matPretransform)))
        return E_FAIL;
    
    if (FAILED(Load_Dirctory_Models_Recursive(LEVEL_GAMEPLAY,
        TEXT("../Bin/Resources/Models/3DObject/"), matPretransform)))
        return E_FAIL;
    if (FAILED(Load_Dirctory_2DModels_Recursive(LEVEL_GAMEPLAY,
        TEXT("../Bin/Resources/Models/2DAnim/"))))
        return E_FAIL;


    lstrcpy(m_szLoadingText, TEXT("객체원형(을)를 로딩중입니다."));

    // ============ Camera
    /* For. Prototype_GameObject_Camera_Free */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Camera_Free"),
        CCamera_Free::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_Camera_Target */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Camera_Target"),
        CCamera_Target::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_Camera_CutScene */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Camera_CutScene"),
        CCamera_CutScene::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_MapObject */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_MapObject"),
        CModelObject::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    /* For. Prototype_GameObject_MapObject */

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_SampleBook"),
        CSampleBook::Create(m_pDevice, m_pContext))))
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
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_ESCHeartPoint"),
		ESC_HeartPoint::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Interaction_Book"),
		CUI_Interaction_Book::Create(m_pDevice, m_pContext))))
		return E_FAIL;

    // TEST
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_ParentShopPannel"),
		CShopPanel::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_ShopPannelBG"),
		CShopPanel_BG::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_ShopPannelYesNo"),
		CShopPanel_YesNo::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_ShopItem"),
		CShopItemBG::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Interaction_Heart"),
		CInteraction_Heart::Create(m_pDevice, m_pContext))))
		return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Dialogue"),
        CDialog::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Dialogue_Portrait"),
        CPortrait::Create(m_pDevice, m_pContext))))
        return E_FAIL;


    
    ///////////////////////////////// UI /////////////////////////////////
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_StoreNPC"), CNPC_Store::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    ///////////////////////////////// NPC /////////////////////////////////
    /* Boss */

    /* For. Prototype_GameObject_ButterGrump */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_ButterGrump"),
        CButterGrump::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Boss_HomingBall"),
        CBoss_HomingBall::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Boss_EnergyBall"),
        CBoss_EnergyBall::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Boss_YellowBall"),
        CBoss_YellowBall::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Boss_PurpleBall"),
        CBoss_PurpleBall::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    // 액터 들어가는넘. 뺌.
    //Map_Object_Create(LEVEL_GAMEPLAY, LEVEL_GAMEPLAY, L"Chapter_04_Default_Desk.mchc");

    Map_Object_Create(LEVEL_STATIC, LEVEL_GAMEPLAY, L"Room_Enviroment.mchc");

    //Create_Trigger(LEVEL_STATIC, LEVEL_GAMEPLAY, TEXT("../Bin/DataFiles/Trigger/"));

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
    path = "../Bin/Resources/Models/NonAnim/";
    string strFileName;
	_uint iLoadedCount = 0;
    _uint iLoadCount = (_uint)strModelNames.size();
    for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
        if (entry.path().extension() != ".model") continue; 
        strFileName = entry.path().filename().replace_extension().string();
        if (strModelNames.find(strFileName) != strModelNames.end())
        {
            if (FAILED(m_pGameInstance->Add_Prototype(_iLevId, StringToWstring( strFileName),
                C3DModel::Create(m_pDevice, m_pContext, entry.path().string().c_str(), _PreTransformMatrix, _isCollider))))
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
            _char		szSaveMeshName[MAX_PATH];
            _float4x4	vWorld = {};


            isTempReturn = ReadFile(hFile, &szSaveMeshName, (DWORD)(sizeof(_char) * MAX_PATH), &dwByte, nullptr);
            isTempReturn = ReadFile(hFile, &vWorld, sizeof(_float4x4), &dwByte, nullptr);


            CMapObject::MAPOBJ_DESC NormalDesc = {};
            NormalDesc.strModelPrototypeTag_3D = m_pGameInstance->StringToWString(szSaveMeshName).c_str();
            NormalDesc.strShaderPrototypeTag_3D = L"Prototype_Component_Shader_VtxMesh";
            NormalDesc.isCoordChangeEnable = false;
            NormalDesc.iModelPrototypeLevelID_3D = _eProtoLevelId;
            NormalDesc.isCulling = false;
            NormalDesc.eStartCoord = COORDINATE_3D;
            NormalDesc.tTransform3DDesc.isMatrix = true;
            NormalDesc.tTransform3DDesc.matWorld = vWorld;
            CGameObject* pGameObject = nullptr;
            m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_MapObject"),
                _eObjectLevelId,
                strLayerTag,
                &pGameObject,
                (void*)&NormalDesc);

            if (pGameObject)
            {
                DWORD	dwByte(0);
                _uint iOverrideCount = 0;
                C3DModel::COLOR_SHADER_MODE eTextureType;
                _float4 fDefaultDiffuseColor;


                isTempReturn = ReadFile(hFile, &eTextureType, sizeof(C3DModel::COLOR_SHADER_MODE), &dwByte, nullptr);
                static_cast<CMapObject*>(pGameObject)->Set_Color_Shader_Mode(eTextureType);

                switch (eTextureType)
                {
                case Engine::C3DModel::COLOR_DEFAULT:
                case Engine::C3DModel::MIX_DIFFUSE:
                {
                    isTempReturn = ReadFile(hFile, &fDefaultDiffuseColor, sizeof(_float4), &dwByte, nullptr);
                    static_cast<CMapObject*>(pGameObject)->Set_Diffuse_Color(fDefaultDiffuseColor);
                }
                break;
                default:
                    break;
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
                //pGameObject->Set_WorldMatrix(vWorld);
            }
        }
    }
    CloseHandle(hFile);
    return S_OK;
}

HRESULT CLoader::Create_Trigger(LEVEL_ID _eProtoLevelId, LEVEL_ID _eObjectLevelId, _wstring _szFilePath)
{
    std::filesystem::path path;
    //path = "../Bin/Resources/DataFiles/";
    for (const auto& entry : std::filesystem::recursive_directory_iterator(_szFilePath)) {
        if (entry.path().extension() == ".json") {
           
            ifstream file(entry.path());

            if (!file.is_open())
            {
                MSG_BOX("파일을 열 수 없습니다.");
                file.close();
                return E_FAIL;
            }

            json Result;
            file >> Result;
            file.close();

            for (auto& Trigger_json : Result) {
                TRIGGEROBJECT_DATA Data = {};

                _uint iTriggerType = Trigger_json["Trigger Type"];
                _float3 vPosition = { Trigger_json["Position"][0].get<_float>(),  Trigger_json["Position"][1].get<_float>() , Trigger_json["Position"][2].get<_float>() };
                _float3 vRotation = { Trigger_json["Rotation"][0].get<_float>(),  Trigger_json["Rotation"][1].get<_float>() , Trigger_json["Rotation"][2].get<_float>() };

                Data.iShapeType = Trigger_json["Shape Type"];
                Data.vHalfExtents = { Trigger_json["Half Extents"][0].get<_float>(),  Trigger_json["Half Extents"][1].get<_float>() , Trigger_json["Half Extents"][2].get<_float>() };
                Data.fRadius = Trigger_json["Radius"];

                Data.iFillterMyGroup = Trigger_json["Fillter My Group"];
                Data.iFillterOtherGroupMask = Trigger_json["Fillter Other Group Mask"];

                switch (iTriggerType) {
                case (_uint)TRIGGER_TYPE::CAMERA_TRIGGER:
                {
                    _uint iCameraTriggerType = Trigger_json["Camera Trigger Type"];
                    _string szEventTag = Trigger_json["Camera Trigger Event Tag"];
                    _uint iReturnMask = Trigger_json["Exit Return Mask"];

                    CCamera_Trigger::CAMERA_TRIGGER_DESC Desc;

                    Desc.iCameraTriggerType = iCameraTriggerType;
                    Desc.szEventTag = m_pGameInstance->StringToWString(szEventTag);
                    Desc.iReturnMask = iReturnMask;

                    Desc.eShapeType = (SHAPE_TYPE)Data.iShapeType;
                    Desc.vHalfExtents = Data.vHalfExtents;
                    Desc.fRadius = Data.fRadius;

                    Desc.iFillterMyGroup = Data.iFillterMyGroup;
                    Desc.iFillterOtherGroupMask = Data.iFillterOtherGroupMask;

                    Desc.tTransform3DDesc.vInitialPosition = vPosition;

                    CGameObject* pTrigger = nullptr;

                    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_Trigger"), LEVEL_GAMEPLAY, TEXT("Layer_Trigger"), &pTrigger, &Desc))) {
                        MSG_BOX("Failed To Load Camera Trigger");
                        return E_FAIL;
                    }

                    // Rotation
                    _matrix RotationMat = XMMatrixRotationX(XMConvertToRadians(vRotation.x)) * XMMatrixRotationY(XMConvertToRadians(vRotation.y)) * XMMatrixRotationZ(XMConvertToRadians(vRotation.z));
                    dynamic_cast<CTriggerObject*>(pTrigger)->Get_ActorCom()->Set_ShapeLocalOffsetMatrix(0, RotationMat);

                   // dynamic_cast<CTriggerObject*>(pTrigger)->Set_TriggerType(m_iTriggerType);
                }
                break;
                }
            }
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
