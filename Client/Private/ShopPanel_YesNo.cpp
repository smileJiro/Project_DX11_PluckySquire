#include "stdafx.h"
#include "ShopPanel_YesNo.h"
#include "UI_Manager.h"




CShopPanel_YesNo::CShopPanel_YesNo(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CShopPanel(_pDevice, _pContext)
{
}

CShopPanel_YesNo::CShopPanel_YesNo(const CShopPanel_YesNo& _Prototype)
	: CShopPanel(_Prototype)
{
}

HRESULT CShopPanel_YesNo::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CShopPanel_YesNo::Initialize(void* _pArg)
{
	UIOBJDESC* pDesc = static_cast<UIOBJDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_iTextureCount = pDesc->iTextureCount;
	m_eShopPanel = pDesc->eShopPanelKind;
	m_isRender = true;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this);

	return S_OK;
}

void CShopPanel_YesNo::Priority_Update(_float _fTimeDelta)
{
}

void CShopPanel_YesNo::Child_Update(_float _fTimeDelta)
{
	//isRender();
}

void CShopPanel_YesNo::Child_LateUpdate(_float _fTimeDelta)
{
	if (true == Uimgr->Get_StoreYesOrno())
	Register_RenderGroup(RENDERGROUP::RG_3D, PRIORITY_3D::PR3D_BOOK2D);
}

HRESULT CShopPanel_YesNo::Render()
{
	if (true == m_isRender && (true == Uimgr->Get_ConfirmStore()))
		__super::Render(0, PASS_VTXPOSTEX::UI_POINTSAMPLE);

	return S_OK;
}

void CShopPanel_YesNo::isRender()
{
	if (m_isRender == false)
	{
		m_isRender = true;
	}
	else if (m_isRender == true)
		m_isRender = false;
}

HRESULT CShopPanel_YesNo::Ready_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderComs[COORDINATE_2D]))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_Model_2D"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_Texture_ShopChooseBG"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;
	
	return S_OK;
}


CShopPanel_YesNo* CShopPanel_YesNo::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CShopPanel_YesNo* pInstance = new CShopPanel_YesNo(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CShopPanel_YesNo Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CShopPanel_YesNo::Clone(void* _pArg)
{
	CShopPanel_YesNo* pInstance = new CShopPanel_YesNo(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CShopPanel_YesNo Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CShopPanel_YesNo::Free()
{
	//CSection_Manager::GetInstance()->Remove_GameObject_ToCurSectionLayer(this);
	__super::Free();
}



HRESULT CShopPanel_YesNo::Cleanup_DeadReferences()
{
	return S_OK;
}

