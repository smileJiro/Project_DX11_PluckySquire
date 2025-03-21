#include "stdafx.h"
#include "UI_Interaction_Tilting.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "PlayerData_Manager.h"
#include "Book.h"
#include "Interactable.h"



CUI_Interaction_Tilting::CUI_Interaction_Tilting(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CUI(_pDevice, _pContext)
{
}

CUI_Interaction_Tilting::CUI_Interaction_Tilting(const CUI_Interaction_Tilting& _Prototype)
	: CUI(_Prototype)
{
}

HRESULT CUI_Interaction_Tilting::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CUI_Interaction_Tilting::Initialize(void* _pArg)
{
	UIOBJDESC* pDesc = static_cast<UIOBJDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_isRender = false;
	wsprintf(m_tFont, TEXT("조종"));

	return S_OK;
}

void CUI_Interaction_Tilting::Priority_Update(_float _fTimeDelta)
{
}

void CUI_Interaction_Tilting::Update(_float _fTimeDelta)
{
	if (true == m_isActive)
	{
		// 플레이어 체력 동기화 진행.

		__super::Update(_fTimeDelta);
	}
	
	if (COORDINATE_3D == Uimgr->Get_Player()->Get_CurCoord())
	{
		CBook* pBook = dynamic_cast<CBook*>(Uimgr->Get_Player()->Get_InteractableObject());

		if (nullptr == pBook)
		{
			if (true == m_isRender)
				m_isRender = false;

			return;
		}

		if (true == pBook->Get_PlayerAround() && true == CPlayerData_Manager::GetInstance()->Is_Own(CPlayerData_Manager::TILTING_GLOVE))
		{
			m_isRender = true;

			if (CPlayer::STATE::TURN_BOOK == Uimgr->Get_Player()->Get_CurrentStateID() && false == CPlayerData_Manager::GetInstance()->Is_Own(CPlayerData_Manager::FLIPPING_GLOVE))
			{
				_float fX = g_iWinSizeX / 2.f - g_iWinSizeX / 12.f;
				_float fY = g_iWinSizeY - g_iWinSizeY / 18.f;

				if (false == m_isBookInteract)
				{
					{
						m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(fX - g_iWinSizeX * 0.5f, -fY + g_iWinSizeY * 0.5f, 0.f, 1.f));
						m_pControllerTransform->Set_Scale(COORDINATE_2D, _float3(144.f, 72.f, 1.f));
						m_isBookInteract = true;
						m_iTextureNum = 1;
						wsprintf(m_tFont, L"책 기울이기");
					}
					
				}
			}
			//else if ((CPlayer::STATE::TURN_BOOK == Uimgr->Get_Player()->Get_CurrentStateID() && false == CPlayerData_Manager::GetInstance()->Is_Own(CPlayerData_Manager::FLIPPING_GLOVE)))
			//{
			//	// 책장 넘기기 건틀릿만 있는 경우
			//	_float fX = g_iWinSizeX / 2.f - g_iWinSizeX / 12.f;
			//	_float fY = g_iWinSizeY - g_iWinSizeY / 18.f;
			//
			//	if (false == m_isBookInteract)
			//	{
			//		m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(fX - g_iWinSizeX * 0.5f, -fY + g_iWinSizeY * 0.5f, 0.f, 1.f));
			//		m_pControllerTransform->Set_Scale(COORDINATE_2D, _float3(144.f, 72.f, 1.f));
			//		m_isBookInteract = true;
			//		m_iTextureNum = 1;
			//		wsprintf(m_tFont, L"책장 넘기기");
			//	}
				else
				{
					if (true == m_isBookInteract && false == CPlayerData_Manager::GetInstance()->Is_Own(CPlayerData_Manager::FLIPPING_GLOVE))
					{
						_float fX = g_iWinSizeX / 2.f - g_iWinSizeX / 20.f;
						_float fY = g_iWinSizeY - g_iWinSizeY / 18.f;

						m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(fX - g_iWinSizeX * 0.5f, -fY + g_iWinSizeY * 0.5f, 0.f, 1.f));
						m_pControllerTransform->Set_Scale(COORDINATE_2D, _float3(72.f, 72.f, 1.f));
						m_isBookInteract = false;
						m_iTextureNum = 0;
						wsprintf(m_tFont, L"조종");
					}
				}
			}
			else
			{
				m_isRender = false;
			}
		}
		else if (COORDINATE_2D == Uimgr->Get_Player()->Get_CurCoord())
		{
			if (true == m_isRender)
				m_isRender = false;
		}
	
}

void CUI_Interaction_Tilting::Late_Update(_float _fTimeDelta)
{
	if (true == m_isRender && false == CUI_Manager::GetInstance()->Get_isESC())
		__super::Late_Update(_fTimeDelta);
}

HRESULT CUI_Interaction_Tilting::Render()
{
	if (true == m_isRender && false == CUI_Manager::GetInstance()->Get_isESC())
	{
		__super::Render(m_iTextureNum);

		if (false == m_isBookInteract)
		{
			m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 1.9f - 2.5f, g_iWinSizeY - g_iWinSizeY / 12.f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
			m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 1.9f + 2.5f, g_iWinSizeY - g_iWinSizeY / 12.f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
			m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 1.9f, g_iWinSizeY - g_iWinSizeY / 12.f - 2.5f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
			m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 1.9f - 2.5f, g_iWinSizeY - g_iWinSizeY / 12.f + 2.5f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));

			m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 1.9f, g_iWinSizeY - g_iWinSizeY / 12.f), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
		}
		else
		{
			if (false == CPlayerData_Manager::GetInstance()->Is_Own(CPlayerData_Manager::FLIPPING_GLOVE))
			{
				m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 1.84f - 2.5f, g_iWinSizeY - g_iWinSizeY / 12.f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
				m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 1.84f + 2.5f, g_iWinSizeY - g_iWinSizeY / 12.f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
				m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 1.84f, g_iWinSizeY - g_iWinSizeY / 12.f - 2.5f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
				m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 1.84f - 2.5f, g_iWinSizeY - g_iWinSizeY / 12.f + 2.5f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));

				m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 1.84f, g_iWinSizeY - g_iWinSizeY / 12.f), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
			}
			else
			{
				m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 1.84f - 2.5f, g_iWinSizeY - g_iWinSizeY / 7.f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
				m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 1.84f + 2.5f, g_iWinSizeY - g_iWinSizeY / 7.f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
				m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 1.84f, g_iWinSizeY - g_iWinSizeY / 7.f - 2.5f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
				m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 1.84f - 2.5f, g_iWinSizeY - g_iWinSizeY / 7.f + 2.5f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));

				m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 1.84f, g_iWinSizeY - g_iWinSizeY / 7.f), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
			}

		}
	}
		
	return S_OK;
}



HRESULT CUI_Interaction_Tilting::Ready_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_Model_2D"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Interact_Book"),
		TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}


CUI_Interaction_Tilting* CUI_Interaction_Tilting::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CUI_Interaction_Tilting* pInstance = new CUI_Interaction_Tilting(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CUI_Interaction_Tilting Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CUI_Interaction_Tilting::Clone(void* _pArg)
{
	CUI_Interaction_Tilting* pInstance = new CUI_Interaction_Tilting(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CUI_Interaction_Tilting Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CUI_Interaction_Tilting::Free()
{
	__super::Free();
}

HRESULT CUI_Interaction_Tilting::Cleanup_DeadReferences()
{
	return S_OK;
}
