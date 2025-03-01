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


class CModel_Tool_Manager final : public CBase
{
public:
	enum LIST_TYPE
	{
		SELECT_MODEL,
		LIST_END
	};

private:
	typedef struct tagMapObjList
	{
		_tchar szName[MAX_PATH];
		_uint  iObjKey;
	}MAP_OBJ;

private:
	CModel_Tool_Manager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	~CModel_Tool_Manager() = default;
	CModel_Tool_Manager(const CModel_Tool_Manager& _pManager) = delete;

public:
	void				Update_Tool();

private:
	HRESULT				Initialize(CImguiLogger* _pLogger);


#pragma region Input & Imgui Method
private:
	// 인풋 로직 
	void				Input_Logic();

	// Imgui 메인 로직
	void				Update_Imgui_Logic();
	void				Model_Material_Imgui(_bool _isLock = false);
	void				Model_Collider_Imgui(_bool _isLock = false);
#pragma endregion

#pragma region Object Tool Method

	void				Load_ModelList();


	// 오브젝트 파일 로드
	void				Load(const _string& strFileName, _bool bSelected = true);



#pragma endregion

	HRESULT				Compute_World_PickingLay(_float3* _fLayPos, _float3* _fLayDir);



private:
	ID3D11Device*					m_pDevice;
	ID3D11DeviceContext*			m_pContext;
	CGameInstance*					m_pGameInstance = { nullptr };
	CImguiLogger*					m_pLogger = { nullptr };
	CTask_Manager*					m_pTaskManager;



	CMapObject*						m_pCurObject			= { nullptr, };
	CMapObject*						m_pPreviewObject		= { nullptr, };
	
	_wstring						m_arrSelectName[LIST_END];

	vector<_wstring>				m_SaveFileLists;
	vector<pair<_wstring, _wstring>>m_ObjectFileLists;
	vector<MAP_OBJ>					m_MapObjLists;

	_float4x4						m_matView;
	_float4x4						m_matProj;

	_char							m_szSaveFileName[MAX_PATH];

public:
	virtual void Free();
	static CModel_Tool_Manager* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CImguiLogger* _pLogger);

};

END

