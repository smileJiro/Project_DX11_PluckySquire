#include "stdafx.h"
#include "Word_Controller.h"
#include "Word.h"
#include "FloorWord.h"
#include "Section_Manager.h"
#include "Word_Container.h"
#include <regex>

CWord_Controller::CWord_Controller(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CContainerObject(_pDevice,_pContext)
{
}

CWord_Controller::CWord_Controller(const CWord_Controller& _Prototype)
	: CContainerObject(_Prototype)
{
}

HRESULT CWord_Controller::Initialize_Prototype()
{
	return __super::Initialize_Prototype();
}

HRESULT CWord_Controller::Initialize(void* _pArg)
{
	return __super::Initialize(_pArg);
}

HRESULT CWord_Controller::Import(CSection_2D* _pSection, json _ControllerJson)
{
	// 컨트롤러  인덱스. 
	m_iControllerIndex = _ControllerJson["Controller_Index"];
	// 풀 텍스트
	_string strText = _ControllerJson["Text"];
	m_strOriginText = StringToWstring(strText);

	_float2 fPos = { _ControllerJson["Position"][0].get<_float>(),  
		_ControllerJson["Position"][1].get<_float>() };

	_float2 fScale = { _ControllerJson["Scale"][0].get<_float>(),  
		_ControllerJson["Scale"][1].get<_float>() };

	CContainerObject::CONTAINEROBJ_DESC Desc = {};
	Desc.Build_2D_Transform(fPos, fScale);


#pragma region 정규식 처리된 단어 확인.

	wregex pattern(L"##");

	auto words_begin = std::wsregex_iterator(m_strOriginText.begin(), m_strOriginText.end(), pattern);
	auto words_end = std::wsregex_iterator();

	_uint iCount = (_uint)std::distance(words_begin, words_end);

#pragma endregion

#pragma region 워드 확인

	// 문자 갯수만큼 파트 컨테이너 초기화 생성
	m_PartObjects.resize(iCount);
	for (_uint i = 0; i < iCount; i++)
	{
		m_PartObjects[i] = CWord_Container::Create(m_pDevice, m_pContext);
		if (nullptr != m_PartObjects[i])
		{
			m_PartObjects[i]->Set_Include_Section_Name(_pSection->Get_SectionName());
			static_cast<CWord_Container*>(m_PartObjects[i])->Set_ControllerIndex(m_iControllerIndex);
			static_cast<CWord_Container*>(m_PartObjects[i])->Set_ContainerIndex(i);
		}
	}

	// 초기화 갯수만큼 워드 삽입
	if (_ControllerJson.contains("Init_Word_Type"))
	{
		_uint iIndex = 0;
		for (auto& Index : _ControllerJson["Init_Word_Type"])
		{
			if (Index.is_number())
			{

				//CWord::WORD_TYPE eType = _ControllerJson["Init_Word_Type"];

				_uint iWordIndex = Index;
				auto pGenerator = SECTION_MGR->Get_Word_Generator();
				CWord* pWord = pGenerator->Find_Word(iWordIndex);

				static_cast<CWord_Container*>(m_PartObjects[iIndex])->Set_Word(pWord);
			}
			iIndex++;
		}
	}
	// 문자

#pragma endregion

	if (FAILED(Update_Text()))
		return E_FAIL;

	return S_OK;

}

void CWord_Controller::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);
}

void CWord_Controller::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
}

void CWord_Controller::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT CWord_Controller::Render()
{
	m_pGameInstance->Render_Font(L"Font28", m_strRenderText.c_str(), { 0.f,0.f },
		XMVectorSet(0.f, 0.f, 0.f, 1.f));
	return __super::Render();
}

HRESULT CWord_Controller::Update_Text()
{
	wregex pattern(L"##");

	auto words_begin = std::wsregex_iterator(m_strOriginText.begin(), m_strOriginText.end(), pattern);
	auto words_end = std::wsregex_iterator();


	_wstring strNewTex = L"";
	_uint iLastPos = 0;
	_int iLoopIndex = 0;

	for (std::wsregex_iterator i = words_begin; i != words_end; ++i)
	{
		std::wsmatch match = *i;

		strNewTex += m_strOriginText.substr(iLastPos, match.position() - iLastPos);

		if (iLoopIndex < m_PartObjects.size())
		{
			CWord* pWord = static_cast<CWord_Container*>(m_PartObjects[iLoopIndex])->Get_Word();
			if (nullptr == pWord)
				strNewTex += L"__";
			else
				strNewTex += pWord->Get_Text();
			iLoopIndex++;
		}
		else
		{
			strNewTex += L"__";
		}
		iLastPos = (_uint)match.position() + (_uint)match.length();
	}
	_vector vScale = m_pGameInstance->Measuring(L"Font28", m_strRenderText);
	m_fRenderSize = { XMVectorGetX(vScale), XMVectorGetY(vScale) };

	return S_OK;
}

void CWord_Controller::Set_Include_Section_Name(const _wstring _strIncludeSectionName)
{
	for (auto pController : m_PartObjects)
	{
		if(nullptr != pController)
			pController->Set_Include_Section_Name(_strIncludeSectionName);
	}
}

CWord_Controller* CWord_Controller::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CSection_2D* _pSection, json _ControllerJson)
{
	CWord_Controller* pInstance = new CWord_Controller(_pDevice, _pContext);

	if (FAILED(pInstance->Import(_pSection, _ControllerJson)))
	{
		MSG_BOX("Failed to Load : CWord_Controller");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWord_Controller::Clone(void* _pArg)
{
	return nullptr;
}

void CWord_Controller::Free()
{
	__super::Free();
}
