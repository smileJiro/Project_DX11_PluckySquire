#include "stdafx.h"
#include "Bulb.h"

#include "GameInstance.h"

CBulb::CBulb(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CModelObject(_pDevice, _pContext)
{
}

CBulb::CBulb(const CBulb& _Prototype)
	: CModelObject(_Prototype)
{
}

HRESULT CBulb::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBulb::Initialize(void* _pArg)
{
	CBulb::BULB_DESC* pDesc = static_cast<CBulb::BULB_DESC*>(_pArg);

	pDesc->eStartCoord = (COORDINATE)COORDINATE_3D;
	pDesc->iCurLevelID = LEVEL_TRIGGER_TOOL;
	pDesc->isCoordChangeEnable = false;

	pDesc->strModelPrototypeTag_3D = TEXT("Latch_SkelMesh_NewRig");
	pDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
	pDesc->strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxAnimMesh");

	if (FAILED(__super::Initialize(pDesc)))
	{
		MSG_BOX("CPlayer super Initialize Failed");
		return E_FAIL;
	}

	return S_OK;
}

void CBulb::Priority_Update(_float _fTimeDelta)
{
}

void CBulb::Update(_float _fTimeDelta)
{
}

void CBulb::Late_Update(_float _fTimeDelta)
{
}

HRESULT CBulb::Render()
{
	return __super::Render();
}

CBulb* CBulb::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CBulb* pInstance = new CBulb(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBulb");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBulb::Clone(void* _pArg)
{
	CBulb* pInstance = new CBulb(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CBulb");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBulb::Free()
{
	__super::Free();
}
