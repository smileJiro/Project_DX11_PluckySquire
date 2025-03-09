#include "stdafx.h"
#include "ShopPanel_New.h"
#include "UI_Manager.h"
#include "PlayerData_Manager.h"
#include "Shop_Manager.h"

CShopPanel_New::CShopPanel_New(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CUI(_pDevice, _pContext)
{
}

CShopPanel_New::CShopPanel_New(const CShopPanel_New& _Prototype)
	: CUI(_Prototype)
{
}

HRESULT CShopPanel_New::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CShopPanel_New::Initialize(void* _pArg)
{
	UIOBJDESC* pDesc = static_cast<UIOBJDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_eShopPanel = SHOP_END;

	if (false == Uimgr->Get_isMakeItem())
	{
		_float2 RTSize = _float2(RTSIZE_BOOK2D_X, RTSIZE_BOOK2D_Y);


		//Uimgr->Set_isMakeItem(true);
	}


	//if (FAILED(CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter5_P0102"), this)))
	//	return E_FAIL;



 	return S_OK;
}

void CShopPanel_New::Priority_Update(_float _fTimeDelta)
{
}

void CShopPanel_New::Update(_float _fTimeDelta)
{

	// 다이얼로그가 끝나면 상점을 오픈 시킨다.
	if (true == Uimgr->Get_DialogueFinishShopPanel())
	{
		//Update_KeyInput(_fTimeDelta, iIndex);
		ChangeState_Panel(_fTimeDelta, Uimgr->Get_DialogueFinishShopPanel());
		m_isOpenPanel = true;
	}

	// 해당 인덱스를 체크해서 true로한다.


	//_float2 RTSize = _float2(RTSIZE_BOOK2D_X, RTSIZE_BOOK2D_Y);

	_float2 RTSize = CSection_Manager::GetInstance()->Get_Section_RenderTarget_Size(CSection_Manager::GetInstance()->Get_Cur_Section_Key());
	if (true == m_isRender)
	{
		Cal_ShopPartPos(RTSize, CShop_Manager::GetInstance()->Get_ShopBGPos());
	}

	
	
		Update_KeyInput(_fTimeDelta);
		if (m_iChooseIndex != -1 && m_iChooseIndex != m_iPreindex)
		{
			CShop_Manager::GetInstance()->Set_ChooseItem(m_iChooseIndex);
			m_iPreindex = m_iChooseIndex;
		}
		else if (m_iChooseIndex == CShop_Manager::GetInstance()->Get_ShopItems().size() - 1)
		{
			CShop_Manager::GetInstance()->Set_ChooseItem(m_iChooseIndex);
		}
	
}

void CShopPanel_New::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);

	if (false == CSection_Manager::GetInstance()->Is_CurSection(this))
		CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this);
}

HRESULT CShopPanel_New::Render(_int _iTextureindex, PASS_VTXPOSTEX _eShaderPass)
{
	//if (true == m_isRender)
	//	__super::Render(_iTextureindex, _eShaderPass);

	CShop_Manager* pShopManager = CShop_Manager::GetInstance();

	if (true == m_isRender)
	{
		//__super::Render(_iTextureindex, _eShaderPass);

		/* RTSIZE 를 가져올 수 있는 방법을 논의해봐야한다. */

		_float2 BGPos = pShopManager->Get_ShopBGPos();
		/* TODO :: 나중에 수정 필요 */

		_float2 vRTSize = _float2(0.f, 0.f);

		vRTSize = CSection_Manager::GetInstance()->Get_Section_RenderTarget_Size(CSection_Manager::GetInstance()->Get_Cur_Section_Key());

		_float2 vMiddlePoint = { vRTSize.x / 2 , vRTSize.y / 2 };
		_float2 vCalPos = { 0.f, 0.f };
		_float2 vBulbPos = { 0.f, 0.f };
		/* TODO :: 나중에 수정 필요 */
		


		_float2 vPos = { 0.f, 0.f };

		vPos.x = BGPos.x + vRTSize.x * 0.06f;
		vPos.y = BGPos.y + vRTSize.y * 0.137f;

		vBulbPos.x = vMiddlePoint.x + vPos.x;
		vBulbPos.y = vMiddlePoint.y - vPos.y;

		
		_wstring szBulbCount = to_wstring(CPlayerData_Manager::GetInstance()->Get_BulbCount());
		wsprintf(m_tFont, szBulbCount.c_str());
		

		m_pGameInstance->Render_Font(TEXT("Font24"), m_tFont, _float2(vBulbPos.x, vBulbPos.y), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));


		vPos.x = BGPos.x + vRTSize.x * 0.045f;
		vPos.y = BGPos.y - vRTSize.y * 0.075f;

		vCalPos.x = vMiddlePoint.x + vPos.x;
		vCalPos.y = vMiddlePoint.y - vPos.y;


		vPos.x = BGPos.x - vRTSize.x * 0.036f;
		vPos.y = BGPos.y - vRTSize.y * 0.2f;


		vCalPos.x = vMiddlePoint.x + vPos.x;
		vCalPos.y = vMiddlePoint.y - vPos.y;

		wsprintf(m_tFont, TEXT("취소"));
		m_pGameInstance->Render_Font(TEXT("Font30"), m_tFont, _float2(vCalPos.x - 2.5f, vCalPos.y),			XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font30"), m_tFont, _float2(vCalPos.x + 2.5f, vCalPos.y),			XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font30"), m_tFont, _float2(vCalPos.x,		vCalPos.y - 2.5f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font30"), m_tFont, _float2(vCalPos.x,		vCalPos.y + 2.5f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font30"), m_tFont, _float2(vCalPos.x ,		vCalPos.y),			XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));

		vPos.x = BGPos.x + vRTSize.x * 0.035f;
		vCalPos.x = vMiddlePoint.x + vPos.x;

		wsprintf(m_tFont, TEXT("선택"));
		m_pGameInstance->Render_Font(TEXT("Font30"), m_tFont, _float2(vCalPos.x - 2.5f,	vCalPos.y),			XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font30"), m_tFont, _float2(vCalPos.x + 2.5f,	vCalPos.y),			XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font30"), m_tFont, _float2(vCalPos.x,		vCalPos.y - 2.5f),	XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font30"), m_tFont, _float2(vCalPos.x,		vCalPos.y + 2.5f),	XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font30"), m_tFont, _float2(vCalPos.x,		vCalPos.y),			XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));



		m_pGameInstance->Render_Font(TEXT("Font30"), m_strName.c_str(), _float2(vCalPos.x, vCalPos.y), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));

	}

	return S_OK;
}

