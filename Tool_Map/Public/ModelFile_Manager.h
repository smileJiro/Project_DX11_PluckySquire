#pragma once

#include "Base.h"
#include "Map_Tool_Defines.h"
#include "ToolObject.h"
#include "Model.h"




BEGIN(Map_Tool)

class CImgui_Manager;


typedef struct tagModelTile
{
	_tchar szFilePath[MAX_PATH];
	MODEL_TYPE eType;
	bool bLoad;
	bool bBinary;
} MODEL_FILE;



class CModelFile_Manager final : public CBase
{
public :
	enum LOAD_MODE
	{
		NORMAL,
		TO_BINARY,
		TO_BINARY_ONE,
		LOAD_ANIMATION,
		LOAD_END
	};

private:
	explicit CModelFile_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	~CModelFile_Manager() = default;
public:
	HRESULT Initialize(CImgui_Manager* pImguiManager);
	HRESULT ReLoad_FileList();
	HRESULT Start_All_binary(_uint* iSuccessFileList = nullptr);
	HRESULT Start_One_binary(const wstring& fileName, CModel* pModel);
	HRESULT Import_Animation(const wstring& fileName, CModel* pModel);

public:
	bool IsBinary(const _wstring& strName);
	bool IsLoad(const _wstring& strName);
	bool IsObjectType(const _wstring& strName, MODEL_TYPE eType);

	HRESULT Find_ModelFile(const _wstring& strName, MODEL_FILE* fModel = nullptr);
	const map<wstring, MODEL_FILE>& Get_FileList() { return m_ModelFileMap; }
	HRESULT To_Binary();
	HRESULT To_Binary_One();
	HRESULT To_Animation_Load();
	HRESULT To_Binary_Model(const string& TagText, Engine::CModel* pModel, HANDLE hFile, LOG_TYPE logType = LOG_BINARY);

	LOAD_MODE	Update_Binary();


	bool	IsBinaryLoading() { return m_bBinaryLoading; }


	



private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;

	CGameInstance* m_pGameInstance = { nullptr };

	CImgui_Manager* m_pImguiManager = { nullptr };

	map<wstring, MODEL_FILE> m_ModelFileMap;
	string strMyBinaryFileExtension = ".chc";


	CRITICAL_SECTION			m_Critical_Section = { nullptr };
	HANDLE						m_hThread = {};

	bool						m_bBinaryLoading = false;
	bool						m_bBinaryFinished = false;
	bool						m_bNeedLoading = false;

	_uint						m_iModelFileCount = { 0 };
	_uint						m_iBinaryFileCount = { 0 };

	CModel*						m_pOneLoadingModel = nullptr;
	_wstring					m_pOneLoadingModelName = {};

	_wstring					m_strLoadAnimationPath = {};


	LOAD_MODE					m_eCurMode = NORMAL;

public:
	virtual void Free() override;
	static CModelFile_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CImgui_Manager* pImguiManager);

};

END

