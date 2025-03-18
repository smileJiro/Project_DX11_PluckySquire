#include "stdafx.h"
#include "Logo_BG.h"
#include "GameInstance.h"
#include "UI_Manager.h"



CLogo_BG::CLogo_BG(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CUI (_pDevice, _pContext)
{
}

CLogo_BG::CLogo_BG(const CLogo_BG& _Prototype)
	: CUI ( _Prototype )
{
}

HRESULT CLogo_BG::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLogo_BG::Initialize(void* _pArg)
{
	UIOBJDESC* pDesc = static_cast<UIOBJDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_vColor = { 246.f / 255.f , 127.f / 255.f, 98.f / 255.f, 1.f };

	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

void CLogo_BG::Priority_Update(_float _fTimeDelta)
{
}

void CLogo_BG::Update(_float _fTimeDelta)
{
	if (true == Uimgr->Get_LogoChanseStage())
	{
		_int iIndex = CUI_Manager::GetInstance()->Get_LogoIndex();

		if (KEY_DOWN(KEY::DOWN))
		{
			if (4 > iIndex)
			{
				iIndex += 1;
				CUI_Manager::GetInstance()->Set_LogoIndex(iIndex);

			}
			else
				return;
		}
		else if (KEY_DOWN(KEY::UP))
		{
			if (1 <= iIndex)
			{
				iIndex -= 1;
				CUI_Manager::GetInstance()->Set_LogoIndex(iIndex);
			}
		}
		else if (KEY_DOWN(KEY::ENTER) && true == Uimgr->Get_LogoChanseStage())
		{
			ProcesssByIndex(iIndex);
		}
	}

	//if (KEY_DOWN(KEY::E))
	//{
	//	Uimgr->Set_LogoChangeState(true);
	//	m_vColor = { 246.f / 255.f , 127.f / 255.f, 98.f / 255.f, 1.f };
	//}
	
}

void CLogo_BG::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT CLogo_BG::Render()
{	
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColors", &m_vColor, sizeof(_float4))))
		return E_FAIL;

	__super::Render(0, PASS_VTXPOSTEX::COLOR_ALPHA);

	_wstring strMenuText[] = {
		TEXT("게임 시작")
		,TEXT("옵션")
		,TEXT("크레딧")
		,TEXT("바탕화면으로 나가기")
	};
	
	_wstring strMenuOverrideText[] = {
		TEXT("챕터 2 메인 맵")
		,TEXT("챕터 4 메인 맵")
		,TEXT("챕터 6 메인 맵")
		,TEXT("챕터 8 메인 맵")
		,TEXT("카메라 툴")
	};

	_wstring* strCurText = nullptr;

	strCurText = strMenuText;
	//strCurText = strMenuOverrideText;
	
	//if (false == Uimgr->Get_LogoChanseStage())
	//{
	//	wsprintf(m_tTest, TEXT("아무 키를 눌러주세요"));
	//	m_pGameInstance->Render_Font(TEXT("Font40"), m_tTest, _float2(g_iWinSizeX - g_iWinSizeX / 4.f, g_iWinSizeY - g_iWinSizeY / 4.f), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
	//}
	if (true == Uimgr->Get_LogoChanseStage())
	{
		_int iIndex = CUI_Manager::GetInstance()->Get_LogoIndex();
		_tchar wMouseTalk[MAX_PATH];

		wsprintf(m_tTest, TEXT("모험을 시작할"));
		m_pGameInstance->Render_Font(TEXT("Font54"), m_tTest, _float2(g_iWinSizeX / 4.f - g_iWinSizeX/16.f, g_iWinSizeY - g_iWinSizeY / 1.8f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
		wsprintf(m_tTest, TEXT("시간이에요"));
		m_pGameInstance->Render_Font(TEXT("Font54"), m_tTest, _float2(g_iWinSizeX / 4.f - g_iWinSizeX * 0.05f, g_iWinSizeY - g_iWinSizeY / 2.1f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
		wsprintf(m_tTest, TEXT("확인"));
		m_pGameInstance->Render_Font(TEXT("Font30"), m_tTest, _float2(g_iWinSizeX - g_iWinSizeX / 13.f, g_iWinSizeY - g_iWinSizeY / 11.f), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));

		if (0 == iIndex)
		{
			wsprintf(wMouseTalk, TEXT("자, 시작해 볼까요!"));
			m_pGameInstance->Render_Font(TEXT("Font30"), wMouseTalk, _float2(g_iWinSizeX - g_iWinSizeX / 2.38f, g_iWinSizeY / 15.f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));


			wsprintf(wMouseTalk, strCurText[0].c_str());
			m_pGameInstance->Render_Font(TEXT("Font40"), wMouseTalk, _float2(g_iWinSizeX - g_iWinSizeX / 2.49f, g_iWinSizeY / 3.f), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));

			wsprintf(wMouseTalk, strCurText[1].c_str());
			m_pGameInstance->Render_Font(TEXT("Font40"), wMouseTalk, _float2(g_iWinSizeX - g_iWinSizeX / 2.63f, g_iWinSizeY / 2.4f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));

			wsprintf(wMouseTalk, strCurText[2].c_str());
			m_pGameInstance->Render_Font(TEXT("Font40"), wMouseTalk, _float2(g_iWinSizeX - g_iWinSizeX / 2.58f, g_iWinSizeY - g_iWinSizeY / 1.98f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));

			wsprintf(wMouseTalk, strCurText[3].c_str());
			m_pGameInstance->Render_Font(TEXT("Font40"), wMouseTalk, _float2(g_iWinSizeX - g_iWinSizeX / 2.275f, g_iWinSizeY - g_iWinSizeY / 2.4f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));

		}
		else if (1 == iIndex)
		{
			wsprintf(wMouseTalk, TEXT("옵션을 설정하고 조작법을 확인합니다."));
			m_pGameInstance->Render_Font(TEXT("Font30"), wMouseTalk, _float2(g_iWinSizeX - g_iWinSizeX / 2.12f, g_iWinSizeY / 15.f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));


			wsprintf(wMouseTalk, strCurText[0].c_str());
			m_pGameInstance->Render_Font(TEXT("Font40"), wMouseTalk, _float2(g_iWinSizeX - g_iWinSizeX / 2.49f, g_iWinSizeY / 3.f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));

			wsprintf(wMouseTalk, strCurText[1].c_str());
			m_pGameInstance->Render_Font(TEXT("Font40"), wMouseTalk, _float2(g_iWinSizeX - g_iWinSizeX / 2.63f, g_iWinSizeY / 2.4f), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));

			wsprintf(wMouseTalk, strCurText[2].c_str());
			m_pGameInstance->Render_Font(TEXT("Font40"), wMouseTalk, _float2(g_iWinSizeX - g_iWinSizeX / 2.58f, g_iWinSizeY - g_iWinSizeY / 1.98f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));

			wsprintf(wMouseTalk, strCurText[3].c_str());
			m_pGameInstance->Render_Font(TEXT("Font40"), wMouseTalk, _float2(g_iWinSizeX - g_iWinSizeX / 2.275f, g_iWinSizeY - g_iWinSizeY / 2.4f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
		}
		else if (2 == iIndex)
		{
			wsprintf(wMouseTalk, TEXT("크레딧을 감상합니다."));
			m_pGameInstance->Render_Font(TEXT("Font30"), wMouseTalk, _float2(g_iWinSizeX - g_iWinSizeX / 2.38f, g_iWinSizeY / 15.f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));


			wsprintf(wMouseTalk, strCurText[0].c_str());
			m_pGameInstance->Render_Font(TEXT("Font40"), wMouseTalk, _float2(g_iWinSizeX - g_iWinSizeX / 2.49f, g_iWinSizeY / 3.f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));

			wsprintf(wMouseTalk, strCurText[1].c_str());
			m_pGameInstance->Render_Font(TEXT("Font40"), wMouseTalk, _float2(g_iWinSizeX - g_iWinSizeX / 2.63f, g_iWinSizeY / 2.4f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));

			wsprintf(wMouseTalk, strCurText[2].c_str());
			m_pGameInstance->Render_Font(TEXT("Font40"), wMouseTalk, _float2(g_iWinSizeX - g_iWinSizeX / 2.58f, g_iWinSizeY - g_iWinSizeY / 1.98f), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));

			wsprintf(wMouseTalk, strCurText[3].c_str());
			m_pGameInstance->Render_Font(TEXT("Font40"), wMouseTalk, _float2(g_iWinSizeX - g_iWinSizeX / 2.275f, g_iWinSizeY - g_iWinSizeY / 2.4f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
		}
		else if (3 == iIndex)
		{
			wsprintf(wMouseTalk, TEXT("또 들러 주세요!"));
			m_pGameInstance->Render_Font(TEXT("Font30"), wMouseTalk, _float2(g_iWinSizeX - g_iWinSizeX / 2.45f, g_iWinSizeY / 15.f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));

			wsprintf(wMouseTalk, strCurText[0].c_str());
			m_pGameInstance->Render_Font(TEXT("Font40"), wMouseTalk, _float2(g_iWinSizeX - g_iWinSizeX / 2.49f, g_iWinSizeY / 3.f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));

			wsprintf(wMouseTalk, strCurText[1].c_str());
			m_pGameInstance->Render_Font(TEXT("Font40"), wMouseTalk, _float2(g_iWinSizeX - g_iWinSizeX / 2.63f, g_iWinSizeY / 2.4f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));

			wsprintf(wMouseTalk, strCurText[2].c_str());
			m_pGameInstance->Render_Font(TEXT("Font40"), wMouseTalk, _float2(g_iWinSizeX - g_iWinSizeX / 2.58f, g_iWinSizeY - g_iWinSizeY / 1.98f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));

			wsprintf(wMouseTalk, strCurText[3].c_str());
			m_pGameInstance->Render_Font(TEXT("Font40"), wMouseTalk, _float2(g_iWinSizeX - g_iWinSizeX / 2.275f, g_iWinSizeY - g_iWinSizeY / 2.4f), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
		}
	}
	

	


	//__super::Render();
	

	return S_OK;
}



void CLogo_BG::ProcesssByIndex(_int _iIndex)
{
	switch (_iIndex)
	{
	case 0 :
	{
		Uimgr->Set_LogoChangeState(false);
		Event_LevelChange(LEVEL_LOADING, LEVEL_CHAPTER_2);
	}
	break;

	case 1:
	{
		Uimgr->Set_LogoChangeState(false);
		Event_LevelChange(LEVEL_LOADING, LEVEL_CHAPTER_4);
	}
	break;

	case 2:
	{
		Uimgr->Set_LogoChangeState(false);
		Event_LevelChange(LEVEL_LOADING, LEVEL_CHAPTER_6);
	}
	break;
	case 3:
	{
		Uimgr->Set_LogoChangeState(false);
		Event_LevelChange(LEVEL_LOADING, LEVEL_CHAPTER_8);
	}
	break;
	case 4:
	{
		Uimgr->Set_LogoChangeState(false);
		Event_LevelChange(LEVEL_LOADING, LEVEL_CAMERA_TOOL);
	}
	break;
	}
}


HRESULT CLogo_BG::Ready_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_Model_2D"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Logo_BG"),
		TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}


CLogo_BG* CLogo_BG::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CLogo_BG* pInstance = new CLogo_BG(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CLogo_BG Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CLogo_BG::Clone(void* _pArg)
{
	CLogo_BG* pInstance = new CLogo_BG(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CLogo_BG Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CLogo_BG::Free()
{
	
	__super::Free();
	
}

HRESULT CLogo_BG::Cleanup_DeadReferences()
{

	return S_OK;
}
