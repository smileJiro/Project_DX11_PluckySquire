#include "stdafx.h"
#include "ShopPanel_RenderFont.h"
#include "UI_Manager.h"
#include "Shop_Manager.h"
#include "PlayerData_Manager.h"




CShopPanel_RenderFont::CShopPanel_RenderFont(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CShopPanel_New(_pDevice, _pContext)
{
}

CShopPanel_RenderFont::CShopPanel_RenderFont(const CShopPanel_RenderFont& _Prototype)
	: CShopPanel_New(_Prototype)
{
}

HRESULT CShopPanel_RenderFont::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CShopPanel_RenderFont::Initialize(void* _pArg)
{
	UIOBJDESC* pDesc = static_cast<UIOBJDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_iTextureCount = pDesc->iTextureCount;
	m_eShopPanel = pDesc->eShopPanelKind;
	m_isRender = false;

	_float2 vCalScale = { 0.f, 0.f };
	vCalScale.x = m_vOriginSize.x * RATIO_BOOK2D_X;
	vCalScale.y = m_vOriginSize.y * RATIO_BOOK2D_Y;

	m_pControllerTransform->Set_Scale(vCalScale.x, vCalScale.y, 1.f);


	//if (FAILED(Ready_Components()))
	//	return E_FAIL;

	//if (FAILED(CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter5_P0102"), this, SECTION_2D_PLAYMAP_UI)))
	//	return E_FAIL;
	

	return S_OK;
}

void CShopPanel_RenderFont::Priority_Update(_float _fTimeDelta)
{
}

void CShopPanel_RenderFont::Update(_float _fTimeDelta)
{
	if (true == CShop_Manager::GetInstance()->Get_isOpenShop())
	{
		if (false == CSection_Manager::GetInstance()->Is_CurSection(this))
			CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this, SECTION_2D_PLAYMAP_UI);

		if (false == m_isRender)
			m_isRender = true;
	}
	else
	{
		if (true == CSection_Manager::GetInstance()->Is_CurSection(this))
			CSection_Manager::GetInstance()->Remove_GameObject_ToCurSectionLayer(this);


		if (true == m_isRender)
			m_isRender = false;
	}


}

void CShopPanel_RenderFont::Late_Update(_float _fTimeDelta)
{

}


HRESULT CShopPanel_RenderFont::Render()
{

	CShop_Manager* pShopManager = CShop_Manager::GetInstance();


	// 선택, 취소, 현재 가지고 있는 전구의 개수
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
		m_pGameInstance->Render_Font(TEXT("Font30"), m_tFont, _float2(vCalPos.x - 2.5f, vCalPos.y), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font30"), m_tFont, _float2(vCalPos.x + 2.5f, vCalPos.y), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font30"), m_tFont, _float2(vCalPos.x, vCalPos.y - 2.5f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font30"), m_tFont, _float2(vCalPos.x, vCalPos.y + 2.5f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font30"), m_tFont, _float2(vCalPos.x, vCalPos.y), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));

		vPos.x = BGPos.x + vRTSize.x * 0.035f;
		vCalPos.x = vMiddlePoint.x + vPos.x;

		wsprintf(m_tFont, TEXT("선택"));
		m_pGameInstance->Render_Font(TEXT("Font30"), m_tFont, _float2(vCalPos.x - 2.5f, vCalPos.y), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font30"), m_tFont, _float2(vCalPos.x + 2.5f, vCalPos.y), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font30"), m_tFont, _float2(vCalPos.x, vCalPos.y - 2.5f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font30"), m_tFont, _float2(vCalPos.x, vCalPos.y + 2.5f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font30"), m_tFont, _float2(vCalPos.x, vCalPos.y), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));

		m_pGameInstance->Render_Font(TEXT("Font30"), m_strName.c_str(), _float2(vCalPos.x, vCalPos.y), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));


		Render_ItemInfo();
		Render_ItemDialog();
	}

	return S_OK;

}

