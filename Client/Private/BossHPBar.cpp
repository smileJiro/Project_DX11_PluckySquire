#include "stdafx.h"
#include "BossHPBar.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "PlayerData_Manager.h"



CBossHPBar::CBossHPBar(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CUI (_pDevice, _pContext)
{
}

CBossHPBar::CBossHPBar(const CBossHPBar& _Prototype)
	: CUI ( _Prototype )
{
}

HRESULT CBossHPBar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBossHPBar::Initialize(void* _pArg)
{
	UIOBJDESC* pDesc = static_cast<UIOBJDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_isMultextrue = pDesc->isMultiTextue;

	m_vBossHPBarScale = _float2(pDesc->fSizeX, pDesc->fSizeY);
	m_vOriginalPos = _float2(pDesc->fX, pDesc->fY);

	if (true == m_isMultextrue)
		m_iBossCurHP = m_iBossMaxHP = Uimgr->Get_BossForUI()->Get_Stat().iMaxHP;


	if (FAILED(Ready_Components()))
		return E_FAIL;


	

	return S_OK;
}

void CBossHPBar::Priority_Update(_float _fTimeDelta)
{
}

void CBossHPBar::Update(_float _fTimeDelta)
{
	Change_Texture();

	//if (BOSS_STATE::TRANSITION == (BOSS_STATE)Uimgr->Get_BossForUI()->Get_State())
	//	return;

	if (2 == m_iBossPhase)
		return;

	
	__super::Update(_fTimeDelta);


}

void CBossHPBar::Late_Update(_float _fTimeDelta)
{
	if (true == m_isMultextrue)
		Cal_BossHP();

	if (2 == m_iBossPhase)
		return;

	if (m_isRender)
		__super::Late_Update(_fTimeDelta);
}

HRESULT CBossHPBar::Render()
{
	if (BOSS_STATE::TRANSITION == (BOSS_STATE)Uimgr->Get_BossForUI()->Get_State())
		return S_OK;

	if (BOSS_STATE::DEAD == (BOSS_STATE)Uimgr->Get_BossForUI()->Get_State())
		return S_OK;

	if (2 == m_iBossPhase)
		return S_OK;

	__super::Render(m_iTextureCount);

	return S_OK;
}

void CBossHPBar::Set_BossPhase()
{
	if (0 <= m_iBossPhase)
	{
		++m_iBossPhase;
		m_iTextureCount = 1;
	}
		

	if (2 == m_iBossPhase)
		m_iBossPhase = 2;
}



HRESULT CBossHPBar::Ready_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_Model_2D"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (false == m_isMultextrue)
	{
		if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_Texture_BossHPBorder"),
			TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
	else if (true == m_isMultextrue)
	{
		if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_Texture_BossHPBar"),
			TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}



	return S_OK;
}


CBossHPBar* CBossHPBar::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CBossHPBar* pInstance = new CBossHPBar(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CBossHPBar Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CBossHPBar::Clone(void* _pArg)
{
	CBossHPBar* pInstance = new CBossHPBar(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CBossHPBar Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CBossHPBar::Free()
{
	
	__super::Free();
	
}

HRESULT CBossHPBar::Cleanup_DeadReferences()
{

	return S_OK;
}

HRESULT CBossHPBar::Cal_BossHP()
{
	_float2 vPos = { 0.f, 0.f };
	_float originalWidth = m_vBossHPBarScale.x;
	_float vHPRatio = 0.f;

	vHPRatio = (_float)m_iBossCurHP / (_float)m_iBossMaxHP;

	_float fBossHPBarScaleX = originalWidth * vHPRatio;
	_float foffsetX = (originalWidth - fBossHPBarScaleX) * 0.5f;
	_float2 vCalPosition = { m_vOriginalPos.x - foffsetX, m_vOriginalPos.y };

	vCalPosition.x = vCalPosition.x - g_iWinSizeX * 0.5f;
	vCalPosition.y = -vCalPosition.y + g_iWinSizeY * 0.5f;

	m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(vCalPosition.x, vCalPosition.y, 0.f, 1.f));
	m_pControllerTransform->Set_Scale(COORDINATE_2D, _float3(fBossHPBarScaleX, m_vBossHPBarScale.y, 1.f));


	return S_OK;
}

HRESULT CBossHPBar::Change_Texture()
{
	if (true == m_isChangeTexture)
		return S_OK;

	if (false == m_isMultextrue)
		return S_OK;

	if (BOSS_STATE::TRANSITION == (BOSS_STATE)Uimgr->Get_BossForUI()->Get_State())
	{
		if (false == m_isChangeTexture)
		{
			if (0 == m_iTextureCount)
			{
				m_iTextureCount = 1;
				m_isChangeTexture = true;
			}
		}
	}

	return S_OK;
}

