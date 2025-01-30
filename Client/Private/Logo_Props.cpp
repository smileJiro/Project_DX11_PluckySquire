#include "stdafx.h"
#include "Logo_Props.h"
#include "UI_Manager.h"


CLogo_Props::CLogo_Props(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CUI(_pDevice, _pContext)
{
}

CLogo_Props::CLogo_Props(const CLogo_Props& _Prototype)
	: CUI( _Prototype )
{
}

HRESULT CLogo_Props::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLogo_Props::Initialize(void* _pArg)
{
	UIOBJDESC* pDesc = static_cast<UIOBJDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

void CLogo_Props::Priority_Update(_float _fTimeDelta)
{
}

void CLogo_Props::Update(_float _fTimeDelta)
{
	isRender();
}

void CLogo_Props::Late_Update(_float _fTimeDelta)
{
	if (true == m_isRender && true == Uimgr->Get_LogoChanseStage())
		Register_RenderGroup(RENDERGROUP::RG_3D, PRIORITY_3D::PR3D_UI);

}

HRESULT CLogo_Props::Render(_int _iTextureindex, PASS_VTXPOSTEX _eShaderPass)
{	

	if (!m_isRender || false == Uimgr->Get_LogoChanseStage())
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



HRESULT CLogo_Props::Ready_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderComs[COORDINATE_2D]))))
		return E_FAIL;
	
	/* Com_VIBuffer */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_Model_2D"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;
	

	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo_Tree1"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Object/Map/tree_green_01.dds"), 1))))
	//	return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo_Tree1_ink0"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Object/Map/tree_green_ink_01.dds"), 1))))
	//	return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo_Tree1_ink1"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Object/Map/tree_green_ink_02.dds"), 1))))


	/* Com_Texture */
	if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_Texture_Logo_BG"),
		TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}


CLogo_Props* CLogo_Props::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CLogo_Props* pInstance = new CLogo_Props(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CLogo_Props Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CLogo_Props::Clone(void* _pArg)
{
	CLogo_Props* pInstance = new CLogo_Props(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CLogo_Props Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CLogo_Props::Free()
{
	
	__super::Free();
	
}

HRESULT CLogo_Props::Cleanup_DeadReferences()
{

	return S_OK;
}
