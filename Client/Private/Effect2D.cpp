#include "stdafx.h"
#include "Effect2D.h"

CEffect2D::CEffect2D(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CModelObject(_pDevice, _pContext)
{
}

CEffect2D::CEffect2D(const CEffect2D& _Prototype)
	:CModelObject(_Prototype)
{
}

HRESULT CEffect2D::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CEffect2D::Initialize(void* _pArg)
{
	EFFECT2D_DESC* pDesc = static_cast<EFFECT2D_DESC*>(_pArg);
	
	// Save Desc

	// Add Desc 
	pDesc->eStartCoord = COORDINATE_2D;
	pDesc->iObjectGroupID = OBJECT_GROUP::EFFECT2D;
	pDesc->isCoordChangeEnable = false;
	pDesc->iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;
	pDesc->pParentMatrices[COORDINATE_2D] = nullptr;
	pDesc->pParentMatrices[COORDINATE_3D] = nullptr;
	pDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
	
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	return S_OK;
}

void CEffect2D::Priority_Update(_float _fTimeDelta)
{

	__super::Priority_Update(_fTimeDelta);
}

void CEffect2D::Update(_float _fTimeDelta)
{


	__super::Update(_fTimeDelta);
}

void CEffect2D::Late_Update(_float _fTimeDelta)
{

	__super::Late_Update(_fTimeDelta);
}

HRESULT CEffect2D::Render()
{

	__super::Render();
	return S_OK;
}

HRESULT CEffect2D::Ready_Components(EFFECT2D_DESC* _pDesc)
{

	return S_OK;
}

CEffect2D* CEffect2D::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CEffect2D* pInstance = new CEffect2D(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed Create : CEffect2D");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect2D::Clone(void* _pArg)
{
	CEffect2D* pInstance = new CEffect2D(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed Clone : CEffect2D");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect2D::Free()
{

	__super::Free();
}
