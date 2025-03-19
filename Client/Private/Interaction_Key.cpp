#include "stdafx.h"
#include "Interaction_Key.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "Book.h"
#include "PlayerData_Manager.h"



CInteraction_Key::CInteraction_Key(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CUI (_pDevice, _pContext)
{
}

CInteraction_Key::CInteraction_Key(const CInteraction_Key& _Prototype)
	: CUI ( _Prototype )
{
}

HRESULT CInteraction_Key::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInteraction_Key::Initialize(void* _pArg)
{
	UIOBJDESC* pDesc = static_cast<UIOBJDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

void CInteraction_Key::Priority_Update(_float _fTimeDelta)
{
}

void CInteraction_Key::Update(_float _fTimeDelta)
{
	CUI_Manager* pUIManager = CUI_Manager::GetInstance();

	if (COORDINATE_3D == pUIManager->Get_Player()->Get_CurCoord())
	{
		CBook* pBook = Uimgr->Get_Book();

		if (nullptr == pBook)
			assert(pBook);

		if (true == pBook->Get_PlayerAbove())
		{
			CPlayer* pPlayer = pUIManager->Get_Player();

			if (false == pPlayer->Is_DetonationMode())
			{
				StampWord();
			}
			else
			{
				ChangeBombWord();
			}
			
			//ChangeBombWord();

		}
		else if (true == pBook->Get_PlayerAround())
		{
			ChangeExitBookState();
		}

		// 도장 변경
		

		// 나가기
		//

		// 터트리기
		//
	}
	
}

void CInteraction_Key::Late_Update(_float _fTimeDelta)
{
	CUI_Manager* pUIManager = CUI_Manager::GetInstance();

	//if (COORDINATE_3D == pUIManager->Get_Player()->Get_CurCoord())
	//{
	//	if ((true == pUIManager->Get_StampHave(0) ||
	//		true == pUIManager->Get_StampHave(1)))
	//	{
	//		__super::Late_Update(_fTimeDelta);
	//	}
	//}






	//CBook* pBook = dynamic_cast<CBook*>(Uimgr->Get_Player()->Get_InteractableObject());
	//
	//if (nullptr == pBook)
	//{
	//	if (true == m_isRender)
	//		m_isRender = false;
	//
	//	return;
	//}

	CBook* pBook = Uimgr->Get_Book();

	if (nullptr == pBook)
		assert(pBook);



	if (true == pBook->Get_PlayerAbove() || true == pBook->Get_PlayerAround()/*&& CPlayerData_Manager::GetInstance()->Is_Own(CPlayerData_Manager::FLIPPING_GLOVE)*/)
	{


		if (COORDINATE_3D == pUIManager->Get_Player()->Get_CurCoord())
		{
			if (true == pBook->Get_PlayerAbove())
			{
				__super::Late_Update(_fTimeDelta);
			}
			else if (true == pBook->Get_PlayerAround())
			{
				if (CPlayer::STATE::TURN_BOOK == Uimgr->Get_Player()->Get_CurrentStateID())
				{
					__super::Late_Update(_fTimeDelta);
				}
			}
		}
	}

	
}

HRESULT CInteraction_Key::Render()
{
	CUI_Manager* pUIManager = CUI_Manager::GetInstance();
	CPlayerData_Manager* pPDM = CPlayerData_Manager::GetInstance();

	if (false == pPDM->Is_Own(CPlayerData_Manager::BOMB_STAMP) && false == pPDM->Is_Own(CPlayerData_Manager::STOP_STAMP))
		return S_OK;

	if (FAILED(m_pControllerTransform->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", m_iDisplayTexture)))
		return E_FAIL;


	m_pShaderCom->Begin((_uint)PASS_VTXPOSTEX::DEFAULT);
	m_pVIBufferCom->Bind_BufferDesc();
	m_pVIBufferCom->Render();

	
	if (TEXT("나가기") == m_strKeyInfomation)
	{
		m_pGameInstance->Render_Font(TEXT("Font30"), m_strKeyInfomation.c_str(), _float2(g_iWinSizeX - g_iWinSizeX / 14.f - 2.5f, g_iWinSizeY - g_iWinSizeY / 14.f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font30"), m_strKeyInfomation.c_str(), _float2(g_iWinSizeX - g_iWinSizeX / 14.f + 2.5f, g_iWinSizeY - g_iWinSizeY / 14.f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font30"), m_strKeyInfomation.c_str(), _float2(g_iWinSizeX - g_iWinSizeX / 14.f, g_iWinSizeY - g_iWinSizeY / 14.f - 2.5f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font30"), m_strKeyInfomation.c_str(), _float2(g_iWinSizeX - g_iWinSizeX / 14.f, g_iWinSizeY - g_iWinSizeY / 14.f + 2.5f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
													 
		m_pGameInstance->Render_Font(TEXT("Font30"), m_strKeyInfomation.c_str(), _float2(g_iWinSizeX - g_iWinSizeX / 14.f, g_iWinSizeY - g_iWinSizeY / 14.f), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
	}
	else if (TEXT("도장 찍기") == m_strKeyInfomation)
	{
		m_pGameInstance->Render_Font(TEXT("Font30"), m_strKeyInfomation.c_str(), _float2(g_iWinSizeX - g_iWinSizeX / 7.4f - 2.5f, g_iWinSizeY - g_iWinSizeY / 14.f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font30"), m_strKeyInfomation.c_str(), _float2(g_iWinSizeX - g_iWinSizeX / 7.4f + 2.5f, g_iWinSizeY - g_iWinSizeY / 14.f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font30"), m_strKeyInfomation.c_str(), _float2(g_iWinSizeX - g_iWinSizeX / 7.4f, g_iWinSizeY - g_iWinSizeY / 14.f - 2.5f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font30"), m_strKeyInfomation.c_str(), _float2(g_iWinSizeX - g_iWinSizeX / 7.4f, g_iWinSizeY - g_iWinSizeY / 14.f + 2.5f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));

		m_pGameInstance->Render_Font(TEXT("Font30"), m_strKeyInfomation.c_str(), _float2(g_iWinSizeX - g_iWinSizeX / 7.4f, g_iWinSizeY - g_iWinSizeY / 14.f), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
	}
	else if (TEXT("폭탄 만들기") == m_strKeyInfomation)
	{
		m_pGameInstance->Render_Font(TEXT("Font30"), m_strKeyInfomation.c_str(), _float2(g_iWinSizeX - g_iWinSizeX / 6.7f - 2.5f, g_iWinSizeY - g_iWinSizeY / 14.f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font30"), m_strKeyInfomation.c_str(), _float2(g_iWinSizeX - g_iWinSizeX / 6.7f + 2.5f, g_iWinSizeY - g_iWinSizeY / 14.f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font30"), m_strKeyInfomation.c_str(), _float2(g_iWinSizeX - g_iWinSizeX / 6.7f, g_iWinSizeY - g_iWinSizeY / 14.f - 2.5f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font30"), m_strKeyInfomation.c_str(), _float2(g_iWinSizeX - g_iWinSizeX / 6.7f, g_iWinSizeY - g_iWinSizeY / 14.f + 2.5f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));

		m_pGameInstance->Render_Font(TEXT("Font30"), m_strKeyInfomation.c_str(), _float2(g_iWinSizeX - g_iWinSizeX / 6.7f, g_iWinSizeY - g_iWinSizeY / 14.f), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
	}
	else if (TEXT("터뜨리기") == m_strKeyInfomation)
	{
		m_pGameInstance->Render_Font(TEXT("Font30"), m_strKeyInfomation.c_str(), _float2(g_iWinSizeX - g_iWinSizeX / 7.3f - 2.5f, g_iWinSizeY - g_iWinSizeY / 14.f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font30"), m_strKeyInfomation.c_str(), _float2(g_iWinSizeX - g_iWinSizeX / 7.3f + 2.5f, g_iWinSizeY - g_iWinSizeY / 14.f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font30"), m_strKeyInfomation.c_str(), _float2(g_iWinSizeX - g_iWinSizeX / 7.3f, g_iWinSizeY - g_iWinSizeY / 14.f - 2.5f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font30"), m_strKeyInfomation.c_str(), _float2(g_iWinSizeX - g_iWinSizeX / 7.3f, g_iWinSizeY - g_iWinSizeY / 14.f + 2.5f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));

		m_pGameInstance->Render_Font(TEXT("Font30"), m_strKeyInfomation.c_str(), _float2(g_iWinSizeX - g_iWinSizeX / 7.3f, g_iWinSizeY - g_iWinSizeY / 14.f), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
	}


	return S_OK;
}



HRESULT CInteraction_Key::Ready_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_Model_2D"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Interact_Key"),
		TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}


CInteraction_Key* CInteraction_Key::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CInteraction_Key* pInstance = new CInteraction_Key(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CInteraction_Key Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CInteraction_Key::Clone(void* _pArg)
{
	CInteraction_Key* pInstance = new CInteraction_Key(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CInteraction_Key Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CInteraction_Key::Free()
{
	
	__super::Free();
	
}

HRESULT CInteraction_Key::Cleanup_DeadReferences()
{

	return S_OK;
}


void CInteraction_Key::StampWord()
{

	CUI_Manager* pUIManager = CUI_Manager::GetInstance();
	CPlayerData_Manager* pPDM = CPlayerData_Manager::GetInstance();
	if ((true == pPDM->Is_Own(CPlayerData_Manager::BOMB_STAMP) ||
		true == pPDM->Is_Own(CPlayerData_Manager::STOP_STAMP)))
	{
		ChangeStampWord();
	}
}

void CInteraction_Key::ChangeStampWord()
{
	CPlayer* pPlayer = Uimgr->Get_Player();

	CPlayer::PLAYER_PART ePlayerPart = pPlayer->Get_CurrentStampType();

	//if (0 != m_iDisplayTexture)
	//{
		m_iDisplayTexture = 0;

		_float2 vPos = { 0.f, 0.f };

		vPos.x = g_iWinSizeX - g_iWinSizeX / 19.f;
		vPos.y = g_iWinSizeY - g_iWinSizeY / 20.f;
		m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(vPos.x - g_iWinSizeX * 0.5f, -vPos.y + g_iWinSizeY * 0.5f, 0.f, 1.f));
	//}

	//if ((CUI_Manager::STAMP)m_iPreStamp != eStamp)
	//{

		if (CPlayer::PLAYER_PART::PLAYER_PART_BOMB_STMAP == ePlayerPart)
		{
			m_strKeyInfomation = TEXT("폭탄 만들기");
		}
		else if (CPlayer::PLAYER_PART::PLAYER_PART_STOP_STMAP == ePlayerPart)
		{
			m_strKeyInfomation = TEXT("도장 찍기");
		}

		m_iPreStamp = (_uint)ePlayerPart;




	//}

}

void CInteraction_Key::ChangeBombWord()
{
	m_strKeyInfomation = TEXT("터뜨리기");
}

void CInteraction_Key::ChangeExitBookState()
{
	if (1 != m_iDisplayTexture)
	{
		m_iDisplayTexture = 1;
	}

	if (CPlayer::STATE::TURN_BOOK == Uimgr->Get_Player()->Get_CurrentStateID())
	{
		_float2 vPos = { 0.f, 0.f };

		vPos.x = g_iWinSizeX - g_iWinSizeX / 10.8f;
		vPos.y = g_iWinSizeY - g_iWinSizeY / 20.f;

		m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(vPos.x - g_iWinSizeX * 0.5f, -vPos.y + g_iWinSizeY * 0.5f, 0.f, 1.f));
		m_strKeyInfomation = TEXT("나가기");

			if (1 != m_iDisplayTexture)
			{
				m_iDisplayTexture = 1;
			}
	}
	else
	{
		m_strKeyInfomation = TEXT("");
	}



	//m_strKeyInfomation = TEXT("나가기");
	//
	//_float2 vPos = { 0.f, 0.f };
	//
	// vPos.x = g_iWinSizeX - g_iWinSizeX / 10.8f;
	// vPos.y = g_iWinSizeY - g_iWinSizeY / 20.f;
	//
	//m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(vPos.x - g_iWinSizeX * 0.5f, -vPos.y + g_iWinSizeY * 0.5f, 0.f, 1.f));

}
