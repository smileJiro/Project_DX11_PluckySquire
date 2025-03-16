#include "stdafx.h"
#include "TiltSwapCrate.h"
#include "Collider_AABB.h"
#include "Section_Manager.h"
#include "GameInstance.h"

CTiltSwapCrate::CTiltSwapCrate(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CSlipperyObject(_pDevice, _pContext)
{
}

CTiltSwapCrate::CTiltSwapCrate(const CTiltSwapCrate& _Prototype)
	:CSlipperyObject(_Prototype)
{
}

HRESULT CTiltSwapCrate::Initialize(void* _pArg)
{
    if (nullptr == _pArg)
        return E_FAIL;

	CSlipperyObject::SLIPPERY_DESC* pDesc = static_cast<CSlipperyObject::SLIPPERY_DESC*>(_pArg);
	m_iCurLevelID = pDesc->iCurLevelID;
	pDesc->iImpactCollisionFilter = OBJECT_GROUP::BLOCKER;
	pDesc->iNumPartObjects = 1;
	pDesc->eStartCoord = COORDINATE_2D;
	pDesc->isCoordChangeEnable = false;
	pDesc->tTransform2DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
	pDesc->tTransform2DDesc.fSpeedPerSec = 750.f;
	pDesc->iObjectGroupID = OBJECT_GROUP::SLIPPERY;

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	if (FAILED(Ready_Parts()))
		return E_FAIL;


    m_p2DColliderComs.resize(2);
    //
    CCollider_AABB::COLLIDER_AABB_DESC tAABBDEsc = {};
    tAABBDEsc.pOwner = this;
    tAABBDEsc.vExtents = m_vColliderSize;
    tAABBDEsc.vScale = { 1.0f, 1.0f };
    tAABBDEsc.vOffsetPosition = { 0.f, tAABBDEsc.vExtents.y };
    tAABBDEsc.isBlock = true;
    tAABBDEsc.isTrigger = false;
    tAABBDEsc.iCollisionGroupID = OBJECT_GROUP::SLIPPERY;
	tAABBDEsc.iColliderUse = (_uint)COLLIDER2D_USE::COLLIDER2D_BODY;
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &tAABBDEsc)))
        return E_FAIL;

	CCollider_Circle::COLLIDER_CIRCLE_DESC tCircleEsc = {};
	tCircleEsc.pOwner = this;
	tCircleEsc.fRadius = 10.F;
	tCircleEsc.vScale = { 1.0f, 1.0f };
	tCircleEsc.vOffsetPosition = { 0.f, 0.f};
	tCircleEsc.isBlock = false;
	tCircleEsc.isTrigger = true;
	tCircleEsc.iCollisionGroupID = OBJECT_GROUP::SLIPPERY;
	tCircleEsc.iColliderUse = (_uint)COLLIDER2D_USE::COLLIDER2D_BODY;
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
		TEXT("Com_Collider2"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[1]), &tCircleEsc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CTiltSwapCrate::Render()
{
	__super::Render();
#ifdef _DEBUG
	if (m_p2DColliderComs[0]->Is_Active())
		m_p2DColliderComs[0]->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));
	if (m_p2DColliderComs[1]->Is_Active())
		m_p2DColliderComs[1]->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));

#endif // _DEBUG
	return S_OK;
}


HRESULT CTiltSwapCrate::Ready_Parts()
{
	CModelObject::MODELOBJECT_DESC tBodyDesc;
	tBodyDesc.eStartCoord = COORDINATE_2D;
	tBodyDesc.isCoordChangeEnable = false;
	tBodyDesc.strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
	tBodyDesc.iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;
			 
	tBodyDesc.tTransform2DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
			 
	tBodyDesc.eActorType = ACTOR_TYPE::LAST;
	tBodyDesc.pActorDesc = nullptr;
	tBodyDesc.iModelPrototypeLevelID_2D = m_iCurLevelID;
	tBodyDesc.strModelPrototypeTag_2D = TEXT("TiltSwapCrate");
	tBodyDesc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);

	m_PartObjects[PART_BODY] = static_cast<CModelObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &tBodyDesc));


	return S_OK;
}

CTiltSwapCrate* CTiltSwapCrate::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CTiltSwapCrate* pInstance = new CTiltSwapCrate(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : TiltSwapCrate");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTiltSwapCrate::Clone(void* _pArg)
{
	CTiltSwapCrate* pInstance = new CTiltSwapCrate(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : TiltSwapCrate");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTiltSwapCrate::Free()
{
	__super::Free();
}
