#include "stdafx.h"
#include "ModelFile_Manager.h"
#include <filesystem>
#include <iostream>
#include "Bone.h"
#include "Mesh.h"
#include "Material.h"
#include "Animation.h"
#include "Channel.h"
#include "GameInstance.h"
#include <codecvt>
#include "Imgui_Manager.h"
using namespace std::filesystem;



_uint APIENTRY AllBinaryMain(void* pArg)
{
	CModelFile_Manager* pManager = static_cast<CModelFile_Manager*>(pArg);

	if (FAILED(pManager->To_Binary()))
		return 1;

	return 0;
}



_uint APIENTRY OneBinaryMain(void* pArg)
{
	CModelFile_Manager* pManager = static_cast<CModelFile_Manager*>(pArg);

	if (FAILED(pManager->To_Binary_One()))
		return 1;

	return 0;
}



_uint APIENTRY AnimationLoadMain(void* pArg)
{
	CModelFile_Manager* pManager = static_cast<CModelFile_Manager*>(pArg);

	if (FAILED(pManager->To_Animation_Load()))
		return 1;

	return 0;
}



CModelFile_Manager::CModelFile_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	m_pDevice(pDevice),
	m_pContext(pContext),
	m_pGameInstance(Engine::CGameInstance::GetInstance())
{
	Safe_AddRef(pDevice);
	Safe_AddRef(pContext);
	Safe_AddRef(m_pGameInstance);

}

HRESULT CModelFile_Manager::Initialize(CImgui_Manager* pImguiManager)
{
	m_pImguiManager = pImguiManager;
	Safe_AddRef(m_pImguiManager);

	return ReLoad_FileList();

}

HRESULT CModelFile_Manager::ReLoad_FileList()
{
	m_iModelFileCount = 0;
	m_iBinaryFileCount = 0;
	if (m_bBinaryLoading)
		return E_FAIL;

	bool bFirstLoad = m_ModelFileMap.empty();

	_wstring strDefaultPath = TEXT("../../Client/Bin/resources/Models/");
	_wstring strPath[] = { TEXT("Object"),
							TEXT("Player"),
							TEXT("Map"),
							TEXT("Enemy"),
	};

	for (_int i = 0; i < size(strPath); ++i)
	{
		for (const auto& entry : ::recursive_directory_iterator(strDefaultPath + strPath[i] + L"/"))
		{
			if (is_directory(entry))
			{
				MODEL_TYPE eModelType = MODEL_END;

				if (TEXT("Anim") == entry.path().filename())
					eModelType = MODEL_ANIM;

				if (TEXT("NonAnim") == entry.path().filename())
					eModelType = MODEL_NONE;

				if (strPath[i] == L"Map")
					eModelType = MODEL_MAP;

				if (eModelType == MODEL_END)
					continue;

				for (const auto& file : ::recursive_directory_iterator(entry))
				{
					if (file.path().extension() == ".fbx" || file.path().extension() == ".chc")
					{

						wstring strKey = file.path().stem().wstring();

						bool bExist = false;
						if (SUCCEEDED(Find_ModelFile(strKey)))
							bExist = true;

						MODEL_FILE tFile;
						lstrcpy(tFile.szFilePath, file.path().relative_path().parent_path().wstring().c_str());
						tFile.bLoad = bFirstLoad;
						tFile.eType = eModelType;

						for (const auto& BinaryfileLoop : ::recursive_directory_iterator(entry))
							if (tFile.bBinary = (BinaryfileLoop.path().extension() == strMyBinaryFileExtension
								&&
								BinaryfileLoop.path().stem() == strKey))
								break;


						if (!bExist)
						{
							m_ModelFileMap.try_emplace(strKey, tFile);
						}
						else
						{
							m_ModelFileMap.find(strKey)->second.bBinary = tFile.bBinary;
						}
						m_iModelFileCount++;
						if (tFile.bBinary)
							m_iBinaryFileCount++;
						else
							int a = 1;
					}
				}
			}
		}
	}


	m_bNeedLoading = m_iModelFileCount == m_iBinaryFileCount;

	return S_OK;
}

HRESULT CModelFile_Manager::Start_All_binary(_uint* iSuccessFileList)
{
	ReLoad_FileList();

	if (m_bNeedLoading)
	{
		LOG_TYPE("No files required for fbx loading or binary loading!",LOG_BINARY);
		return E_FAIL;
	}

	LOG("============================================");


	m_bBinaryLoading = true;
	InitializeCriticalSection(&m_Critical_Section);
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, AllBinaryMain, this, 0, nullptr);
	return S_OK;
}

HRESULT CModelFile_Manager::Start_One_binary(const wstring& fileName, CModel* pModel)
{
	m_pOneLoadingModel = pModel;
	m_pOneLoadingModelName = fileName;
	if (!m_pOneLoadingModel)
	{
		LOG_TYPE("No files required for fbx loading or binary loading!",LOG_BINARY);
		return E_FAIL;
	}



	m_bBinaryLoading = true;
	InitializeCriticalSection(&m_Critical_Section);
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, OneBinaryMain, this, 0, nullptr);
	return S_OK;
}

