#include "stdafx.h"
#include "StampKey_Q.h"
#include "GameInstance.h"
#include "UI_Manager.h"



CStampKey_Q::CStampKey_Q(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CUI (_pDevice, _pContext)
{
}

CStampKey_Q::CStampKey_Q(const CStampKey_Q& _Prototype)
	: CUI ( _Prototype )
{
}

HRESULT CStampKey_Q::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStampKey_Q::Initialize(void* _pArg)
{
	UIOBJDESC* pDesc = static_cast<UIOBJDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

void CStampKey_Q::Priority_Update(_float _fTimeDelta)
{
}

void CStampKey_Q::Update(_float _fTimeDelta)
{
	ChangeStampWord();
}

void CStampKey_Q::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT CStampKey_Q::Render()
{
	CUI_Manager* pUIManager = CUI_Manager::GetInstance();
	if (false == pUIManager->Get_StampHave(0) || false == pUIManager->Get_StampHave(1))
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



HRESULT CStampKey_Q::Ready_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_Model_2D"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_Texture_Interact_Book"),
		TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}


CStampKey_Q* CStampKey_Q::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CStampKey_Q* pInstance = new CStampKey_Q(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CStampKey_Q Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CStampKey_Q::Clone(void* _pArg)
{
	CStampKey_Q* pInstance = new CStampKey_Q(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CStampKey_Q Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CStampKey_Q::Free()
{
	
	__super::Free();
	
}

HRESULT CStampKey_Q::Cleanup_DeadReferences()
{

	return S_OK;
}

void CStampKey_Q::ChangeStampWord()
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
