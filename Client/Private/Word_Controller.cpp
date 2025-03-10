#include "stdafx.h"
#include "Word_Controller.h"
#include "Word.h"
#include "FloorWord.h"
#include "Section_Manager.h"
#include "Word_Container.h"
#include "PlayerData_Manager.h"
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
	Desc.eStartCoord = COORDINATE_2D;


	if (_ControllerJson.contains("FadeIn"))
		m_isWordControllerPopup = !((_bool)_ControllerJson["FadeIn"]);


	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

#pragma region 정규식 처리된 단어 확인.

	wregex pattern(L"##");

	auto words_begin = std::wsregex_iterator(m_strOriginText.begin(), m_strOriginText.end(), pattern);
	auto words_end = std::wsregex_iterator();

	_uint iCount = (_uint)std::distance(words_begin, words_end);

#pragma endregion

#pragma region 워드 확인

	_float2 fWindowPos = Convert_Pos_ToWindow(fPos, _pSection->Get_RenderTarget_Size());
	auto& Positions = Get_PatternPositions(fPos, fWindowPos);

	// 문자 갯수만큼 파트 컨테이너 초기화 생성
	m_PartObjects.resize(iCount);
	for (_uint i = 0; i < iCount; i++)
	{
		CWord_Container::WORD_CONTAINER_DESC Desc = {};

		Desc.strInitSectionName = _pSection->Get_SectionName();
		Desc.iControllerIndex = m_iControllerIndex;
		Desc.iContainerIndex = i;
		Desc.pOnwer = this;
		Desc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
		if(i < (_uint)Positions.size())
			Desc.Build_2D_Transform(Positions[i]);
		CBase* pBase = m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, 
			L"Prototype_GameObject_Word_Container", &Desc);
		
		if (nullptr != pBase)
			m_PartObjects[i] = static_cast<CWord_Container*>(pBase);
		//m_PartObjects[i] = CWord_Container::Create(m_pDevice, m_pContext);
		if (nullptr != m_PartObjects[i])  
		{
			m_PartObjects[i]->Enter_Section(_pSection->Get_SectionName());
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

				static_cast<CWord_Container*>(m_PartObjects[iIndex])->Set_Word(pWord, CWord::WORD_MODE_SETUP, false);
			}
			iIndex++;
		}
	}
	// 문자

#pragma endregion

	if (FAILED(Update_Text()))
		return E_FAIL;

	if (m_isWordControllerPopup)
	{
		Set_Render(false);
		for (auto pCollider : m_p2DColliderComs)
			pCollider->Set_Active(true);
		for (auto pPartObject : m_PartObjects)
			pPartObject->Set_Active(false);
	}

	return S_OK;

}

void CWord_Controller::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);
}

void CWord_Controller::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);

	if (false == m_isWordControllerPopup)
	{
		auto pPlayer = CPlayerData_Manager::GetInstance()->Get_CurrentPlayer_Ptr();

		if (nullptr != pPlayer && COORDINATE_2D == pPlayer->Get_CurCoord() )
		{
			_vector vControllerPos  = Get_FinalPosition(COORDINATE_2D);
			_vector vPlayerPos  = pPlayer->Get_FinalPosition(COORDINATE_2D);
			_float fLength = XMVectorGetX(XMVector2Length(vControllerPos - vPlayerPos));

			if (250.f > fabs(fLength))
			{
				m_isWordControllerPopup = true;
				Set_Render(true);
				for (auto pCollider : m_p2DColliderComs)
					pCollider->Set_Active(true);
				for (auto pPartObject : m_PartObjects)
					pPartObject->Set_Active(true);
			}
		}
	}
	else if (false == m_isFadeIn)
	{
		m_fAmount += _fTimeDelta * 1.8f;

		if (m_fAmount > 1.f)
		{
			m_fAmount = 1.f;
			m_isFadeIn = true;
		}
	
	}
}

void CWord_Controller::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}




