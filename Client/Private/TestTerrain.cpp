#include "stdafx.h"
#include "TestTerrain.h"
#include "GameInstance.h"

CTestTerrain::CTestTerrain(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CModelObject(_pDevice, _pContext)
{
}

CTestTerrain::CTestTerrain(const CTestTerrain& _Prototype)
	:CModelObject(_Prototype)
{
}

HRESULT CTestTerrain::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTestTerrain::Initialize(void* _pArg)
{
	MODELOBJECT_DESC* pDesc = static_cast<MODELOBJECT_DESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CTestTerrain::Priority_Update(_float _fTimeDelta)
{

	CPartObject::Priority_Update(_fTimeDelta);
}

void CTestTerrain::Update(_float _fTimeDelta)
{

	/* Update Parent Matrix */
	CPartObject::Update(_fTimeDelta);
}

void CTestTerrain::Late_Update(_float _fTimeDelta)
{

	/* Add Render Group */
	if (COORDINATE_3D == m_pControllerTransform->Get_CurCoord())
		m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	else if (COORDINATE_2D == m_pControllerTransform->Get_CurCoord())
		m_pGameInstance->Add_RenderObject(CRenderer::RG_EFFECT, this);

	/* Update Parent Matrix */
	CPartObject::Late_Update(_fTimeDelta);
}


HRESULT CTestTerrain::Render_Shadow()
{
	return S_OK;
}

HRESULT CTestTerrain::Ready_Components()
{
	return S_OK;
}

CTestTerrain* CTestTerrain::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CTestTerrain* pInstance = new CTestTerrain(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTestTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTestTerrain::Clone(void* _pArg)
{

	CTestTerrain* pInstance = new CTestTerrain(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CTestTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTestTerrain::Free()
{

	__super::Free();
}
