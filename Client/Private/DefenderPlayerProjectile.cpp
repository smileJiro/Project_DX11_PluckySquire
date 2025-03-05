#include "stdafx.h"
#include "DefenderPlayerProjectile.h"
#include "Collider_AABB.h"

CDefenderPlayerProjectile::CDefenderPlayerProjectile(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CModelObject(_pDevice, _pContext)
{
}

CDefenderPlayerProjectile::CDefenderPlayerProjectile(const CDefenderPlayerProjectile& _Prototype)
	: CModelObject(_Prototype)
{
}

HRESULT CDefenderPlayerProjectile::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CDefenderPlayerProjectile::Initialize(void* _pArg)
{
	DEFENDERPLAYER_PROJECTILE_DESC* pDesc = static_cast<DEFENDERPLAYER_PROJECTILE_DESC*>(_pArg);
	m_eTDirection = pDesc->_eTDirection;

	pDesc->eStartCoord = COORDINATE_2D;
	pDesc->iCurLevelID = m_iCurLevelID;
	pDesc->isCoordChangeEnable = false;

	pDesc->iModelPrototypeLevelID_2D = m_iCurLevelID;
	pDesc->strModelPrototypeTag_2D = TEXT("DefenderPlayerProjectile_01_Sprite");
	pDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
	pDesc->iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;
	pDesc->pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
	pDesc->tTransform2DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
	pDesc->tTransform2DDesc.vInitialScaling = _float3(1, 1, 1);
	pDesc->tTransform2DDesc.fSpeedPerSec = 1500.f;

    if(FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	m_p2DColliderComs.resize(1);
	/*BODY 2D Collider */
	CCollider_AABB::COLLIDER_AABB_DESC AABBDesc = {};
	AABBDesc.pOwner = this;
	AABBDesc.vExtents = {};
	AABBDesc.vScale = { 50.f, 24.f };
	AABBDesc.vOffsetPosition = { 0.f,0.f };
	AABBDesc.isBlock = false;
	AABBDesc.isTrigger = true;
	AABBDesc.iCollisionGroupID = OBJECT_GROUP::PLAYER;
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
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
	if (m_eTDirection == T_DIRECTION::LEFT)
		m_pControllerTransform->Go_Left(_fTimeDelta);
	else
		m_pControllerTransform->Go_Right(_fTimeDelta);
	__super::Update(_fTimeDelta);  
}

void CDefenderPlayerProjectile::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);  
}

HRESULT CDefenderPlayerProjectile::Render()
{
    return __super::Render();
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
