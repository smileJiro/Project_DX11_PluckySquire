#include "stdafx.h"
#include "UI_JotMain.h"
#include "ModelObject.h"
#include "GameInstance.h"
#include "Section_Manager.h"
#include "UI_Manager.h"
#include "2DModel.h"

CUI_JotMain::CUI_JotMain(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CUI(_pDevice, _pContext)
{
}

CUI_JotMain::CUI_JotMain(const CUI_JotMain& _Prototype)
	:CUI(_Prototype)
{
}



HRESULT CUI_JotMain::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_JotMain::Initialize(void* _pArg)
{
	UIOBJDESC* pDesc = static_cast<UIOBJDESC*>(_pArg);




	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_isRender = true;

	return S_OK;
}

void CUI_JotMain::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);
}

void CUI_JotMain::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
}

void CUI_JotMain::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT CUI_JotMain::Render()
{
	int a = 0;
	//Register_RenderGroup()
	__super::Render();;
	return S_OK;
}



HRESULT CUI_JotMain::Ready_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderComs[COORDINATE_2D]))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_Model_2D"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(Add_Component(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo_WhiteFlower1"),
		TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}




CUI_JotMain* CUI_JotMain::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CUI_JotMain* pInstance = new CUI_JotMain(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CPick_Bulb Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CUI_JotMain::Clone(void* _pArg)
{
	CUI_JotMain* pInstance = new CUI_JotMain(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CUI_JotMain Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CUI_JotMain::Free()
{
	__super::Free();
}

HRESULT CUI_JotMain::Cleanup_DeadReferences()
{
	return S_OK;
}
