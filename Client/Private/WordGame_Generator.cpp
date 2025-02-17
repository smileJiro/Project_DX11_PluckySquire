#include "stdafx.h"
#include "WordGame_Generator.h"
#include "Word.h"
#include "Word_Controller.h"
#include "GameInstance.h"
#include "RenderGroup_MRT.h"
#include "WordPrinter.h"
#include "Section_Manager.h"

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
}

HRESULT CWordGame_Generator::Load_WordTexture(const _wstring& _strJsonPath)
{
	std::string filePathDialog = WstringToString(_strJsonPath);
	std::ifstream inputFile(filePathDialog);
	json jsonDialogs;
	inputFile >> jsonDialogs;

	if (!inputFile.is_open()) 
		return E_FAIL;

	if (jsonDialogs.is_array())
	{
		_uint iWordCount = (_uint)jsonDialogs.size();
		m_Words.resize(iWordCount);
		for (auto& ChildJson : jsonDialogs)
		{
			_string strWord = ChildJson["Word_Name"];
			_uint   iIndex=  ChildJson["Word_Index"];

			_vector vScale = m_pGameInstance->Measuring(L"Font28", StringToWstring(strWord));
			_float2 fTargetSize = { XMVectorGetX(vScale), XMVectorGetY(vScale) };


			ID3D11ShaderResourceView* pSRV = m_pWordPrinter->Print_Word(strWord, fTargetSize);


			if (0 >= iIndex || iWordCount <= iIndex)
				return E_FAIL;

			CWord::WORD_DESC Desc = {};

			Desc.fSize = fTargetSize;
			Desc.pSRV = pSRV;

			CBase* pBase = m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, SECTION_MGR->Get_SectionLeveID(),
				L"Prototype_GameObject_Word",
				&Desc
				);

			if (nullptr == pBase)
			{
				Safe_Release(pSRV);
				return E_FAIL;
			}
			else 
				m_Words[iIndex] = static_cast<CWord*>(pBase);

		}
	}

	return S_OK;
}

HRESULT CWordGame_Generator::Clear_WordTexture()
{
	for (auto pWord : m_Words)
		Safe_Release(pWord);

	m_Words.clear();
	return S_OK;
}

CWord* CWordGame_Generator::Find_Word(_uint _iWordIndex)
{
	if (0 >= _iWordIndex || m_Words.size() <= _iWordIndex)
		return nullptr;


	return m_Words[_iWordIndex];
}

HRESULT CWordGame_Generator::WordGame_Generate(CSection_2D* _pSection, const _wstring& _strJsonPath)
{

	std::string filePathDialog = WstringToString(_strJsonPath);
	std::ifstream inputFile(filePathDialog);
	json jsonDialogs;
	inputFile >> jsonDialogs;
	if (!inputFile.is_open()) {
		return E_FAIL;
	}
	if (jsonDialogs.is_array())
	{
		for (auto& ChildJson : jsonDialogs)
		{
			//문장 생성.
			if(
				ChildJson.contains("Container_List") &&
				ChildJson["Container_List"].is_array())			
			{
				for (auto& Container: ChildJson["Container_List"])
				{
					CWord_Controller* pContainer = CWord_Controller::Create(m_pDevice, m_pContext, Container);
					if (nullptr != pContainer)
						return E_FAIL;
					else
					{
						_pSection->Add_GameObject_ToSectionLayer(pContainer, SECTION_2D_PLAYMAP_OBJECT);
						Event_CreateObject(LEVEL_CHAPTER_2,
							L"Layer_2DWordObject",
							pContainer);
					}
				}
			}
			// 오브젝트 생성
			if (
				ChildJson.contains("Word_Object_List") &&
				ChildJson["Word_Object_List"].is_array())
			{
				for (auto& WordObject : ChildJson["Word_Object_List"])
				{
					_string strText = WordObject["Word_Object_Tag"];
					_float2 fPos = { WordObject["Position"][0].get<_float>(),  WordObject["Position"][1].get<_float>() };
					_float2 fScale = { WordObject["Scale"][0].get<_float>(),  WordObject["Scale"][1].get<_float>() };


					if (
						WordObject.contains("Image_Tag_List") &&
						WordObject["Image_Tag_List"].is_array())
					{
						for (auto& Image : ChildJson["Image_Tag_List"])
						{
							_string strPath = Image["ImagePath"];
						}
					}
					if (
						WordObject.contains("Word_Action_List") &&
						WordObject["Word_Action_List"].is_array())
					{
						for (auto& WordAction : ChildJson["Word_Action_List"])
						{
							_uint iContainerIndex = WordAction["Container_Index"];
							CWord::WORD_TYPE eType = WordAction["Word_Type"]; 
							_string strActionType = WordAction["Action_Type"];
							_bool isParam = WordAction["Param"];
						}
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
	__super::Free();
}
