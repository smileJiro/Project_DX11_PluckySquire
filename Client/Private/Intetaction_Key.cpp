#include "stdafx.h"
#include "Interaction_Key.h"
#include "GameInstance.h"
#include "UI_Manager.h"



CInteraction_Key::CInteraction_Key(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CUI (_pDevice, _pContext)
{
}

CInteraction_Key::CInteraction_Key(const CInteraction_Key& _Prototype)
	: CUI ( _Prototype )
{
}

HRESULT CInteraction_Key::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInteraction_Key::Initialize(void* _pArg)
{
	UIOBJDESC* pDesc = static_cast<UIOBJDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

void CInteraction_Key::Priority_Update(_float _fTimeDelta)
{
}

void CInteraction_Key::Update(_float _fTimeDelta)
{
	CUI_Manager* pUIManager = CUI_Manager::GetInstance();

	if ((true == pUIManager->Get_StampHave(0) ||
		true == pUIManager->Get_StampHave(1)))
	{
		ChangeStampWord();
	}
}

void CInteraction_Key::Late_Update(_float _fTimeDelta)
{
	CUI_Manager* pUIManager = CUI_Manager::GetInstance();

	if ((true == pUIManager->Get_StampHave(0) ||
		true == pUIManager->Get_StampHave(1)))
	{
		__super::Late_Update(_fTimeDelta);
	}
	
}

HRESULT CInteraction_Key::Render()
{
	CUI_Manager* pUIManager = CUI_Manager::GetInstance();
	if (false == pUIManager->Get_StampHave(0) && false == pUIManager->Get_StampHave(1))
		return S_OK;

	if (FAILED(m_pControllerTransform->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;


	m_pShaderCom->Begin((_uint)PASS_VTXPOSTEX::DEFAULT);
	m_pVIBufferCom->Bind_BufferDesc();
	m_pVIBufferCom->Render();

	

	return S_OK;
}



HRESULT CInteraction_Key::Ready_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_Model_2D"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Interact_Book"),
		TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}


CInteraction_Key* CInteraction_Key::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CInteraction_Key* pInstance = new CInteraction_Key(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CInteraction_Key Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CInteraction_Key::Clone(void* _pArg)
{
	CInteraction_Key* pInstance = new CInteraction_Key(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CInteraction_Key Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CInteraction_Key::Free()
{
	
	__super::Free();
	
}

HRESULT CInteraction_Key::Cleanup_DeadReferences()
{

	return S_OK;
}

void CInteraction_Key::ChangeStampWord()
{
	CUI_Manager::STAMP eStamp;
	eStamp = CUI_Manager::GetInstance()->Get_StampIndex();

	if ((CUI_Manager::STAMP)m_iPreStamp != eStamp)
	{

		if (CUI_Manager::STAMP::STAMP_BOMB == eStamp)
		{
			m_strStampKeyInfomation = TEXT("ÆøÅº ¸¸µé±â");
		}
		else if (CUI_Manager::STAMP::STAMP_BOMB == eStamp)
		{
			m_strStampKeyInfomation = TEXT("µµÀå Âï±â");
		}

		m_iPreStamp = (_uint)eStamp;
	}

}
