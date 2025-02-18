#include "stdafx.h"
#include "WordGame_Generator.h"
#include "Word.h"
#include "Word_Controller.h"
#include "GameInstance.h"
#include "RenderGroup_MRT.h"
#include "WordPrinter.h"
#include "Section_Manager.h"
#include "2DMapWordObject.h"

CWordGame_Generator::CWordGame_Generator(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:
	m_pDevice(_pDevice),
	m_pContext(_pContext),
	m_pGameInstance(CGameInstance::GetInstance()),
	CBase()
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CWordGame_Generator::Initialize()
{
	m_pWordPrinter = CWordPrinter::Create(m_pDevice,m_pContext);
	if (nullptr == m_pWordPrinter)
		return E_FAIL;
	return S_OK;
}

HRESULT CWordGame_Generator::Load_Word(const _wstring& _strJsonPath)
{
	Clear_Word();

	_wstring strChapterPath = MINIGAME_WORD_PATH;
	strChapterPath += _strJsonPath + L".json";
	if (!filesystem::exists(strChapterPath))
	{ 
		return S_OK;
	}


	std::string filePathDialog = WstringToString(strChapterPath);
	std::ifstream inputFile(filePathDialog);
	json jsonDialogs;

	if (!inputFile.is_open()) 
		return E_FAIL;

	inputFile >> jsonDialogs;

	if (jsonDialogs.is_array())
	{
		_uint iWordCount = (_uint)jsonDialogs.size();
		m_Words.resize(iWordCount);
		_uint iLoop = 0;
		for (auto& ChildJson : jsonDialogs)
		{
			_string strWord = ChildJson["Word_Name"];
			_wstring wstrWord = StringToWstring(strWord);
			_uint   iIndex=  ChildJson["Word_Index"];

			_vector vScale = m_pGameInstance->Measuring(L"Font28", wstrWord);
			_float2 fTargetSize = { XMVectorGetX(vScale), XMVectorGetY(vScale) };


			ID3D11ShaderResourceView* pSRV = m_pWordPrinter->Print_Word(wstrWord, fTargetSize);


			if (0 > iIndex || iWordCount <= iIndex)
				return E_FAIL;

			CWord::WORD_DESC Desc = {};

			Desc.fSize = fTargetSize;
			Desc.pSRV = pSRV;
			Desc.eType = (CWord::WORD_TYPE)iLoop;
			Desc.strText = wstrWord;

			CBase* pBase = m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC,
				L"Prototype_GameObject_Word",
				&Desc
				);

			if (nullptr == pBase)
			{
				Safe_Release(pSRV);
				return E_FAIL;
			}
			else
			{	
				m_Words[iIndex] = static_cast<CWord*>(pBase);
				m_Words[iIndex]->Set_Active(false);

				Event_CreateObject(SECTION_MGR->Get_SectionLeveID(), L"Layer_WordObj", m_Words[iIndex]);

			}
			iLoop++;
		}
	}

	return S_OK;
}

HRESULT CWordGame_Generator::Clear_Word()
{
	for (auto pWord : m_Words)
		Safe_Release(pWord);

	m_Words.clear();
	return S_OK;
}

CWord* CWordGame_Generator::Find_Word(_uint _iWordIndex)
{
	if (0 > _iWordIndex || m_Words.size() <= _iWordIndex)
		return nullptr;


	return m_Words[_iWordIndex];
}

HRESULT CWordGame_Generator::WordGame_Generate(CSection_2D* _pSection, const _wstring& _strJsonPath)
{
	_wstring strChapterPath = MINIGAME_WORD_PATH;
	strChapterPath += _strJsonPath + L".json";
	std::string filePathDialog = WstringToString(strChapterPath);
	std::ifstream inputFile(filePathDialog);
	json jsonDialogs;
	inputFile >> jsonDialogs;

	LEVEL_ID eCurLevelID = (LEVEL_ID)SECTION_MGR->Get_SectionLeveID();

	if (!inputFile.is_open()) {
		return E_FAIL;
	}
	if (jsonDialogs.is_array())
	{
		for (auto& ChildJson : jsonDialogs)
		{
			// 오브젝트 생성
			if (
				ChildJson.contains("Word_Object_List") &&
				ChildJson["Word_Object_List"].is_array())
			{
				for (auto& WordObject : ChildJson["Word_Object_List"])
				{
					C2DMapWordObject::WORD_OBJECT_DESC Desc = {};
					Desc.WordObjectJson = WordObject;

					CGameObject* pGameObject = nullptr;

					if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, 
						TEXT("Prototype_GameObject_2DMap_WordObject"),
						eCurLevelID, L"Layer_2DMapWordObject", &pGameObject, &Desc)))
						return E_FAIL;
					if (nullptr != pGameObject)
					{
						_pSection->Add_GameObject_ToSectionLayer(pGameObject, SECTION_2D_PLAYMAP_OBJECT);
					}

				}
			}

			//문장 생성.
			if(
				ChildJson.contains("Controller_List") &&
				ChildJson["Controller_List"].is_array())			
			{
				for (auto& Container: ChildJson["Controller_List"])
				{
					CWord_Controller* pContainer = CWord_Controller::Create(m_pDevice, m_pContext, _pSection, Container);
					if (nullptr == pContainer)
						return E_FAIL;
					else
					{
						_pSection->Add_GameObject_ToSectionLayer(pContainer, SECTION_2D_PLAYMAP_OBJECT);
						Event_CreateObject(eCurLevelID,
							L"Layer_Word",
							pContainer);
					}
				}
			}
		}
	}

	return S_OK;
}

CWordGame_Generator* CWordGame_Generator::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CWordGame_Generator* pInstance = new CWordGame_Generator(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CWordGame_Generator");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWordGame_Generator::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pWordPrinter);
	__super::Free();
}
