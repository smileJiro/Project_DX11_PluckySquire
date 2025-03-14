#include "stdafx.h"
#include "StopStamp_UI.h"
#include "GameInstance.h"
#include "Book.h"
#include "Player.h"




CStopStamp_UI::CStopStamp_UI(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CUI (_pDevice, _pContext)
{
}

CStopStamp_UI::CStopStamp_UI(const CStopStamp_UI& _Prototype)
	: CUI ( _Prototype )
{
}

HRESULT CStopStamp_UI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStopStamp_UI::Initialize(void* _pArg)
{
	UIOBJDESC* pDesc = static_cast<UIOBJDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;



	//m_ePreStamp = CUI_Manager::GetInstance()->Get_StampIndex();

	return S_OK;
}

void CStopStamp_UI::Priority_Update(_float _fTimeDelta)
{
}

void CStopStamp_UI::Update(_float _fTimeDelta)
{
	
	CUI_Manager* pUIManager = CUI_Manager::GetInstance();
	if (false == pUIManager->Get_StampHave(1))
		return;


	if (m_isActive == false)
		return;

	// 둘다 있으면 체인지 스탬프 준비하고
	if (true == pUIManager->Get_StampHave(0) &&
		true == pUIManager->Get_StampHave(1))
	{
		ChangeStamp(_fTimeDelta);
	}

	// 밤만 가지고 있으면 밤 도장 위치를 조정해주자.
	else if (true == pUIManager->Get_StampHave(1) &&
		false == pUIManager->Get_StampHave(0))
	{
		if (false == m_isFirstPositionAdjusted)
		{
			_float2 vPos;
			// 크기 및 위치 조정
			vPos.x = g_iWinSizeX - g_iWinSizeX / 12.f;
			vPos.y = g_iWinSizeY - g_iWinSizeY / 6.f;

			m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(vPos.x - g_iWinSizeX * 0.5f, -vPos.y + g_iWinSizeY * 0.5f, 0.f, 1.f));

			m_pControllerTransform->Set_Scale(96.f, 148.f, 1.f);

			//m_isFirstPositionAdjusted = true;
		}
	}

}

void CStopStamp_UI::Late_Update(_float _fTimeDelta)
{
	if (false == Uimgr->GetInstance()->Get_StampHave(1))
		return;


	CBook* pBook = Uimgr->Get_Book();

	if (nullptr == pBook)
		assert(pBook);

	if (true == pBook->Get_PlayerAbove())
		__super::Late_Update(_fTimeDelta);
}

