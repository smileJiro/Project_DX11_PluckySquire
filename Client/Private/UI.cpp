#include "stdafx.h"
#include "UI.h"
#include "UI_Manager.h"




CUI::CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{

}

CUI::CUI(const CUI& Prototype)
	: CUIObject(Prototype)
{
}

HRESULT CUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI::Initialize(void* _pArg)
{

	UIOBJDESC* pDesc = static_cast<UIOBJDESC*>(_pArg);


	m_vOriginSize = _float2(pDesc->fSizeX, pDesc->fSizeY);



	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	//if (FAILED(Ready_Components()))
	//	return E_FAIL;

	return S_OK;
}

void CUI::Priority_Update(_float fTimeDelta)
{
}

void CUI::Update(_float fTimeDelta)
{

}

void CUI::Late_Update(_float fTimeDelta)
{
	Register_RenderGroup(RENDERGROUP::RG_3D, PRIORITY_3D::PR3D_UI);
}

HRESULT CUI::Render(_int _iTextureindex, PASS_VTXPOSTEX _eShaderPass)
{
	if (FAILED(m_pControllerTransform->Bind_ShaderResource(m_pShaderComs[COORDINATE_2D], "g_WorldMatrix")))
		return E_FAIL;

	if (L"" == m_strSectionName)
	{
		if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
			return E_FAIL;

		if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
			return E_FAIL;
	}

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderComs[COORDINATE_2D], "g_DiffuseTexture", _iTextureindex)))
		return E_FAIL;

	m_pShaderComs[COORDINATE_2D]->Begin((_uint)_eShaderPass);
	m_pVIBufferCom->Bind_BufferDesc();
	m_pVIBufferCom->Render();


	return S_OK;
}

HRESULT CUI::Render(C2DModel* _Model)
{
	if (FAILED(m_pControllerTransform->Bind_ShaderResource(m_pShaderComs[COORDINATE_2D], "g_WorldMatrix")))
		return E_FAIL;

	if (L"" == m_strSectionName)
	{
		if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
			return E_FAIL;

		if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
			return E_FAIL;
	}

	_Model->Render(m_pShaderComs[COORDINATE_2D], (_uint)PASS_VTXPOSTEX::SPRITE2D);
}

HRESULT CUI::Ready_Components()
{
	return S_OK;
}

HRESULT CUI::Bind_ShaderResources()
{
	if (FAILED(m_pControllerTransform->Bind_ShaderResource(m_pShaderComs[COORDINATE_2D], "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderComs[COORDINATE_2D], "g_DiffuseTexture")))
		return E_FAIL;

	return S_OK;
}

void CUI::Change_BookScale_ForShop(_float2 _vRTSize)
{
	_float2 vCalSize;


	if (SHOPPANEL::SHOP_BG != m_eShopPanel)
	{
		_float2 BGPos = { 0.f, 0.f };
		BGPos.x = Uimgr->Get_ShopPanels()[0]->Get_FX();
		BGPos.y = Uimgr->Get_ShopPanels()[0]->Get_FY();


		switch ((_uint)m_eShopPanel)
		{
		case SHOP_DIALOGUEBG:	{ m_fY = BGPos.y + _vRTSize.y * RATIO_BOOK2D_Y; }
		break;

		case SHOP_CHOOSEBG:		{ m_fX = BGPos.x + _vRTSize.x * RATIO_BOOK2D_X * 0.3f;	m_fY = BGPos.y + _vRTSize.y * RATIO_BOOK2D_Y; }
		break;

		case SHOP_BULB:			{ m_fX = BGPos.x + _vRTSize.x * RATIO_BOOK2D_X * 0.3f;	m_fY = BGPos.y - _vRTSize.y * RATIO_BOOK2D_Y * 0.8f; }
		break;
		
		case SHOP_ESCBG:		{ m_fX = BGPos.x - _vRTSize.x * RATIO_BOOK2D_X * 0.2f;	m_fY = BGPos.y + _vRTSize.y * RATIO_BOOK2D_Y * 1.35f; }
		break;

		case SHOP_BACKESC:		{ m_fX = BGPos.x - _vRTSize.x * RATIO_BOOK2D_X * 0.2f;	m_fY = BGPos.y + _vRTSize.y * RATIO_BOOK2D_Y * 1.35f; }
		break;
		
		case SHOP_BACKARROW:	{ m_fX = BGPos.x - _vRTSize.x  * RATIO_BOOK2D_X * 0.3f;	m_fY = BGPos.y + _vRTSize.y * RATIO_BOOK2D_Y * 1.35f; }
		break;
		
		case SHOP_ENTERBG:		{ m_fX = BGPos.x + _vRTSize.x * RATIO_BOOK2D_X * 0.2f;	m_fY = BGPos.y + _vRTSize.y * RATIO_BOOK2D_Y * 1.35f; }
		break;
		
		case SHOP_ENTER:		{ m_fX = BGPos.x + _vRTSize.x * RATIO_BOOK2D_X * 0.2f;	m_fY = BGPos.y + _vRTSize.y * RATIO_BOOK2D_Y * 1.35f; }
		break;
		}
	}

	vCalSize = _float2(m_vOriginSize.x * RATIO_BOOK2D_X, m_vOriginSize.y * RATIO_BOOK2D_Y);

	m_pControllerTransform->Set_Scale(COORDINATE_2D, vCalSize.x, vCalSize.y, 1.f);

	m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - m_fSizeX * 0.5f, -m_fY + m_fSizeY * 0.5f, 0.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)_vRTSize.x, (_float)_vRTSize.y, 0.0f, 1.0f));
}






void CUI::Free()
{
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderComs[COORDINATE_2D]);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);

	__super::Free();



}
