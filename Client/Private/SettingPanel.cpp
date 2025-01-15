#include "stdafx.h"
#include "SettingPanel.h"
#include "GameInstance.h"



CSettingPanel::CSettingPanel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CUI(_pDevice, _pContext)
{
}

CSettingPanel::CSettingPanel(const CSettingPanel& _Prototype)
	: CUI(_Prototype)
{
}

HRESULT CSettingPanel::Initialize_Prototype()
{
	if (FAILED(Ready_Components()))
		return E_FAIL;



	return S_OK;
}

HRESULT CSettingPanel::Initialize(void* _pArg)
{
	UIOBJDESC* pDesc = static_cast<UIOBJDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;



	//m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(g_iWinSizeX/ 2, g_iWinSizeY / 2, 1.f, 1.f));
	//m_pControllerTransform->Set_Scale(50.f, 50.f, 1.f);
	return S_OK;
}

void CSettingPanel::Priority_Update(_float _fTimeDelta)
{
}

void CSettingPanel::Update(_float _fTimeDelta)
{
}

void CSettingPanel::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT CSettingPanel::Render()
{

	if (FAILED(m_pControllerTransform->Bind_ShaderResource(m_pShaderComs[COORDINATE_2D], "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderComs[COORDINATE_2D], "g_DiffuseTexture")))
		return E_FAIL;


	m_pShaderComs[COORDINATE_2D]->Begin((_uint)PASS_VTXPOSTEX::DEFAULT);
	m_pVIBufferCom->Bind_BufferDesc();
	m_pVIBufferCom->Render();

	//__super::Render();


	return S_OK;
}



HRESULT CSettingPanel::Ready_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderComs[COORDINATE_2D]))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_Model_2D"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_PickBulb"),
		TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}


CSettingPanel* CSettingPanel::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CSettingPanel* pInstance = new CSettingPanel(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CSettingPanel Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CSettingPanel::Clone(void* _pArg)
{
	CSettingPanel* pInstance = new CSettingPanel(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CSettingPanel Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CSettingPanel::Free()
{

	__super::Free();

}

HRESULT CSettingPanel::Cleanup_DeadReferences()
{

	return S_OK;
}
