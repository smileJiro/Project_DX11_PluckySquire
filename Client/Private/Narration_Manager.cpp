#include "stdafx.h"
#include "Narration_Manager.h"
#include "Narration_New.h"


IMPLEMENT_SINGLETON(CNarration_Manager)

CNarration_Manager::CNarration_Manager()
	:m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}



HRESULT CNarration_Manager::Level_Exit(_int iCurLevelID, _int _iChangeLevelID, _int _iNextChangeLevelID)
{

	return S_OK;
}

HRESULT CNarration_Manager::Level_Logo_Exit(_int _iChangeLevelID, _int _iNextChangeLevelID)
{


	return S_OK;
}

HRESULT CNarration_Manager::Level_Enter(_int _iChangeLevelID)
{
	//if (_iChangeLevelID == 3)
	//	LoadFromJson(TEXT("../Bin/Resources/Narration/Level_Chapter_02_Narration.json"), (LEVEL_ID)_iChangeLevelID);
	
	return S_OK;
}

HRESULT CNarration_Manager::Set_PlayNarration(const _wstring& _strid)
{

	auto& pair = m_Narrations.find(_strid);
	CNarration_New* pNarration = pair->second;

	if (nullptr == pNarration)
		return E_FAIL;

	if (false == pNarration->CBase::Is_Active())
		pNarration->CBase::Set_Active(true);

	if (false == pNarration->is_PlayAnimation())
	{
		if (false == pNarration->is_NarrationPlayed())
		{
			pNarration->Set_PlayAnimation(true);
		}
	}

	m_isPlaying = true;

	m_strCurNarrationIndex = _strid;

	//wsprintf(m_strNarrationID, _strid.c_str()); m_isPlayerNarration = true;

	return S_OK;

}

HRESULT CNarration_Manager::NarrationActive()
{
	auto& pair = m_Narrations.find(m_strCurNarrationIndex);

	CNarration_New* pNarration = pair->second;

	if (nullptr == pNarration)
		return E_FAIL;

	if (false == pNarration->CBase::Is_Active())
		pNarration->CBase::Set_Active(true);


	return S_OK;
}

