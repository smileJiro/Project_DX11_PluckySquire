#include "stdafx.h"
#include "BombStamp.h"
#include "GameInstance.h"


CBombStamp::CBombStamp(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CUI (_pDevice, _pContext)
{
}

CBombStamp::CBombStamp(const CBombStamp& _Prototype)
	: CUI ( _Prototype )
{
}

HRESULT CBombStamp::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBombStamp::Initialize(void* _pArg)
{
	UIOBJDESC* pDesc = static_cast<UIOBJDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;



	

	//m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(g_iWinSizeX/ 2, g_iWinSizeY / 2, 1.f, 1.f));
	//m_pControllerTransform->Set_Scale(50.f, 50.f, 1.f);
	return S_OK;
}

void CBombStamp::Priority_Update(_float _fTimeDelta)
{
}

void CBombStamp::Update(_float _fTimeDelta)
{
	if (m_isActive == false)
		return;

	ChangeStamp(_fTimeDelta);
}

void CBombStamp::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT CBombStamp::Render()
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

	__super::Render();
	

	return S_OK;
}



void CBombStamp::ChangeStamp(_float _fTimeDelta)
{
	CUI_Manager::STAMP eStamp;
	eStamp = CUI_Manager::GetInstance()->Get_StampIndex();

	if (m_ePreStamp != eStamp && false == m_isScaling)
	{

		if (eStamp == CUI_Manager::STAMP_STOP)
		{
			//위치 변경이 필요한가요?
			m_fX = g_iWinSizeX / 7.5;
			m_fY = g_iWinSizeY - g_iWinSizeY / 10;

			m_isSmall = true;
			m_isScaling = true;
		}
		else if (eStamp == CUI_Manager::STAMP_BOMB)
		{
			//위치 변경이 필요한가요?
			m_fX = g_iWinSizeX / 7.5;
			m_fY = g_iWinSizeY - g_iWinSizeY / 10;

			m_isBig = true;
			m_isScaling = true;
		}

		m_ePreStamp = eStamp;
	}


	if (true == m_isBig || true == m_isSmall)
	{
		if (true == m_isBig)
		{
			if (m_fSizeX <= 96)
			{
				m_fSizeX += _fTimeDelta * 100;
				m_fSizeY += (_fTimeDelta * 1.54) * 100;

				m_pControllerTransform->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
			}
			else
			{
				m_isBig = false;
				m_isScaling = false;
			}

		}
		else if (true == m_isSmall)
		{
			if (m_fSizeX > 72)
			{
				m_fSizeX -= _fTimeDelta * 100;
				m_fSizeY -= (_fTimeDelta * 1.54) * 100;

				m_pControllerTransform->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
			}
			else
			{
				m_isSmall = false;
				m_isScaling = false;
			}

		}
	}

}

HRESULT CBombStamp::Ready_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderComs[COORDINATE_2D]))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_Model_2D"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_Texture_BombStamp"),
		TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}


CBombStamp* CBombStamp::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CBombStamp* pInstance = new CBombStamp(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CBombStamp Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CBombStamp::Clone(void* _pArg)
{
	CBombStamp* pInstance = new CBombStamp(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CBombStamp Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CBombStamp::Free()
{
	
	__super::Free();
	
}

HRESULT CBombStamp::Cleanup_DeadReferences()
{

	return S_OK;
}
