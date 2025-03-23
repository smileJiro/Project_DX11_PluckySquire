#include "stdafx.h"
#include "Minigame_Defender.h"
#include "Collider_AABB.h"
#include "PlayerData_Manager.h"
#include "Player.h"
#include "DefenderPlayer.h"
#include "DefenderCapsule.h"
#include "Section_Manager.h"
#include "GameInstance.h"
#include "DefenderSpawner.h"
#include "Camera_Manager.h"
#include "Camera_Target.h"
#include "Camera_2D.h"
#include "DefenderPerson.h"
#include "ScrollModelObject.h"
#include "ModelObject.h"
#include "Dialog_Manager.h"
#include "Dialogue.h"
#include "Camera_Manager.h"
#include "FatherPart_Prop.h"
#include "FatherGame.h"
#include "Portal_Default.h"
#include "Section_2D_PlayMap.h"

CMiniGame_Defender::CMiniGame_Defender(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CContainerObject(_pDevice, _pContext)
{
}

CMiniGame_Defender::CMiniGame_Defender(const CMiniGame_Defender& _Prototype)
	:CContainerObject(_Prototype)
{
}

HRESULT CMiniGame_Defender::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CMiniGame_Defender::Initialize(void* _pArg)
{
    m_pSectionManager = SECTION_MGR;
    m_pDialogManager = CDialog_Manager::GetInstance();
    m_mapMonsterPrototypeTag.insert(make_pair(DEFENDER_MONSTER_ID::SM_SHIP, _wstring(L"Prototype_GameObject_DefenderSmShip")));
   //m_mapMonsterPrototypeTag.insert(make_pair(DEFENDER_MONSTER_ID::TURRET, _wstring(L"Prototype_GameObject_DefenderTurret")));
    m_mapMonsterPrototypeTag.insert(make_pair(DEFENDER_MONSTER_ID::MED_SHIP_UP, _wstring(L"Prototype_GameObject_DefenderMedShip_UP")));
    m_mapMonsterPrototypeTag.insert(make_pair(DEFENDER_MONSTER_ID::MED_SHIP_DOWN, _wstring(L"Prototype_GameObject_DefenderMedShip_DOWN")));
    m_mapMonsterPrototypeTag.insert(make_pair(DEFENDER_MONSTER_ID::PERSON_CAPSULE, _wstring(L"Prototype_GameObject_PersonCapsule")));


    DEFENDER_DESC* pDesc = static_cast<DEFENDER_DESC*> (_pArg);
    m_iCurLevelID = pDesc->iCurLevelID;

    pDesc->iObjectGroupID = OBJECT_GROUP::MAPOBJECT;
    pDesc->eStartCoord = COORDINATE_2D;
    pDesc->isCoordChangeEnable = false;
    pDesc->iNumPartObjects = DEFENDER_PART_LAST;

    pDesc->tTransform2DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);

    pDesc->pActorDesc = nullptr;
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;
	if (FAILED(Ready_ControllTower()))
		return E_FAIL;

    m_p2DColliderComs.resize(2);
    /* Test 2D Collider */
    CCollider_AABB::COLLIDER_AABB_DESC tAABBDesc = {};
    tAABBDesc.pOwner = this;
    tAABBDesc.vExtents = { 50.f , 300.f};
    tAABBDesc.vScale = { 1.0f, 1.0f };
    tAABBDesc.vOffsetPosition = { 400.f,0.f };
    tAABBDesc.isBlock = false;
    tAABBDesc.isTrigger = true;
    tAABBDesc.iCollisionGroupID = OBJECT_GROUP::TRIGGER_OBJECT;
    tAABBDesc.iColliderUse = (_uint)COLLIDER2D_USE::COLLIDER2D_BODY;
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_TowerCollider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &tAABBDesc)))
        return E_FAIL;

    /* Test 2D Collider */
    CCollider_AABB::COLLIDER_AABB_DESC tAABBDesc2 = {};
    tAABBDesc2.pOwner = this;
    tAABBDesc2.vExtents = {1500.f , 300.f };
    tAABBDesc2.vScale = { 1.0f, 1.0f };
    tAABBDesc2.vOffsetPosition = { 0.f,0.f };
    tAABBDesc2.isBlock = false;
    tAABBDesc2.isTrigger = true;
    tAABBDesc2.iCollisionGroupID = OBJECT_GROUP::TRIGGER_OBJECT;
    tAABBDesc2.iColliderUse = (_uint)COLLIDER2D_USE::COLLIDER2D_TRIGGER;
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_DialogCollider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[1]), &tAABBDesc2)))
        return E_FAIL;

	return S_OK;
}
void CMiniGame_Defender::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);
    if (m_pCurrentCapsule && m_pCurrentCapsule->Is_Dead())
    {
        m_pCurrentCapsule = nullptr;
        m_fLastCapsuleDestroyTime = m_fTimeAcc;
    }
}
void CMiniGame_Defender::Set_GameState(DEFENDER_PROGRESS_STATE _eState)
{
	if (m_eGameState == _eState)
		return;
    m_eGameState = _eState;
    switch (m_eGameState)
    {
    case Client::CMiniGame_Defender::DEFENDER_PROG_NONE:
        break;
    case Client::CMiniGame_Defender::DEFENDER_PROG_ENTERED:
    {
        END_BGM();
        START_BGM(TEXT("LCD_MUS_C06_SPACE_DESK_COMBAT_FULL"), g_BGMVolume * 1.4f);

        m_pPlayer->Set_BlockPlayerInput(true);
        m_pPlayer->Set_Position(m_vStart_Position);
		m_pPlayer->Set_2DDirection(E_DIRECTION::RIGHT);
        CPortal* pTargetPortal = static_cast<CPortal_Default*>(static_cast<CSection_2D_PlayMap*>(CSection_Manager::GetInstance()->Find_Section(m_strSectionName))->Get_Portal(0));
        if (pTargetPortal)
            pTargetPortal->Set_Active(false);
        static_cast<CCamera_Target*>(CCamera_Manager::GetInstance()->Get_CurrentCamera())->Change_Target(this, m_fCamTargetChangeTime);
        m_fCamTargetChangeTimeAcc = 0.f;
        break;
    }
    case Client::CMiniGame_Defender::DEFENDER_PROG_BEGINNING_DIALOG:
    {
        //DIALOG
        m_pDialogManager->Set_DialogId(TEXT("DEFENDER_Dialogue_01"), m_strSectionName.c_str());
        //Uimgr->Set_DialogId(m_strDialogueIndex, m_strCurSecion);

        _vector vPos = Get_FinalPosition();
        _float3 vPosition; XMStoreFloat3(&vPosition, vPos);
        m_pDialogManager->Set_DialoguePos(vPosition);
        //Uimgr->Set_DialoguePos(vPos);
        m_pDialogManager->Set_DisPlayDialogue(true);
        break;
    }
	case Client::CMiniGame_Defender::DEFENDER_PROG_BEGINNING_DIALOG_ENDED:
	{

		_vector vPalyerPos = m_pPlayer->Get_FinalPosition();
		AUTOMOVE_COMMAND tCommand{};
		tCommand.eType = AUTOMOVE_TYPE::MOVE_TO;
		tCommand.iAnimIndex = (_uint)CPlayer::ANIM_STATE_2D::PLAYER_RUN_SWORD_RIGHT;
        
		tCommand.vTarget = vPalyerPos += { 1000.f, 0.f, 0.f };
        m_pPlayer->Add_AutoMoveCommand(tCommand);
        m_pPlayer->Start_AutoMove(true);
		break;
	}
    case Client::CMiniGame_Defender::DEFENDER_PROG_TRANSFORM_IN:
    {

        CPlayerData_Manager* pPDM = CPlayerData_Manager::GetInstance();
        m_pDefenderPlayer = pPDM->Get_DefenderPlayer_Ptr();
        Safe_AddRef(m_pDefenderPlayer);
        m_pDefenderPlayer->Register_AnimEndCallback(bind(&CMiniGame_Defender::On_PlayerAnimEnd, this, placeholders::_1, placeholders::_2));
        m_pPlayer->Set_Active(false);
        m_pPlayer->Clear_AutoMove();
        m_pDefenderPlayer->Set_Active(true);

        _vector vNormalPlayerPos = m_pPlayer->Get_FinalPosition();
        m_pDefenderPlayer->Set_Position(vNormalPlayerPos + _vector{0.f,20.f,0.f});
        m_pDefenderPlayer->Start_Transform();

        static_cast<CCamera_Target*>(CCamera_Manager::GetInstance()->Get_CurrentCamera())->Change_Target(m_pDefenderPlayer);
        m_p2DColliderComs[0]->Set_Offset(_float2{ 0.f,0.f });

        break;
    }
    case Client::CMiniGame_Defender::DEFENDER_PROG_GAME:
        break;
    case Client::CMiniGame_Defender::DEFENDER_PROG_MISSION_COMPLETE_FONT:
        break;
    case Client::CMiniGame_Defender::DEFENDER_PROG_MISSION_COMPLETE_FADEOUT:
    {
        CCamera_Manager::GetInstance()->Start_FadeOut();
        m_pDefenderPlayer->Set_BlockPlayerInput(true);
        break;
    }
    case Client::CMiniGame_Defender::DEFENDER_PROG_MISSION_COMPLETE_FADEIN:
    {
        CCamera_Manager::GetInstance()->Start_FadeIn();
        m_pSidePersonUI->Set_Active(false);
        m_pDefenderPlayer->Set_Position(m_vStart_Position);
		
        for (_uint i = DEFENDER_PART_COUNTER_BACK; i < DEFENDER_PART_LAST; i++)
        {
            m_PartObjects[i]->Set_Active(false);
        }
        break;
    }
    case Client::CMiniGame_Defender::DEFENDER_PROG_TRANSFORM_OUT:
    {
        m_pDefenderPlayer->Switch_Animation(CDefenderPlayer::ANIM_STATE_CYBERJOT2D::CYBER2D_TRANSFORM_OUT);
		m_pDefenderPlayer->Set_Direction(T_DIRECTION::RIGHT);
        break;
    }
    case Client::CMiniGame_Defender::DEFENDER_PROG_ENDING_DIALOG:
    {
        CPlayerData_Manager* pPDM = CPlayerData_Manager::GetInstance();
        CPlayer* pNormalPlayer = pPDM->Get_NormalPlayer_Ptr();
        pNormalPlayer->Set_Active(true);
        m_pDefenderPlayer->Set_Active(false);
        pPDM->Set_CurrentPlayer(PLAYABLE_ID::NORMAL);
        pNormalPlayer->Set_Mode(CPlayer::PLAYER_MODE_ZETPACK);
        pNormalPlayer->Set_State(CPlayer::IDLE);
        pNormalPlayer->Set_BlockPlayerInput(true);
        pNormalPlayer->Set_Position(m_vStart_Position);
        pNormalPlayer->Set_2DDirection(E_DIRECTION::RIGHT);

        static_cast<CCamera_2D*>(CCamera_Manager::GetInstance()->Get_CurrentCamera())->Change_Target(this);
        //DIALOG
        m_pDialogManager->Set_DialogId(TEXT("DEFENDER_Dialogue_02"), m_strSectionName.c_str());

        _vector vPos = Get_FinalPosition();
        _float3 vPosition; XMStoreFloat3(&vPosition, vPos);
        m_pDialogManager->Set_DialoguePos(vPosition);
        m_pDialogManager->Set_DisPlayDialogue(true);
        break;
    }
    case Client::CMiniGame_Defender::DEFENDER_PROG_REWARDING:
    {


        //아빠머ㅏ리 만들기
        CFatherPart_Prop::FATHERPART_PROP_DESC Desc{};
        Desc.iCurLevelID = m_iCurLevelID;
        Desc.iFatherPartID = CFatherGame::FATER_HEAD;
        Desc.Build_2D_Transform(_float2(-1685.0, -229.0), _float2(150.0f, 150.0f));
        CGameObject* pGameObject = nullptr;
        if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHAPTER_6, TEXT("Prototype_GameObject_FatherPart_Prop"), LEVEL_CHAPTER_6, TEXT("Layer_FatherPart_Prop"), &pGameObject, &Desc)))
            assert(nullptr);

        CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(m_strSectionName, pGameObject, SECTION_2D_PLAYMAP_TRIGGER);
        static_cast<CCamera_2D*>(CCamera_Manager::GetInstance()->Get_CurrentCamera())->Change_Target(pGameObject);
        break;
    }
    case Client::CMiniGame_Defender::DEFENDER_PROG_CLEAR:
    {
        END_BGM();
        START_BGM(TEXT("LCD_MUS_C06_SPACE_DESK_SKETCHSPACE_FULL"), g_BGMVolume * 1.3f);
        //포탈 켜기
        CPortal* pTargetPortal = static_cast<CPortal_Default*>(static_cast<CSection_2D_PlayMap*>(CSection_Manager::GetInstance()->Find_Section(m_strSectionName))->Get_Portal(0));
        if (pTargetPortal)
            pTargetPortal->Set_Active(true);

        //플레이어 바라보고 움직일 수 있게 하기
        CPlayerData_Manager* pPDM = CPlayerData_Manager::GetInstance();
        CPlayer* pNormalPlayer = pPDM->Get_NormalPlayer_Ptr();
        static_cast<CCamera_2D*>(CCamera_Manager::GetInstance()->Get_CurrentCamera())->Change_Target(pNormalPlayer);
        pNormalPlayer->Set_BlockPlayerInput(false);

        CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(m_strSectionName, pNormalPlayer, SECTION_2D_PLAYMAP_TRIGGER);

        break;
    }
    default:
        break;
    }
}
HRESULT CMiniGame_Defender::Ready_ControllTower()
{
	CModelObject::MODELOBJECT_DESC tModelDesc = {};
    tModelDesc.iObjectGroupID = OBJECT_GROUP::MAPOBJECT;
    tModelDesc.eStartCoord = COORDINATE_2D;
    tModelDesc.isCoordChangeEnable = false;
    tModelDesc.tTransform2DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
    tModelDesc.iModelPrototypeLevelID_2D = m_iCurLevelID;
    tModelDesc.strModelPrototypeTag_2D = TEXT("DefenderBase_Sprite");
    tModelDesc.strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
    tModelDesc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
    tModelDesc.iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;
    m_PartObjects[DEFENDER_PART_TOWER] = static_cast<CModelObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &tModelDesc));
    if (nullptr == m_PartObjects[DEFENDER_PART_TOWER])
        return E_FAIL;

    CModelObject::MODELOBJECT_DESC tModelDesc2 = {};
    tModelDesc2.iObjectGroupID = OBJECT_GROUP::MAPOBJECT;
    tModelDesc2.eStartCoord = COORDINATE_2D;
    tModelDesc2.isCoordChangeEnable = false;
    tModelDesc2.tTransform2DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
    tModelDesc2.iModelPrototypeLevelID_2D = m_iCurLevelID;
    tModelDesc2.strModelPrototypeTag_2D = TEXT("DefenderCounter_Sprite");
    tModelDesc2.strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
    tModelDesc2.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
    tModelDesc2.iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;
    m_PartObjects[DEFENDER_PART_COUNTER_BACK] = static_cast<CModelObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &tModelDesc2));
    if (nullptr == m_PartObjects[DEFENDER_PART_COUNTER_BACK])
        return E_FAIL;
    m_PartObjects[DEFENDER_PART_COUNTER_BACK]->Set_Active(false);
    m_PartObjects[DEFENDER_PART_COUNTER_BACK]->Set_Position(_vector{0.f,100.f,0.f});

    for (_uint i = 0; i < 10; i++)
    {
        CModelObject::MODELOBJECT_DESC tModelDesc3 = {};
        tModelDesc3.iObjectGroupID = OBJECT_GROUP::MAPOBJECT;
        tModelDesc3.eStartCoord = COORDINATE_2D;
        tModelDesc3.isCoordChangeEnable = false;
        tModelDesc3.tTransform2DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
        tModelDesc3.iModelPrototypeLevelID_2D = m_iCurLevelID;
        tModelDesc3.strModelPrototypeTag_2D = TEXT("DefenderCounter_");
		tModelDesc3.strModelPrototypeTag_2D += to_wstring(i);
		tModelDesc3.strModelPrototypeTag_2D += TEXT("_Sprite");
        tModelDesc3.strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
        tModelDesc3.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
        tModelDesc3.iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;
        m_PartObjects[DEFENDER_PART_COUNTER_0 + i] = static_cast<CModelObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &tModelDesc3));
        if (nullptr == m_PartObjects[DEFENDER_PART_COUNTER_0 + i])
            return E_FAIL;
        m_PartObjects[DEFENDER_PART_COUNTER_0 + i]->Set_Active(false);
        m_PartObjects[DEFENDER_PART_COUNTER_0 + i]->Set_Position(_vector{ 0.f, 100.f,0.f });

    }


    return S_OK;
}
HRESULT CMiniGame_Defender::Ready_Spanwer()
{
    Pooling_DESC Pooling_Desc;
    Pooling_Desc.iPrototypeLevelID = m_iCurLevelID;
    Pooling_Desc.strLayerTag = TEXT("Layer_Defender");
    Pooling_Desc.eSection2DRenderGroup = SECTION_2D_PLAYMAP_OBJECT;

    for (auto& pairMonster : m_mapMonsterPrototypeTag)
    {
        Pooling_Desc.strPrototypeTag = pairMonster.second;
        CDefenderMonster::DEFENDER_MONSTER_DESC* pMonsterDesc = new CDefenderMonster::DEFENDER_MONSTER_DESC;
        pMonsterDesc->iCurLevelID = m_iCurLevelID;
		_wstring strPoolTag = pairMonster.second + TEXT("_Pool");
        CPooling_Manager::GetInstance()->Register_PoolingObject(strPoolTag, Pooling_Desc, pMonsterDesc);


        CDefenderSpawner::DEFENDER_SPAWNER_DESC tDesc = {};
        tDesc.iCurLevelID = m_iCurLevelID;
        tDesc.strSectionName = m_strSectionName;
        tDesc.strPoolTag = strPoolTag;
        tDesc.pPlayer = m_pDefenderPlayer;

        m_Spawners[pairMonster.first] = static_cast<CDefenderSpawner*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, m_iCurLevelID, TEXT("Prototype_GameObject_DefenderSpawner"), &tDesc));
        Safe_AddRef(m_Spawners[pairMonster.first]);
		m_pGameInstance->Add_GameObject_ToLayer(m_iCurLevelID, TEXT("Layer_Defender"), m_Spawners[pairMonster.first]);
    }


    //MONSTER SPAWNS================================================================================================================
	SPAWN_DESC tSpawnDesc = {};

    tSpawnDesc.ePattern = SPAWN_PATTERN::SPAWN_PATTERN_RANDOM;
    tSpawnDesc.eDirection = T_DIRECTION::LEFT;
    tSpawnDesc.fAutoCycleTime = 13.f;//13
    tSpawnDesc.fUnitDelay = 0.f;
    tSpawnDesc.iSpawnCount = 6;//6
    tSpawnDesc.fMoveSpeed = 200.f;
    tSpawnDesc.bAbsolutePosition = false;
    tSpawnDesc.fPlayerDistance = m_fSpawnDistance;
    tSpawnDesc.fHeight = 0;
    m_Spawners[DEFENDER_MONSTER_ID::SM_SHIP]->Add_Spawn(tSpawnDesc);

	tSpawnDesc.ePattern = SPAWN_PATTERN::SPAWN_PATTERN_DOT;
	tSpawnDesc.eDirection = T_DIRECTION::RIGHT;
	tSpawnDesc.fPatternStartDelay = 3.5f;
	tSpawnDesc.fAutoCycleTime = 15.f;//15
	tSpawnDesc.fUnitDelay = 0.5f;
    tSpawnDesc.iSpawnCount = 5;
    tSpawnDesc.fMoveSpeed = 300.f;
    tSpawnDesc.bAbsolutePosition = false;
	tSpawnDesc.fPlayerDistance = m_fSpawnDistance;
	tSpawnDesc.fHeight = 0;
    m_Spawners[DEFENDER_MONSTER_ID::MED_SHIP_UP]->Add_Spawn(tSpawnDesc);
    m_Spawners[DEFENDER_MONSTER_ID::MED_SHIP_DOWN]->Add_Spawn(tSpawnDesc);

    tSpawnDesc.ePattern = SPAWN_PATTERN::SPAWN_PATTERN_ARROW;
    tSpawnDesc.eDirection = T_DIRECTION::LEFT;
    tSpawnDesc.fPatternStartDelay = 7.f;//7
    tSpawnDesc.fAutoCycleTime = 13.f;//13
    tSpawnDesc.fUnitDelay = 0.1f;
    tSpawnDesc.iSpawnCount = 5;
    tSpawnDesc.fMoveSpeed = 200.f;
    tSpawnDesc.bAbsolutePosition = false;
    tSpawnDesc.fPlayerDistance = m_fSpawnDistance* 0.7f;
    tSpawnDesc.fHeight = 0;
    m_Spawners[DEFENDER_MONSTER_ID::SM_SHIP]->Add_Spawn(tSpawnDesc);
    tSpawnDesc.eDirection = T_DIRECTION::RIGHT;
    m_Spawners[DEFENDER_MONSTER_ID::SM_SHIP]->Add_Spawn(tSpawnDesc);

    tSpawnDesc.ePattern = SPAWN_PATTERN::SPAWN_PATTERN_DOT;
    tSpawnDesc.eDirection = T_DIRECTION::LEFT;
    tSpawnDesc.fPatternStartDelay = 10.5f;
    tSpawnDesc.fAutoCycleTime = 15.f;
    tSpawnDesc.fUnitDelay = 0.5f;
    tSpawnDesc.iSpawnCount = 5;
    tSpawnDesc.fMoveSpeed = 300.f;
    tSpawnDesc.bAbsolutePosition = false;
    tSpawnDesc.fPlayerDistance = m_fSpawnDistance;
    tSpawnDesc.fHeight = 0;
    m_Spawners[DEFENDER_MONSTER_ID::MED_SHIP_UP]->Add_Spawn(tSpawnDesc);
    m_Spawners[DEFENDER_MONSTER_ID::MED_SHIP_DOWN]->Add_Spawn(tSpawnDesc);

    tSpawnDesc.ePattern = SPAWN_PATTERN::SPAWN_PATTERN_ARROW;
    tSpawnDesc.eDirection = T_DIRECTION::RIGHT;
    tSpawnDesc.fPatternStartDelay = 13.f;
    tSpawnDesc.fAutoCycleTime = 13.f;
    tSpawnDesc.fUnitDelay = 0.1f;
    tSpawnDesc.iSpawnCount = 5;
    tSpawnDesc.fMoveSpeed = 200.f;
    tSpawnDesc.bAbsolutePosition = false;
    tSpawnDesc.fPlayerDistance = m_fSpawnDistance * 0.5f;
    tSpawnDesc.fHeight = 0;
    m_Spawners[DEFENDER_MONSTER_ID::SM_SHIP]->Add_Spawn(tSpawnDesc);

    return S_OK;
}
HRESULT CMiniGame_Defender::Ready_UI()
{

    CScrollModelObject::SCROLLMODELOBJ_DESC tScrollDesc = {};
    tScrollDesc.eStartCoord = COORDINATE_2D;
    tScrollDesc.iCurLevelID = m_iCurLevelID;
    tScrollDesc.isCoordChangeEnable = false;
    tScrollDesc.iModelPrototypeLevelID_2D = m_iCurLevelID;
    tScrollDesc.strModelPrototypeTag_2D = TEXT("icon_scientist_Sprite");
    tScrollDesc.strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
    tScrollDesc.iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;
    tScrollDesc.tTransform2DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    tScrollDesc.tTransform2DDesc.vInitialScaling = _float3(1, 1, 1);

    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_ScrollModelObject"), m_iCurLevelID, TEXT("Layer_Defender"), (CGameObject**)&m_pSidePersonUI, &tScrollDesc)))
        return E_FAIL;
    Safe_AddRef(m_pSidePersonUI);
    m_pSidePersonUI->Set_Active(false);
    CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(m_strSectionName, m_pSidePersonUI, SECTION_2D_PLAYMAP_UI);

    return S_OK;
}
void CMiniGame_Defender::Enter_Section(const _wstring _strIncludeSectionName)
{
    __super::Enter_Section(_strIncludeSectionName);
    m_vSectionSize = m_pSectionManager->Get_Section_RenderTarget_Size(m_strSectionName);
    if (FAILED(Ready_UI()))
        return;

}
_vector CMiniGame_Defender::Get_SidePosition(T_DIRECTION _eDirection)
{
    _vector vPlayerPos = m_pDefenderPlayer->Get_FinalPosition();
    _vector vSidePos = vPlayerPos + _vector{T_DIRECTION::LEFT == _eDirection ? -m_fSideUIDIstance : m_fSideUIDIstance ,0.f,0.f};
	vSidePos = XMVectorSetY(vSidePos, m_fCenterHeight);
    return Get_ScrolledPosition(vSidePos);
}
_vector CMiniGame_Defender::Get_ScrolledPosition(_vector _vPosition)
{

    _float fDefaultWitdh = (m_vSectionSize.x * 0.5f);

    if (-fDefaultWitdh > _vPosition.m128_f32[0])
    {
        _vPosition = XMVectorSetX(_vPosition, _vPosition.m128_f32[0] + m_vSectionSize.x);
    }
    if (fDefaultWitdh < _vPosition.m128_f32[0])
    {
        _vPosition = XMVectorSetX(_vPosition, _vPosition.m128_f32[0] - m_vSectionSize.x);
    }
    return _vPosition;
}
_bool CMiniGame_Defender::Is_LeftSide(_vector _vPos)
{
	//평면 상에서 vPos가 플레이어보다 왼쪽에 있으면
	//PlayerX - PosX 거리와 SectionSizeX -PlayerX + PosX 거리를 비교해서
    //전자가 짧으면 LEFT
    //평면 상에서 vPos가 플레이어보다 오른쪽에 있으면
	//PlayerX - PosX 거리와 PlayerX + SectionSizeX - PosX 거리를 비교해서
	_vector vPlayerPos = m_pDefenderPlayer->Get_FinalPosition();
	_float fDistance = abs(vPlayerPos.m128_f32[0] - _vPos.m128_f32[0]);
	_float fRightScreenDistance = Get_RightScreenDistance(_vPos);
	_float fLeftScreenDistance = Get_LeftScreenDistance(_vPos);
    //일단 오른쪾에 있으면?
	if (vPlayerPos.m128_f32[0] < _vPos.m128_f32[0])
	{
        if (fDistance < Get_LeftScreenDistance(vPlayerPos) + Get_RightScreenDistance(_vPos))
			return false;
		else
			return true;
	}
    else
    {
        if (fDistance < Get_LeftScreenDistance(_vPos) + Get_RightScreenDistance(vPlayerPos))
            return true;
        else
            return false;
    }
}
_bool CMiniGame_Defender::Is_InsideScreen(_vector _vPos)
{
    _float fDistance = Get_ScrolledDistance(_vPos);
	if (fDistance > m_fHalfScreenRange)
		return false;
	else
        return true;
}
_float CMiniGame_Defender::Get_ScrolledDistance(_vector _vPos)
{
    _vector vPlayerPos = m_pDefenderPlayer->Get_FinalPosition();
    _float fDistance = abs(vPlayerPos.m128_f32[0] - _vPos.m128_f32[0]);
    _float fOverDistance = 0.f;

    //오른쪾에있으미ㅕㄴ
    if (vPlayerPos.m128_f32[0] < _vPos.m128_f32[0])
        fOverDistance = Get_RightScreenDistance(_vPos) + Get_LeftScreenDistance(vPlayerPos);
    else
        fOverDistance = Get_RightScreenDistance(vPlayerPos) + Get_LeftScreenDistance(_vPos);

    return  min(fDistance, fOverDistance);
}
_float CMiniGame_Defender::Get_RightScreenDistance(_vector _vPos)
{
    return m_vSectionSize.x*0.5f - _vPos.m128_f32[0];
}
_float CMiniGame_Defender::Get_LeftScreenDistance(_vector _vPos)
{
    return _vPos.m128_f32[0] + m_vSectionSize.x * 0.5f;
}
void CMiniGame_Defender::Set_LeftPersonCount(_uint _iCount)
{
    m_iPersonLeft = _iCount;
	for (_uint i = 0; i < 10; i++)
	{
		m_PartObjects[DEFENDER_PART_COUNTER_0 + i]->Set_Active(false);
	}
    m_PartObjects[DEFENDER_PART_COUNTER_0 + _iCount]->Set_Active(true);
}
void CMiniGame_Defender::On_PlayerAnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
    if (DEFENDER_PROG_TRANSFORM_OUT == m_eGameState 
        &&(_uint)CDefenderPlayer::ANIM_STATE_CYBERJOT2D::CYBER2D_TRANSFORM_OUT == iAnimIdx)
    {
        Set_GameState(DEFENDER_PROG_ENDING_DIALOG);



    }
}
void CMiniGame_Defender::Update(_float _fTimeDelta)
{

	__super::Update(_fTimeDelta);
    if(DEFENDER_PROG_ENTERED == m_eGameState)
    {
		m_fCamTargetChangeTimeAcc += _fTimeDelta;
		if (m_fCamTargetChangeTimeAcc >= m_fCamTargetChangeTime)
		{
			m_fCamTargetChangeTimeAcc = 0.f;
			Set_GameState(DEFENDER_PROG_BEGINNING_DIALOG);
		}
    }
    else if (DEFENDER_PROG_BEGINNING_DIALOG == m_eGameState
        && false == m_pDialogManager->Get_DisPlayDialogue())
    {
        Set_GameState(DEFENDER_PROG_BEGINNING_DIALOG_ENDED);
    }
    else if (DEFENDER_PROG_GAME == m_eGameState)
    {

        m_fTimeAcc += _fTimeDelta;

        if (m_fTimeAcc - m_fLastCapsuleDestroyTime >= m_fCapsuleSpawnTerm)
        {

            if (nullptr == m_pCurrentCapsule && m_iCapsuleSpawnCount < m_iMaxCapsuleSpawnCount)
            {
     
                CDefenderCapsule::DEFENDER_CAPSULE_DESC tDesc = {};
                tDesc.iCurLevelID = m_iCurLevelID;
                tDesc.iPersonCount = m_iCapsulePersonCount[m_iCapsuleSpawnCount];
				XMStoreFloat3(&tDesc.tTransform2DDesc.vInitialPosition, Get_ScrolledPosition(_vector{ m_iCapsuleSpawnCount * 800.f,0.f,0.f }));
                m_pCurrentCapsule = static_cast<CDefenderCapsule*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, m_iCurLevelID, TEXT("Prototype_GameObject_PersonCapsule"), &tDesc));
                m_pGameInstance->Add_GameObject_ToLayer(m_iCurLevelID, TEXT("Layer_Defender"), m_pCurrentCapsule);
                CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(m_strSectionName, m_pCurrentCapsule, SECTION_2D_PLAYMAP_OBJECT);

                m_iCapsuleSpawnCount++;
            }
        }
        if (m_pCurrentCapsule && false == m_pCurrentCapsule->Is_Dead())
        {
            _vector vCapsulePos = Get_ScrolledPosition(m_pCurrentCapsule->Get_FinalPosition());
            if (Is_InsideScreen(vCapsulePos))
            {
                m_pSidePersonUI->Set_Active(false);
            }
            else
            {
                m_pSidePersonUI->Set_Active(true);
                _bool bLeft = Is_LeftSide(vCapsulePos);
                _vector vUIPos = Get_SidePosition(bLeft ? T_DIRECTION::LEFT : T_DIRECTION::RIGHT);
                m_pSidePersonUI->Set_Position(vUIPos);
                m_pSidePersonUI->Set_Direction(bLeft ? T_DIRECTION::LEFT : T_DIRECTION::RIGHT);
            }
        }
        else
        {
            m_pSidePersonUI->Set_Active(false);
        }
    }
    else if (DEFENDER_PROG_MISSION_COMPLETE_FONT == m_eGameState)
    {
        m_fMissionClearFontTImeAcc += _fTimeDelta;
		if (m_fMissionClearFontTImeAcc >= m_fMissionClearFontTIme)
		{
			Set_GameState(DEFENDER_PROG_MISSION_COMPLETE_FADEOUT);

		}
    }
	else if (DEFENDER_PROG_MISSION_COMPLETE_FADEOUT == m_eGameState)
	{
        //카메라 FadeOut 끝인지 확인
        if (0.f >= static_cast<CCamera_2D*>(CCamera_Manager::GetInstance()->Get_CurrentCamera())->Get_DofBufferData().fFadeRatio)
        {
			Set_GameState(DEFENDER_PROG_MISSION_COMPLETE_FADEIN);
   
        }


	}
    else	if (DEFENDER_PROG_MISSION_COMPLETE_FADEIN == m_eGameState)
    {
        if (1.f >= static_cast<CCamera_2D*>(CCamera_Manager::GetInstance()->Get_CurrentCamera())->Get_DofBufferData().fFadeRatio)
        {
			Set_GameState(DEFENDER_PROG_TRANSFORM_OUT);
         
        }

    }
	else if (DEFENDER_PROG_TRANSFORM_OUT == m_eGameState)
	{

	}
	else if (DEFENDER_PROG_ENDING_DIALOG == m_eGameState)
	{
		if (false == m_pDialogManager->Get_DisPlayDialogue())
		{
			Set_GameState(DEFENDER_PROG_REWARDING);
        }
	}
    else if (DEFENDER_PROG_REWARDING == m_eGameState)
    {
		m_fCamTargetChangeTimeAcc += _fTimeDelta;
        if(m_fCamTargetChangeTimeAcc >= m_fCamTargetChangeTime)
        {
            m_fCamTargetChangeTimeAcc = 0.f;
            Set_GameState(DEFENDER_PROG_CLEAR);
        }
    }

	if (KEY_DOWN(KEY::Z))
	{
        if(DEFENDER_PROG_GAME == m_eGameState)
            Mission_Complete();
	}
}

