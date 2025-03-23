#include "stdafx.h"
#include "Section_2D_Narration.h"
#include "GameInstance.h"
#include "2DMapObject.h"
#include "Map_2D.h"
#include "Section_Manager.h"
#include "MapObjectFactory.h"
#include "Trigger_Manager.h"
#include "Player.h"
#include "NPC.h"
#include "UI_Manager.h"


CSection_2D_Narration::CSection_2D_Narration(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, SECTION_2D_PLAY_TYPE _ePlayType)
	:CSection_2D(_pDevice, _pContext, _ePlayType, SECTION_2D_BOOK)
{
}

HRESULT CSection_2D_Narration::Initialize(void* _pDesc)
{
	if (nullptr == _pDesc)
		return E_FAIL;

	SECTION_2D_NARRATION_DESC* pDesc = static_cast<SECTION_2D_NARRATION_DESC*>(_pDesc);

	if (FAILED(__super::Initialize(_pDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CSection_2D_Narration::Import(void* _pDesc)
{
	SECTION_2D_NARRATION_DESC* pDesc = static_cast<SECTION_2D_NARRATION_DESC*>(_pDesc);
#pragma region json 객체의 기본 정보 읽습니다. 하위 주석 참조.
	if (FAILED(__super::Import(_pDesc)))
		return E_FAIL;
#pragma endregion
	return S_OK;
}


HRESULT CSection_2D_Narration::Section_AddRenderGroup_Process()
{
	//if (FAILED(Copy_DefaultMap_ToRenderTarget()))
	//	return E_FAIL;

	if (FAILED(Add_RenderGroup_GameObjects()))
		return E_FAIL;

	return S_OK;
}
HRESULT CSection_2D_Narration::Add_GameObject_ToSectionLayer(CGameObject* _pGameObject, _uint _iLayerIndex)
{
	HRESULT hr = __super::Add_GameObject_ToSectionLayer(_pGameObject, _iLayerIndex);
	return hr;
}
HRESULT CSection_2D_Narration::Remove_GameObject_ToSectionLayer(CGameObject* _pGameObject)
{
	HRESULT hr = __super::Remove_GameObject_ToSectionLayer(_pGameObject);
	return hr;
}
HRESULT CSection_2D_Narration::Start_Narration()
{
	// 나레이션 매니져에서 가져와서 해당 나레이션이 재생 되었는지 판별해야한다.

	if (nullptr != Uimgr->Get_Narration())
	{
		if (Get_SectionName() == L"Chapter1_P1112" && false == m_FirstPlay[0])
		{
			CUI_Manager::GetInstance()->Set_PlayNarration(TEXT("Chapter1_P1112_Narration_01"));
			m_FirstPlay[0] = true;
		}
		else if (Get_SectionName() == L"Chapter2_P0506" && false == m_FirstPlay[1])
		{
			CUI_Manager::GetInstance()->Set_PlayNarration(TEXT("Chapter2_P0506_Narration_01"));
			m_FirstPlay[1] = true;
		}
		else if (Get_SectionName() == L"Chapter6_P0304" && false == m_FirstPlay[2])
		{
			CUI_Manager::GetInstance()->Set_PlayNarration(TEXT("Chapter6_P0304_Narration_01"));
			m_FirstPlay[2] = true;
		}
		else if (Get_SectionName() == L"Chapter6_P0910" && false == m_FirstPlay[3])
		{
			CUI_Manager::GetInstance()->Set_PlayNarration(TEXT("Chapter6_P0910_Narration_01"));
			m_FirstPlay[3] = true;
		}
		else if (Get_SectionName() == L"Chapter6_P1516" && false == m_FirstPlay[4])
		{
			CUI_Manager::GetInstance()->Set_PlayNarration(TEXT("Chapter6_P1516_Narration_01"));
			m_FirstPlay[4] = true;
		}
		//else if (Get_SectionName() == L"Chapter8_P0102" && false  == m_FirstPlay[5])
		//{
		//	CUI_Manager::GetInstance()->Set_PlayNarration(TEXT("Chapter8_P0102_Narration_01"));
		//	m_FirstPlay[5] = true;
		//}
		else if (Get_SectionName() == L"Chapter8_P0910" && false == m_FirstPlay[6])
		{
			CUI_Manager::GetInstance()->Set_PlayNarration(TEXT("Chapter8_P0910_Narration_01"));
			m_FirstPlay[6] = true;
		}
		else if (Get_SectionName() == L"Chapter8_P1314" && false == m_FirstPlay[7])
		{
			CUI_Manager::GetInstance()->Set_PlayNarration(TEXT("CChapter8_P1314_Narration_01"));
			m_FirstPlay[7] = true;
		}
		else if (Get_SectionName() == L"Chapter8_P2526" && false == m_FirstPlay[8])
		{
			CUI_Manager::GetInstance()->Set_PlayNarration(TEXT("CChapter8_END_Narration_01"));
			m_FirstPlay[8] = true;
		}
			
	}

	return S_OK;
}

HRESULT CSection_2D_Narration::Section_Enter(const _wstring& _strPreSectionTag)
{

	auto pPlayer = Uimgr->Get_Player();
	if (nullptr != pPlayer)
		pPlayer->Set_BlockPlayerInput(true);
	

	return __super::Section_Enter(_strPreSectionTag);
}

HRESULT CSection_2D_Narration::Section_Exit(const _wstring& _strNextSectionTag)
{
	auto pPlayer = Uimgr->Get_Player();
	if (nullptr != pPlayer)
		pPlayer->Set_BlockPlayerInput(false);
	

	return __super::Section_Exit(_strNextSectionTag);;
}

CSection_2D_Narration* CSection_2D_Narration::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, SECTION_2D_PLAY_TYPE _ePlayType, void* _pDesc)
{
	CSection_2D_Narration* pInstance = new CSection_2D_Narration(_pDevice, _pContext, _ePlayType);

	if (FAILED(pInstance->Initialize(_pDesc)))
	{
		MSG_BOX("Failed Create CSection_2D_Narration");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSection_2D_Narration::Free()
{
	__super::Free();
}
