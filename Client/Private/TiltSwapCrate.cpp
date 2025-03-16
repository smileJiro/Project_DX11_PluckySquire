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

	TILTSWAPCRATE_DESC* pDesc = static_cast<TILTSWAPCRATE_DESC*>(_pArg);
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


    m_p2DColliderComs.resize(1);
    //
    CCollider_AABB::COLLIDER_AABB_DESC tAABBDEsc = {};
    tAABBDEsc.pOwner = this;
    tAABBDEsc.vExtents = m_vColliderHalfSize;
    tAABBDEsc.vScale = { 1.0f, 1.0f };
    tAABBDEsc.vOffsetPosition = { 0.f, tAABBDEsc.vExtents.y };
    tAABBDEsc.isBlock = true;
    tAABBDEsc.isTrigger = false;
    tAABBDEsc.iCollisionGroupID = OBJECT_GROUP::SLIPPERY;
	tAABBDEsc.iColliderUse = (_uint)COLLIDER2D_USE::COLLIDER2D_BODY;
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &tAABBDEsc)))
        return E_FAIL;

	//CCollider_Circle::COLLIDER_CIRCLE_DESC tCircleEsc = {};
	//tCircleEsc.pOwner = this;
	//tCircleEsc.fRadius = 10.f;
	//tCircleEsc.vScale = { 1.0f, 1.0f };
	//tCircleEsc.vOffsetPosition = { 0.f, 0.f};
	//tCircleEsc.isBlock = false;
	//tCircleEsc.isTrigger = true;
	//tCircleEsc.iCollisionGroupID = OBJECT_GROUP::EXPLOSION;
	//tCircleEsc.iColliderUse = (_uint)COLLIDER2D_USE::COLLIDER2D_BODY;
	//if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
	//	TEXT("Com_Collider2"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[1]), &tCircleEsc)))
	//	return E_FAIL;

	Set_FlipState(pDesc->eFlipState);
	return S_OK;
}

HRESULT CTiltSwapCrate::Render()
{
	__super::Render();
#ifdef _DEBUG
	for (auto& pCollider : m_p2DColliderComs)
	{
		if(pCollider && pCollider->Is_Active())
			pCollider->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));
	}

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

	m_PartObjects[PART_BODY] = m_pBody = static_cast<CModelObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &tBodyDesc));
	Safe_AddRef(m_pBody);

	return S_OK;
}

_vector CTiltSwapCrate::Get_CenterPos()
{
	_vector vResult = Get_FinalPosition();
	vResult.m128_f32[1] += m_vColliderHalfSize.y;
	return vResult;
}

void CTiltSwapCrate::Set_Position_ByCenter(_fvector _vCenterPos)
{
	_vector vPos =_vCenterPos;
	vPos.m128_f32[1] -= m_vColliderHalfSize.y;
	Set_Position(vPos);
}

void CTiltSwapCrate::Flip()
{
	Set_FlipState(m_eFlipState == FLIP_BOTTOM? FLIP_TOP : FLIP_BOTTOM);
}

void CTiltSwapCrate::Set_FlipState(FLIP_STATE _eState)
{
	if (m_eFlipState == _eState)
		return;
	m_eFlipState = _eState;
	m_pBody->Switch_Animation(m_eFlipState);

}

void CTiltSwapCrate::Decalcomani(_bool _bXAxis)
{
	Stop_Slip();
	Flip();

	_vector vPos = Get_CenterPos();
	if (_bXAxis)
		Set_Position_ByCenter(XMVectorSetX( vPos, XMVectorGetX(vPos) * -1.f));
	else
		Set_Position_ByCenter(XMVectorSetY( vPos, XMVectorGetY(vPos) * -1.f));

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
	Safe_Release(m_pBody);
	__super::Free();
}