HRESULT CModelFile_Manager::Import_Animation(const wstring& fileName, CModel* pModel)
{
	m_strLoadAnimationPath = fileName;
	m_bBinaryLoading = true;
	m_pOneLoadingModel = pModel;
	InitializeCriticalSection(&m_Critical_Section);
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, AnimationLoadMain, this, 0, nullptr);
	return S_OK;
}



bool CModelFile_Manager::IsBinary(const _wstring& strName)
{
	MODEL_FILE tFile = {};
	if (SUCCEEDED(Find_ModelFile(strName, &tFile)))
		return tFile.bBinary;
	return false;
}

bool CModelFile_Manager::IsLoad(const _wstring& strName)
{
	MODEL_FILE tFile = {};
	if (SUCCEEDED(Find_ModelFile(strName, &tFile)))
		return tFile.bLoad;
	return false;
}

bool CModelFile_Manager::IsObjectType(const _wstring& strName, MODEL_TYPE eType)
{
	MODEL_FILE tFile = {};
	if (SUCCEEDED(Find_ModelFile(strName, &tFile)))
		return tFile.eType == eType;
	return false;
}

HRESULT CModelFile_Manager::Find_ModelFile(const _wstring& strName, MODEL_FILE* fModel)
{
	auto iter = m_ModelFileMap.find(strName);

	if (iter == m_ModelFileMap.end())
		return E_FAIL;
	if (fModel)
		*fModel = iter->second;
	return S_OK;
}

HRESULT CModelFile_Manager::To_Binary()
{
	wstring strProtoName = L"Prototype_Component_Model_";
	std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;

	for (auto& Pair : m_ModelFileMap)
	{
		string TagText = myconv.to_bytes(Pair.first);

		if (!Pair.second.bLoad)
		{
			LOG_TYPE("New File Access ! : " + TagText,LOG_BINARY);

			_matrix matDefault = XMMatrixIdentity();
				XMMatrixScaling(0.01f, 0.01f, 0.01f);
			_wstring wideFilePath = Pair.second.szFilePath;
			wideFilePath += L"/";
			wideFilePath += Pair.first;
			wideFilePath += L".fbx";

			if (SUCCEEDED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, strProtoName + Pair.first,
				CModel::Create(m_pDevice, m_pContext, Pair.second.eType,
					myconv.to_bytes(wideFilePath).c_str(), matDefault))))
				LOG_TYPE(TagText + " : Load Complete!",LOG_BINARY);
			else
			{
				LOG_TYPE(TagText + " : Load Failed...!",LOG_BINARY);
				continue;
			}


		}

		if (!Pair.second.bBinary)
		{

			CModel* pModel = nullptr;


			CBase* pObj = m_pGameInstance->
				Clone_Prototype(Engine::PROTOTYPE::PROTO_COMPONENT,
					LEVEL_STATIC, strProtoName + Pair.first
				);

			if (nullptr != pObj)
			{
				pModel = dynamic_cast<CModel*>(pObj);
				if (pModel)
				{
					LOG_TYPE(TagText + " : Binary Start...",LOG_BINARY);
					MODEL_DEFAULT tDefault = pModel->Get_Info();
					HANDLE	hFile = CreateFile((wstring(Pair.second.szFilePath) + L"/" + Pair.first + L".chc").c_str(),		
						GENERIC_WRITE,			
						NULL,					
						NULL,					
						CREATE_ALWAYS,			
						FILE_ATTRIBUTE_NORMAL,	
						NULL);					

					if (INVALID_HANDLE_VALUE == hFile)
					{
						LOG_TYPE(TagText + " : FileHandle Open Failed...",LOG_BINARY);
						continue;
					}

					if (SUCCEEDED(To_Binary_Model(TagText, pModel, hFile)))
						LOG_TYPE(TagText + " : Binary Save Success!!!",LOG_BINARY);
					else
						LOG_TYPE(TagText + " : Binary Save Failed...",LOG_BINARY);

					CloseHandle(hFile);

				}
			}
		}


	}
	m_eCurMode = TO_BINARY;
	m_bBinaryFinished = true;
	return S_OK;
}