HRESULT CStopStamp_UI::Render()
{
	if (false == Uimgr->GetInstance()->Get_StampHave(1))
		return S_OK;

	if (FAILED(m_pControllerTransform->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;


	m_pShaderCom->Begin((_uint)PASS_VTXPOSTEX::DEFAULT);
	m_pVIBufferCom->Bind_BufferDesc();
	m_pVIBufferCom->Render();

	//__super::Render();
	

	return S_OK;
}

void CStopStamp_UI::ChangeStamp(_float _fTimeDelta)
{
	//CUI_Manager::STAMP eStamp;
	//eStamp = CUI_Manager::GetInstance()->Get_StampIndex();


	CPlayer* pPlayer = Uimgr->Get_Player();
	if (nullptr == pPlayer)
	{
		assert(pPlayer);
	}


	CPlayer::PLAYER_PART ePlayerPart = pPlayer->Get_CurrentStampType();

	if (CPlayer::PLAYER_PART::PLAYER_PART_STOP_STMAP == ePlayerPart ||
		CPlayer::PLAYER_PART::PLAYER_PART_BOMB_STMAP == ePlayerPart)
	{
		if (m_ePreStamp != ePlayerPart && false == m_isScaling)
		{
			if (ePlayerPart == CPlayer::PLAYER_PART::PLAYER_PART_STOP_STMAP)
			{
				//위치 변경이 필요한가요?
				m_fX = g_iWinSizeX / 7.5f;
				m_fY = g_iWinSizeY - g_iWinSizeY / 10.f;

				m_isBig = true;
				m_isScaling = true;
			}
			else if (ePlayerPart == CPlayer::PLAYER_PART::PLAYER_PART_BOMB_STMAP)
			{
				//위치 변경이 필요한가요?
				m_fX = g_iWinSizeX / 7.5f;
				m_fY = g_iWinSizeY - g_iWinSizeY / 10.f;

				m_isSmall = true;
				m_isScaling = true;
			}
			m_ePreStamp = ePlayerPart;
		}


		if (true == m_isBig || true == m_isSmall)
		{
			if (true == m_isBig)
			{
				if (m_fSizeX <= 96.f)
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
				if (m_fSizeX > 72.f)
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

	//if (m_ePreStamp != eStamp && false == m_isScaling)
	//{
	//	if (eStamp == CUI_Manager::STAMP_STOP)
	//	{
	//		//위치 변경이 필요한가요?
	//		m_fX = g_iWinSizeX / 7.5f;
	//		m_fY = g_iWinSizeY - g_iWinSizeY / 10.f;
	//
	//		m_isBig = true;
	//		m_isScaling = true;
	//	}
	//	else if (eStamp == CUI_Manager::STAMP_BOMB)
	//	{
	//		//위치 변경이 필요한가요?
	//		m_fX = g_iWinSizeX / 7.5f;
	//		m_fY = g_iWinSizeY - g_iWinSizeY / 10.f;
	//
	//		m_isSmall = true;
	//		m_isScaling = true;
	//	}
	//	m_ePreStamp = eStamp;
	//}
	//
	//
	//if (true == m_isBig || true == m_isSmall)
	//{
	//	if (true == m_isBig)
	//	{
	//		if (m_fSizeX <= 96.f)
	//		{
	//			m_fSizeX += _fTimeDelta * 100.f;
	//			m_fSizeY += (_fTimeDelta * 1.54f) * 100.f;
	//
	//			m_pControllerTransform->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	//		}
	//		else
	//		{
	//			m_isBig = false;
	//			m_isScaling = false;
	//		}
	//
	//	}
	//	else if (true == m_isSmall)
	//	{
	//		if (m_fSizeX > 72.f)
	//		{
	//			m_fSizeX -= _fTimeDelta * 100.f;
	//			m_fSizeY -= (_fTimeDelta * 1.54f) * 100.f;
	//
	//			m_pControllerTransform->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	//		}
	//		else
	//		{
	//			m_isSmall = false;
	//			m_isScaling = false;
	//		}
	//	}
	//}

	//CUI_Manager::STAMP eStamp;
	//eStamp = CUI_Manager::GetInstance()->Get_StampIndex();
	//
	//if (m_ePreStamp != eStamp && false == m_isScaling)
	//{
	//	if (eStamp == CUI_Manager::STAMP_BOMB)
	//	{
	//		//위치 변경이 필요한가요?
	//		m_fX = g_iWinSizeX / 7.5f;
	//		m_fY = g_iWinSizeY - g_iWinSizeY / 10.f;
	//
	//		m_isSmall = true;
	//		m_isScaling = true;
	//	}
	//	else if (eStamp == CUI_Manager::STAMP_STOP)
	//	{
	//		//위치 변경이 필요한가요?
	//		m_fX = g_iWinSizeX / 7.5f;
	//		m_fY = g_iWinSizeY - g_iWinSizeY / 10.f;
	//
	//		m_isBig = true;
	//		m_isScaling = true;
	//	}
	//	m_ePreStamp = eStamp;
	//}
	//
	//
	//if (true == m_isBig || true == m_isSmall)
	//{
	//	if (true == m_isBig)
	//	{
	//		if (m_fSizeX <= 96.f)
	//		{
	//			m_fSizeX += _fTimeDelta * 100.f;
	//			m_fSizeY += (_fTimeDelta * 1.54f) * 100.f;
	//
	//			m_pControllerTransform->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	//		}
	//		else
	//		{
	//			m_isBig = false;
	//			m_isScaling = false;
	//		}
	//			
	//	}
	//	else if (true == m_isSmall)
	//	{
	//		if (m_fSizeX > 72.f)
	//		{
	//			m_fSizeX -= _fTimeDelta * 100.f;
	//			m_fSizeY -= (_fTimeDelta * 1.54f) * 100.f;
	//
	//			m_pControllerTransform->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	//		}
	//		else
	//		{
	//			m_isSmall = false;
	//			m_isScaling = false;
	//		}	
	//	}
	//}

	
}

HRESULT CStopStamp_UI::Ready_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_Model_2D"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_Texture_StopStamp"),
		TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

CStopStamp_UI* CStopStamp_UI::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CStopStamp_UI* pInstance = new CStopStamp_UI(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CStopStamp Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CStopStamp_UI::Clone(void* _pArg)
{
	CStopStamp_UI* pInstance = new CStopStamp_UI(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CStopStamp Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CStopStamp_UI::Free()
{
	
	__super::Free();
	
}

HRESULT CStopStamp_UI::Cleanup_DeadReferences()
{

	return S_OK;
}
