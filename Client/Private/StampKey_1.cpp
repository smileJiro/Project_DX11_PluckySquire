#include "stdafx.h"
#include "StampKey_1.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "PlayerData_Manager.h"



CStampKey_1::CStampKey_1(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CUI (_pDevice, _pContext)
{
}

CStampKey_1::CStampKey_1(const CStampKey_1& _Prototype)
	: CUI ( _Prototype )
{
}

HRESULT CStampKey_1::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStampKey_1::Initialize(void* _pArg)
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

void CStampKey_1::Priority_Update(_float _fTimeDelta)
{
}

void CStampKey_1::Update(_float _fTimeDelta)
{
	CBook* pBook = Uimgr->Get_Book();
	CPlayerData_Manager* pPDM = CPlayerData_Manager::GetInstance();
	if (nullptr == pBook)
		return;

	if (true == pBook->Get_PlayerAbove())
	{
		if (true == pPDM->Is_Own(CPlayerData_Manager::BOMB_STAMP) && true == pPDM->Is_Own(CPlayerData_Manager::STOP_STAMP))
		{
			Change_StampKeyWord();
		}
	}

}

void CStampKey_1::Late_Update(_float _fTimeDelta)
{
	CBook* pBook = Uimgr->Get_Book();
	CPlayerData_Manager* pPDM = CPlayerData_Manager::GetInstance();
	if (nullptr == pBook)
		return;

	if (true == pBook->Get_PlayerAbove())
	{
		if (true == pPDM->Is_Own(CPlayerData_Manager::BOMB_STAMP) && true == pPDM->Is_Own(CPlayerData_Manager::STOP_STAMP))
			__super::Late_Update(_fTimeDelta);
	}
	
}

HRESULT CStampKey_1::Render()
{
	CUI_Manager* pUIManager = CUI_Manager::GetInstance();
	CPlayerData_Manager* pPDM = CPlayerData_Manager::GetInstance();
	if (false == pPDM->Is_Own(CPlayerData_Manager::BOMB_STAMP) || false == pPDM->Is_Own(CPlayerData_Manager::STOP_STAMP))
		return S_OK;

	if (FAILED(m_pControllerTransform->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", m_iTextureKind)))
		return E_FAIL;


	m_pShaderCom->Begin((_uint)PASS_VTXPOSTEX::DEFAULT);
	m_pVIBufferCom->Bind_BufferDesc();
	m_pVIBufferCom->Render();


	/*
		pDesc.fX = g_iWinSizeX / 18.f;
	pDesc.fY = g_iWinSizeY - g_iWinSizeY / 20.f;
	*/


	m_pGameInstance->Render_Font(TEXT("Font30"), m_strStampInfo.c_str(), _float2(g_iWinSizeX / 13.f - 2.5f, g_iWinSizeY - g_iWinSizeY / 14.f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
	m_pGameInstance->Render_Font(TEXT("Font30"), m_strStampInfo.c_str(), _float2(g_iWinSizeX / 13.f + 2.5f, g_iWinSizeY - g_iWinSizeY / 14.f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
	m_pGameInstance->Render_Font(TEXT("Font30"), m_strStampInfo.c_str(), _float2(g_iWinSizeX / 13.f, g_iWinSizeY - g_iWinSizeY / 14.f - 2.5f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
	m_pGameInstance->Render_Font(TEXT("Font30"), m_strStampInfo.c_str(), _float2(g_iWinSizeX / 13.f, g_iWinSizeY - g_iWinSizeY / 14.f + 2.5f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));

	m_pGameInstance->Render_Font(TEXT("Font30"), m_strStampInfo.c_str(), _float2(g_iWinSizeX / 13.f, g_iWinSizeY - g_iWinSizeY / 14.f), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));

	//__super::Render();
	

	return S_OK;
}



HRESULT CStampKey_1::Ready_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_Model_2D"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_StampChange"),
		TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

void CStampKey_1::Change_StampKeyWord()
{
	CPlayer* pPlayer = Uimgr->Get_Player();
	if (nullptr == pPlayer)
	{
		assert(pPlayer);
	}

	if (m_ePreStampChoose == pPlayer->Get_CurrentStampType())
	{
		return;
	}
	else
	{
		if (CPlayer::PLAYER_PART::PLAYER_PART_STOP_STMAP == pPlayer->Get_CurrentStampType())
		{
			m_iTextureKind = 1;
			m_ePreStampChoose = CPlayer::PLAYER_PART::PLAYER_PART_STOP_STMAP;
		}
		else if (CPlayer::PLAYER_PART::PLAYER_PART_BOMB_STMAP == pPlayer->Get_CurrentStampType())
		{
			m_iTextureKind = 0;
			m_ePreStampChoose = CPlayer::PLAYER_PART::PLAYER_PART_BOMB_STMAP;
		}
	}

}


CStampKey_1* CStampKey_1::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CStampKey_1* pInstance = new CStampKey_1(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CStampKey_1 Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CStampKey_1::Clone(void* _pArg)
{
	CStampKey_1* pInstance = new CStampKey_1(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CStampKey_1 Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CStampKey_1::Free()
{
	
	__super::Free();
	
}

HRESULT CStampKey_1::Cleanup_DeadReferences()
{

	return S_OK;
}
