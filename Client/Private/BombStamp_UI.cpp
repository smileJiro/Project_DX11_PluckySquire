#include "stdafx.h"
#include "BombStamp_UI.h"
#include "GameInstance.h"
#include "UI_Manager.h"


CBombStamp_UI::CBombStamp_UI(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CUI (_pDevice, _pContext)
{
}

CBombStamp_UI::CBombStamp_UI(const CBombStamp_UI& _Prototype)
	: CUI ( _Prototype )
{
}

HRESULT CBombStamp_UI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBombStamp_UI::Initialize(void* _pArg)
{
	UIOBJDESC* pDesc = static_cast<UIOBJDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CBombStamp_UI::Priority_Update(_float _fTimeDelta)
{
}

void CBombStamp_UI::Update(_float _fTimeDelta)
{
	if (m_isActive == false)
		return;

	ChangeStamp(_fTimeDelta);
}

void CBombStamp_UI::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT CBombStamp_UI::Render()
{
	__super::Render();
	return S_OK;
}



void CBombStamp_UI::ChangeStamp(_float _fTimeDelta)
{
	CUI_Manager::STAMP eStamp;
	eStamp = CUI_Manager::GetInstance()->Get_StampIndex();

	if (m_ePreStamp != eStamp && false == m_isScaling)
	{
		if (eStamp == CUI_Manager::STAMP_STOP)
		{
			//위치 변경이 필요한가요?
			m_fX = g_iWinSizeX / 7.5f;
			m_fY = g_iWinSizeY - g_iWinSizeY / 10.f;

			m_isSmall = true;
			m_isScaling = true;
		}
		else if (eStamp == CUI_Manager::STAMP_BOMB)
		{
			//위치 변경이 필요한가요?
			m_fX = g_iWinSizeX / 7.5f;
			m_fY = g_iWinSizeY - g_iWinSizeY / 10.f;

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
				m_fSizeX += _fTimeDelta * 100.f;
				m_fSizeY += (_fTimeDelta * 1.54f) * 100.f;

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
				m_fSizeX -= _fTimeDelta * 100.f;
				m_fSizeY -= (_fTimeDelta * 1.54f) * 100.f;

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

HRESULT CBombStamp_UI::Ready_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
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


CBombStamp_UI* CBombStamp_UI::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CBombStamp_UI* pInstance = new CBombStamp_UI(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CBombStamp Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CBombStamp_UI::Clone(void* _pArg)
{
	CBombStamp_UI* pInstance = new CBombStamp_UI(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CBombStamp Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CBombStamp_UI::Free()
{
	
	__super::Free();
	
}

HRESULT CBombStamp_UI::Cleanup_DeadReferences()
{

	return S_OK;
}
