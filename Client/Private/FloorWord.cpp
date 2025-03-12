#include "stdafx.h"
#include "FloorWord.h"
#include "GameInstance.h"
#include "UI_Manager.h"



CFloorWord::CFloorWord(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CUI(_pDevice, _pContext)
{
}

CFloorWord::CFloorWord(const CFloorWord& _Prototype)
	: CUI(_Prototype)
{
}

HRESULT CFloorWord::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CFloorWord::Initialize(void* _pArg)
{
	UIOBJDESC* pDesc = static_cast<UIOBJDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	Create_PrintFloorWord(pDesc->iCurLevelID, pDesc->strLayerTag);


	//if (false == m_isMake)
	//{
	//	m_isMake = true;
	//	if (FAILED(Load_Json(TEXT("../Bin/Resources/Dialogue/FloorWord_Section01.json"), TEXT("Layer_UI"))))
	//		return E_FAIL;
	//}



	// FloorWord는 해당 레벨에서 진행하는 모든 애들을 가지고 있는다.
	// FloorWord는 이니셜라이즈할때 자식을 만들며, 자식을 만들때 json과 파싱하여 각각 글자를 가지고 있게 한다.
			// --FloorWord는 자식을 모두 업데이트를 돌리면서 플레이어와의 위치를 계속 파악을한다.
	
	// PrintWord는 해당 레벨에서만 사용한다.
			// --PrintWord는 업데이트를 돌면서 플레이어와의 위치를 계속 파악을한다.
			// --PrintWord는 플레이어가 특정 거리 안으로 들어올 경우 Fadein을 진행한다.
	

	return S_OK;
}

HRESULT CFloorWord::Initialize_Child(void* _pArg)
{
	UIOBJDESC* pDesc = static_cast<UIOBJDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CFloorWord::Priority_Update(_float _fTimeDelta)
{
}

void CFloorWord::Update(_float _fTimeDelta)
{

}

void CFloorWord::Late_Update(_float _fTimeDelta)
{

}

HRESULT CFloorWord::Render()
{
	return S_OK;
}



HRESULT CFloorWord::Ready_Components()
{
	return S_OK;
}


CFloorWord* CFloorWord::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CFloorWord* pInstance = new CFloorWord(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CFloorWord Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CFloorWord::Clone(void* _pArg)
{
	CFloorWord* pInstance = new CFloorWord(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CFloorWord Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CFloorWord::Free()
{
	__super::Free();
}

HRESULT CFloorWord::Cleanup_DeadReferences()
{


	return S_OK;
}


HRESULT CFloorWord::Create_PrintFloorWord(_int _CurLevel, _wstring strLayerTag)
{
	LEVEL_ID eId = (LEVEL_ID)_CurLevel;

	switch (eId)
	{
	case Client::LEVEL_CHAPTER_2:
		if (FAILED(Load_Json(TEXT("../Bin/Resources/Dialogue/FloorWord_Chapter_02.json"), strLayerTag)))
			return E_FAIL;
		break;
	case Client::LEVEL_CHAPTER_4:
		if (FAILED(Load_Json(TEXT("../Bin/Resources/Dialogue/FloorWord_Chapter_04.json"), strLayerTag)))
			return E_FAIL;
		break;
	case Client::LEVEL_CHAPTER_6:
		if (FAILED(Load_Json(TEXT("../Bin/Resources/Dialogue/FloorWord_Chapter_06.json"), strLayerTag)))
			return E_FAIL;
		break;
	case Client::LEVEL_CHAPTER_8:
		if (FAILED(Load_Json(TEXT("../Bin/Resources/Dialogue/FloorWord_Chapter_08.json"), strLayerTag)))
			return E_FAIL;
		break;
	default:
		break;
	}

	return S_OK;
}

HRESULT CFloorWord::Load_Json(const wstring& filePath, const _wstring& _strLayerTag)
{
	if (GetFileAttributes(filePath.c_str()) == INVALID_FILE_ATTRIBUTES)
	{
		MSG_BOX("FloorWord File Not Found");
		return E_FAIL;
	}

	ifstream file(filePath , ios::binary);
	json parsedJson;
	file >> parsedJson;


	for (const auto& text : parsedJson["Texts"]) 
	{
		FLOORTEXT tFloorText;
		CGameObject* pGameObject;

		tFloorText.strSection = StringToWstring(text["strSection"]);
		tFloorText.fPosX = text["PosX"].get<_float>();
		tFloorText.fPosY = text["PosY"].get<_float>();
		tFloorText.strText = StringToWstring(text["Text"]);
		
		tFloorText.strSFX = StringToWstring(text["strSFX"]);
		if (L"" == tFloorText.strSFX)
		{
			tFloorText.strSFX = TEXT("NOTWORD");
		}

		tFloorText.tHightLight.strSubstring = StringToWstring(text["highlight"]["substring"]);
		tFloorText.tHightLight.fDefultSize = text["highlight"]["defaultSize"].get<_float>();
		tFloorText.tHightLight.fHighlightSize = text["highlight"]["highlightSize"].get<_float>();
		

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_iCurLevelID, TEXT("Prototype_GameObject_PrintFloorWord"), m_iCurLevelID, _strLayerTag, &pGameObject, &tFloorText)))
			return E_FAIL;

		//Uimgr->PushBack_FloorWords(tFloorText);

	}


	return S_OK;
}



