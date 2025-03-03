#include "stdafx.h"
#include "ESC_HeartPoint.h"
#include "UI_Manager.h"



ESC_HeartPoint::ESC_HeartPoint(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CSettingPanel(_pDevice, _pContext)
{
}

ESC_HeartPoint::ESC_HeartPoint(const ESC_HeartPoint& _Prototype)
	: CSettingPanel(_Prototype)
{
}

HRESULT ESC_HeartPoint::Initialize_Prototype()
{
	return S_OK;
}

HRESULT ESC_HeartPoint::Initialize(void* _pArg)
{
	UIOBJDESC* pDesc = static_cast<UIOBJDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_eSettingPanel = pDesc->eSettingPanelKind;
	m_isRender = false;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	

	return S_OK;
}

void ESC_HeartPoint::Priority_Update(_float _fTimeDelta)
{

}

void ESC_HeartPoint::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
}

void ESC_HeartPoint::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT ESC_HeartPoint::Render()
{
	if (true == m_isRender)
		__super::Render(Uimgr->Get_Player()->Get_Stat().iHP);

	return S_OK;
}



HRESULT ESC_HeartPoint::Ready_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_Model_2D"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_Texture_HeartPoint"),
		TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}


ESC_HeartPoint* ESC_HeartPoint::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	ESC_HeartPoint* pInstance = new ESC_HeartPoint(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created ESC_HeartPoint Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* ESC_HeartPoint::Clone(void* _pArg)
{
	ESC_HeartPoint* pInstance = new ESC_HeartPoint(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone ESC_HeartPoint Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void ESC_HeartPoint::Free()
{
	__super::Free();
}

