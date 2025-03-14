#include "stdafx.h"
#include "Npc_Humgrump.h"

CNpc_Humgrump::CNpc_Humgrump(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CModelObject(_pDevice, _pContext)
{
}

CNpc_Humgrump::CNpc_Humgrump(const CNpc_Humgrump& _Prototype)
    : CModelObject(_Prototype)
{
}

HRESULT CNpc_Humgrump::Initialize(void* _pArg)
{
	if (nullptr == _pArg)
		return E_FAIL;

	MODELOBJECT_DESC* pDesc = static_cast<MODELOBJECT_DESC*>(_pArg);

	pDesc->eStartCoord = COORDINATE_2D;
	pDesc->isCoordChangeEnable = false;
	pDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
	pDesc->iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;

	pDesc->tTransform2DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);

	pDesc->iModelPrototypeLevelID_2D = LEVEL_CHAPTER_4;
	pDesc->strModelPrototypeTag_2D = TEXT("Humgrump");
	//pDesc->iObjectGroupID = OBJECT_GROUP::NP;

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

    return S_OK;
}

void CNpc_Humgrump::Update(_float _fTimeDelta)
{
    __super::Update(_fTimeDelta);
}

HRESULT CNpc_Humgrump::Render()
{
	__super::Render();
#ifdef _DEBUG
	//for (auto& p2DCollider : m_p2DColliderComs)
	//{
	//	p2DCollider->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));
	//}
#endif // _DEBUG

	return S_OK;
}

CNpc_Humgrump* CNpc_Humgrump::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CNpc_Humgrump* pInstance = new CNpc_Humgrump(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CNpc_Humgrump");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CNpc_Humgrump::Clone(void* _pArg)
{
	CNpc_Humgrump* pInstance = new CNpc_Humgrump(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CNpc_Humgrump");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNpc_Humgrump::Free()
{
	__super::Free();
}