HRESULT CShopPanel_New::ShopRender(_int _iTextureindex, PASS_VTXPOSTEX _eShaderPass)
{
	if (true == m_isRender)
		__super::Render(_iTextureindex, _eShaderPass);

	return S_OK;
}


void CShopPanel_New::Update_KeyInput(_float _fTimeDelta)
{
	
	CShop_Manager* pShopManager = CShop_Manager::GetInstance();

	// SHOP  패널 오픈 기능
	
	bool iskeyI = KEY_DOWN(KEY::I);
	bool iskeyESC = KEY_DOWN(KEY::ESC);
	bool iskeyE = KEY_DOWN(KEY::E);

	if (false == pShopManager->Get_isOpenShop())
		return;


	// 인덱스 변경 키
	if (KEY_DOWN(KEY::DOWN))
	{
		if (true == pShopManager->Get_Confirm())
		{
			if (true == pShopManager->Get_isPurchase())
				pShopManager->Set_isPurchase(false);
			return;
		}
			

		if (m_iChooseIndex == pShopManager->Get_ShopItems().size() - 1)
			return;

		++m_iChooseIndex;
	}

	if (KEY_DOWN(KEY::UP))
	{
		if (true == pShopManager->Get_Confirm())
		{
			if (false == pShopManager->Get_isPurchase())
				pShopManager->Set_isPurchase(true);
			return;
		}

		if (m_iChooseIndex == 0)
			return;

		--m_iChooseIndex;
	}




	// 아이템 선택 후 정말로 구매할거에요? 노출
	_bool isYesorNo = Uimgr->Get_StoreYesOrno();
	if (true == m_isOpenPanel)
	{

		if (KEY_DOWN(KEY::ENTER) && false == pShopManager->Get_Confirm())
		{
			if (-1 != m_iChooseIndex)
			{
				m_iConfirmItemIndex = m_iChooseIndex;
				pShopManager->Set_Confirm(true);
				pShopManager->Set_OpenConfirmUI(true);
			}
		}
		else if (KEY_DOWN(KEY::ENTER) && true == pShopManager->Get_Confirm())
		{
			if (true == pShopManager->Get_OpenConfirmUI())
			{
				if (true == pShopManager->Get_isPurchase())
				{
					pShopManager->Delete_ShopItems(m_iChooseIndex);

					if (m_iChooseIndex == pShopManager->Get_ShopItems().size() - 1)
					{
						if (0 == m_iChooseIndex)
							return;

						else
							--m_iChooseIndex;
					}
				}
				
				m_iConfirmItemIndex = 0;
				pShopManager->Set_Confirm(false);
				pShopManager->Set_OpenConfirmUI(false);
				pShopManager->Set_isPurchase(false);
			}
			else if (false == pShopManager->Get_OpenConfirmUI())
			{
				pShopManager->Set_Confirm(false);
				pShopManager->Set_OpenConfirmUI(true);
			}

		}
	}


	if (KEY_DOWN(KEY::K))
	{
		Uimgr->Set_DialogueFinishShopPanel(false);
		pShopManager->OpenClose_Shop();
		//ChangeState_Panel(_fTimeDelta, Uimgr->Get_DialogueFinishShopPanel());
		m_isOpenPanel = false;
	}

}

