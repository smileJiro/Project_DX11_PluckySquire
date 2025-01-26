#pragma once

#include "Map_Tool_Defines.h"
#include "Base.h"
BEGIN(Engine)
class CGameInstance;
class CGameObject;
class CLayer;
END



BEGIN(Map_Tool)


class CMapObject;
class CModelFile_Manager;
class CImguiLogger;
class CTask_Manager;
class C2DDefault_RenderObject;
class C2DTile_RenderObject;


class C2DMap_Tool_Manager final : public CBase
{
private :
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
private:
	typedef struct tagMapObjList
	{
		_tchar szName[MAX_PATH];
		_uint  iObjKey;
	}MAP_OBJ;

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
	void				Input_Object_Tool_Mode();

	// Imgui 메인 로직
	void				Update_Imgui_Logic();
	void				Object_Create_Imgui(_bool _isLock = false);
	void				SaveLoad_Imgui(_bool _isLock = false);

#pragma endregion

#pragma region Object Tool Method

	void				Load_2DModelList();
	void				Load_SaveFileList();

	void				Object_Clear(_bool _bSelected = true);

	void				Save_Popup();


	// 오브젝트 파일 저장
	void				Save(bool bSelected = true);
	// 오브젝트 파일 로드
	void				Load(bool bSelected = true);



	HRESULT				Picking_On_Terrain(_float3* _fPickingPos, CMapObject** _ppMap);
	HRESULT				Picking_On_Terrain(_float3* _fPickingPos);

	CMapObject*			Picking_On_Object();


#pragma endregion

	HRESULT				Compute_World_PickingLay(_float3* _fLayPos, _float3* _fLayDir);

	void				Init_Egnore_Layer();
	HRESULT				Setting_Action_Layer(vector<pair<wstring, CLayer*>>& _TargetLayerPairs);


	
	HRESULT				Setting_TileMap(const _string _strFileMapJsonName);



private:
	ID3D11Device*					m_pDevice;
	ID3D11DeviceContext*			m_pContext;
	CGameInstance*					m_pGameInstance = { nullptr };
	CImguiLogger*					m_pLogger = nullptr;
	CTask_Manager*					m_pTaskManager;
	C2DTile_RenderObject*			m_pTileRenderObject = nullptr;
	
	_wstring						m_arrSelectName[LIST_END];


	vector<pair<_wstring, _wstring>>m_ObjectFileLists;

	vector<_wstring>				m_SaveFileLists;

	_wstring						m_strMapBinaryPath = L"../../Client/Bin/MapSaveFiles/2D/";
	_char							m_szSaveFileName[MAX_PATH];
	_char							m_szImportLayerTag[MAX_PATH];
	_wstring						m_strPickingLayerTag;

	vector<wstring>					m_DefaultEgnoreLayerTags;
	vector<wstring>					m_EgnoreLayerTags;
	C2DDefault_RenderObject*		m_DefaultRenderObject;



public:
	virtual void Free();
	static C2DMap_Tool_Manager* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CImguiLogger* _pLogger);

};

END

