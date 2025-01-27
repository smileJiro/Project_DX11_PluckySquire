

namespace Map_Tool
{


	enum IMGUI_MAPTOOL_BUTTON_STYLE_TYPE
	{
		ALIGN_SQUARE,
		MINI,
		BUTTON_STYLE_END
	};


	_bool StyleButton(IMGUI_MAPTOOL_BUTTON_STYLE_TYPE _eButtonType, const string& _strTrueText);
	_bool StartPopupButton(IMGUI_MAPTOOL_BUTTON_STYLE_TYPE _eButtonType, const string& _strButtonText);
	_bool ActiveButton(IMGUI_MAPTOOL_BUTTON_STYLE_TYPE _eButtonType, _bool _isActivePlag, const string& _strTrueText, const string& _strFalseText);
	void  Begin_Draw_ColorButton(const string& key, ImVec4 vStartColor);
	void  End_Draw_ColorButton();
	_bool InputText(const char* _label, std::string& _str, ImGuiInputTextFlags _flags = 0);


}