HRESULT CModelFile_Manager::To_Binary_One()
{
	bool bLoad = false;
	string TagText = "";
	std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
	wstring strProtoName = L"Prototype_Component_Model_";

	
	for (auto& Pair : m_ModelFileMap)
	{
		if (Pair.first == m_pOneLoadingModelName) 
		{
			bLoad = Pair.second.bLoad;
			TagText = myconv.to_bytes(Pair.first);
			HANDLE	hFile = CreateFile((wstring(Pair.second.szFilePath) + L"/" + Pair.first + L".chc").c_str(),		
				GENERIC_WRITE,			
				NULL,					
				NULL,					
				CREATE_ALWAYS,			
				FILE_ATTRIBUTE_NORMAL,	
				NULL);
			if (INVALID_HANDLE_VALUE == hFile)
			{
				DWORD err = GetLastError();
				LOG_TYPE(TagText + " : FileHandle Open Failed...", LOG_SAVE);
				continue;
			}

			if (SUCCEEDED(To_Binary_Model(TagText, m_pOneLoadingModel, hFile)))
				LOG_TYPE(TagText + " : Binary Save Success!!!", LOG_SAVE);
			else
				LOG_TYPE(TagText + " : Binary Save Failed...", LOG_SAVE);

			CloseHandle(hFile);
			break;
		}
	}			

	
	if (bLoad)
	{
		// TODO :: 모델 교체하는 작업. 하자. 
		//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, strProtoName + myconv.from_bytes(TagText),
		//	m_pOneLoadingModel)))
		//	return E_FAIL;
	
	}

	m_eCurMode = TO_BINARY_ONE;

	m_pOneLoadingModel = nullptr;
	m_bBinaryFinished = true;

	return S_OK;
}

HRESULT CModelFile_Manager::To_Animation_Load()
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;

	bool bLoad = false;

	_matrix matDefault = XMMatrixIdentity();

	LOG_TYPE("Animation Load Start...", LOG_LOAD);

	CModel* pModel = CModel::Create(m_pDevice, m_pContext, MODEL_ANIM,
		myconv.to_bytes(m_strLoadAnimationPath).c_str(), matDefault);

	if(nullptr == pModel)	
		LOG_TYPE("Animation Load Fail...", LOG_ERROR);

	LOG_TYPE("Animation Load Success!!!", LOG_LOAD);


	_uint iAnimImport = pModel->Get_Info().iNumAnimations;

	_bool bCompare = pModel->Compare_Model(m_pOneLoadingModel);

	if (bCompare)
	{
		pModel->Add_Animations(m_pOneLoadingModel);
		LOG_TYPE("Animation Add : " + std::to_string(iAnimImport) + " Animations Import Success!!!", LOG_LOAD);
	
	}
	else 
	{
		LOG_TYPE("The selected model does not match the animation...", LOG_LOAD);
	}

	Safe_Release(pModel);

	m_pOneLoadingModel = nullptr;
	m_eCurMode = LOAD_ANIMATION;
	m_bBinaryFinished = true;

	return S_OK;
}

HRESULT CModelFile_Manager::To_Binary_Model(const string& TagText, Engine::CModel* pModel, HANDLE hFile, LOG_TYPE logType)
{
	DWORD	dwByte(0);

	MODEL_TYPE eType = pModel->Get_Info().eType;

	WriteFile(hFile, &pModel->Get_Info(), sizeof(MODEL_DEFAULT), &dwByte, nullptr);


	if (eType == MODEL_ANIM)
	{
		LOG_TYPE(TagText + " : Bones Save...", logType);
		if (FAILED(pModel->Bones_Export(hFile)))
			return E_FAIL;

		LOG_TYPE(TagText + " : Animations Save...", logType);
		if (FAILED(pModel->Animations_Export(hFile)))
			return E_FAIL;
	}


	LOG_TYPE(TagText + " : Mesh Save...", logType);
	if (FAILED(pModel->Meshes_Export(hFile)))
		return E_FAIL;

	LOG_TYPE(TagText + " : Material Save...", logType);
	if (FAILED(pModel->Materials_Export(hFile)))
		return E_FAIL;

	return S_OK;
}


CModelFile_Manager::LOAD_MODE CModelFile_Manager::Update_Binary()
{
	if (m_bBinaryLoading)
	{
		if (m_bBinaryFinished)
		{
			DeleteObject(m_hThread);
			DeleteCriticalSection(&m_Critical_Section);
			m_bBinaryLoading = false;
			m_bBinaryFinished = false;
			if(m_eCurMode == TO_BINARY)
				ReLoad_FileList();
			LOAD_MODE eReturnModel = m_eCurMode;
			m_eCurMode = NORMAL;
			return eReturnModel;
		}
	}
	return NORMAL;
}

void CModelFile_Manager::Free()
{
	if (m_bBinaryLoading)
	{
		WaitForSingleObject(m_hThread, INFINITE);

		DeleteObject(m_hThread);

		DeleteCriticalSection(&m_Critical_Section);
	}

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pImguiManager);

	__super::Free();
}

CModelFile_Manager* CModelFile_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CImgui_Manager* pImguiManager)
{
	CModelFile_Manager* pInstance = new CModelFile_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pImguiManager)))
	{
		MSG_BOX("Failed to Created : CModelFile_Manager");
		Safe_Release(pInstance);
	}
	return pInstance;
}
