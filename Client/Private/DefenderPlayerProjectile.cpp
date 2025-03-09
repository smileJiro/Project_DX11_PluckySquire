#include "stdafx.h"
#include "DefenderPlayerProjectile.h"
#include "Collider_AABB.h"
#include "Section_Manager.h"
#include "Character.h"

CDefenderPlayerProjectile::CDefenderPlayerProjectile(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CScrollModelObject(_pDevice, _pContext)
{
}

CDefenderPlayerProjectile::CDefenderPlayerProjectile(const CDefenderPlayerProjectile& _Prototype)
	: CScrollModelObject(_Prototype)
{
}

HRESULT CDefenderPlayerProjectile::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CDefenderPlayerProjectile::Initialize(void* _pArg)
{
	DEFENDERPLAYER_PROJECTILE_DESC* pDesc = static_cast<DEFENDERPLAYER_PROJECTILE_DESC*>(_pArg);
	pDesc->iObjectGroupID = OBJECT_GROUP::PLAYER_PROJECTILE;
	m_iCurLevelID = pDesc->iCurLevelID;
	pDesc->eStartCoord = COORDINATE_2D;
	pDesc->isCoordChangeEnable = false;

	pDesc->iModelPrototypeLevelID_2D = m_iCurLevelID;
	pDesc->strModelPrototypeTag_2D = TEXT("DefenderPlayerProjectile_01_Sprite");
	pDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
	pDesc->iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;
	pDesc->tTransform2DDesc.vInitialScaling = _float3(1, 1, 1);
	pDesc->tTransform2DDesc.fSpeedPerSec = 1500.f;

	//pDesc->iModelPrototypeLevelID_3D = m_iCurLevelID;
	//pDesc->strModelPrototypeTag_3D = TEXT("DefenderPlayerProjectile_01_Sprite");
	//pDesc->strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
	//pDesc->iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;
	//pDesc->tTransform2DDesc.vInitialScaling = _float3(1, 1, 1);
	//pDesc->tTransform2DDesc.fSpeedPerSec = 10.f;
	//pDesc->iRenderGroupID_3D = RG_3D;
	//pDesc->iPriorityID_3D = PR3D_GEOMETRY;
    if(FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	m_p2DColliderComs.resize(1);
	/*BODY 2D Collider */
	CCollider_AABB::COLLIDER_AABB_DESC AABBDesc = {};
	AABBDesc.pOwner = this;
	AABBDesc.vExtents = { 30.f, 18.f };
	AABBDesc.vOffsetPosition = { 0.f,0.f };
	AABBDesc.isBlock = false;
	AABBDesc.isTrigger = true;
	AABBDesc.iCollisionGroupID = OBJECT_GROUP::PLAYER_PROJECTILE;
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Body2DCollider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &AABBDesc)))
		return E_FAIL;

    return S_OK;
}

void CDefenderPlayerProjectile::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);  
}

void CDefenderPlayerProjectile::Update(_float _fTimeDelta)
{
	m_pControllerTransform->Go_Right(_fTimeDelta);
	m_fLifeTimeAcc += _fTimeDelta;
	if (m_fLifeTimeAcc >= m_fLifeTime)
	{
		Event_DeleteObject(this);
		m_fLifeTimeAcc = 0.f;
	}
	__super::Update(_fTimeDelta);  

	
}

void CDefenderPlayerProjectile::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);  
}

HRESULT CDefenderPlayerProjectile::Render()
{
//#ifdef _DEBUG
//	if (m_p2DColliderComs[0]->Is_Active())
//		m_p2DColliderComs[0]->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));
//#endif // _DEBUG
    return __super::Render();
}

void CDefenderPlayerProjectile::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	if (OBJECT_GROUP::MONSTER == _pOtherCollider->Get_CollisionGroupID())
	{
		Event_Hit(this, static_cast<CCharacter*>(_pOtherObject), 1, _vector{0.f,0.f,0.f});
		Event_DeleteObject(this);
	}
}

void CDefenderPlayerProjectile::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CDefenderPlayerProjectile::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

CDefenderPlayerProjectile* CDefenderPlayerProjectile::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CDefenderPlayerProjectile* pInstance = new CDefenderPlayerProjectile(_pDevice, _pContext);
	if (nullptr == pInstance)
	{
		MSG_BOX("Failed to Create CDefenderPlayerProjectile");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CDefenderPlayerProjectile::Clone(void* _pArg)
{
	CGameObject* pInstance = new CDefenderPlayerProjectile(*this);
	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Clone CDefenderPlayerProjectile");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDefenderPlayerProjectile::Free()
{
	__super::Free();
}
