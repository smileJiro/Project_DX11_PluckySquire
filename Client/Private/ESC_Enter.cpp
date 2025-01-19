#include "stdafx.h"
#include "ESC_Enter.h"



CESC_Enter::CESC_Enter(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CSettingPanel(_pDevice, _pContext)
{
}

CESC_Enter::CESC_Enter(const CESC_Enter& _Prototype)
	: CSettingPanel(_Prototype)
{
}

HRESULT CESC_Enter::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CESC_Enter::Initialize(void* _pArg)
{
	UIOBJDESC* pDesc = static_cast<UIOBJDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;


	if (FAILED(Ready_Components()))
		return E_FAIL;

	

	return S_OK;
}

void CESC_Enter::Priority_Update(_float _fTimeDelta)
{
}

void CESC_Enter::Update(_float _fTimeDelta)
{
	if (true == m_isActive)
	{
		// 플레이어 체력 동기화 진행.

		__super::Update(_fTimeDelta);
	}
	
	

	
}

void CESC_Enter::Late_Update(_float _fTimeDelta)
{
	if (true == m_isRender)
		__super::Late_Update(_fTimeDelta);
}

HRESULT CESC_Enter::Render()
{
	if (true == m_isRender)
		__super::Render();

	return S_OK;
}



HRESULT CESC_Enter::Ready_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderComs[COORDINATE_2D]))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_Model_2D"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ESCEnter"),
		TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}


CESC_Enter* CESC_Enter::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CESC_Enter* pInstance = new CESC_Enter(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CESC_Enter Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CESC_Enter::Clone(void* _pArg)
{
	CESC_Enter* pInstance = new CESC_Enter(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CESC_Enter Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CESC_Enter::Free()
{


	__super::Free();
}

//HRESULT CESC_Enter::Cleanup_DeadReferences()
//{
//	return S_OK;
//}

