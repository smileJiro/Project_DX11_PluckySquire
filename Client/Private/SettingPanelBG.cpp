#include "stdafx.h"
#include "SettingPanelBG.h"




CSettingPanelBG::CSettingPanelBG(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CSettingPanel(_pDevice, _pContext)
{
}

CSettingPanelBG::CSettingPanelBG(const CSettingPanelBG& _Prototype)
	: CSettingPanel(_Prototype)
{
}

HRESULT CSettingPanelBG::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSettingPanelBG::Initialize(void* _pArg)
{
	UIOBJDESC* pDesc = static_cast<UIOBJDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	

	return S_OK;
}

void CSettingPanelBG::Priority_Update(_float _fTimeDelta)
{
}

void CSettingPanelBG::Update(_float _fTimeDelta)
{

	if (KEY_DOWN(KEY::ESC))
	{
		isRender();
	}

}

void CSettingPanelBG::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT CSettingPanelBG::Render(_int _index)
{
	if (true == m_isRender)
		__super::Render(_index);

	return S_OK;
}



void CSettingPanelBG::isRender()
{
	if (m_isRender == false)
	{
		m_isRender = true;
	}
	else if (m_isRender == true)
		m_isRender = false;
}

HRESULT CSettingPanelBG::Ready_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderComs[COORDINATE_2D]))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_Model_2D"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_Texture_OptionBG"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}


CSettingPanelBG* CSettingPanelBG::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CSettingPanelBG* pInstance = new CSettingPanelBG(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CSettingPanelBG Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CSettingPanelBG::Clone(void* _pArg)
{
	CSettingPanelBG* pInstance = new CSettingPanelBG(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CSettingPanelBG Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CSettingPanelBG::Free()
{


	__super::Free();
}

HRESULT CSettingPanelBG::Cleanup_DeadReferences()
{
	return S_OK;
}

