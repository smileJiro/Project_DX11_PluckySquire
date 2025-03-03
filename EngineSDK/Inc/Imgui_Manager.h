#pragma once
#include "Base.h"

BEGIN(Engine)


class CGameInstance;
class CGameObject;

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
	HRESULT				Imgui_Debug_Render();
	HRESULT				Imgui_Debug_Render_RT();
	HRESULT				Imgui_Debug_Render_RT_FullScreen();
	HRESULT				Imgui_Debug_Render_ObjectInfo();
	HRESULT				Imgui_Debug_Render_ObjectInfo_Detail(CGameObject* _pGameObject);
	HRESULT				Imgui_Debug_IBLGlobalVariable();
	HRESULT				Imgui_Debug_Lights();


	HRESULT				Imgui_Select_Debug_ObjectInfo(const wstring _strLayerTag, _uint _iObjectId);
#endif //  _DEBUG

public :
	
private:
	CGameInstance*			m_pGameInstance = nullptr;
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

	map<wstring, _int>		m_LayerToSelectObjects;
	_float2					m_vViewportSize = {};
	HWND					m_hWnd = {};

	_bool					m_isImguiRTRender = true;
	_bool					m_isImguiObjRender = true;

public:
	static CImgui_Manager* Create(HWND _hWnd, ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _float2 _vViewportSize);
	virtual void Free();
	
};

END