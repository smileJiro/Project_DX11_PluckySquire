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
	m_pGameInstance->Set_DebugRender(false);



 	return S_OK;
}

void CLevel_Static::Update(_float _fTimeDelta)
{
	if (KEY_DOWN(KEY::F1))
		Event_LevelChange(LEVEL_LOADING, LEVEL_TOOL_3D_MAP);
	else if (KEY_DOWN(KEY::F2))
		Event_LevelChange(LEVEL_LOADING, LEVEL_TOOL_3D_MODEL);
	else if (KEY_DOWN(KEY::F3))
		Event_LevelChange(LEVEL_LOADING, LEVEL_TOOL_2D_MAP);

	RECT clientRect;
	GetClientRect(g_hWnd, &clientRect);           
	POINT clientPos = { clientRect.left, clientRect.top };
	ClientToScreen(g_hWnd, &clientPos);

	ImGui::SetNextWindowPos(ImVec2(clientPos.x + 500.f,  clientPos.y + 120.f), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(500.f,200.f), ImGuiCond_Always);
	ImVec4 customBackgroundColor = ImVec4(0.992f, 0.831f, 0.286f, 1.0f);

	// 윈도우 배경색 변경
	ImGui::PushStyleColor(ImGuiCol_WindowBg, customBackgroundColor);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

	ImGui::Begin("Select Tool", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	
	Begin_Draw_ColorButton("##Add_Model_Style", (ImVec4)ImColor::HSV(0.5f, 0.6f, 0.6f));
	if(StyleButton(ALIGN_SQUARE,"3D Map Tool", 3.5f))
		Event_LevelChange(LEVEL_LOADING, LEVEL_TOOL_3D_MAP);
	End_Draw_ColorButton();

	Begin_Draw_ColorButton("##Add_Model_Style", (ImVec4)ImColor::HSV(0.3f, 0.6f, 0.6f));
	if(StyleButton(ALIGN_SQUARE,"3D Model Tool", 3.5f))
		Event_LevelChange(LEVEL_LOADING, LEVEL_TOOL_3D_MODEL);
	End_Draw_ColorButton();

	Begin_Draw_ColorButton("##Add_Model_Style", (ImVec4)ImColor::HSV(0.2f, 0.8f, 0.6f));
	if(StyleButton(ALIGN_SQUARE,"2D Map Tool", 3.5f))
		Event_LevelChange(LEVEL_LOADING, LEVEL_TOOL_2D_MAP);
	End_Draw_ColorButton();


	ImGui::End();
	ImGui::PopStyleVar();
	ImGui::PopStyleColor();
}

HRESULT CLevel_Static::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("F1-> 3D MapTool F2-> 3D ModelTool F3-> F3->2D MapTool"));
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
