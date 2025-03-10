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


CSection_2D_Narration::CSection_2D_Narration(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CSection_2D(_pDevice, _pContext, NARRAION, SECTION_2D_BOOK)
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

	
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(_pGameObject);
	// TODO:: 추후 개선 오버라이드 2개
	if (nullptr != pPlayer)
		pPlayer->Set_Render(false);
		
	CNPC* pNPC = dynamic_cast<CNPC*>(_pGameObject);
	// TODO:: 추후 개선 오버라이드 2개
	if (nullptr != pNPC)
		pNPC->Set_Render(false);

	return hr;
}
HRESULT CSection_2D_Narration::Remove_GameObject_ToSectionLayer(CGameObject* _pGameObject)
{
	HRESULT hr = __super::Remove_GameObject_ToSectionLayer(_pGameObject);
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(_pGameObject);

	if (nullptr != pPlayer)
		pPlayer->Set_Render(true);

	CNPC* pNPC = dynamic_cast<CNPC*>(_pGameObject);
	// TODO:: 추후 개선 오버라이드 2개
	if (nullptr != pNPC)
		pNPC->Set_Render(true);


	return hr;
}
HRESULT CSection_2D_Narration::Start_Narration()
{
	// 나레이션 매니져에서 가져와서 해당 나레이션이 재생 되었는지 판별해야한다.

	//if (nullptr != Uimgr->Get_Narration())
	//{
	//	if (Get_SectionName() == L"Chapter1_P1112")
	//		CUI_Manager::GetInstance()->Set_PlayNarration(TEXT("Chapter1_P1112_Narration_01"));
	//
	//	else if (Get_SectionName() == L"Chapter2_P0506")
	//		CUI_Manager::GetInstance()->Set_PlayNarration(TEXT("Chapter2_P0506_Narration_01"));
	//
	//	else if (Get_SectionName() == L"Chapter6_P0304")
	//		CUI_Manager::GetInstance()->Set_PlayNarration(TEXT("Chapter6_P0304_Narration_01"));
	//
	//	else if (Get_SectionName() == L"Chapter6_P0910")
	//		CUI_Manager::GetInstance()->Set_PlayNarration(TEXT("Chapter6_P0910_Narration_01"));
	//
	//	else if (Get_SectionName() == L"Chapter6_P1516")
	//		CUI_Manager::GetInstance()->Set_PlayNarration(TEXT("Chapter6_P1516_Narration_01"));
	//
	//	else if (Get_SectionName() == L"Chapter8_P0102")
	//		CUI_Manager::GetInstance()->Set_PlayNarration(TEXT("Chapter8_P0102_Narration_01"));
	//
	//	else if (Get_SectionName() == L"Chapter8_P0910")
	//		CUI_Manager::GetInstance()->Set_PlayNarration(TEXT("Chapter8_P0910_Narration_01"));
	//
	//	else if (Get_SectionName() == L"Chapter8_P1314")
	//		CUI_Manager::GetInstance()->Set_PlayNarration(TEXT("CChapter8_P1314_Narration_01"));
	//}

	return S_OK;
}
;

CSection_2D_Narration* CSection_2D_Narration::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pDesc)
{
	CSection_2D_Narration* pInstance = new CSection_2D_Narration(_pDevice, _pContext);

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
