#pragma once

#include "Map_Tool_Defines.h"
#include "Base.h"
BEGIN(Engine)
class CGameInstance;
class CGameObject;
END



BEGIN(Map_Tool)


class CMapObject;
class CEditableCell;
class CNavigationVertex;
class CModelFile_Manager;
class CImguiLogger;
class CCellContainor;


class CMap_Tool_Manager final : public CBase
{
public:
	enum OBJECT_MODE
	{
		OBJECT_PREVIEW,
		OBJECT_PICKING,
		OBJECT_END
	};
	enum LIST_TYPE
	{
		CREATE_MAP,
		SELECT_MAP,
		CREATE_OBJECT,
		MESH_LIST,
		CELL_LIST,
		SAVE_LIST,
		LIST_END
	};

	enum NAVIGATION_MODE
	{
		NAV_CREATE,
		NAV_EDIT,
		NAV_END
	};

	enum CELL_STATE
	{
		NORMAL,
		START,
		CELL_STATE_END
	};

private:
	typedef struct tagMapObjList
	{
		_tchar szName[MAX_PATH];
		_uint  iObjKey;
	}MAP_OBJ;

private:
	CMap_Tool_Manager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	~CMap_Tool_Manager() = default;
	CMap_Tool_Manager(const CMap_Tool_Manager& _pManager) = delete;

public:
	void				Update_Tool();

private:
	HRESULT				Initialize(CImguiLogger* _pLogger);


#pragma region Input & Imgui Method
private:
	// 인풋 로직 
	void				Input_Object_Tool_Mode();
	void				Input_Navigation_Tool_Mode();

	// Imgui 메인 로직
	void				Update_Imgui_Logic();
	void				Object_Info_Imgui(_bool _isLock = false);
	void				Object_Create_Imgui(_bool _isLock = false);
	void				Navigation_Imgui(_bool _isLock = false);
	void				SaveLoad_Imgui(_bool _isLock = false);
#pragma endregion

#pragma region Object Tool Method

	void				Load_ModelList();
	void				Load_SaveFileList();

	void				Object_Clear(_bool _bSelected = true);

	void				Save_Popup();

	void				Object_Open_PickingMode();
	void				Object_Clear_PickingMode();

	void				Object_Open_PreviewMode();
	void				Object_Close_PreviewMode();



	// 오브젝트 파일 저장
	void				Save(bool bSelected = true);
	// 오브젝트 파일 로드
	void				Load(bool bSelected = true);



	HRESULT				Picking_On_Terrain(_float3* _fPickingPos, CMapObject** _ppMap);
	HRESULT				Picking_On_Terrain(_float3* _fPickingPos);

	CMapObject*			Picking_On_Object();


#pragma endregion

#pragma region Navigation Tool Method

	CNavigationVertex*	Picking_On_Vertex();
	HRESULT				Create_Cell();
	HRESULT				Compute_World_PickingLay(_float3* _fLayPos, _float3* _fLayDir);


	void				Clear_StackVertex();
	void				Clear_SelectCell();
	bool				Check_VertexSelect();
#pragma endregion




private:
	ID3D11Device*					m_pDevice;
	ID3D11DeviceContext*			m_pContext;
	CGameInstance*					m_pGameInstance = { nullptr };
	CImguiLogger*					m_pLogger = nullptr;


	CMapObject*						m_arrObjects[OBJECT_END] = { nullptr, };
	_wstring						m_arrSelectName[LIST_END];

	vector<_wstring>				m_SaveFileLists;
	vector<_wstring>				m_ObjectFileLists;
	vector<MAP_OBJ>					m_MapObjLists;


	_wstring						m_strCacheFilePath = L"../../Client/Bin/cashe.cchc";
	_wstring						m_strObjectLoadFilePath = L"../../Client/Bin/Resources/Models/Object/";
	_char							m_szSaveFileName[MAX_PATH];

	string							m_arrCellStateName[CELL_STATE_END] =
	{
		"Normal",
		"Start"
	};


	_float3							m_fPickingPos = _float3();
	_float3							m_fPreviewPos = _float3();


	_bool							m_bNaviMode = false;
	NAVIGATION_MODE					m_eNaviMode = NAV_CREATE;

	CCellContainor*					m_pCellContainor;
	vector<CNavigationVertex*>		m_vecVertexStack;
	_int							m_iSelectCellIndex = { -1 };
	CNavigationVertex*				m_pPickingVertex = { nullptr };

public:
	virtual void Free();
	static CMap_Tool_Manager* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CImguiLogger* _pLogger);

};

END

