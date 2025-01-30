#include "stdafx.h"
#include "Logo_Choose.h"
#include "UI_Manager.h"


CLogo_Choose::CLogo_Choose(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CUI(_pDevice, _pContext)
{
}

CLogo_Choose::CLogo_Choose(const CLogo_Choose& _Prototype)
	: CUI( _Prototype )
{
}

HRESULT CLogo_Choose::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLogo_Choose::Initialize(void* _pArg)
{
	UIOBJDESC* pDesc = static_cast<UIOBJDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

void CLogo_Choose::Priority_Update(_float _fTimeDelta)
{
}

void CLogo_Choose::Update(_float _fTimeDelta)
{
	isRender();
}

void CLogo_Choose::Late_Update(_float _fTimeDelta)
{
	if (true == m_isRender && true == Uimgr->Get_LogoChanseStage())
		Register_RenderGroup(RENDERGROUP::RG_3D, PRIORITY_3D::PR3D_UI);

}

HRESULT CLogo_Choose::Render(_int _iTextureindex, PASS_VTXPOSTEX _eShaderPass)
{	


	if (!m_isRender)
		return S_OK;

	if (FAILED(m_pControllerTransform->Bind_ShaderResource(m_pShaderComs[COORDINATE_2D], "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderComs[COORDINATE_2D], "g_DiffuseTexture", _iTextureindex)))
		return E_FAIL;

	m_pShaderComs[COORDINATE_2D]->Begin((_uint)_eShaderPass);
	m_pVIBufferCom->Bind_BufferDesc();
	m_pVIBufferCom->Render();
	

	return S_OK;
}



HRESULT CLogo_Choose::Ready_Components()
{
	//if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
	//	TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderComs[COORDINATE_2D]))))
	//	return E_FAIL;
	//
	///* Com_VIBuffer */
	//if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
	//	TEXT("Com_Model_2D"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
	//	return E_FAIL;
	//
	///* Com_Texture */
	//if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_Texture_Logo_BG"),
	//	TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	//	return E_FAIL;

	return S_OK;
}


CLogo_Choose* CLogo_Choose::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CLogo_Choose* pInstance = new CLogo_Choose(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CLogo_Choose Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CLogo_Choose::Clone(void* _pArg)
{
	CLogo_Choose* pInstance = new CLogo_Choose(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CLogo_Choose Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CLogo_Choose::Free()
{
	
	__super::Free();
	
}

HRESULT CLogo_Choose::Cleanup_DeadReferences()
{

	return S_OK;
}