void CMiniGame_Defender::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT CMiniGame_Defender::Render()
{
#ifdef _DEBUG
    if (m_p2DColliderComs[0]->Is_Active())
        m_p2DColliderComs[0]->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));
#endif
	return __super::Render();
}

void CMiniGame_Defender::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    //Entered
	if (DEFENDER_PROG_NONE == m_eGameState
        && OBJECT_GROUP::PLAYER & _pOtherObject->Get_ObjectGroupID()
        && m_p2DColliderComs[1] == _pMyCollider)
    {
		m_pPlayer = dynamic_cast<CPlayer*>(_pOtherObject);
		Safe_AddRef(m_pPlayer);
		Set_GameState(DEFENDER_PROG_ENTERED);

    }
    //GameStart
    else if (DEFENDER_PROG_BEGINNING_DIALOG_ENDED == m_eGameState
        && OBJECT_GROUP::PLAYER & _pOtherObject->Get_ObjectGroupID()
        && m_p2DColliderComs[0] == _pMyCollider)
    {
		Set_GameState(DEFENDER_PROG_TRANSFORM_IN);
    }

	if (OBJECT_GROUP::GIMMICK_OBJECT & _pOtherObject->Get_ObjectGroupID()
        && m_p2DColliderComs[0] == _pMyCollider)
	{
		CDefenderPerson* pPerson = dynamic_cast<CDefenderPerson*>(_pOtherObject);
        if(pPerson)
            Rescue_Person(pPerson);
	}
}

