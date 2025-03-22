#include "stdafx.h"
#include "BombStamp_UI.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "Book.h"
#include "PlayerData_Manager.h"


#include "Dialog_Manager.h"


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
	
	m_pControllerTransform->Set_Scale(m_fSizeX, m_fSizeY, 1.f);

	return S_OK;
}

void CBombStamp_UI::Priority_Update(_float _fTimeDelta)
{
}

void CBombStamp_UI::Update(_float _fTimeDelta)
{
	CUI_Manager* pUIManager = CUI_Manager::GetInstance();
	
	CPlayerData_Manager* pPDM = CPlayerData_Manager::GetInstance();


	// 밤 도장을 가지고 있지 않으면 리턴을 시켜주자.
	if (false == pPDM->Is_Own(CPlayerData_Manager::BOMB_STAMP))
		return;

	if (m_isActive == false)
		return;

	// 둘다 있으면 체인지 스탬프 준비하고
	if (true == pPDM->Is_Own(CPlayerData_Manager::BOMB_STAMP) &&
		true == pPDM->Is_Own(CPlayerData_Manager::STOP_STAMP))
	{ 
		ChangeStamp(_fTimeDelta);
	}

	// 밤만 가지고 있으면 밤 도장 위치를 조정해주자.
	else if (true == pPDM->Is_Own(CPlayerData_Manager::BOMB_STAMP) &&
			false == pPDM->Is_Own(CPlayerData_Manager::STOP_STAMP))
	{
		if (false == m_isFirstPositionAdjusted)
		{
			_float2 vPos;
			// 크기 및 위치 조정
			vPos.x = g_iWinSizeX - g_iWinSizeX / 12.f;
			vPos.y = g_iWinSizeY - g_iWinSizeY / 6.f;

			m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(vPos.x - g_iWinSizeX * 0.5f, -vPos.y + g_iWinSizeY * 0.5f, 0.f, 1.f));

			//m_pControllerTransform->Set_Scale(96.f, 148.f, 1.f);

			m_isFirstPositionAdjusted = true;
		}
	}
	
}

void CBombStamp_UI::Late_Update(_float _fTimeDelta)
{
	CPlayerData_Manager* pPDM = CPlayerData_Manager::GetInstance();
	if (false == pPDM->Is_Own(CPlayerData_Manager::BOMB_STAMP))
		return;

	CBook* pBook = Uimgr->Get_Book();

	if (nullptr == pBook)
		return;

	if (true == pBook->Get_PlayerAbove())
		__super::Late_Update(_fTimeDelta);
}

HRESULT CBombStamp_UI::Render()
{
	CPlayer* pPlayer = Uimgr->Get_Player();
	if (true == pPlayer->Is_PlayerInputBlocked())
		return S_OK;


	CPlayerData_Manager* pPDM = CPlayerData_Manager::GetInstance();
	if (false == pPDM->Is_Own(CPlayerData_Manager::BOMB_STAMP))
		return S_OK;


	__super::Render();
	return S_OK;
}



void CBombStamp_UI::ChangeStamp(_float _fTimeDelta)
{
	CPlayer* pPlayer = Uimgr->Get_Player();

	CPlayer::PLAYER_PART ePlayerPart = pPlayer->Get_CurrentStampType();

	if (CPlayer::PLAYER_PART::PLAYER_PART_STOP_STMAP == ePlayerPart ||
		CPlayer::PLAYER_PART::PLAYER_PART_BOMB_STMAP == ePlayerPart)
	{

		if (m_ePreStamp != ePlayerPart && false == m_isScaling)
		{
			if (ePlayerPart == CPlayer::PLAYER_PART::PLAYER_PART_BOMB_STMAP)
			{
				//위치 변경이 필요한가요?
				m_fX = g_iWinSizeX / 7.2f;
				m_fY = g_iWinSizeY - g_iWinSizeY / 6.f;

				m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

				m_isBig = true;
				m_isScaling = true;
			}
			else if (ePlayerPart == CPlayer::PLAYER_PART::PLAYER_PART_STOP_STMAP)
			{
				//위치 변경이 필요한가요?
				m_fX = g_iWinSizeX / 7.2f;
				m_fY = g_iWinSizeY - g_iWinSizeY / 6.f;

				m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

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
