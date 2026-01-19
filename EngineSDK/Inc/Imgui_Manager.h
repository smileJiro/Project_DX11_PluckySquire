#pragma once
#include "Base.h"

BEGIN(Engine)


class CGameInstance;
class CGameObject;
class CLight;

class CImgui_Manager final:  public CBase
{
private:
	CImgui_Manager();
	virtual ~CImgui_Manager() = default;

public:
	HRESULT				Initialize(HWND _hWnd, ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _float2 _vViewportSize);
	HRESULT				Start_Imgui(); // 시작을 알림.
	HRESULT				End_Imgui(); // 종료를 알림.
	void				Render_DrawData(); // 수집 된 정보를 기반으로 그리기를 수행함.

	HRESULT				LevelChange_Imgui();

#ifdef _DEBUG
	bool				Is_ImguiFocused() const;
	HRESULT				Imgui_Debug_Render();
	HRESULT				Imgui_Debug_Render_RT();
	HRESULT				Imgui_Debug_Render_RT_FullScreen();
	HRESULT				Imgui_Debug_Render_ObjectInfo();
	HRESULT				Imgui_Debug_Render_ObjectInfo_Detail(CGameObject* _pGameObject);
	HRESULT				Imgui_Debug_IBLGlobalVariable();
	HRESULT				Imgui_Debug_Lights();

	HRESULT				Imgui_LevelLightingTool();
	void				DrawLevelPresetBar(const char* _szCurrentPresetName, bool _isDirty);
	void				DrawLightsList();
	void				DrawLightsListTable(const list<CLight*>& LightsList, const char* _szTableName, const ImGuiTableFlags _flagTable, const LIGHT_TYPE _eLightType);
	void				Set_SelectedLight(CLight* _pNewLight);

	HRESULT				Imgui_Select_Debug_ObjectInfo(const wstring _strLayerTag, _uint _iObjectId);
#endif //  _DEBUG

	
private:
	CGameInstance*			m_pGameInstance = nullptr;
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

	map<wstring, _int>		m_LayerToSelectObjects;
	_float2					m_vViewportSize = {};
	HWND					m_hWnd = {};

	_bool					m_isImguiRTRender = true;
	_bool					m_isImguiObjRender = true;

private: // Level Light Tool
	class CLight* m_pSelectedLight = nullptr;
	_int m_iSelectedIndex = -1;
	// rename state
	CLight* m_pRenamingLight = nullptr;
	char    m_RenameBuffer[256] = {};
	bool    m_RenameFocusRequested = false; // InputText 직전에 포커스
	bool    m_RenameEverActive = false;     // 한 번이라도 편집이 Active였나?

public:
	static CImgui_Manager* Create(HWND _hWnd, ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _float2 _vViewportSize);
	virtual void Free();

};

END