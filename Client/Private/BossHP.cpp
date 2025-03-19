#include "stdafx.h"
#include "BossHP.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "PlayerData_Manager.h"



CBossHP::CBossHP(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CUI (_pDevice, _pContext)
{
}

CBossHP::CBossHP(const CBossHP& _Prototype)
	: CUI ( _Prototype )
{
}



HRESULT CBossHP::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBossHP::Initialize(void* _pArg)
{
	UIOBJDESC* pDesc = static_cast<UIOBJDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Create_BossHPBar()))
		return E_FAIL;

	m_pButterGtump = Uimgr->Get_BossForUI();
	Safe_AddRef(m_pButterGtump);

	return S_OK;
}

void CBossHP::Priority_Update(_float _fTimeDelta)
{
}

void CBossHP::Update(_float _fTimeDelta)
{
	m_pBossHPBar->Set_BossCurHP(m_pButterGtump->Get_Stat().iHP);

	if (false == m_isNextphase &&BOSS_STATE::TRANSITION == (BOSS_STATE)m_pButterGtump->Get_State())
	{
		if (2 == m_pBossHPBar->Get_BossPhase())
			return;

		m_isNextphase = true;

		m_pBossHPBar->Set_BossPhase();
	}
	else if (false == m_isDeadPhase && BOSS_STATE::DEAD == (BOSS_STATE)m_pButterGtump->Get_State())
	{
		if (2 == m_pBossHPBar->Get_BossPhase())
			return;

		m_isDeadPhase = true;
		m_pBossHPBar->Set_BossPhase();
	}
		
}

void CBossHP::Late_Update(_float _fTimeDelta)
{
	if (m_isRender)
		__super::Late_Update(_fTimeDelta);
}

HRESULT CBossHP::Render()
{

	return S_OK;
}



HRESULT CBossHP::Ready_Components()
{

	return S_OK;
}


CBossHP* CBossHP::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CBossHP* pInstance = new CBossHP(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CBossHP Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CBossHP::Clone(void* _pArg)
{
	CBossHP* pInstance = new CBossHP(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CBossHP Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CBossHP::Free()
{
	Safe_Release(m_pBossHPBar);
	Safe_Release(m_pButterGtump);
	__super::Free();
	
}

HRESULT CBossHP::Cleanup_DeadReferences()
{

	return S_OK;
}


HRESULT CBossHP::Create_BossHPBar()
{
	CGameObject* pBossHPBar = { nullptr };

	CUI::UIOBJDESC pDesc = {};
	pDesc.iCurLevelID = LEVEL_ID::LEVEL_CHAPTER_8;
	pDesc.strLayerTag = TEXT("Layer_UI");
	pDesc.fX = g_iWinSizeX / 2.f;
	pDesc.fY = g_iWinSizeY / 10.f;

	pDesc.fSizeX = 1080.f / 1.5f;
	pDesc.fSizeY = 80.f / 1.5f;
	pDesc.isMultiTextue = false;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHAPTER_8, TEXT("Prototype_GameObject_BossHPBar"), pDesc.iCurLevelID, pDesc.strLayerTag, &pDesc)))
		return E_FAIL;

	pDesc.fX = g_iWinSizeX / 2.01f;
	pDesc.fY = g_iWinSizeY / 10.02f;
	pDesc.fSizeX = 1024.f / 1.5f;
	pDesc.fSizeY = 32.f / 1.5f;
	pDesc.isMultiTextue = true;
	pDesc.iTextureCount = 0;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHAPTER_8, TEXT("Prototype_GameObject_BossHPBar"), pDesc.iCurLevelID, pDesc.strLayerTag, &pBossHPBar, &pDesc)))
		return E_FAIL;

	m_pBossHPBar = static_cast<CBossHPBar*>(pBossHPBar);
	Safe_AddRef(m_pBossHPBar);

	return S_OK;
}