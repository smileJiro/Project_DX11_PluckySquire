#include "stdafx.h"
#include "Pick_Bulb.h"
#include "GameInstance.h"

#include "PlayerData_Manager.h"



CPick_Bulb::CPick_Bulb(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CUI (_pDevice, _pContext)
{
}

CPick_Bulb::CPick_Bulb(const CPick_Bulb& _Prototype)
	: CUI ( _Prototype )
{
}

HRESULT CPick_Bulb::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPick_Bulb::Initialize(void* _pArg)
{
	UIOBJDESC* pDesc = static_cast<UIOBJDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_isRender = true;

	return S_OK;
}

void CPick_Bulb::Priority_Update(_float _fTimeDelta)
{
}

void CPick_Bulb::Update(_float _fTimeDelta)
{
	m_iCurBulbCount = CPlayerData_Manager::GetInstance()->Get_BulbCount();

	if (m_iPreBulbCount != m_iCurBulbCount)
	{
		m_fAmountTime = 0.f;
		m_isRender = true;


		m_iPreBulbCount = m_iCurBulbCount;
	}

	if (m_isRender)
	{
		m_fAmountTime += _fTimeDelta;

		if (3.f < m_fAmountTime)
		{
			m_isRender = false;
		}
	}


}

void CPick_Bulb::Late_Update(_float _fTimeDelta)
{
	if (m_isRender)
		__super::Late_Update(_fTimeDelta);
}

HRESULT CPick_Bulb::Render()
{
	
	//if (FAILED(m_pControllerTransform->Bind_ShaderResource(m_pShaderComs[COORDINATE_2D], "g_WorldMatrix")))
	//	return E_FAIL;
	//
	//if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
	//	return E_FAIL;
	//
	//if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
	//	return E_FAIL;
	//
	//if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderComs[COORDINATE_2D], "g_DiffuseTexture")))
	//	return E_FAIL;
	//
	//
	//m_pShaderComs[COORDINATE_2D]->Begin((_uint)PASS_VTXPOSTEX::DEFAULT);
	//m_pVIBufferCom->Bind_BufferDesc();
	//m_pVIBufferCom->Render();
	if (true == m_isRender)
	{
		__super::Render(0, PASS_VTXPOSTEX::DEFAULT);

		_wstring szBulbCount = to_wstring(CPlayerData_Manager::GetInstance()->Get_BulbCount());
		wsprintf(m_tFont, szBulbCount.c_str());
		m_pGameInstance->Render_Font(TEXT("Font35"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 11.5f, g_iWinSizeY / 14.f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
	}
	


	/*
		pDesc.fX = g_iWinSizeX - g_iWinSizeX / 12.f;
	pDesc.fY = g_iWinSizeY / 10.f;
	*/

	return S_OK;
}



HRESULT CPick_Bulb::Ready_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderComs[COORDINATE_2D]))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_Model_2D"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_Texture_PickBulb"),
		TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}


CPick_Bulb* CPick_Bulb::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CPick_Bulb* pInstance = new CPick_Bulb(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CPick_Bulb Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CPick_Bulb::Clone(void* _pArg)
{
	CPick_Bulb* pInstance = new CPick_Bulb(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CPick_Bulb Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CPick_Bulb::Free()
{
	
	__super::Free();
	
}

HRESULT CPick_Bulb::Cleanup_DeadReferences()
{

	return S_OK;
}
