#include "pch.h"
#include "TestEmissive.h"
#include "GameInstance.h"
#include "EffectTool_Defines.h"

CTestEmissive::CTestEmissive(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CModelObject(_pDevice, _pContext)
{
}

CTestEmissive::CTestEmissive(const CModelObject& _Prototype)
	: CModelObject(_Prototype)
{
}

void CTestEmissive::Late_Update(_float _fTimeDelta)
{
	ImGui::Begin();

	ImGui::End();
	m_pGameInstance->Add_RenderObject_New(RG_3D, PR3D_EFFECT, this);
}

HRESULT CTestEmissive::Render()
{
	if (FAILED(Bind_ShaderResources_WVP()))
		return E_FAIL;

	Bind_ShaderResources();

	m_pControllerModel->Render_Default(m_pShaderComs[COORDINATE_3D], m_iShaderPasses[COORDINATE_3D]);

	return S_OK;
}

HRESULT CTestEmissive::Bind_ShaderResources()
{

	m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_vBloomColor", &m_vBloomColor, sizeof(_float4));


	return S_OK;
}

CTestEmissive* CTestEmissive::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CTestEmissive* pInstance = new CTestEmissive(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTestEmissive");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTestEmissive::Clone(void* _pArg)
{
	CTestEmissive* pInstance = new CTestEmissive(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CTestEmissive");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTestEmissive::Free()
{
	__super::Free();
}

HRESULT CTestEmissive::Cleanup_DeadReferences()
{
	return S_OK;
}
