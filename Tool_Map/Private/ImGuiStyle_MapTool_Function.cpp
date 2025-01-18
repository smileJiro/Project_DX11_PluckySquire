#include "stdafx.h"
namespace Map_Tool
{

	_bool StyleButton(IMGUI_MAPTOOL_BUTTON_STYLE_TYPE _eButtonType, const string& _strText)
	{
		switch (_eButtonType)
		{
		case Map_Tool::ALIGN_SQUARE:
			return ImGui::Button(_strText.c_str(), ImVec2(-FLT_MIN, 1.5f * ImGui::GetTextLineHeightWithSpacing()));
		case Map_Tool::MINI:
			return ImGui::Button(_strText.c_str());
		default:
			return false;
		}

	}

	_bool StartPopupButton(IMGUI_MAPTOOL_BUTTON_STYLE_TYPE _eButtonType, const string& _strButtonText)
	{
		const string strPopupText = "##" + _strButtonText;
		if(StyleButton(_eButtonType,_strButtonText.c_str()))
			ImGui::OpenPopup(strPopupText.c_str());
		return ImGui::BeginPopup(strPopupText.c_str());
	}

	_bool ActiveButton(IMGUI_MAPTOOL_BUTTON_STYLE_TYPE _eButtonType, _bool _isActivePlag, const string& _strTrueText, const string& _strFalseText)
	{
		return StyleButton(_eButtonType, _isActivePlag ? _strTrueText : _strFalseText);
	}

	void Begin_Draw_ColorButton(const string& key, ImVec4 vStartColor)
	{
		ImGui::PushID(key.c_str());
		ImGui::PushStyleColor(ImGuiCol_Button, vStartColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, vStartColor + (ImVec4)ImColor::HSV(vStartColor.x, 0.1f, 0.1f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, vStartColor + (ImVec4)ImColor::HSV(vStartColor.x, 0.2f, 0.2f));
	}
	void End_Draw_ColorButton()
	{
		ImGui::PopStyleColor(3);
		ImGui::PopID();
	}
}