void CShopPanel_New::ChangeState_Panel(_float _fTimeDelta, _bool _isOpenState)
{
	_bool isDialogueFinishShopPanel = Uimgr->Get_DialogueFinishShopPanel();

	if (m_isPreState != isDialogueFinishShopPanel)
	{
		m_isPreState = isDialogueFinishShopPanel;

		if (true == isDialogueFinishShopPanel) /* && SHOP_END != m_eShopPanel*/
		{
			CShop_Manager::GetInstance()->OpenClose_Shop();

			false == Uimgr->Get_DialogueFinishShopPanel() ? Uimgr->Set_DialogueFinishShopPanel(true) : Uimgr->Set_DialogueFinishShopPanel(false);
			m_isPreState = Uimgr->Get_DialogueFinishShopPanel();
			//Uimgr->Set_DialogueFinishShopPanel(false);
		}
	}
}


void CShopPanel_New::Cal_ShopPartPos(_float2 _vRTSize, _float2 _vBGPos)
{

	_float2 vCalPos = { 0.f, 0.f };

	auto& ShopItems = CShop_Manager::GetInstance()->Get_ShopItems();

	if (nullptr == &ShopItems)
		assert(nullptr);

	for (_int i = 0; i < ShopItems.size(); ++i)
	{
		if (0 == i)
		{


			for (int j = 0; j < ShopItems[i].size(); ++j)
			{
				if (0 == j)
				{
					vCalPos.x = _vBGPos.x;
					vCalPos.y = _vBGPos.y + _vRTSize.y * 0.07f;
				}
				else if (1 == j)
				{
					vCalPos.x = _vBGPos.x - _vRTSize.x * 0.035f;
					vCalPos.y = _vBGPos.y + _vRTSize.y * 0.07f;
				}
				else if (2 == j)
				{
					vCalPos.x = _vBGPos.x + _vRTSize.x * 0.02f;
					vCalPos.y = _vBGPos.y + _vRTSize.y * 0.07f;
				}

				ShopItems[i][j]->Get_Transform()->Set_State(CTransform::STATE_POSITION, _float4(vCalPos.x, vCalPos.y, 0.f, 1.f));
				CShop_Manager::GetInstance()->Get_ShopItems()[i][j]->Set_Pos(vCalPos);
			}
			
		}
		else if (1 == i)
		{
			for (int j = 0; j < ShopItems[i].size(); ++j)
			{
				if (0 == j)
				{
					vCalPos.x = _vBGPos.x;
					vCalPos.y = _vBGPos.y - _vRTSize.y * 0.005f;
				}
				else if (1 == j)
				{
					vCalPos.x = _vBGPos.x - _vRTSize.x * 0.035f;
					vCalPos.y = _vBGPos.y - _vRTSize.y * 0.005f;
				}
				else if (2 == j)
				{
					vCalPos.x = _vBGPos.x + _vRTSize.x * 0.02f;
					vCalPos.y = _vBGPos.y - _vRTSize.y * 0.005f;
				}

				ShopItems[i][j]->Get_Transform()->Set_State(CTransform::STATE_POSITION, _float4(vCalPos.x, vCalPos.y, 0.f, 1.f));
				CShop_Manager::GetInstance()->Get_ShopItems()[i][j]->Set_Pos(vCalPos);
			}
		}
		else if (2 == i)
		{
			for (int j = 0; j < ShopItems[i].size(); ++j)
			{
				if (0 == j)
				{
					vCalPos.x = _vBGPos.x;
					vCalPos.y = _vBGPos.y - _vRTSize.y * 0.08f;
				}
				else if (1 == j)
				{
					vCalPos.x = _vBGPos.x - _vRTSize.x * 0.035f;
					vCalPos.y = _vBGPos.y - _vRTSize.y * 0.08f;
				}
				else if (2 == j)
				{
					vCalPos.x = _vBGPos.x + _vRTSize.x * 0.02f;
					vCalPos.y = _vBGPos.y - _vRTSize.y * 0.08f;
				}

				ShopItems[i][j]->Get_Transform()->Set_State(CTransform::STATE_POSITION, _float4(vCalPos.x, vCalPos.y, 0.f, 1.f));
				CShop_Manager::GetInstance()->Get_ShopItems()[i][j]->Set_Pos(vCalPos);
			}
		}
	}
}


HRESULT CShopPanel_New::Ready_Components()
{
	return S_OK;
}




CShopPanel_New* CShopPanel_New::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CShopPanel_New* pInstance = new CShopPanel_New(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CShopPanel_New Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CShopPanel_New::Clone(void* _pArg)
{
	CShopPanel_New* pInstance = new CShopPanel_New(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CShopPanel_New Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CShopPanel_New::Free()
{

	__super::Free();
}