HRESULT CNarration_Manager::LoadFromJson(const wstring& filePath, LEVEL_ID _eLevelID)
{
	if (GetFileAttributes(filePath.c_str()) == INVALID_FILE_ATTRIBUTES)
	{
		MSG_BOX("Dialog File Not Found");
		return E_FAIL;
	}

	ifstream ifs(filePath, ios::binary);
	json doc;
	ifs >> doc;

	if (doc.contains("Narration") && doc["Narration"].is_array())
	{
		_int iTurnoverPage = 0;

		for (auto& Nar : doc["Narration"])
		{
			CNarration_New::NarrationData NarData;

			if (Nar.contains("strSectionid") && Nar["strSectionid"].is_string())
				NarData.strSectionid = StringToWstring(Nar["strSectionid"].get<string>());

			if (Nar.contains("strid") && Nar["strid"].is_string())
				NarData.strid = StringToWstring(Nar["strid"].get<string>());

			if (Nar.contains("CurlevelId") && Nar["CurlevelId"].is_number_integer())
				NarData.eCurlevelId = (LEVEL_ID)Nar["CurlevelId"].get<_int>();

			if (Nar.contains("bTerminal") && Nar["bTerminal"].is_boolean())
				NarData.bTerminal = Nar["bTerminal"].get<bool>();

			if (Nar.contains("ispeed") && Nar["ispeed"].is_number_integer())
				NarData.ispeed = Nar["ispeed"].get<_int>();

			if (Nar.contains("iduration") && Nar["iduration"].is_number_integer())
				NarData.iduration = Nar["iduration"].get<_int>();


			_int iNar = { 0 };

			if (Nar.contains("NarAnim") && Nar["NarAnim"].is_array())
			{
				for (auto& NarAnim : Nar["NarAnim"])
				{
					CNarration_New::NarrationDialogData DialogueData;

					if (NarAnim.contains("strSFX") && NarAnim["strSFX"].is_string())
						DialogueData.strSFX = StringToWstring(NarAnim["strSFX"].get<string>());

					if (NarAnim.contains("strtext") && NarAnim["strtext"].is_string())
						DialogueData.strtext = StringToWstring(NarAnim["strtext"].get<string>());

					if (NarAnim.contains("strSubSFX") && NarAnim["strSubSFX"].is_string())
						DialogueData.strSubSFX = StringToWstring(NarAnim["strSubSFX"].get<string>());

					if (NarAnim.contains("fscale") && NarAnim["fscale"].is_number_float())
						DialogueData.fscale = NarAnim["fscale"].get<float>();

					if (NarAnim.contains("isLeft") && NarAnim["isLeft"].is_boolean())
						DialogueData.isLeft = NarAnim["isLeft"].get<_bool>();

					if (NarAnim.contains("fFontPosX") && NarAnim["fFontPosX"].is_number_float())
						DialogueData.fFontPos.x = NarAnim["fFontPosX"].get<float>();

					if (NarAnim.contains("fFontPosY") && NarAnim["fFontPosY"].is_number_float())
						DialogueData.fFontPos.y = NarAnim["fFontPosY"].get<float>();

					if (NarAnim.contains("fwaitingTime") && NarAnim["fwaitingTime"].is_number_float())
						DialogueData.fwaitingTime = NarAnim["fwaitingTime"].get<float>();

					if (NarAnim.contains("fLineHeight") && NarAnim["fLineHeight"].is_number_float())
						DialogueData.fLineHieght = NarAnim["fLineHeight"].get<float>();

					if (NarAnim.contains("fDelayNextLine") && NarAnim["fDelayNextLine"].is_number_float())
						DialogueData.fDelayNextLine = NarAnim["fDelayNextLine"].get<float>();

					if (NarAnim.contains("isFinishedThisLine") && NarAnim["isFinishedThisLine"].is_boolean())
						DialogueData.isFinishedThisLine = NarAnim["isFinishedThisLine"].get<_bool>();

					if (NarAnim.contains("isDirTurn") && NarAnim["isDirTurn"].is_boolean())
						DialogueData.isDirTurn = NarAnim["isDirTurn"].get<_bool>();

					if (NarAnim.contains("strAnimationid") && NarAnim["strAnimationid"].is_string())
					{
						wstring animId = StringToWstring(NarAnim["strAnimationid"].get<string>());
						if (animId == L"NULL")
							continue;

						DialogueData.strAnimationid = animId;
					}

					if (NarAnim.contains("strSectionid") && NarAnim["strSectionid"].is_string())
						DialogueData.strSectionid = StringToWstring(NarAnim["strSectionid"].get<string>());

					if (NarAnim.contains("fPosX") && NarAnim["fPosX"].is_number_float())
						DialogueData.vPos.x = NarAnim["fPosX"].get<float>();

					if (NarAnim.contains("fPosY") && NarAnim["fPosY"].is_number_float())
						DialogueData.vPos.y = NarAnim["fPosY"].get<float>();

					if (NarAnim.contains("fwaitingTime") && NarAnim["fwaitingTime"].is_number_float())
						DialogueData.fWaitingTime = NarAnim["fwaitingTime"].get<float>();

					if (NarAnim.contains("vAnimationScaleX") && NarAnim["vAnimationScaleX"].is_number_float())
						DialogueData.vAnimationScale.x = NarAnim["vAnimationScaleX"].get<float>();

					if (NarAnim.contains("vAnimationScaleY") && NarAnim["vAnimationScaleY"].is_number_float())
						DialogueData.vAnimationScale.y = NarAnim["vAnimationScaleY"].get<float>();

					if (NarAnim.contains("AnimationIndex") && NarAnim["AnimationIndex"].is_number_integer())
						DialogueData.iAnimationIndex = NarAnim["AnimationIndex"].get<_int>();

					if (NarAnim.contains("isLoop") && NarAnim["isLoop"].is_boolean())
						DialogueData.isLoop = NarAnim["isLoop"].get<_bool>();


					NarData.NarAnim = DialogueData;
					// 여기서 객체를 생성한다.
					CGameObject* pNarrationObject;

					if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(_eLevelID, TEXT("Prototype_GameObject_Narration"), NarData.eCurlevelId, TEXT("Layer_Narration"), &pNarrationObject, &NarData)))
						return E_FAIL;

					pNarrationObject->Set_Active(false);

					m_Narrations.emplace(NarData.strid, static_cast<CNarration_New*>(pNarrationObject));
					Safe_AddRef(pNarrationObject);

				}
			}
		}

		//
		//	// 임시 대화 데이터를 생성하여 해당 애니메이션만 담는다.
		//	NarrationDialogData tempDialogue = DialogueData;
		//	tempDialogue.NarAnim.clear();
		//	tempDialogue.NarAnim.push_back(Animation);
		//
		//	// 임시 나레이션 데이터를 생성하고 대화 리스트에 방금 만든 tempDialogue만 넣는다.
		//	NarrationData tempData = NarData;
		//	tempData.lines.clear();
		//	tempDialogue.AnimationCount = iAnim;
		//	++iAnim;
		//	tempData.lines.push_back(tempDialogue);
		//	// 대화 라인이 단일 객체이므로 LineCount는 0으로 설정
		//	tempData.LineCount = 0;
		//
		//
		//	// 원본 DialogueData에도 해당 애니메이션 데이터를 저장(필요에 따라 사용)
		//	DialogueData.NarAnim.push_back(Animation);
		//					}
		//
		//					//CGameObject* pNarrationObject;
		//
		//					//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(_eLevelID, TEXT("Prototype_GameObject_Narration"), 
		//					//	NarData.eCurlevelId, TEXT("Layer_Narration"), &pNarrationObject, &NarData)))
		//					//	return E_FAIL;
		//					//
		//					//pNarrationObject->Set_Active(false);
		//					//
		//					//m_Narrations.emplace(NarData.strid, static_cast<CNarration_New*>(pNarrationObject));
		//					//Safe_AddRef(pNarrationObject);
		//
		//				}
		//				NarData.lines.push_back(DialogueData);
		//				++iLine;
		//
		//				//CGameObject* pNarrationObject;
		//				//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(_eLevelID, TEXT("Prototype_GameObject_Narration"),
		//				//	NarData.eCurlevelId, TEXT("Layer_Narration"), &pNarrationObject, &NarData)))
		//				//	return E_FAIL;
		//				//
		//				//pNarrationObject->Set_Active(false);
		//				//
		//				//m_Narrations.emplace(NarData.strid, static_cast<CNarration_New*>(pNarrationObject));
		//				//Safe_AddRef(pNarrationObject);
		//			}
		//
		//		}
		//
		//		// 여기서 이제 나레이션를 만든다.
		//		CGameObject* pNarrationObject;
		//
		//		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(_eLevelID, TEXT("Prototype_GameObject_Narration"), NarData.eCurlevelId, TEXT("Layer_Narration"), &pNarrationObject, &NarData)))
		//			return E_FAIL;
		//
		//		pNarrationObject->Set_Active(false);
		//
		//		m_Narrations.emplace(NarData.strid, static_cast<CNarration_New*>(pNarrationObject));
		//		Safe_AddRef(pNarrationObject);
		//	}
		//
		//}

	}


	return S_OK;
}


void CNarration_Manager::Free()
{
	Safe_Release(m_pGameInstance);

	for (auto& pair : m_Narrations)
	{
		Safe_Release(pair.second);
	}

	__super::Free();
}

