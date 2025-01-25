#include "stdafx.h"
#include "ShopItemBG.h"




CShopItemBG::CShopItemBG(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CShopPanel(_pDevice, _pContext)
{
}

CShopItemBG::CShopItemBG(const CShopItemBG& _Prototype)
	: CShopPanel(_Prototype)
{
}

HRESULT CShopItemBG::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CShopItemBG::Initialize(void* _pArg)
{
	UIOBJDESC* pDesc = static_cast<UIOBJDESC*>(_pArg);

	


	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_eSkillShopIcon = pDesc->eShopSkillKind; // 어떤 아이템의 종류이니?
	m_iSkillLevel = pDesc->iSkillLevel; // 아이템의 몇레벨 이니?
	m_isRender = false;
	m_vColor = { 227.f / 255.f , 37.f / 255.f,82.f / 255.f, 1.f };
	m_isChooseItem = pDesc->isChooseItem;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CShopItemBG::Priority_Update(_float _fTimeDelta)
{
}

void CShopItemBG::Child_Update(_float _fTimeDelta)
{
	isRender();
}

void CShopItemBG::Child_LateUpdate(_float _fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CShopItemBG::Render()
{
	if (true == m_isRender && SKILLSHOP_BG != m_eSkillShopIcon)
	{
		__super::Render(0, PASS_VTXPOSTEX::UI_POINTSAMPLE);
	}
	else if (true == m_isRender && SKILLSHOP_BG == m_eSkillShopIcon)
	{
		if (true == m_isChooseItem)
		{
			__super::Render(0, PASS_VTXPOSTEX::UI_POINTSAMPLE);
		}
		else if (false == m_isChooseItem)
		{

			if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_RawValue("g_vColors", &m_vColor, sizeof(_float4))))
				return E_FAIL;

			__super::Render(0, PASS_VTXPOSTEX::COLOR_ALPHA);
		}
	}
		

	return S_OK;
}

void CShopItemBG::isRender()
{
	if (m_isRender == false)
	{
		m_isRender = true;
	}
	else if (m_isRender == true)
		m_isRender = false;
}

HRESULT CShopItemBG::Ready_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderComs[COORDINATE_2D]))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_Model_2D"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	switch ((CUI::SKILLSHOP)m_eSkillShopIcon)
	{

	case SKILLSHOP_BG:
	{
		if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_Texture_ItemSelectedBG"),
			TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
	break;

	case SKILLSHOP_JUMPATTACKBADGE:
	{
		_tchar wSkillShop[MAX_PATH] = {};

		wsprintf(wSkillShop, TEXT("Prototype_Component_Texture_JumpAttack%d"), m_iSkillLevel);

		if (FAILED(Add_Component(m_iCurLevelID, wSkillShop,
			TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
	break;

	case SKILLSHOP_SPINATTACKBADGE:
	{

		_tchar wSkillShop[MAX_PATH] = {};

		wsprintf(wSkillShop, TEXT("Prototype_Component_Texture_JumpAttack%d"), m_iSkillLevel);

		if (FAILED(Add_Component(m_iCurLevelID, wSkillShop,
			TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
	break;

	case SKILLSHOP_ATTACKPLUSBADGE:
	{

		_tchar wSkillShop[MAX_PATH] = {};

		wsprintf(wSkillShop, TEXT("Prototype_Component_Texture_SpinAttack%d"), m_iSkillLevel);

		if (FAILED(Add_Component(m_iCurLevelID, wSkillShop,
			TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
	break;

	case SKILLSHOP_THROWATTBADGE:
	{
		_tchar wSkillShop[MAX_PATH] = {};

		wsprintf(wSkillShop, TEXT("Prototype_Component_Texture_JumpAttack%d"), m_iSkillLevel);

		if (FAILED(Add_Component(m_iCurLevelID, wSkillShop,
			TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
	break;

	case SKILLSHOP_SCROLLITEM:
	{
		if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_Texture_ScrollItem"),
			TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
	break;

	case SKILLSHOP_BULB:
	{
		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ESCBulb"),
			TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
	break;

	}

	return S_OK;
}


CShopItemBG* CShopItemBG::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CShopItemBG* pInstance = new CShopItemBG(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CShopItemBG Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CShopItemBG::Clone(void* _pArg)
{
	CShopItemBG* pInstance = new CShopItemBG(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CShopItemBG Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CShopItemBG::Free()
{
	__super::Free();
}



HRESULT CShopItemBG::Cleanup_DeadReferences()
{
	return S_OK;
}

