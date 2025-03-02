#include "stdafx.h"
#include "MudPit.h"
#include "Collider_Circle.h"
#include "Collider_AABB.h"
#include "Section_Manager.h"

CMudPit::CMudPit(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CModelObject(_pDevice, _pContext)
{
}


CMudPit::CMudPit(const CMudPit& _Prototype)
	: CModelObject(_Prototype)
{
}

HRESULT CMudPit::Initialize(void* _pArg)
{
	CModelObject::MODELOBJECT_DESC* pDesc = static_cast<CModelObject::MODELOBJECT_DESC*>(_pArg);
	pDesc->iObjectGroupID = OBJECT_GROUP::MAPOBJECT;

	pDesc->Build_2D_Model(pDesc->iCurLevelID, TEXT("mudpit"), TEXT("Prototype_Component_Shader_VtxPosTex"), (_uint)PASS_VTXPOSTEX::SPRITE2D, false);
	pDesc->tTransform2DDesc.vInitialPosition = _float3(-950.0f, 500.0f, 0.0f);
	pDesc->tTransform2DDesc.vInitialScaling = _float3(1, 1, 1);

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	m_p2DColliderComs.resize(1);
	/* Test 2D Collider */
	CCollider_AABB::COLLIDER_AABB_DESC AABBDesc = {};
	AABBDesc.pOwner = this;
	AABBDesc.vExtents = {100.f ,100.f };
	AABBDesc.vScale = { 1.0f, 1.0f };
	AABBDesc.vOffsetPosition = { -AABBDesc.vExtents.x,AABBDesc.vExtents.y * 0.5f };
	AABBDesc.isBlock = false;
	AABBDesc.isTrigger = true;
	AABBDesc.iCollisionGroupID = OBJECT_GROUP::MAPOBJECT;
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Body2DCollider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &AABBDesc)))
		return E_FAIL;

	//Set_Render(false);
	return S_OK;
}

HRESULT CMudPit::Render()
{
#ifdef _DEBUG
	if (m_p2DColliderComs[0]->Is_Active())
		m_p2DColliderComs[0]->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));

#endif // _DEBUG
	return S_OK;
}



CMudPit* CMudPit::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CMudPit* pInstance = new CMudPit(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : MudPit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMudPit::Clone(void* _pArg)
{
	CMudPit* pInstance = new CMudPit(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : MudPit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMudPit::Free()
{
	__super::Free();
}