HRESULT CWord_Controller::Render()
{
	if (m_isWordControllerPopup)
	{
		_vector vPosition = Get_FinalPosition();
		_float2 fPos = { XMVectorGetX(vPosition),XMVectorGetY(vPosition) };

		_float2 fSize = SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName);
		_float2 fScale = SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName);

		fPos = Convert_Pos_ToWindow(fPos, fSize);
		fPos.x -= m_fRenderSize.x * 0.5f;
		fPos.y -= m_fRenderSize.y * 0.5f;


		m_pGameInstance->Render_Font(L"Font28", m_strRenderText.c_str(), fPos,
			XMVectorSet(0.f, 0.f, 0.f, m_fAmount));


		for (auto& pPartObj : m_PartObjects)
		{
			if (nullptr != pPartObj && true == pPartObj->Is_Active())
				pPartObj->Render();
		}
		
	}
	return __super::Render();
}

vector<_float2> CWord_Controller::Get_PatternPositions(_float2 _fProjPos, _float2 _fWindowInPos)
{
	vector<_float2> PatternPositions;

	std::wregex pattern(L"##");

	auto words_begin = std::wsregex_iterator(m_strOriginText.begin(), m_strOriginText.end(), pattern);
	auto words_end = std::wsregex_iterator();

	_float fLine = 36.f;

	_float x = _fWindowInPos.x; 
	_float y = _fWindowInPos.y; 
	size_t lastIndex = 0;


	for (std::wsregex_iterator i = words_begin; i != words_end; ++i)
	{
		std::wsmatch match = *i;
		size_t matchIndex = match.position();

		for (size_t j = lastIndex; j < matchIndex; j++)
		{
			if (m_strOriginText[j] == L'\n')
			{
				x = _fWindowInPos.x;
				y += fLine;
			}
			else
			{
				_float fWitdh = m_pGameInstance->Measuring(L"Font28",std::wstring(1, m_strOriginText[j]).c_str()).m128_f32[0];
				x += fWitdh;
			}
		}

		PatternPositions.push_back({ x, y });
		lastIndex = matchIndex + match.length();
	}
	_vector fScale = m_pGameInstance->Measuring(L"Font28", m_strOriginText);



	_float2 fCenter = _fWindowInPos;

	fCenter.x += XMVectorGetX(fScale) * 0.333f;
	fCenter.y += XMVectorGetY(fScale) * 0.333f;
	

	for (auto& fPos : PatternPositions)
	{
		fPos.x -= fCenter.x;
		fPos.y -= fCenter.y;
		fPos.y *= -1.f;
	}



	return PatternPositions;
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
	strNewTex += m_strOriginText.substr(iLastPos);

	m_strRenderText = strNewTex;
	_vector vScale = m_pGameInstance->Measuring(L"Font28", m_strRenderText);
	m_fRenderSize = { XMVectorGetX(vScale), XMVectorGetY(vScale) };
	return S_OK;
}

HRESULT CWord_Controller::Register_RenderGroup(_uint _iGroupId, _uint _iPriorityID)
{
	return m_pGameInstance->Add_RenderObject_New(_iGroupId, _iPriorityID, this); // Collider2D Render 및 debug관련 렌더를 container에서 수행.
}

void CWord_Controller::Enter_Section(const _wstring _strIncludeSectionName)
{
	__super::Enter_Section(_strIncludeSectionName);
	for (auto pController : m_PartObjects)
	{
		if(nullptr != pController)
			pController->Enter_Section(_strIncludeSectionName);
	}
}

void CWord_Controller::Active_OnEnable()
{
	__super::Active_OnEnable();

	if (false == m_isWordControllerPopup)
	{
		Set_Render(false);
		for (auto pCollider : m_p2DColliderComs)
			pCollider->Set_Active(true);
		for (auto pPartObject : m_PartObjects)
			pPartObject->Set_Active(false);
	}

}

void CWord_Controller::Active_OnDisable()
{
	__super::Active_OnDisable();
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
