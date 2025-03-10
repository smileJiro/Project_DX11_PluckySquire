#include "stdafx.h"
#include "Minigame_Defender.h"
#include "Collider_AABB.h"
#include "PlayerData_Manager.h"
#include "Player.h"
#include "DefenderPlayer.h"
#include "Section_Manager.h"
#include "GameInstance.h"
#include "DefenderSpawner.h"

CMiniGame_Defender::CMiniGame_Defender(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CModelObject(_pDevice, _pContext)
{
}

CMiniGame_Defender::CMiniGame_Defender(const CMiniGame_Defender& _Prototype)
	:CModelObject(_Prototype)
{
}

HRESULT CMiniGame_Defender::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CMiniGame_Defender::Initialize(void* _pArg)
{
    m_mapMonsterPrototypeTag.insert(make_pair(DEFENDER_MONSTER_ID::SM_SHIP, _wstring(L"Prototype_GameObject_DefenderSmShip")));
    m_mapMonsterPrototypeTag.insert(make_pair(DEFENDER_MONSTER_ID::TURRET, _wstring(L"Prototype_GameObject_DefenderTurret")));
    m_mapMonsterPrototypeTag.insert(make_pair(DEFENDER_MONSTER_ID::MED_SHIP_UP, _wstring(L"Prototype_GameObject_DefenderMedShip_UP")));
    m_mapMonsterPrototypeTag.insert(make_pair(DEFENDER_MONSTER_ID::MED_SHIP_DOWN, _wstring(L"Prototype_GameObject_DefenderMedShip_DOWN")));


	DEFENDER_CONTROLLTOWER_DESC* pDesc =static_cast<DEFENDER_CONTROLLTOWER_DESC*> (_pArg);
    m_iCurLevelID = pDesc->iCurLevelID;

    pDesc->iObjectGroupID = OBJECT_GROUP::MAPOBJECT;
    pDesc->eStartCoord = COORDINATE_2D;
    pDesc->isCoordChangeEnable = false;

    pDesc->iModelPrototypeLevelID_2D = m_iCurLevelID;
    pDesc->strModelPrototypeTag_2D = TEXT("DefenderBase_Sprite");
    pDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
    pDesc->iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;

    pDesc->tTransform2DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);

    pDesc->pActorDesc = nullptr;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;


    m_p2DColliderComs.resize(1);
    /* Test 2D Collider */
    CCollider_AABB::COLLIDER_AABB_DESC tAABBDesc = {};
    tAABBDesc.pOwner = this;
    tAABBDesc.vExtents = { 100.f , 300.f};
    tAABBDesc.vScale = { 1.0f, 1.0f };
    tAABBDesc.vOffsetPosition = { 0.f,0.f };
    tAABBDesc.isBlock = false;
    tAABBDesc.isTrigger = true;
    tAABBDesc.iCollisionGroupID = OBJECT_GROUP::TRIGGER_OBJECT;
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Body2DCollider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &tAABBDesc)))
        return E_FAIL;


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


    //SPAWNS================================================================================================================
	SPAWN_DESC tSpawnDesc = {};
    tSpawnDesc.ePattern = SPAWN_PATTERN::SPAWN_PATTERN_RANDOM;
    tSpawnDesc.eDirection = T_DIRECTION::LEFT;
    tSpawnDesc.fAutoCycleTime = 13.f;
    tSpawnDesc.fUnitDelay = 0.f;
    tSpawnDesc.iSpawnCount = 6;
    tSpawnDesc.bAbsolutePosition = false;
    tSpawnDesc.fPlayerDistance = m_fSpawnDistance;
    tSpawnDesc.fHeight = 0;
    m_Spawners[DEFENDER_MONSTER_ID::SM_SHIP]->Add_Spawn(tSpawnDesc);

	tSpawnDesc.ePattern = SPAWN_PATTERN::SPAWN_PATTERN_DOT;
	tSpawnDesc.eDirection = T_DIRECTION::RIGHT;
	tSpawnDesc.fPatternStartDelay = 3.5f;
	tSpawnDesc.fAutoCycleTime = 15.f;
	tSpawnDesc.fUnitDelay = 0.5f;
    tSpawnDesc.iSpawnCount = 5;
    tSpawnDesc.bAbsolutePosition = false;
	tSpawnDesc.fPlayerDistance = m_fSpawnDistance;
	tSpawnDesc.fHeight = 0;
    m_Spawners[DEFENDER_MONSTER_ID::MED_SHIP_UP]->Add_Spawn(tSpawnDesc);
    m_Spawners[DEFENDER_MONSTER_ID::MED_SHIP_DOWN]->Add_Spawn(tSpawnDesc);


    tSpawnDesc.ePattern = SPAWN_PATTERN::SPAWN_PATTERN_ARROW;
    tSpawnDesc.eDirection = T_DIRECTION::LEFT;
    tSpawnDesc.fPatternStartDelay = 7.f;
    tSpawnDesc.fAutoCycleTime = 13.f;
    tSpawnDesc.fUnitDelay = 0.1f;
    tSpawnDesc.iSpawnCount = 5;
    tSpawnDesc.bAbsolutePosition = false;
    tSpawnDesc.fPlayerDistance = m_fSpawnDistance;
    tSpawnDesc.fHeight = 0;
    m_Spawners[DEFENDER_MONSTER_ID::SM_SHIP]->Add_Spawn(tSpawnDesc);


    return S_OK;
}
void CMiniGame_Defender::Enter_Section(const _wstring _strIncludeSectionName)
{
    __super::Enter_Section(_strIncludeSectionName);

}
void CMiniGame_Defender::Update(_float _fTimeDelta)
{

	__super::Update(_fTimeDelta);
    if(false == m_bGameStart)
		return;
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
    if (false == m_bGameStart && OBJECT_GROUP::PLAYER & _pOtherObject->Get_ObjectGroupID())
    {
        m_bGameStart = true;
        CPlayerData_Manager* pPDM = CPlayerData_Manager::GetInstance();
        CPlayer* pNormalPlayer =  pPDM->Get_NormalPlayer_Ptr();
        m_pDefenderPlayer =  pPDM->Get_DefenderPlayer_Ptr();
		Safe_AddRef(m_pDefenderPlayer);

        pNormalPlayer->Set_Active(false);
        m_pDefenderPlayer->Set_Active(true);

        _vector vNormalPlayerPos = pNormalPlayer->Get_FinalPosition();
        m_pDefenderPlayer->Set_Position(vNormalPlayerPos);
		m_pDefenderPlayer->Start_Transform();

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
}

void CMiniGame_Defender::Restart_Game()
{
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
	Safe_Release(m_pDefenderPlayer);
    for (auto& pSpawner : m_Spawners)
    {
		Safe_Release(pSpawner.second);
    }
    m_Spawners.clear();
	__super::Free();
}
