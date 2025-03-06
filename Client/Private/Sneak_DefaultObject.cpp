#include "stdafx.h"
#include "Sneak_DefaultObject.h"
#include "GameInstance.h"

CSneak_DefaultObject::CSneak_DefaultObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CModelObject(_pDevice, _pContext)
{
}

CSneak_DefaultObject::CSneak_DefaultObject(const CSneak_DefaultObject& _Prototype)
	: CModelObject(_Prototype)
{
}

HRESULT CSneak_DefaultObject::Initialize(void* _pArg)
{
	CModelObject::MODELOBJECT_DESC* pDesc = static_cast<CModelObject::MODELOBJECT_DESC*>(_pArg);

	pDesc->eStartCoord = COORDINATE_2D;
	pDesc->isCoordChangeEnable = false;
	pDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
	pDesc->iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;

	pDesc->tTransform2DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);

	pDesc->eActorType = ACTOR_TYPE::LAST;
	pDesc->pActorDesc = nullptr;
	pDesc->iModelPrototypeLevelID_2D = pDesc->iCurLevelID;


	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	return S_OK;
}

CSneak_DefaultObject* CSneak_DefaultObject::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CSneak_DefaultObject* pInstance = new CSneak_DefaultObject(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSneak_DefaultObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSneak_DefaultObject::Clone(void* _pArg)
{
	CSneak_DefaultObject* pInstance = new CSneak_DefaultObject(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CSneak_DefaultObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSneak_DefaultObject::Free()
{
	__super::Free();
}
