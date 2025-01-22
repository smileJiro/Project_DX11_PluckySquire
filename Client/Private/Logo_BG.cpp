#include "stdafx.h"
#include "Logo_BG.h"
#include "GameInstance.h"



CLogo_BG::CLogo_BG(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CUI (_pDevice, _pContext)
{
}

CLogo_BG::CLogo_BG(const CLogo_BG& _Prototype)
	: CUI ( _Prototype )
{
}

HRESULT CLogo_BG::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLogo_BG::Initialize(void* _pArg)
{
	UIOBJDESC* pDesc = static_cast<UIOBJDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_vColor = { 227.f / 255.f , 37.f / 255.f,82.f / 255.f, 1.f };

	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

void CLogo_BG::Priority_Update(_float _fTimeDelta)
{
}

void CLogo_BG::Update(_float _fTimeDelta)
{
	
}

void CLogo_BG::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT CLogo_BG::Render()
{	




	if (FAILED(m_pControllerTransform->Bind_ShaderResource(m_pShaderComs[COORDINATE_2D], "g_WorldMatrix")))
		return E_FAIL;
	
	if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	
	if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderComs[COORDINATE_2D], "g_DiffuseTexture")))
		return E_FAIL;
	if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_RawValue("g_fRed", &m_vColor.x, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_RawValue("g_fGreen", &m_vColor.y, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_RawValue("g_fBlue", &m_vColor.z, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_RawValue("g_fOpaque", &m_vColor.w, sizeof(_float))))
		return E_FAIL;

	
	m_pShaderComs[COORDINATE_2D]->Begin((_uint)PASS_VTXPOSTEX::COLOR_ALPHA);
	m_pVIBufferCom->Bind_BufferDesc();
	m_pVIBufferCom->Render();

#pragma region 테스트폰트노출
	wsprintf(m_tTest, TEXT("Font18 : 배고파요"));
	m_pGameInstance->Render_Font(TEXT("Font18"), m_tTest, _float2(g_iWinSizeX / 2.f, g_iWinSizeY / 10.f), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
	wsprintf(m_tTest, TEXT("Font20 : 배고파요"));
	m_pGameInstance->Render_Font(TEXT("Font20"), m_tTest, _float2(g_iWinSizeX / 2.f, g_iWinSizeY / 8.f), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
	wsprintf(m_tTest, TEXT("Font24 : 배고파요"));
	m_pGameInstance->Render_Font(TEXT("Font24"), m_tTest, _float2(g_iWinSizeX / 2.f, g_iWinSizeY / 6.f), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
	wsprintf(m_tTest, TEXT("Font28 : 배고파요"));
	m_pGameInstance->Render_Font(TEXT("Font28"), m_tTest, _float2(g_iWinSizeX / 2.f, g_iWinSizeY / 5.f), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
	wsprintf(m_tTest, TEXT("Font30 : 배고파요"));
	m_pGameInstance->Render_Font(TEXT("Font30"), m_tTest, _float2(g_iWinSizeX / 2.f, g_iWinSizeY / 4.f), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
	wsprintf(m_tTest, TEXT("Font35 : 배고파요"));
	m_pGameInstance->Render_Font(TEXT("Font35"), m_tTest, _float2(g_iWinSizeX / 2.f, g_iWinSizeY / 3.f), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
	wsprintf(m_tTest, TEXT("Font40 : 배고파요"));
	m_pGameInstance->Render_Font(TEXT("Font40"), m_tTest, _float2(g_iWinSizeX / 2.f, g_iWinSizeY / 2.5f), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
	wsprintf(m_tTest, TEXT("Font43 : 배고파요"));
	m_pGameInstance->Render_Font(TEXT("Font43"), m_tTest, _float2(g_iWinSizeX / 1.f, g_iWinSizeY / 2.5f), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
	wsprintf(m_tTest, TEXT("Font54 : 배고파요"));
	m_pGameInstance->Render_Font(TEXT("Font54"), m_tTest, _float2(g_iWinSizeX / 2.f, g_iWinSizeY - g_iWinSizeY / 2.5f), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
#pragma endregion 테스트폰트노출
	


	//__super::Render();
	

	return S_OK;
}



HRESULT CLogo_BG::Ready_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderComs[COORDINATE_2D]))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_Model_2D"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_Texture_Logo_BG"),
		TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}


CLogo_BG* CLogo_BG::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CLogo_BG* pInstance = new CLogo_BG(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CLogo_BG Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CLogo_BG::Clone(void* _pArg)
{
	CLogo_BG* pInstance = new CLogo_BG(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CLogo_BG Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CLogo_BG::Free()
{
	
	__super::Free();
	
}

HRESULT CLogo_BG::Cleanup_DeadReferences()
{

	return S_OK;
}
