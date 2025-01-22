#include "stdafx.h"
#include "ShopPanel.h"
#include "UI_Manager.h"

CShopPanel::CShopPanel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CUI(_pDevice, _pContext)
{
}

CShopPanel::CShopPanel(const CShopPanel& _Prototype)
	: CUI(_Prototype)
{
}

HRESULT CShopPanel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CShopPanel::Initialize(void* _pArg)
{
	UIOBJDESC* pDesc = static_cast<UIOBJDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;


	return S_OK;
}

void CShopPanel::Priority_Update(_float _fTimeDelta)
{
}

void CShopPanel::Update(_float _fTimeDelta)
{
	if (KEY_DOWN(KEY::I) && SHOP_END == m_eShopPanel)
	{
		isRender();
	}
	else if (KEY_DOWN(KEY::I) && SHOP_END != m_eShopPanel)
	{
		for (auto iter : CUI_Manager::GetInstance()->Get_ShopPanels())
		{
			if (SHOP_END != iter.second->Get_ShopPanel())
			{
				iter.second->Child_Update(_fTimeDelta);
			}
		}
	}
}

void CShopPanel::Late_Update(_float _fTimeDelta)
{
	//m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, 


	//if (m_eShopPanel != SHOP_END)
	//{
	//	for (auto iter : CUI_Manager::GetInstance()->Get_ShopPanels())
	//	{
	//		iter.second->Child_LateUpdate(_fTimeDelta);
	//	}
	//}
	//else
	//{
	//	__super::Late_Update(_fTimeDelta);
	//}
}

HRESULT CShopPanel::Render(_int _index)
{
	if (true == m_isRender)
		__super::Render(_index);


	return S_OK;
}



void CShopPanel::isRender()
{
	if (m_isRender == false)
	{
		m_isRender = true;
	}
	else if (m_isRender == true)
		m_isRender = false;
}

HRESULT CShopPanel::Ready_Components()
{
	return S_OK;
}


CShopPanel* CShopPanel::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CShopPanel* pInstance = new CShopPanel(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CShopPanel Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CShopPanel::Clone(void* _pArg)
{
	CShopPanel* pInstance = new CShopPanel(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CShopPanel Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CShopPanel::Free()
{

	__super::Free();
}






