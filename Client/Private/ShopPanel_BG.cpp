#include "stdafx.h"
#include "ShopPanel_BG.h"
#include "Section_2D.h"
#include "UI_Manager.h"




CShopPanel_BG::CShopPanel_BG(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CShopPanel(_pDevice, _pContext)
{
}

CShopPanel_BG::CShopPanel_BG(const CShopPanel_BG& _Prototype)
	: CShopPanel(_Prototype)
{
}

HRESULT CShopPanel_BG::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CShopPanel_BG::Initialize(void* _pArg)
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

	

	return S_OK;
}

void CShopPanel_BG::Priority_Update(_float _fTimeDelta)
{
}

void CShopPanel_BG::Child_Update(_float _fTimeDelta)
{
	isRender();
}

void CShopPanel_BG::Child_LateUpdate(_float _fTimeDelta)
{
	if (true == m_isRender)
	{

		_float2 RTSize = _float2(RTSIZE_BOOK2D_X, RTSIZE_BOOK2D_Y);

		if (SHOP_BG == m_eShopPanel)
		{
			// 플레이어의 좌표를 가져와서 계산한다.
			Cal_ShopBGPos(RTSize);
		}
		else if (SHOP_BG != m_eShopPanel)
		{
			// shop_bg의 위치를 가져온다.
			Cal_ShopPartPos(RTSize, Uimgr->Get_ShopPos());

		}
	}
}

HRESULT CShopPanel_BG::Render()
{
	if (true == m_isRender)
		__super::Render(0, PASS_VTXPOSTEX::DEFAULT);

	return S_OK;
}

void CShopPanel_BG::isRender()
{
	if (m_isRender == false)
	{

		/* 나중에 수정 필요 */
		_float2 RTSize = _float2(RTSIZE_BOOK2D_X, RTSIZE_BOOK2D_Y);

		m_isRender = true;
		CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this, CSection_2D::SECTION_2D_UI);
		Change_BookScale_ForShop(RTSize);	

	}
	else if (m_isRender == true)
	{
		m_isRender = false;
		CSection_Manager::GetInstance()->Remove_GameObject_ToCurSectionLayer(this);
	}
		
}

void CShopPanel_BG::Cal_ShopBGPos(_float2 _vRTSize)
{
	// 플레이어 Position
	_float2 vPos = { 0.f, 0.f };
	vPos.x = Uimgr->Get_Player()->Get_Transform()->Get_State(CTransform_2D::STATE_POSITION).m128_f32[0];
	vPos.y = Uimgr->Get_Player()->Get_Transform()->Get_State(CTransform_2D::STATE_POSITION).m128_f32[1];

	vPos.x -= _vRTSize.x * 0.2f;
	vPos.y += _vRTSize.y * 0.11f;

	m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(vPos.x, vPos.y, 0.f, 1.f));
	Uimgr->Set_ShopPos(vPos);
}

void CShopPanel_BG::Cal_ShopPartPos(_float2 _vRTSize, _float2 _vBGPos)
{
	_float2 vPos = { 0.f, 0.f };

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
		vPos.y = _vBGPos.y - _vRTSize.y * 0.32f;
	}
	break;

	case SHOP_CHOOSEBG:
	{
		vPos.x = _vBGPos.x + _vRTSize.x * 0.15f;
		vPos.y = _vBGPos.y - _vRTSize.y * 0.32f;
	}
	break;

	case SHOP_BULB:
	{
		vPos.x = _vBGPos.x + _vRTSize.x * 0.12f;
		vPos.y = _vBGPos.y + _vRTSize.y * 0.25f;
	}
	break;

	case SHOP_ESCBG:
	{
		vPos.x = _vBGPos.x - _vRTSize.x * 0.04f;
		vPos.y = _vBGPos.y - _vRTSize.y * 0.43f;
	}
	break;

	case SHOP_BACKESC:
	{
		vPos.x = _vBGPos.x - _vRTSize.x * 0.04f;
		vPos.y = _vBGPos.y - _vRTSize.y * 0.43f;
	}
	break;

	case SHOP_BACKARROW:
	{
		vPos.x = _vBGPos.x - _vRTSize.x * 0.08f;
		vPos.y = _vBGPos.y - _vRTSize.y * 0.43f;
	}
	break;

	case SHOP_ENTERBG:
	{
		vPos.x = _vBGPos.x + _vRTSize.x * 0.06f;
		vPos.y = _vBGPos.y - _vRTSize.y * 0.43f;
	}
	break;

	case SHOP_ENTER:
	{
		vPos.x = _vBGPos.x + _vRTSize.x * 0.06f;
		vPos.y = _vBGPos.y - _vRTSize.y * 0.43f;
	}
	break;

	}

	Get_Transform()->Set_State(CTransform::STATE_POSITION, XMVectorSet(vPos.x, vPos.y, 0.f, 1.f));

}

HRESULT CShopPanel_BG::Ready_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderComs[COORDINATE_2D]))))
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


CShopPanel_BG* CShopPanel_BG::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CShopPanel_BG* pInstance = new CShopPanel_BG(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CShopPanel_BG Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CShopPanel_BG::Clone(void* _pArg)
{
	CShopPanel_BG* pInstance = new CShopPanel_BG(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CShopPanel_BG Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CShopPanel_BG::Free()
{
	//CSection_Manager::GetInstance()->Remove_GameObject_ToCurSectionLayer(this);
	__super::Free();
}



HRESULT CShopPanel_BG::Cleanup_DeadReferences()
{
	return S_OK;
}

