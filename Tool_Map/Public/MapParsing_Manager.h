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
	HRESULT	Update();
public :
	HRESULT		Open_ParsingDialog();
	void		Push_Parsing(const string& _strParsingFileName, const wstring& _strLayerName);
	HRESULT		Parsing();


private :
	void		Open_Parsing();
	HRESULT		Parsing(json _jsonObj);
private :
	CGameInstance*					m_pGameInstance = nullptr;
	ID3D11Device*					m_pDevice = nullptr;
	ID3D11DeviceContext*			m_pContext = nullptr;
	CImguiLogger*					m_pLogger =  nullptr;

	// ÆÄ½ÌÇØ¿Â ¸ðµ¨ Á¤º¸.
	vector<pair<string, MAP_DATA>>	m_Models;
	queue<pair<string, wstring>>	m_LoadInfos;
	// ÆÄ½Ì½º·¹µå ºÐ¸®.
	CRITICAL_SECTION				m_Critical_Section = { nullptr };
	HANDLE							m_hThread = {};


	_bool							m_isLoading = false;
	_bool							m_isLoadComp = true;


public :
	static CMapParsing_Manager* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CImguiLogger* _pLogger);
	virtual void Free() override;

};

END
