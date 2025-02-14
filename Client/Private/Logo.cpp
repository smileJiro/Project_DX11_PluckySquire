#include "stdafx.h"
#include "Logo.h"
#include "UI_Manager.h"

CLogo::CLogo(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CUI(_pDevice, _pContext)
{
}

CLogo::CLogo(const CLogo& _Prototype)
	: CUI(_Prototype)
{
}

HRESULT CLogo::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLogo::Initialize(void* _pArg)
{
	UIOBJDESC* pDesc = static_cast<UIOBJDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_eShopPanel = SHOP_END;


	if (FAILED(Ready_Props(LEVEL_ID(pDesc->iCurLevelID), pDesc->strLayerTag)))
		return E_FAIL;

	return S_OK;
}

void CLogo::Priority_Update(_float _fTimeDelta)
{
}

void CLogo::Update(_float _fTimeDelta)
{

	
}

void CLogo::Late_Update(_float _fTimeDelta)
{

	

	if (true == m_isRender && true == Uimgr->Get_LogoChanseStage())
	{

		for (int i = 0; i < Uimgr->Get_LogoProps().size(); ++i)
		{
			Uimgr->Get_LogoProps()[i]->Child_LateUpdate(_fTimeDelta);
		}

	}
	else
	{
		__super::Late_Update(_fTimeDelta);
	}

}

HRESULT CLogo::Render(_int _iTextureindex, PASS_VTXPOSTEX _eShaderPass)
{
	

	return S_OK;
}




HRESULT CLogo::Ready_Props(LEVEL_ID _eCurLevel, const _wstring& _strLayerTag)
{
	CUI::UIOBJDESC pDesc = {};
	CUI::UIOBJDESC LogoPropsDesc[CUI::LOGOPROP::LOGOPROP_END] = {};
	_uint LogoPropUICount = { CUI::LOGOPROP::LOGOPROP_END };

	//pDesc.iCurLevelID = _eCurLevel;


	if (LogoPropUICount != Uimgr->Get_LogoProps().size())
	{
		CGameObject* pLogoProp = { nullptr };

		for (_uint i = 0; i < CUI::LOGOPROP::LOGOPROP_END; ++i)
		{



			switch (i)
			{
				case CUI::LOGOPROP::LOGOPROP_BORDER:
				{
					CUI::UIOBJDESC pLogo_TreeInk0{};
					pLogo_TreeInk0.fX = g_iWinSizeX / 2.f;
					pLogo_TreeInk0.fY = g_iWinSizeY / 2.f;
					pLogo_TreeInk0.fSizeX = 1600.f;
					pLogo_TreeInk0.fSizeY = 900.f;
					pLogo_TreeInk0.iTextureCount = (_uint)CUI::LOGOPROP::LOGOPROP_BORDER;
					pLogo_TreeInk0.strLayerTag = _strLayerTag;

					if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOGO, TEXT("Prototype_UIObejct_LogoProp"), LEVEL_LOGO, _strLayerTag, &pLogoProp, &pLogo_TreeInk0)))
						return E_FAIL;

					Uimgr->pushBack_LogoPorp(static_cast<CLogo_Props*>(pLogoProp));


				}
				break;

			case CUI::LOGOPROP::LOGOPROP_TREEINK0:
			{
				CUI::UIOBJDESC pLogo_TreeInk0{};
				pLogo_TreeInk0.fX = g_iWinSizeX / 8.5f;
				pLogo_TreeInk0.fY = g_iWinSizeY / 5.f;
				pLogo_TreeInk0.fSizeX = 192.f;
				pLogo_TreeInk0.fSizeY = 278.f;
				pLogo_TreeInk0.iTextureCount = (_uint)CUI::LOGOPROP::LOGOPROP_TREEINK0;
				pLogo_TreeInk0.strLayerTag = _strLayerTag;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOGO, TEXT("Prototype_UIObejct_LogoProp"), LEVEL_LOGO, _strLayerTag, &pLogoProp, &pLogo_TreeInk0)))
					return E_FAIL;

				Uimgr->pushBack_LogoPorp(static_cast<CLogo_Props*>(pLogoProp));


			}
			break;

			case CUI::LOGOPROP::LOGOPROP_TREEINK1:
			{
				CUI::UIOBJDESC pLogo_TreeInk1{};
				pLogo_TreeInk1.fX = g_iWinSizeX - g_iWinSizeX / 12.f;
				pLogo_TreeInk1.fY = g_iWinSizeY / 9.f;
				pLogo_TreeInk1.fSizeX = 192.f;
				pLogo_TreeInk1.fSizeY = 278.f;
				pLogo_TreeInk1.iTextureCount = (_uint)CUI::LOGOPROP::LOGOPROP_TREEINK1;
				pLogo_TreeInk1.strLayerTag = _strLayerTag;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOGO, TEXT("Prototype_UIObejct_LogoProp"), _eCurLevel, _strLayerTag, &pLogoProp, &pLogo_TreeInk1)))
					return E_FAIL;

				Uimgr->pushBack_LogoPorp(static_cast<CLogo_Props*>(pLogoProp));

			}
			break;

			case CUI::LOGOPROP::LOGOPROP_TREE:
			{
				CUI::UIOBJDESC pLogo_Tree{};
				pLogo_Tree.fX = g_iWinSizeX / 12.f;
				pLogo_Tree.fY = g_iWinSizeY - g_iWinSizeY / 5.f;
				pLogo_Tree.fSizeX = 192.f;
				pLogo_Tree.fSizeY = 278.f;
				pLogo_Tree.iTextureCount = (_uint)CUI::LOGOPROP::LOGOPROP_TREE;
				pLogo_Tree.strLayerTag = _strLayerTag;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOGO, TEXT("Prototype_UIObejct_LogoProp"), _eCurLevel, _strLayerTag, &pLogoProp, &pLogo_Tree)))
					return E_FAIL;

				Uimgr->pushBack_LogoPorp(static_cast<CLogo_Props*>(pLogoProp));

			}
			break;

			case CUI::LOGOPROP::LOGOPROP_WHITEFLOWER0:
			{

				CUI::UIOBJDESC pLogo_Flower{};
				pLogo_Flower.fX = g_iWinSizeX / 2.3f;
				pLogo_Flower.fY = g_iWinSizeY / 6.5f;
				pLogo_Flower.fSizeX = 40.f;
				pLogo_Flower.fSizeY = 37.f;
				pLogo_Flower.iTextureCount = (_uint)CUI::LOGOPROP::LOGOPROP_WHITEFLOWER0;
				pLogo_Flower.strLayerTag = _strLayerTag;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOGO, TEXT("Prototype_UIObejct_LogoProp"), _eCurLevel, _strLayerTag, &pLogoProp, &pLogo_Flower)))
					return E_FAIL;
				Uimgr->pushBack_LogoPorp(static_cast<CLogo_Props*>(pLogoProp));

				pLogo_Flower.fX = g_iWinSizeX / 4.f;
				pLogo_Flower.fY = g_iWinSizeY - g_iWinSizeY / 6.f;
				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOGO, TEXT("Prototype_UIObejct_LogoProp"), _eCurLevel, _strLayerTag, &pLogoProp, &pLogo_Flower)))
					return E_FAIL;
				Uimgr->pushBack_LogoPorp(static_cast<CLogo_Props*>(pLogoProp));

				pLogo_Flower.fX = g_iWinSizeX / 18.f;
				pLogo_Flower.fY = g_iWinSizeY / 2.2f;
				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOGO, TEXT("Prototype_UIObejct_LogoProp"), _eCurLevel, _strLayerTag, &pLogoProp, &pLogo_Flower)))
					return E_FAIL;
				Uimgr->pushBack_LogoPorp(static_cast<CLogo_Props*>(pLogoProp));




			}
			break;

			case CUI::LOGOPROP::LOGOPROP_WHITEFLOWER1:
			{
				CUI::UIOBJDESC pLogo_Flower{};
				pLogo_Flower.fX = g_iWinSizeX / 4.f;
				pLogo_Flower.fY = g_iWinSizeY / 8.f;

				pLogo_Flower.fSizeX = 40.f;
				pLogo_Flower.fSizeY = 37.f;
				pLogo_Flower.iTextureCount = (_uint)CUI::LOGOPROP::LOGOPROP_WHITEFLOWER1;
				pLogo_Flower.strLayerTag = _strLayerTag;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOGO, TEXT("Prototype_UIObejct_LogoProp"), _eCurLevel, _strLayerTag, &pLogoProp, &pLogo_Flower)))
					return E_FAIL;
				Uimgr->pushBack_LogoPorp(static_cast<CLogo_Props*>(pLogoProp));

				pLogo_Flower.fX = g_iWinSizeX / 2.5f;
				pLogo_Flower.fY = g_iWinSizeY - g_iWinSizeY / 4.f;
				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOGO, TEXT("Prototype_UIObejct_LogoProp"), _eCurLevel, _strLayerTag, &pLogoProp, &pLogo_Flower)))
					return E_FAIL;
				Uimgr->pushBack_LogoPorp(static_cast<CLogo_Props*>(pLogoProp));
			}
			break;

			case CUI::LOGOPROP::LOGOPROP_ENTER:
			{
				CUI::UIOBJDESC pLogo_Enter;
				pLogo_Enter.fX = g_iWinSizeX - g_iWinSizeX / 10.5f;
				pLogo_Enter.fY = g_iWinSizeY - g_iWinSizeY / 15.f;
				pLogo_Enter.fSizeX = 56.f;
				pLogo_Enter.fSizeY = 56.f;
				pLogo_Enter.iTextureCount = (_uint)CUI::LOGOPROP::LOGOPROP_ENTER;
				pLogo_Enter.strLayerTag = _strLayerTag;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOGO, TEXT("Prototype_UIObejct_LogoProp"), _eCurLevel, _strLayerTag, &pLogoProp, &pLogo_Enter)))
					return E_FAIL;
				Uimgr->pushBack_LogoPorp(static_cast<CLogo_Props*>(pLogoProp));
			}
			break;
			}
		}
	}
	return S_OK;
}

HRESULT CLogo::Ready_Components()
{
	return S_OK;
}


CLogo* CLogo::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CLogo* pInstance = new CLogo(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CLogo Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CLogo::Clone(void* _pArg)
{
	CLogo* pInstance = new CLogo(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CLogo Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CLogo::Free()
{

	__super::Free();
}






