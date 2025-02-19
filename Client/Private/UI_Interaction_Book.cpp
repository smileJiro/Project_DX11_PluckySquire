#include "stdafx.h"
#include "UI_Interaction_Book.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "PlayerData_Manager.h"

#include "Interactable.h"



CUI_Interaction_Book::CUI_Interaction_Book(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CUI(_pDevice, _pContext)
{
}

CUI_Interaction_Book::CUI_Interaction_Book(const CUI_Interaction_Book& _Prototype)
	: CUI(_Prototype)
{
}

HRESULT CUI_Interaction_Book::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CUI_Interaction_Book::Initialize(void* _pArg)
{
	UIOBJDESC* pDesc = static_cast<UIOBJDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_isRender = true;
	wsprintf(m_tFont, TEXT("조종"));

	return S_OK;
}

void CUI_Interaction_Book::Priority_Update(_float _fTimeDelta)
{
}

void CUI_Interaction_Book::Update(_float _fTimeDelta)
{
	if (true == m_isActive)
	{
		// 플레이어 체력 동기화 진행.

		__super::Update(_fTimeDelta);
	}
	
	
	if (true == Uimgr->Get_isQIcon() && CPlayerData_Manager::GetInstance()->Is_Own(CPlayerData_Manager::FLIPPING_GLOVE))
	{
		m_isRender = true;
	}
	else
	{
		m_isRender = false;
	}
	
	

		// 반환 값이 널이 아니다.
		//interactable get_interactkey() 
		// 
		
		// 이 키가 Q다

	
}

void CUI_Interaction_Book::Late_Update(_float _fTimeDelta)
{
	if (true == m_isRender && false == CUI_Manager::GetInstance()->Get_isESC())
		__super::Late_Update(_fTimeDelta);
}

HRESULT CUI_Interaction_Book::Render()
{
	if (true == m_isRender && false == CUI_Manager::GetInstance()->Get_isESC())
	{
		__super::Render();

		//fX = g_iWinSizeX / 2.f - g_iWinSizeX / 20.f;
		//fY = g_iWinSizeY - g_iWinSizeY / 18.f;

		
		m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 1.9f - 2.5f, g_iWinSizeY - g_iWinSizeY / 12.f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 1.9f + 2.5f, g_iWinSizeY - g_iWinSizeY / 12.f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 1.9f, g_iWinSizeY - g_iWinSizeY / 12.f - 2.5f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 1.9f - 2.5f, g_iWinSizeY - g_iWinSizeY / 12.f + 2.5f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));

		m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 1.9f, g_iWinSizeY - g_iWinSizeY / 12.f) , XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
	}
		

	return S_OK;
}



HRESULT CUI_Interaction_Book::Ready_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderComs[COORDINATE_2D]))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_Model_2D"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_Texture_KEYQ"),
		TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}


CUI_Interaction_Book* CUI_Interaction_Book::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CUI_Interaction_Book* pInstance = new CUI_Interaction_Book(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CUI_Interaction_Book Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CUI_Interaction_Book::Clone(void* _pArg)
{
	CUI_Interaction_Book* pInstance = new CUI_Interaction_Book(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CUI_Interaction_Book Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CUI_Interaction_Book::Free()
{
	__super::Free();
}

HRESULT CUI_Interaction_Book::Cleanup_DeadReferences()
{
	return S_OK;
}

//HRESULT CUI_Interaction_Book::Cleanup_DeadReferences()
//{
//	return S_OK;
//}