void CShopPanel_RenderFont::Render_ItemInfo()
{
	
	CShop_Manager* pShopManager = CShop_Manager::GetInstance();
	assert(pShopManager);

	_float2 vRTSize = CSection_Manager::GetInstance()->Get_Section_RenderTarget_Size(CSection_Manager::GetInstance()->Get_Cur_Section_Key());
	

	if (0 == pShopManager->Get_ShopItems().size())
		return;

	for (int i = 0; i < pShopManager->Get_ShopItems().size(); ++i)
	{
		_float2 vBadgePos = pShopManager->Get_BadgePositions()[i];
		/* 나중에 수정 필요 */

		_float2 vMiddlePoint = { vRTSize.x / 2 , vRTSize.y / 2 };
		_float2 vCalPos = { 0.f, 0.f };
		/* 나중에 수정 필요 */

		_float2 vPos = { 0.f, 0.f };

		vPos.x = vBadgePos.x + vRTSize.x * 0.01f;
		vPos.y = vBadgePos.y + vRTSize.y * 0.015f;

		vCalPos.x = vMiddlePoint.x + vPos.x;
		vCalPos.y = vMiddlePoint.y - vPos.y;

		//이름 노출
		_wstring strName = pShopManager->Get_ShopItems()[i][1]->Get_strName();
		m_pGameInstance->Render_Font(TEXT("Font30"), strName.c_str(), _float2(vCalPos.x, vCalPos.y), XMVectorSet(0.f, 0.f, 0.f, 1.0f));

		// 가격 노출
		vPos.x = vBadgePos.x + vRTSize.x * 0.06f;
		vPos.y = vBadgePos.y + vRTSize.y * 0.013f;

		vCalPos.x = vMiddlePoint.x + vPos.x;
		vCalPos.y = vMiddlePoint.y - vPos.y;

		_wstring szBulbCount = to_wstring(pShopManager->Get_ShopItems()[i][1]->Get_Price());

		m_pGameInstance->Render_Font(TEXT("Font24"), szBulbCount.c_str(), _float2(vCalPos.x, vCalPos.y), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
	}
}

void CShopPanel_RenderFont::Render_ItemDialog()
{
	CShop_Manager* pShopManager = CShop_Manager::GetInstance();
	assert(pShopManager);

	_int iChooseIndex = pShopManager->Get_ChooseIndex();
	_float2 vRTSize = CSection_Manager::GetInstance()->Get_Section_RenderTarget_Size(CSection_Manager::GetInstance()->Get_Cur_Section_Key());
	_float2 vBGPos = pShopManager->Get_ShopBGPos();
	_float2 vMiddlePoint = { vRTSize.x / 2 , vRTSize.y / 2 };
	_float2 vCalPos = { 0.f, 0.f };
	/* 나중에 수정 필요 */



	_float2 vPos = { 0.f, 0.f };

	if (0 == pShopManager->Get_ShopItems().size())
	{
		vPos.x = vBGPos.x - vRTSize.x * 0.03f;
		vPos.y = vBGPos.y - vRTSize.y * 0.13f;

		vCalPos.x = vMiddlePoint.x + vPos.x;
		vCalPos.y = vMiddlePoint.y - vPos.y;

		m_pGameInstance->Render_Font(TEXT("Font30"), TEXT("아이템을 전부 구매했어요!\n다음에도 이용해주세요!"), _float2(vCalPos.x, vCalPos.y), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
	}
	else if (true == pShopManager->Get_Confirm())
	{
		vPos.x = vBGPos.x - vRTSize.x * 0.03f;
		vPos.y = vBGPos.y - vRTSize.y * 0.145f;

		vCalPos.x = vMiddlePoint.x + vPos.x;
		vCalPos.y = vMiddlePoint.y - vPos.y;

		m_pGameInstance->Render_Font(TEXT("Font30"), TEXT("정말로 구매 하시겠어요?"), _float2(vCalPos.x, vCalPos.y), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
	}
	else if (false == pShopManager->Get_Confirm())
	{
		if (0 < pShopManager->Get_ShopItems().size())
		{
			vPos.x = vBGPos.x - vRTSize.x * 0.04f;
			vPos.y = vBGPos.y - vRTSize.y * 0.13f;

			vCalPos.x = vMiddlePoint.x + vPos.x;
			vCalPos.y = vMiddlePoint.y - vPos.y;

			_wstring strDialog = pShopManager->Get_ShopItems()[iChooseIndex][1]->Get_strDialog();

			m_pGameInstance->Render_Font(TEXT("Font30"), strDialog.c_str(), _float2(vCalPos.x, vCalPos.y), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		}
	}
}

void CShopPanel_RenderFont::Cal_ShopYesNOPos(_float2 _vRTSize, _float2 _vBGPos)
{
	_float2 vPos = { 0.f, 0.f };

	vPos.x = _vBGPos.x + _vRTSize.x * 0.10f;
	vPos.y = _vBGPos.y - _vRTSize.x * 0.06f;

	Get_Transform()->Set_State(CTransform::STATE_POSITION, XMVectorSet(vPos.x, vPos.y, 0.f, 1.f));
}

HRESULT CShopPanel_RenderFont::Ready_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
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


CShopPanel_RenderFont* CShopPanel_RenderFont::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CShopPanel_RenderFont* pInstance = new CShopPanel_RenderFont(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CShopPanel_RenderFont Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CShopPanel_RenderFont::Clone(void* _pArg)
{
	CShopPanel_RenderFont* pInstance = new CShopPanel_RenderFont(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CShopPanel_RenderFont Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CShopPanel_RenderFont::Free()
{
	__super::Free();
}



HRESULT CShopPanel_RenderFont::Cleanup_DeadReferences()
{
	return S_OK;
}

