#include "stdafx.h"
#include "Level_Logo.h"
#include "Level_Loading.h"

#include "GameInstance.h"

#include "UI.h"
#include "Npc.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CLevel(_pDevice, _pContext)
{
}

HRESULT CLevel_Logo::Initialize()
{
	Ready_Layer_UI(TEXT("Layer_UI"));
	//Ready_Layer_NPC(TEXT("Layer_NPC"));

 	return S_OK;
}

void CLevel_Logo::Update(_float _fTimeDelta)
{
	if (KEY_DOWN(KEY::ENTER))
		Event_LevelChange(LEVEL_LOADING, LEVEL_GAMEPLAY);

}

HRESULT CLevel_Logo::Render()
{
#ifdef _DEBUG
	//m_pGameInstance->Render_FPS(TEXT("Timer_Default"));
	SetWindowText(g_hWnd, TEXT("로고레벨입니다."));
#endif
	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_UI(const _wstring& _strLayerTag)
{
	CUI::UIOBJDESC pDesc = {};

	pDesc.iCurLevelID = LEVEL_LOGO;
	pDesc.fX = g_iWinSizeX - g_iWinSizeX / 2.f;
	pDesc.fY = g_iWinSizeY / 2.f;
	pDesc.fSizeX = g_iWinSizeX;
	pDesc.fSizeY = g_iWinSizeY;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOGO, TEXT("Prototype_UIObejct_LogoBG"), LEVEL_LOGO, _strLayerTag, &pDesc)))
		return E_FAIL;

	pDesc.strLayerTag = _strLayerTag;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOGO, TEXT("Prototype_UIObejct_Logo"), LEVEL_LOGO, _strLayerTag, &pDesc)))
		return E_FAIL;

		

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_NPC(const _wstring& _strLayerTag)
{
	CNPC::tagNPCDesc NpcDesc;
	NpcDesc.iCurLevelID = LEVEL_LOGO;
	NpcDesc.tTransform2DDesc.vInitialPosition = _float3(0.f, 0.f, 0.f);
	NpcDesc.tTransform3DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
	NpcDesc.iMainIndex = 0;
	NpcDesc.iSubIndex = 0;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOGO, TEXT("Prototype_Obejct_NPC_Logo"), LEVEL_LOGO, _strLayerTag, &NpcDesc)))
		return E_FAIL;

	return S_OK;
}



CLevel_Logo* CLevel_Logo::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CLevel_Logo* pInstance = new CLevel_Logo(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Logo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();
}
