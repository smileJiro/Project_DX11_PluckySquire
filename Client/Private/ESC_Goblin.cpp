#include "stdafx.h"
#include "ESC_Goblin.h"



CESC_Goblin::CESC_Goblin(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CSettingPanel(_pDevice, _pContext)
{
}

CESC_Goblin::CESC_Goblin(const CESC_Goblin& _Prototype)
	: CSettingPanel(_Prototype)
{
}

HRESULT CESC_Goblin::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CESC_Goblin::Initialize(void* _pArg)
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

void CESC_Goblin::Priority_Update(_float _fTimeDelta)
{

}

void CESC_Goblin::Update(_float _fTimeDelta)
{
	if (m_isRender == true)
	{
		_float2 vPos = {};
		vPos.x = g_iWinSizeX - g_iWinSizeX / 2.8f;
		vPos.y = m_pControllerTransform->Get_State(CTransform::STATE_POSITION).m128_f32[1];

		if (false == m_isWait && false == m_isUp && false == m_isDown)
		{
			m_isWait = true;
			m_fWaitGoblinTime += _fTimeDelta;
		}
		else if (true == m_isWait)
		{
			m_fWaitGoblinTime += _fTimeDelta * 100;

			if (2.f <= m_fWaitGoblinTime)
			{
				m_isUp = true;
				m_isWait = false;
				m_fWaitGoblinTime = 0;
			}
		}
		else if (true == m_isUp)
		{
			vPos.y -= _fTimeDelta * 50.f;

			if (g_iWinSizeY / 6.5f >= vPos.y)
			{
				m_isUp = false;
				m_isMWait = true;
				
			}

			m_pControllerTransform->Set_State(CTransform::STATE_POSITION,
				XMVectorSet(vPos.x, vPos.y, 0.f, 1.f));
		}
		else if (true == m_isMWait)
		{
			m_fWaitGoblinTime += _fTimeDelta * 100;

			if (2.f <= m_fWaitGoblinTime)
			{
				m_isDown = true;
				m_isMWait = false;
				m_fWaitGoblinTime = 0;
			}
		}
		else if (true == m_isDown)
		{
			vPos.y += _fTimeDelta * 50.f;

			if (g_iWinSizeY / 4.2f <= vPos.y)
			{
				m_isDown = false;
				m_isWait = true;
				m_fWaitGoblinTime = 0;
			}

			m_pControllerTransform->Set_State(CTransform::STATE_POSITION,
				XMVectorSet(vPos.x, vPos.y, 0.f, 1.f));

		}
	}
	
	
	__super::Update(_fTimeDelta);

}

void CESC_Goblin::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT CESC_Goblin::Render()
{
	if (true == m_isRender)
		__super::Render();

	return S_OK;
}



HRESULT CESC_Goblin::Ready_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderComs[COORDINATE_2D]))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_Model_2D"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ESCGoblin"),
		TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}


CESC_Goblin* CESC_Goblin::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CESC_Goblin* pInstance = new CESC_Goblin(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CESC_Goblin Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CESC_Goblin::Clone(void* _pArg)
{
	CESC_Goblin* pInstance = new CESC_Goblin(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CESC_Goblin Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CESC_Goblin::Free()
{
	__super::Free();
}

