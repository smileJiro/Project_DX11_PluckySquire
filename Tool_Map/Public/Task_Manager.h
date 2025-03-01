#pragma once


#include "Base.h"
#include "Map_Tool_Defines.h"
#include <json.hpp>
using json = nlohmann::json;


BEGIN(Engine)
class CGameInstance;
class CBone;
END

BEGIN(Map_Tool)

class CMapObject;

class CTask_Manager final : public CBase
{

public :
	enum COOKING_MODE
	{
		COOKING_CONVEX,
		COOKING_TRI,
		COOKING_MULTI,
		COOKING_LAST
	};

private :

	enum TASK_TYPE
	{
		MAPDATA_PARSING,
		MODEL_RE_PACKAGING,
		TASK_END
	};
	

	typedef struct tagMapObjectParsingData
	{
		_float3 fPos;
		_float3 fRotate;
		_float3 fScale = {1.f,1.f,1.f};
		_bool isOverrideMaterial = false;
		_char szMaterialKey[MAX_PATH];
	}MAP_DATA;



	typedef struct tagWork
	{
		TASK_TYPE eType;
		_string strFilePath;
		
		_wstring strLayerName;
		_bool	isLayerClear;
		
		CMapObject* pTargetObject;
	}TASK;

private :
	CTask_Manager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CTask_Manager() = default;
	

public :
	HRESULT					Initialize(CImguiLogger* _pLogger);

	// 진행상황을 체크하고, 진행이 완료되었을 때 스레드 정지. 이후 큐에 작업이 쌓여있는지 확인 후, 작업이 남아있으면 다시금 스레드를 작동시킨다. 
	HRESULT					Update();


public :
	
	// 3D Map .umap(.json) 파일을 툴에 파싱해오는 작업을 등록하는 메소드
	void					Register_Parsing(const string& _strParsingFileName, const wstring& _strLayerName, _bool _isClear = true);
	// 3D Map .umap(.json) 파일을 선택하는 다이얼로그를 호출함 
	HRESULT					Open_ParsingDialog(const wstring& _strLayerName);
	

	// 3D 모델 리패킹 작업을 queue에 등록하는 메소드
	void					Register_RePackaging(const wstring& _strParsingFileName, CMapObject* pMapObject);
	// 리패킹 진행메소드. 
	HRESULT					RePackaging();
	// 파싱 진행메소드
	HRESULT					Parsing();
private:
	// 해당 코드작업 진행시 json 라이브러리에 대한 이해가 부족하여 실제 탐색은 해당 재귀메소드로 진행했다. Parsing() 내부에서만 쓰임. 아쉬움 
	HRESULT					Parsing(json _jsonObj);

public :
	// 3D Map SaveFile Result File(모델 사용이력 불러올때 쓰인다. 같은 경로에 저장된다.)
	HRESULT					Export_SaveResult_ToJson(const _wstring _strFIlePath, const vector<_string>& _SaveModelProtoNames, _bool isStatic = false);

	// 로그 저장용 ㄱ-
	HRESULT					Export_Result(const _wstring& _strFilePath, const _wstring& _strFileName, vector<_string> _ExportStrings );


	HRESULT					Cooking(const _string& _strCookingPath, CMapObject* _pMapObject, COOKING_MODE _eMode);



#pragma region Get_Set

	const vector<_string>&	Get_EgnoreObjectNames() const { return m_EgnoreObjectNames; };
	HRESULT					Push_EgnoreModelName(_string _strModelName) 
	{ if (!m_isLoading) m_EgnoreObjectNames.push_back(_strModelName); }

	_bool					Is_Loading() { return m_isLoading; }
	_bool					Is_Loading_Complete() { return m_isLoadComp; }
#pragma endregion

private:
	//void					Model_ReParsing(const _wstring _strModelPath);



private :
	void					Open_Parsing();
	void					Open_RePackaging();
	HRESULT					Find_Override_Material_Texture(CMapObject* _pMapObject, json _jsonObj);
	HRESULT					Find_Override_Material_Color(CMapObject* _pMapObject, json _jsonObj);

	_bool					Check_EgnoreObject(_string _strModelName);
private :
	CGameInstance*								m_pGameInstance = nullptr;
	ID3D11Device*								m_pDevice = nullptr;
	ID3D11DeviceContext*						m_pContext = nullptr;
	CImguiLogger*								m_pLogger =  nullptr;

	// 파싱해온 모델 정보.
	vector<pair<string, MAP_DATA>>				m_Models;
	queue<TASK>									m_LoadTasks;
	// 파싱스레드 분리.
	CRITICAL_SECTION							m_Critical_Section = { nullptr };
	HANDLE										m_hThread = {};

	vector<_string>								m_MapObjectNames;
	// 파싱할 때 무시할 모델 이름
	vector<_string>								m_EgnoreObjectNames;
	vector<_string>								m_RePackaging;
	vector<pair<_wstring,CMapObject*>>			m_RepackNeededModels;

	_wstring									m_strUmapJsonPath = L"../Bin/json/3DMapJson/";

	TASK										m_tCurWork;

	_bool										m_isLoading = false;
	_bool										m_isLoadComp = true;


public :
	static CTask_Manager* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CImguiLogger* _pLogger);
	virtual void Free() override;

};

END
