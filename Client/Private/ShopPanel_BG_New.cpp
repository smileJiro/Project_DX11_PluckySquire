#include "stdafx.h"
#include "ShopPanel_BG_New.h"
#include "Section_2D.h"
#include "UI_Manager.h"
#include "Shop_Manager.h"




CShopPanel_BG_New::CShopPanel_BG_New(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CShopPanel_New(_pDevice, _pContext)
{
}

CShopPanel_BG_New::CShopPanel_BG_New(const CShopPanel_BG_New& _Prototype)
	: CShopPanel_New(_Prototype)
{
}

HRESULT CShopPanel_BG_New::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CShopPanel_BG_New::Initialize(void* _pArg)
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

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//if (FAILED(CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter5_P0102"), this, SECTION_2D_PLAYMAP_UI)))
	//	return E_FAIL;

	return S_OK;
}

void CShopPanel_BG_New::Priority_Update(_float _fTimeDelta)
{
}

void CShopPanel_BG_New::Update(_float _fTimeDelta)
{
	int a = 0;

	if (SHOP_BG == m_eShopPanel)
	{
		// 플레이어의 좌표를 가져와서 계산한다.
		//_float2 RTSize = _float2(RTSIZE_BOOK2D_X, RTSIZE_BOOK2D_Y);
		_float2 RTSize = CSection_Manager::GetInstance()->Get_Section_RenderTarget_Size(CSection_Manager::GetInstance()->Get_Cur_Section_Key());
		Cal_ShopBGPos(RTSize);
	}
}

void CShopPanel_BG_New::Late_Update(_float _fTimeDelta)
{
	if (true == m_isRender)
	{

		//_float2 RTSize = _float2(RTSIZE_BOOK2D_X, RTSIZE_BOOK2D_Y);
		_float2 RTSize = CSection_Manager::GetInstance()->Get_Section_RenderTarget_Size(CSection_Manager::GetInstance()->Get_Cur_Section_Key());
		
		if (SHOP_BG != m_eShopPanel)
		{
			// shop_bg의 위치를 가져온다.
			Cal_ShopPartPos(RTSize, CShop_Manager::GetInstance()->Get_ShopBGPos());
			//Change_BookScale_ForShop(RTSize);
		}
	}
}


HRESULT CShopPanel_BG_New::Render()
{
	if (true == m_isRender)
		__super::ShopRender(0, PASS_VTXPOSTEX::DEFAULT);

	return S_OK;
}

void CShopPanel_BG_New::isRender()
{
	if (m_isRender == false)
	{

		/* 나중에 수정 필요 */
		//_float2 RTSize = _float2(RTSIZE_BOOK2D_X, RTSIZE_BOOK2D_Y);
		_float2 RTSize = CSection_Manager::GetInstance()->Get_Section_RenderTarget_Size(CSection_Manager::GetInstance()->Get_Cur_Section_Key());
		m_isRender = true;
		
		Change_BookScale_ForShop(RTSize);	


	}
	else if (m_isRender == true)
	{
		m_isRender = false;
	}
		
}


void CShopPanel_BG_New::Cal_ShopBGPos(_float2 _vRTSize)
{
	// 플레이어 Position
	_float2 vPos = { 0.f, 0.f };
	vPos.x = Uimgr->Get_Player()->Get_Transform()->Get_State(CTransform_2D::STATE_POSITION).m128_f32[0];
	vPos.y = Uimgr->Get_Player()->Get_Transform()->Get_State(CTransform_2D::STATE_POSITION).m128_f32[1];

	_float2 vRatio = _float2(_vRTSize.x / RTSIZE_BOOK2D_X, _vRTSize.y / RTSIZE_BOOK2D_Y);


	vPos.x -= _vRTSize.x * 0.15f / vRatio.x;
	vPos.y += _vRTSize.y * 0.11f / vRatio.y;

	m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(vPos.x, vPos.y, 0.f, 1.f));
	CShop_Manager::GetInstance()->Set_ShopBGPos(vPos);
}

