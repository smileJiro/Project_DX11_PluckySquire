#include "stdafx.h"
#include "Minigame_Defender.h"
#include "Collider_AABB.h"
#include "PlayerData_Manager.h"
#include "Player.h"
#include "DefenderPlayer.h"
#include "Section_Manager.h"

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

void CMiniGame_Defender::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
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
        CDefenderPlayer* pDefenderPlayer =  pPDM->Get_DefenderPlayer_Ptr();

        _vector vNormalPlayerPos = pNormalPlayer->Get_FinalPosition();
        pDefenderPlayer->Set_Position(vNormalPlayerPos);

        pDefenderPlayer->Start_Game();
    }
}

void CMiniGame_Defender::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CMiniGame_Defender::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
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
	__super::Free();
}
