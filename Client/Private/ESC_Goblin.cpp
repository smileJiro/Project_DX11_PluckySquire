#include "stdafx.h"
#include "ESC_Goblin.h"
#include "UI_Manager.h"



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
	if (KEY_DOWN(KEY::ESC))
	{
		m_bPreRender = m_isRender;
		false == m_isRender ? m_isRender = true : m_isRender = false;
	}

	if (KEY_DOWN(KEY::E) && 0 == Uimgr->Get_SettingPanelIndex() && true == m_isRender)
	{
		m_bPreRender = m_isRender;
		false == m_isRender ? m_isRender = true : m_isRender = false;
	}
	//m_bPreRender = m_isRender;

	if (m_isRender == true)
	{
		_float2 vPos = {};
		vPos.x = g_iWinSizeX - g_iWinSizeX / 2.8f;
		vPos.y = m_pControllerTransform->Get_State(CTransform::STATE_POSITION).m128_f32[1];
		vPos.y = -vPos.y + g_iWinSizeY * 0.5f;

		switch (m_eState)
		{
		case STATE_WAIT:
			m_fWaitGoblinTime += _fTimeDelta;
			if (m_fWaitGoblinTime >= 2.f)
			{
				m_eState = STATE_UP;
				m_fWaitGoblinTime = 0.f;
			}
			break;

		case STATE_UP: // 위로 이동
			vPos.y -= _fTimeDelta * 30.f;
			if (vPos.y <= g_iWinSizeY / 6.5f)
			{
				m_eState = STATE_MWAIT;
				m_fWaitGoblinTime = 0.f;
			}
			break;

		case STATE_MWAIT: // 중간 대기 상태
			m_fWaitGoblinTime += _fTimeDelta;
			if (m_fWaitGoblinTime >= 2.5f)
			{
				m_eState = STATE_DOWN;
				m_fWaitGoblinTime = 0.f;
			}
			break;

		case STATE_DOWN: // 아래로 이동
			vPos.y += _fTimeDelta * 30.f;
			if (vPos.y >= g_iWinSizeY / 4.2f)
			{
				m_eState = STATE_WAIT;
				m_fWaitGoblinTime = 0.f;
			}
			break;

		default:
			break;
		}

		// 위치 업데이트 (모든 상태에서 적용)

		vPos.x = vPos.x - g_iWinSizeX * 0.5f;
		vPos.y = -vPos.y + g_iWinSizeY * 0.5f;
		m_pControllerTransform->Set_State(CTransform::STATE_POSITION,
			XMVectorSet(vPos.x, vPos.y, 0.f, 1.f));
	}
	else 
	{
		if (m_bPreRender == true)
		{
			_float2 vInitialPos = { g_iWinSizeX - g_iWinSizeX / 2.8f, g_iWinSizeY / 4.2f };

			vInitialPos.x = vInitialPos.x - g_iWinSizeX * 0.5f;
			vInitialPos.y = -vInitialPos.y + g_iWinSizeY * 0.5f;
			m_pControllerTransform->Set_State(CTransform::STATE_POSITION,
				XMVectorSet(vInitialPos.x, vInitialPos.y, 0.f, 1.f));

			// 모든 상태 초기화
			m_eState = STATE_WAIT;
			m_fWaitGoblinTime = 0.f;
		}
	}
	m_bPreRender = m_isRender;
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
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
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

