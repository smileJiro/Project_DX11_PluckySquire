#include "stdafx.h"
#include "Level_Static.h"
#include "Level_Loading.h"
#include "GameInstance.h"
#include "BackGround.h"

CLevel_Static::CLevel_Static(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CLevel(_pDevice, _pContext)
{
}

HRESULT CLevel_Static::Initialize()
{
	CBackGround::BACKGROUND_DESC Desc = {};

	Desc.strTextureComponentTag = L"Prototype_Component_Texture_MapTool_Logo";
	CGameObject* pGameObject = nullptr;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_BackGround"),
		LEVEL_STATIC, L"Layer_Background", &pGameObject, &Desc)))
		return E_FAIL;



 	return S_OK;
}

void CLevel_Static::Update(_float _fTimeDelta)
{
	if (KEY_DOWN(KEY::F1))
		Event_LevelChange(LEVEL_LOADING, LEVEL_TOOL_3D_MAP);
	else if (KEY_DOWN(KEY::F2))
		Event_LevelChange(LEVEL_LOADING, LEVEL_TOOL_2D_MAP);
	
}

HRESULT CLevel_Static::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("F1-> 3D Tool F2-> 2D Tool"));
#endif
	return S_OK;
}

CLevel_Static* CLevel_Static::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CLevel_Static* pInstance = new CLevel_Static(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Static");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Static::Free()
{
	__super::Free();
}
