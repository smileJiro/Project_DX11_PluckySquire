#include "stdafx.h"
#include "ShopItemBG_New.h"
#include "Section_2D.h"
#include "UI_Manager.h"
#include "Shop_Manager.h"




CShopItemBG_New::CShopItemBG_New(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CShopPanel_New(_pDevice, _pContext)
{
}

CShopItemBG_New::CShopItemBG_New(const CShopItemBG_New& _Prototype)
	: CShopPanel_New(_Prototype)
{
}

HRESULT CShopItemBG_New::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CShopItemBG_New::Initialize(void* _pArg)
{
	ShopUI* pDesc = static_cast<ShopUI*>(_pArg);
	m_vOriginSize = _float2(pDesc->fSizeX, pDesc->fSizeY);
	m_strName = pDesc->strName;

	if (m_strName == TEXT("검 공격력 강화"))
		m_strDialog = TEXT("저트가 지닌 검의 위력이 높아집니다.\n검을 이용한 모든 공격에 적용 됩니다.");

	else if (m_strName == TEXT("회전 공격 배지"))
		m_strDialog = TEXT("충전 후 발동하면, 강력한 회전 공격으로\n모여 있는 적을 손쉽게 처치할 수 있습니다.");

	else if (m_strName == TEXT("검 던지기 배지"))
		m_strDialog = TEXT("검을 던져 적을 공격합니다.");


	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_eSkillShopIcon = pDesc->eShopSkillKind; // 어떤 아이템의 종류이니?
	m_iSkillLevel = pDesc->iSkillLevel; // 아이템의 몇레벨 이니?
	m_isRender = false;
	m_vColor = { 227.f / 255.f , 37.f / 255.f,82.f / 255.f, 1.f };
	m_fOpaque = 0.3f;
	m_isChooseItem = pDesc->isChooseItem;
	m_iPrice = pDesc->iPrice;

	

	_float2 vCalScale = { 0.f, 0.f };
	vCalScale.x = m_vOriginSize.x * RATIO_BOOK2D_X;
	vCalScale.y = m_vOriginSize.y * RATIO_BOOK2D_Y;

	m_pControllerTransform->Set_Scale(vCalScale.x, vCalScale.y, 1.f);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//m_strSectionName = TEXT("Chapter5_P0102");
	//if (FAILED(CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter5_P0102"), this, SECTION_2D_PLAYMAP_UI)))
	//	return E_FAIL;
	
	if (SKILLSHOP_BG == m_eSkillShopIcon)
	{
		m_iItemCount = CShop_Manager::GetInstance()->Get_ItemCount();
		CShop_Manager::GetInstance()->Set_ItemCount(++m_iItemCount);
	}



	return S_OK;
}

void CShopItemBG_New::Priority_Update(_float _fTimeDelta)
{
}

void CShopItemBG_New::Update(_float _fTimeDelta)
{

	_float2 vRTSize = _float2(0.f, 0.f);

	vRTSize = CSection_Manager::GetInstance()->Get_Section_RenderTarget_Size(CSection_Manager::GetInstance()->Get_Cur_Section_Key());

	Cal_ShopPartPos(vRTSize, CShop_Manager::GetInstance()->Get_ShopBGPos());
}

void CShopItemBG_New::Late_Update(_float _fTimeDelta)
{

}


HRESULT CShopItemBG_New::Render()
{
	if (false == m_isRender)
		return S_OK;


	_int itextcount = m_iItemCount;




	if (SKILLSHOP_BG != m_eSkillShopIcon)
	{
		__super::ShopRender(0, PASS_VTXPOSTEX::UI_POINTSAMPLE);
	}
	else if (true == m_isRender && SKILLSHOP_BG == m_eSkillShopIcon)
	{
		if (true == m_isChooseItem)
		{
			__super::ShopRender(0, PASS_VTXPOSTEX::UI_POINTSAMPLE);
		}
		else if (false == m_isChooseItem)
		{

			if (FAILED(m_pShaderCom->Bind_RawValue("g_fOpaque", &m_fOpaque, sizeof(_float))))
				return E_FAIL;

			//__super::Render(0, PASS_VTXPOSTEX::UI_ALPHA);

			if (FAILED(m_pControllerTransform->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
				return E_FAIL;

			if (L"" == m_strSectionName)
			{
				if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
					return E_FAIL;

				if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
					return E_FAIL;
			}

			if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
				return E_FAIL;

			m_pShaderCom->Begin((_uint)PASS_VTXPOSTEX::UI_ALPHA);
			m_pVIBufferCom->Bind_BufferDesc();
			m_pVIBufferCom->Render();
		}
	}
		
	return S_OK;
}

void CShopItemBG_New::isRender()
{
	if (m_isRender == false)
	{
		m_isRender = true;
		/* 변경해야함. */
		_float2 RTSize = _float2(RTSIZE_BOOK2D_X, RTSIZE_BOOK2D_Y);
		

	}
	else if (m_isRender == true)
	{
		m_isRender = false;
	}
		
}

void CShopItemBG_New::Set_isChooseItem(_bool _Choose)
{
	m_isChooseItem = _Choose;
}

HRESULT CShopItemBG_New::Ready_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
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

CShopItemBG_New* CShopItemBG_New::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CShopItemBG_New* pInstance = new CShopItemBG_New(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CShopItemBG_New Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CShopItemBG_New::Clone(void* _pArg)
{
	CShopItemBG_New* pInstance = new CShopItemBG_New(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CShopItemBG_New Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CShopItemBG_New::Free()
{
	//CSection_Manager::GetInstance()->Remove_GameObject_ToCurSectionLayer(this);
	__super::Free();
}

HRESULT CShopItemBG_New::Cleanup_DeadReferences()
{
	return S_OK;
}

