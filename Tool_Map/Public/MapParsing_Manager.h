#pragma once


#include "Base.h"
#include "Map_Tool_Defines.h"
#include <json.hpp>
using json = nlohmann::json;


BEGIN(Engine)
class CGameInstance;
END
BEGIN(Map_Tool)





class CMapParsing_Manager final : public CBase
{

private :
	typedef struct tagMapObjectParsingData
	{
		_float3 fPos;
		_float3 fRotate;
		_float3 fScale = {1.f,1.f,1.f};
	}MAP_DATA;

private :
	CMapParsing_Manager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CMapParsing_Manager() = default;

public :
	HRESULT Initialize(CImguiLogger* _pLogger);

public :
	HRESULT Open_ParsingDialog();
	HRESULT Parsing(const string& _strParsingName);
	HRESULT Parsing(json _jsonObj);

private :
	CGameInstance*			m_pGameInstance = nullptr;

	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;
	CImguiLogger*			m_pLogger =  nullptr ;

	vector<pair<string, MAP_DATA>> m_Models;
public :
	static CMapParsing_Manager* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CImguiLogger* _pLogger);
	virtual void Free() override;

};

END
