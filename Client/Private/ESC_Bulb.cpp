#include "stdafx.h"
#include "ESC_Bulb.h"



ESC_Bulb::ESC_Bulb(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CSettingPanel(_pDevice, _pContext)
{
}

ESC_Bulb::ESC_Bulb(const ESC_Bulb& _Prototype)
	: CSettingPanel(_Prototype)
{
}

HRESULT ESC_Bulb::Initialize_Prototype()
{
	return S_OK;
}

HRESULT ESC_Bulb::Initialize(void* _pArg)
{
	UIOBJDESC* pDesc = static_cast<UIOBJDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;


	if (FAILED(Ready_Components()))
		return E_FAIL;

	

	return S_OK;
}

void ESC_Bulb::Priority_Update(_float _fTimeDelta)
{
}

void ESC_Bulb::Update(_float _fTimeDelta)
{
	if (true == m_isActive)
	{
		// 플레이어 체력 동기화 진행.

		__super::Update(_fTimeDelta);
	}
	
	

	
}

void ESC_Bulb::Late_Update(_float _fTimeDelta)
{
	if (true == m_isRender)
		__super::Late_Update(_fTimeDelta);
}

HRESULT ESC_Bulb::Render()
{
	if (true == m_isRender)
		__super::Render();

	return S_OK;
}



HRESULT ESC_Bulb::Ready_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderComs[COORDINATE_2D]))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_Model_2D"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_Texture_ESCBulb"),
		TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}


ESC_Bulb* ESC_Bulb::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	ESC_Bulb* pInstance = new ESC_Bulb(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created ESC_Bulb Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* ESC_Bulb::Clone(void* _pArg)
{
	ESC_Bulb* pInstance = new ESC_Bulb(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone ESC_Bulb Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void ESC_Bulb::Free()
{


	__super::Free();
}

//HRESULT ESC_Bulb::Cleanup_DeadReferences()
//{
//	return S_OK;
//}