void CMiniGame_Defender::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CMiniGame_Defender::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CMiniGame_Defender::Start_Game()
{
    if (FAILED(Ready_Spanwer()))
        return;
    m_PartObjects[DEFENDER_PART_COUNTER_BACK]->Set_Active(true);
	Set_LeftPersonCount(m_iMaxPersonCount);
    Set_GameState(DEFENDER_PROG_GAME);
}

void CMiniGame_Defender::Restart_Game()
{
    Set_LeftPersonCount(0);
    Set_GameState(DEFENDER_PROG_GAME);
}

void CMiniGame_Defender::Rescue_Person(CDefenderPerson* _pPerson)
{
	m_iPersonLeft--;
    Set_LeftPersonCount(m_iPersonLeft);
	m_pDefenderPlayer->Remove_Follower(_pPerson);
    _pPerson->Dissapear();
    wstring strSFX = TEXT("A_sfx_humans_rescued");
    END_SFX(strSFX);
    START_SFX_DELAY(strSFX, 0.f, g_SFXVolume * 0.65f, false);
    if (m_iPersonLeft <= 0)
        Mission_Complete();
}

void CMiniGame_Defender::Mission_Complete()
{
    m_bClear = true;
    wstring strSFX = TEXT("A_sfx_resotub_mission_complete");
    END_SFX(strSFX);
    START_SFX_DELAY(strSFX, 0.f, g_SFXVolume * 0.65f, false);
    for (auto& pSpawner : m_Spawners)
    {
        pSpawner.second->Delete_Pool();
        pSpawner.second->Set_Dead();
        Safe_Release(pSpawner.second);
    }
    m_Spawners.clear();
    Set_GameState(DEFENDER_PROG_MISSION_COMPLETE_FONT);
    if(m_pMissionCompleteFont)
	    m_pMissionCompleteFont->Set_Active(true);
}



CMiniGame_Defender* CMiniGame_Defender::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CMiniGame_Defender* pInstance = new CMiniGame_Defender(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Defender_ControllTower");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMiniGame_Defender::Clone(void* _pArg)
{
	CMiniGame_Defender* pInstance = new CMiniGame_Defender(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : Defender_ControllTower");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMiniGame_Defender::Free()
{
	Safe_Release(m_pSidePersonUI);
	Safe_Release(m_pPlayer);
	Safe_Release(m_pDefenderPlayer);
    for (auto& pSpawner : m_Spawners)
    {
		Safe_Release(pSpawner.second);
    }
    m_Spawners.clear();
	__super::Free();
}

HRESULT CMiniGame_Defender::Cleanup_DeadReferences()
{
    __super::Cleanup_DeadReferences();
    if(m_pCurrentCapsule->Is_Dead())
    {
        m_pCurrentCapsule = nullptr;
        m_fLastCapsuleDestroyTime = m_fTimeAcc;
    }
    return S_OK;
}
