#pragma once

#include "Map_Tool_Defines.h"
#include "Base.h"

#include "2DMapObject.h"
#include "2DMapObjectInfo.h"

BEGIN(Engine)
class CGameInstance;
class CGameObject;
class CLayer;
END



BEGIN(Map_Tool)


class C2DMapObject;
class CModelFile_Manager;
class CImguiLogger;
class CTask_Manager;
class C2DMapObject;
class C2DDefault_RenderObject;
class C2DTile_RenderObject;


class C2DMap_Tool_Manager final : public CBase
{
private :
	enum LIST_TYPE
	{
		MODEL_LIST,
		SELECT_MAP,
		SAVE_LIST,
		TRIGGER_LIST,
		TRIGGER_EVENT_LIST,
		LIST_END
	};
private:
	typedef struct tagMapObjList
	{
		_tchar szName[MAX_PATH];
		_uint  iObjKey;
	}MAP_OBJ;
public :
	typedef struct tagTriggerEvent
	{
		_string strEventName = "";
		_uint	uTriggerCondition = 0;
		_uint	uTriggerEvent = 0;
	}TRIGGER_EVENT;
private:
	C2DMap_Tool_Manager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	~C2DMap_Tool_Manager() = default;
	C2DMap_Tool_Manager(const C2DMap_Tool_Manager& _pManager) = delete;

public:
	void				Update_Tool();
	HRESULT				Render();

private:
	HRESULT				Initialize(CImguiLogger* _pLogger);

#pragma region Input & Imgui Method
private:
	// 인풋 로직 
	void				Input_Logic();

	// Imgui 메인 로직
	void				Update_Imgui_Logic();
	void				Map_Import_Imgui(_bool _isLock = false);
	void				Model_Edit_Imgui(_bool _isLock = false);
	void				SaveLoad_Imgui(_bool _isLock = false);
	void				TriggerSetting_Imgui(_bool bLock = false);
	void				TriggerEvent_Imgui(_bool bLock = false);
#pragma endregion

#pragma region Object Tool Method

	void				Load_2DModelList();
	void				Save_2DModelList();

	void				Load_SaveFileList();

	void				Load_String();

	void				Object_Clear(_bool _bSelected = true);

	void				Save_Popup();


	// 2D 맵, 오브젝트 출력
	void				Save(_bool _bSelected = true);
	// 2D 맵, 오브젝트 입력
	void				Load(_bool _bSelected = true);

	void				Load_3D_Map(_bool _bSelected = true);


	C2DMapObject*	Picking_2DMap();

#pragma endregion

	HRESULT				Setting_TileMap(const _string _strFileMapJsonName);

	C2DMapObjectInfo*	Find_Info(const _wstring _strTag);

	HRESULT				Update_Model_Index();


private:
	ID3D11Device*					m_pDevice;
	ID3D11DeviceContext*			m_pContext;
	CGameInstance*					m_pGameInstance = { nullptr };
	CImguiLogger*					m_pLogger = nullptr;
	CTask_Manager*					m_pTaskManager;
	C2DTile_RenderObject*			m_pTileRenderObject = nullptr;
	
	_wstring						m_arrSelectName[LIST_END];

	vector<C2DMapObjectInfo*>		m_ObjectInfoLists;
	vector<TRIGGER_EVENT>			m_TriggerEvents;
	vector<_wstring>				m_SaveFileLists;

	C2DMapObjectInfo*				m_pPickingInfo = nullptr;
	C2DMapObject*					m_pPickingObject = nullptr;
	class C2DTrigger_Sample*		m_pPickingTrigger = nullptr;
	_int							m_SelectTriggerEventIdx = -1;

	_wstring						m_strMapBinaryPath = L"../../Client/Bin/MapSaveFiles/2D/";
	_char							m_szSaveFileName[MAX_PATH];

	C2DDefault_RenderObject*		m_DefaultRenderObject;
	
	vector<_string>					m_ModelTypeTexts;
	vector<_string>					m_ActiveTypeTexts;
	vector<_string>					m_ColliderTypeTexts;

	_float2		m_fOffsetPos = { 0.f, 0.f };

public:
	virtual void Free();
	static C2DMap_Tool_Manager* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CImguiLogger* _pLogger);

};

END