void CShopPanel_BG_New::Cal_ShopPartPos(_float2 _vRTSize, _float2 _vBGPos)
{
	_float2 vPos = { 0.f, 0.f };

	_float2 vRatio = _float2(_vRTSize.x / RTSIZE_BOOK2D_X, _vRTSize.y / RTSIZE_BOOK2D_Y);

	switch (m_eShopPanel)
	{
		/*
		SHOP_DIALOGUEBG,
		SHOP_CHOOSEBG,
		SHOP_BULB,
		SHOP_ESCBG,
		SHOP_BACKESC,
		SHOP_BACKARROW,
		SHOP_ENTERBG,
		SHOP_ENTER,
		*/

	case SHOP_DIALOGUEBG:
	{
		vPos.x = _vBGPos.x;
		vPos.y = _vBGPos.y - _vRTSize.y * 0.32f / vRatio.y;
	}
	break;

	case SHOP_CHOOSEBG:
	{
		vPos.x = _vBGPos.x + _vRTSize.x * 0.15f / vRatio.x;
		vPos.y = _vBGPos.y - _vRTSize.y * 0.32f / vRatio.y;
	}
	break;

	case SHOP_BULB:
	{
		vPos.x = _vBGPos.x + _vRTSize.x * 0.12f / vRatio.x;
		vPos.y = _vBGPos.y + _vRTSize.y * 0.25f / vRatio.y;
	}
	break;

	case SHOP_ESCBG:
	{
		vPos.x = _vBGPos.x - _vRTSize.x * 0.04f / vRatio.x;
		vPos.y = _vBGPos.y - _vRTSize.y * 0.43f / vRatio.y;
	}
	break;

	case SHOP_BACKESC:
	{
		vPos.x = _vBGPos.x - _vRTSize.x * 0.04f / vRatio.x;
		vPos.y = _vBGPos.y - _vRTSize.y * 0.43f / vRatio.y;
	}
	break;

	case SHOP_BACKARROW:
	{
		vPos.x = _vBGPos.x - _vRTSize.x * 0.08f / vRatio.x;
		vPos.y = _vBGPos.y - _vRTSize.y * 0.43f / vRatio.y;
	}
	break;

	case SHOP_ENTERBG:
	{
		vPos.x = _vBGPos.x + _vRTSize.x * 0.06f / vRatio.x;
		vPos.y = _vBGPos.y - _vRTSize.y * 0.43f / vRatio.y;
	}
	break;

	case SHOP_ENTER:
	{
		vPos.x = _vBGPos.x + _vRTSize.x * 0.06f / vRatio.x;
		vPos.y = _vBGPos.y - _vRTSize.y * 0.43f / vRatio.y;
	}
	break;

	}

	Get_Transform()->Set_State(CTransform::STATE_POSITION, XMVectorSet(vPos.x, vPos.y, 0.f, 1.f));

}

HRESULT CShopPanel_BG_New::Ready_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_Model_2D"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	switch ((CUI::SHOPPANEL)m_iTextureCount)
	{
	case SHOP_BG:
	{
		if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_Texture_ShopBG"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
	break;

	case SHOP_DIALOGUEBG:
	{
		if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_Texture_ShopDialogue"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;

	}
	break;

	

	case SHOP_BULB:
	{
		if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_Texture_ShopBulb"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;


	}
	break;

	case SHOP_BACKESC:
	{
		if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_Texture_BACK"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
	break;

	case SHOP_BACKARROW:
	{
		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ESCBackArrow"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
	break;

	case SHOP_ENTER:
	{
		if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_Texture_Enter"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
	break;
	case SHOP_ESCBG:
	{
		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_IconBG"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;

	}
	break;
	case SHOP_ENTERBG:
	{
		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_IconBG"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
	break;
	} 
	
	return S_OK;
}


CShopPanel_BG_New* CShopPanel_BG_New::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CShopPanel_BG_New* pInstance = new CShopPanel_BG_New(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CShopPanel_BG_New Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CShopPanel_BG_New::Clone(void* _pArg)
{
	CShopPanel_BG_New* pInstance = new CShopPanel_BG_New(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CShopPanel_BG_New Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CShopPanel_BG_New::Free()
{
	//CSection_Manager::GetInstance()->Remove_GameObject_ToCurSectionLayer(this);
	__super::Free();
}



HRESULT CShopPanel_BG_New::Cleanup_DeadReferences()
{
	return S_OK;
}

