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
	HRESULT					Update();
public :
	HRESULT					Open_ParsingDialog(const wstring& _strLayerName);
	void					Register_Parsing(const string& _strParsingFileName, const wstring& _strLayerName, _bool _isClear = true);
	HRESULT					Parsing();

	void					Register_RePackaging(const wstring& _strParsingFileName, CMapObject* pMapObject);
	HRESULT					RePackaging();
	void					Bone_Packaging(vector<CBone*>& _Bones, _uint _iBoneIdx, ofstream& _outFile);


	HRESULT					Export_SaveResult_ToJson(const _wstring _strFIlePath, const vector<_string>& _SaveModelProtoNames, _bool isStatic = false);



	const vector<_string>&	Get_EgnoreObjectNames() const { return m_EgnoreObjectNames; };
	HRESULT					Push_EgnoreModelName(_string _strModelName) 
	{ if (!m_isLoading) m_EgnoreObjectNames.push_back(_strModelName); }
	_bool					Is_Loading() { return m_isLoading; }
	_bool					Is_Loading_Complete() { return m_isLoadComp; }


private:
	//void					Model_ReParsing(const _wstring _strModelPath);



private :
	void					Open_Parsing();
	void					Open_RePackaging();
	HRESULT					Parsing(json _jsonObj);

	_bool					Check_EgnoreObject(_string _strModelName);
private :
	CGameInstance*								m_pGameInstance = nullptr;
	ID3D11Device*								m_pDevice = nullptr;
	ID3D11DeviceContext*						m_pContext = nullptr;
	CImguiLogger*								m_pLogger =  nullptr;

	// ÆÄ½ÌÇØ¿Â ¸ðµ¨ Á¤º¸.
	vector<pair<string, MAP_DATA>>				m_Models;
	queue<TASK>									m_LoadTasks;
	// ÆÄ½Ì½º·¹µå ºÐ¸®.
	CRITICAL_SECTION							m_Critical_Section = { nullptr };
	HANDLE										m_hThread = {};

	vector<string>								m_MapObjectNames;
	vector<string>								m_EgnoreObjectNames;

	_wstring									m_strUmapJsonPath = L"..\\Bin\\json\\";

	TASK										m_tCurWork;

	_bool										m_isLoading = false;
	_bool										m_isLoadComp = true;


public :
	static CTask_Manager* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CImguiLogger* _pLogger);
	virtual void Free() override;

};

END
