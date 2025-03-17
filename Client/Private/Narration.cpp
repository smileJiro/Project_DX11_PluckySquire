#pragma once
#include "stdafx.h"
#include "Narration.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "Section_Manager.h"
#include "Trigger_Manager.h"
#include "CustomFont.h"


CNarration::CNarration(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CUI(_pDevice, _pContext)
{
}

CNarration::CNarration(const CNarration& _Prototype)
	: CUI(_Prototype)
{
}

HRESULT CNarration::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNarration::Initialize(void* _pArg)
{
	UIOBJDESC* pDesc = static_cast<UIOBJDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (LEVEL_CHAPTER_2 == pDesc->iCurLevelID)
	{
		if (FAILED(LoadFromJson(TEXT("../Bin/Resources/Narration/Level_Chapter_02_Narration.json"))))
			return E_FAIL;
	}

	else if (LEVEL_CHAPTER_6 == pDesc->iCurLevelID)
	{
		if (FAILED(LoadFromJson(TEXT("../Bin/Resources/Narration/Level_Chapter_06_Narration.json"))))
			return E_FAIL;
	}

	else if (LEVEL_CHAPTER_8 == pDesc->iCurLevelID)
	{
		if (FAILED(LoadFromJson(TEXT("../Bin/Resources/Narration/Level_Chapter_08_Narration.json"))))
			return E_FAIL;
	}

	_float2 vCalScale = { 0.f, 0.f };
	vCalScale.x = m_vOriginSize.x * RATIO_BOOK2D_X;
	vCalScale.y = m_vOriginSize.y * RATIO_BOOK2D_Y;
	m_isRender = false;
	m_isLeftRight = true;
	m_iCameraPos = CAMERA_LEFT;

	return S_OK;
}


void CNarration::Update(_float _fTimeDelta)
{
	if (KEY_DOWN(KEY::G))
	{
		StopNarration();
		return;
	}

	if (false == m_isPlayNarration && true == Uimgr->Get_PlayNarration())
	{
		m_isPlayNarration = true;
		_wstring targetNarrationID = Uimgr->Get_strNarrationID();
		Set_NarrationByStrid(targetNarrationID);
		m_isStartNarration = true;

		if (m_isStartNarration)
			GetAnimationObjectForLine(m_iCurrentLine);

		// 새로운 나레이션의 첫 라인에 연결된 애니메이션 시작
		for (auto& animObj : m_pCurrentAnimObj)
		{
			//if (animObj)
			//{
			if (false == animObj->CBase::Is_Active())
				animObj->CBase::Set_Active(true);

			animObj->StartAnimation();

			m_DisPlayTextLine = 0;
			//}
		}

		m_iCameraPos = m_NarrationDatas[m_iNarrationCount].lines[m_iCurrentLine].iCameraPos;
	}

	if (true == m_isStartNarration)
	{
		if (true == m_isWaitingNextPage)
		{
			m_fWaitingNextPageTime += _fTimeDelta;

			if (m_fWaitingNextPageTime <= 1.5f)
				return;
			else
			{
				m_fWaitingNextPageTime = 0.f;
				m_isWaitingNextPage = false;
				GetAnimationObjectForLine(m_iCurrentLine, 0);
				m_fWaitingTime = 0.f;
				m_fWaitingNextPageTime = 0.f;
			}
		}

		Update_Narration(_fTimeDelta);
	}

}

void CNarration::Late_Update(_float _fTimeDelta)
{

}

HRESULT CNarration::Render()
{
	if (false == CBase::Is_Active())
	{
		CBase::Set_Active(true);
	}

	// TODO :: 해당 부분은 가변적이다 추후 변경해야한다.
	DisplayText(_float2(RTSIZE_BOOK2D_X, RTSIZE_BOOK2D_Y));
	return S_OK;
}

HRESULT CNarration::LoadFromJson(const wstring& filePath)
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
			NarrationData NarData;

			if (Nar.contains("strSectionid") && Nar["strSectionid"].is_string())
				NarData.strSectionid = StringToWstring(Nar["strSectionid"].get<string>());

			if (Nar.contains("strid") && Nar["strid"].is_string())
				NarData.strid = StringToWstring(Nar["strid"].get<string>());

			if (Nar.contains("CurlevelId") && Nar["CurlevelId"].is_number_integer())
				NarData.eCurlevelId = (LEVEL_ID)Nar["CurlevelId"].get<_int>();

			if (Nar.contains("bTerminal") && Nar["bTerminal"].is_boolean())
				NarData.bTerminal = Nar["bTerminal"].get<bool>();






			_int iLine = { 0 };

			if (Nar.contains("lines") && Nar["lines"].is_array())
			{
				for (auto& line : Nar["lines"])
				{
					NarrationDialogData DialogueData;

					if (line.contains("iduration") && line["iduration"].is_number_integer())
						DialogueData.iduration = line["iduration"].get<_int>();

					if (line.contains("ispeed") && line["ispeed"].is_number_integer())
						DialogueData.ispeed = line["ispeed"].get<_int>();

					if (line.contains("strtext") && line["strtext"].is_string())
						DialogueData.strtext = StringToWstring(line["strtext"].get<string>());



					if (line.contains("fR") && line["fR"].is_number_float())
						DialogueData.fRGB.x = line["fR"].get<_float>();

					if (line.contains("fG") && line["fG"].is_number_float())
						DialogueData.fRGB.y = line["fG"].get<_float>();

					if (line.contains("fB") && line["fB"].is_number_float())
						DialogueData.fRGB.z = line["fB"].get<_float>();




					if (line.contains("strSFX") && line["strSFX"].is_string())
						DialogueData.strSFX = StringToWstring(line["strSFX"].get<string>());


					if (line.contains("strSubSFX") && line["strSubSFX"].is_string())
						DialogueData.strSubSFX = StringToWstring(line["strSubSFX"].get<string>());


					if (line.contains("isLeft") && line["isLeft"].is_boolean())
						DialogueData.isLeft = line["isLeft"].get<bool>();

					if (line.contains("iCameraPos") && line["iCameraPos"].is_number_integer())
						DialogueData.iCameraPos = (CAMERAPOS)line["iCameraPos"].get<_int>();

					if (line.contains("fscale") && line["fscale"].is_number_float())
						DialogueData.fscale = line["fscale"].get<float>();

					//if (line.contains("isLeft") && line["isLeft"].is_boolean())
					//{
					//	DialogueData.isLeft = line["isLeft"].get<_bool>();
					//}

					if (line.contains("fposX") && line["fposX"].is_number_float())
						DialogueData.fpos.x = line["fposX"].get<float>();

					if (line.contains("fposY") && line["fposY"].is_number_float())
						DialogueData.fpos.y = line["fposY"].get<float>();

					if (line.contains("fwaitingTime") && line["fwaitingTime"].is_number_float())
						DialogueData.fwaitingTime = line["fwaitingTime"].get<float>();

					if (line.contains("fLineHeight") && line["fLineHeight"].is_number_float())
						DialogueData.fLineHieght = line["fLineHeight"].get<float>();

					if (line.contains("fFadeDuration") && line["fFadeDuration"].is_number_float())
						DialogueData.fFadeDuration = line["fFadeDuration"].get<float>();

					if (line.contains("fDelayNextLine") && line["fDelayNextLine"].is_number_float())
						DialogueData.fDelayNextLine = line["fDelayNextLine"].get<float>();

					if (line.contains("isFinishedThisLine") && line["isFinishedThisLine"].is_boolean())
					{
						DialogueData.isFinishedThisLine = line["isFinishedThisLine"].get<_bool>();

					}

					if (line.contains("isDirTurn") && line["isDirTurn"].is_boolean())
						DialogueData.isDirTurn = line["isDirTurn"].get<_bool>();

					// 애니메이션 데이터 파싱 및 개별 객체 생성
					vector<CNarration_Anim*> pAnimation;
					if (line.contains("NarAnim") && line["NarAnim"].is_array())
					{
						_int iAnim = { 0 };

						// line에 여러 애니메이션 정보가 있다면 각각 별도로 생성한다.
						for (auto& Anim : line["NarAnim"])
						{
							NarrationAnimation Animation;

							if (Anim.contains("strAnimationid") && Anim["strAnimationid"].is_string())
							{
								wstring animId = StringToWstring(Anim["strAnimationid"].get<string>());
								if (animId == L"NULL")
									continue;
								Animation.strAnimationid = animId;
							}

							if (Anim.contains("strSectionid") && Anim["strSectionid"].is_string())
								Animation.strSectionid = StringToWstring(Anim["strSectionid"].get<string>());

							if (Anim.contains("fPosX") && Anim["fPosX"].is_number_float())
								Animation.vPos.x = Anim["fPosX"].get<float>();

							if (Anim.contains("fPosY") && Anim["fPosY"].is_number_float())
								Animation.vPos.y = Anim["fPosY"].get<float>();

							if (Anim.contains("fwaitingTime") && Anim["fwaitingTime"].is_number_float())
								Animation.fWaitingTime = Anim["fwaitingTime"].get<float>();

							if (Anim.contains("vAnimationScaleX") && Anim["vAnimationScaleX"].is_number_float())
								Animation.vAnimationScale.x = Anim["vAnimationScaleX"].get<float>();

							if (Anim.contains("vAnimationScaleY") && Anim["vAnimationScaleY"].is_number_float())
								Animation.vAnimationScale.y = Anim["vAnimationScaleY"].get<float>();

							if (Anim.contains("AnimationIndex") && Anim["AnimationIndex"].is_number_integer())
								Animation.iAnimationIndex = Anim["AnimationIndex"].get<_int>();

							if (Anim.contains("isLoop") && Anim["isLoop"].is_boolean())
							{
								Animation.isLoop = Anim["isLoop"].get<_bool>();
							}


							// 임시 대화 데이터를 생성하여 해당 애니메이션만 담는다.
							NarrationDialogData tempDialogue = DialogueData;
							tempDialogue.NarAnim.clear();
							tempDialogue.NarAnim.push_back(Animation);

							// 임시 나레이션 데이터를 생성하고 대화 리스트에 방금 만든 tempDialogue만 넣는다.
							NarrationData tempData = NarData;
							tempData.lines.clear();
							tempDialogue.AnimationCount = iAnim;
							++iAnim;
							tempData.lines.push_back(tempDialogue);
							// 대화 라인이 단일 객체이므로 LineCount는 0으로 설정
							tempData.LineCount = 0;

							CGameObject* pObject;

							if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(tempData.eCurlevelId, TEXT("Prototype_GameObject_Narration_Anim"), tempData.eCurlevelId, TEXT("Layer_UI"), &pObject, &tempData)))
								return E_FAIL;

							
							// 생성한 애니메이션 객체 넣기
							CNarration_Anim* pAnim;
							pAnim = static_cast<CNarration_Anim*>(pObject);

							if (nullptr == pAnim)
								return E_FAIL;

							// 250312::박상욱
							
							//Safe_Release(pObject);
							//
							//pAnimation.push_back(pAnim);
							//Safe_AddRef(pAnim);

							//

							// 원본 DialogueData에도 해당 애니메이션 데이터를 저장(필요에 따라 사용)
							DialogueData.NarAnim.push_back(Animation);
						}
					}
					// 애니메이션 처리 후, 완성된 대화 데이터를 NarData에 추가한다.

					NarData.lines.push_back(DialogueData);
					++iLine;
				}

			}

			m_NarrationDatas.push_back(NarData);
		}
	}

	return S_OK;
}

// 폰트 렌더 해주는 역할
HRESULT CNarration::DisplayText(_float2 _vRTSize)
{
	if (true == m_isWaitingNextPage)
	{
		return S_OK;
	}

	if (m_NarrationDatas.empty() || m_NarrationDatas[m_iNarrationCount].lines.empty())
		return S_OK;

	// 예를 들어, 첫 번째 NarrationData의 텍스트만 사용한다고 가정
	auto& lines = m_NarrationDatas[m_iNarrationCount].lines;

	// 이쪽이 문제다...
	// 0번부터 m_iCurrentLine까지의 모든 라인을 렌더링 (이미 fade-in이 완료된 라인은 alpha = 1.0)

	for (int i = m_DisPlayTextLine; i <= m_iCurrentLine && i < lines.size(); i++)
	{

		// 이전 라인은 완전 불투명, 현재 라인은 fade-in 중
		float alpha = (i < m_iCurrentLine) ? 1.f : m_fTextAlpha;
		NarrationDialogData& dialogue = lines[i];

		// 해당 라인의 시작 위치
		float fx = { 0.f };
		float fy = { 0.f };

		if (true == dialogue.isLeft)
		{
			fx = dialogue.fpos.x;
			fy = dialogue.fpos.y;
		}
		else if (false == dialogue.isLeft)
		{
			fx = dialogue.fpos.x + _vRTSize.x / 2.f;
			fy = dialogue.fpos.y;
		}

		// 토큰화 처리 (텍스트 내의 제어문자들({, }, # 등)을 토큰화하여 각각 렌더링)
		vector<TextTokens> tokens;
		PaseTokens(dialogue.strtext, tokens);

		for (const auto& token : tokens)
		{
			// 줄바꿈 토큰이면 줄 이동 처리
			if (token.strText == L"\n")
			{
				if (dialogue.isLeft)
				{
					fx = dialogue.fpos.x; // 시작 x 좌표 복원
					fy += dialogue.fLineHieght; // 각 라인의 고유 fLineHeight 사용
				}
				else // 오른쪽 텍스트의 경우
				{
					fx = dialogue.fpos.x + _vRTSize.x / 2.f; // 시작 x 좌표 복원
					fy += dialogue.fLineHieght; // 각 라인의 고유 fLineHeight 사용
				}
				continue;
			}



			// 토큰의 크기를 측정하고 텍스트 렌더링 (알파값 적용)
			_vector vecSize = m_pGameInstance->Measuring(TEXT("Font40"), token.strText.c_str());
			_float2 vTextSize = { 0.f, 0.f };
			XMStoreFloat2(&vTextSize, vecSize);

			m_pGameInstance->Render_Scaling_Font(TEXT("Font40"), token.strText.c_str(), _float2(fx, fy), XMVectorSet(0.f, 0.f, 0.f, alpha) // alpha 적용
				, 0.f, _float2(0.f, 0.f), token.fScale);


			fx += vTextSize.x * token.fScale * 0.98f;
		}
	}

	return S_OK;
}

vector<CNarration_Anim*> CNarration::GetAnimationObjectForLine(const _uint iLine, _int AnimCount)
{
	// 기존 애니메이션 객체 해제
	for (auto& iter : m_pCurrentAnimObj)
	{
		Safe_Release(iter);
	}
	m_pCurrentAnimObj.clear();

	// 현재 iLine에 해당하는 애니메이션 객체 가져오기
	auto iter = m_vAnimObjectsByLine.find(iLine);
	if (iter == m_vAnimObjectsByLine.end())
	{
		vector<CNarration_Anim*> newAnims = CreateAnimationObjectsForLine(iLine);
		m_vAnimObjectsByLine[iLine] = newAnims;
		iter = m_vAnimObjectsByLine.find(iLine);
	}

	if (iter != m_vAnimObjectsByLine.end())
	{
		m_pCurrentAnimObj = iter->second;

		// TODO :: 누수 예상 - 박상욱
		for (auto& animObj : m_pCurrentAnimObj)
		{
			Safe_AddRef(animObj);
		}
	}

	// NarAnim과 동기화하여 섹션 레이어에 추가
	size_t ianimCount = m_NarrationDatas[m_iNarrationCount].lines[iLine].NarAnim.size();
	size_t icurrentAnimCount = m_pCurrentAnimObj.size();
	size_t iloopCount = min(ianimCount, icurrentAnimCount);

	for (size_t i = 0; i < iloopCount; ++i)
	{
		CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(
			m_NarrationDatas[m_iNarrationCount].lines[iLine].NarAnim[i].strSectionid,
			m_pCurrentAnimObj[i]
		);
	}

	if (TEXT("NOT") != m_NarrationDatas[m_iNarrationCount].lines[iLine].strSFX)
		m_pGameInstance->Start_SFX_Delay(m_NarrationDatas[m_iNarrationCount].lines[iLine].strSFX, 0.f, 50.f, false);

	if (TEXT("NOT") != m_NarrationDatas[m_iNarrationCount].lines[iLine].strSubSFX)
		m_pGameInstance->Start_SFX_Delay(m_NarrationDatas[m_iNarrationCount].lines[iLine].strSubSFX, 0.f, 30.f, false);

	return m_pCurrentAnimObj;
}

void CNarration::Set_NarrationByStrid(const _wstring& strTargetID)
{
	for (size_t i = 0; i < m_NarrationDatas.size(); ++i)
	{
		if (m_NarrationDatas[i].strid == strTargetID)
		{
			m_iNarrationCount = static_cast<_int>(i);
			m_iCurrentLine = 0; // 새로운 나레이션의 첫 라인부터 시작

			// 애니메이션 및 페이드인 관련 변수를 초기화
			m_fTextAlpha = 0.f;
			m_fFadeTimer = 0.f;
			m_fDelayTimer = 0.f;
			m_bAnimationStarted = false;
			m_isFade = false;
			m_isNarrationEnd = false;

			// 기존 애니메이션 객체도 초기화 (필요 시)

			for (auto& iter : m_pCurrentAnimObj)
			{
				Safe_Release(iter);
			}

			m_pCurrentAnimObj.clear();

			break;
		}
	}

	CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(m_NarrationDatas[m_iNarrationCount].lines[m_iCurrentLine].NarAnim[0].strSectionid, this);
}


vector<CNarration_Anim*> CNarration::CreateAnimationObjectsForLine(_uint iLine)
{
	vector<CNarration_Anim*> newAnims;
	// 현재 나레이션 데이터에서 iLine에 해당하는 DialogueData의 애니메이션 정보를 가져옴
	NarrationDialogData dialogueData = m_NarrationDatas[m_iNarrationCount].lines[iLine];

	// dialogueData의 NarAnim 배열에 있는 각 애니메이션 정보를 토대로
	// 애니메이션 객체를 생성해 주는 로직 (m_pGameInstance->Add_GameObject_ToLayer 호출 등)
	// 예시)
	_int iAnim = 0;
	for (auto& animInfo : dialogueData.NarAnim)
	{
		// tempData 구성 (대화 라인은 단일 데이터임)
		NarrationData tempData = m_NarrationDatas[m_iNarrationCount];
		tempData.lines.clear();

		// tempDialogue에 애니메이션 정보만 넣음
		NarrationDialogData tempDialogue = dialogueData;
		tempDialogue.NarAnim.clear();

		tempDialogue.NarAnim.push_back(animInfo);
		tempDialogue.AnimationCount = iAnim;
		++iAnim;

		tempData.lines.push_back(tempDialogue);

		// 대화 라인은 단일 객체이므로 LineCount는 0로 설정
		tempData.LineCount = 0;
		tempData.AnimIndex = 0;

		CGameObject* pObject = nullptr;
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(tempData.eCurlevelId, TEXT("Prototype_GameObject_Narration_Anim"), tempData.eCurlevelId, TEXT("Layer_UI"), &pObject, &tempData)))
		{
			continue;
		}

		CNarration_Anim* pAnim = static_cast<CNarration_Anim*>(pObject);
		newAnims.push_back(pAnim);

		// TODO :: 누수 예상 - 박상욱
		Safe_AddRef(pAnim);
	}

	return newAnims;
}

void CNarration::Update_Narration(_float _fTimeDelta)
{
	// 페이드인 진행중일 경우
	if (m_fTextAlpha < 1.f)
	{
		m_fFadeTimer += _fTimeDelta;
		m_fTextAlpha = min(m_fFadeTimer / m_NarrationDatas[m_iNarrationCount].lines[m_iCurrentLine].fFadeDuration, 1.f);

		if (!m_bAnimationStarted && m_fTextAlpha > 0.f)
		{
			m_bAnimationStarted = true;
			m_isFade = true;

			// 처음 진행 되는 경우
			for (int i = 0; i < m_pCurrentAnimObj.size(); ++i)
			{
				if (nullptr == m_pCurrentAnimObj[i])
				{
					m_isLeftRight = true;
					m_iCameraPos = m_NarrationDatas[m_iNarrationCount].lines[m_iCurrentLine].iCameraPos;
					GetAnimationObjectForLine(m_iCurrentLine);

				}
				// 애니메이션 시작해
				if (nullptr != m_pCurrentAnimObj[i])
				{
					m_pCurrentAnimObj[i]->StartAnimation();
				}

			}
		}
	}
	// 페이드인이 1 이상되어서 더이상 페이드 하면 안되는 경우
	else if (1.f <= m_fTextAlpha)
	{
		// fade-in 완료 후, 대기 타이머 갱신
		// 현재 라인이 나레이션의 최대 라인수와 같고 알파값이 1이상인 경우
		if (m_iCurrentLine == m_NarrationDatas[m_iNarrationCount].lines.size() - 1 && m_fTextAlpha >= 1.f)
		{
			// 페이드 상태가 아니다.
			m_isFade = false;
		}

		// fDelayNextLine 값을 사용하여 다음 단계로 전환
		// 딜레이만큼 대기 하고 다음 라인으로 이동한다.
		m_fDelayTimer += _fTimeDelta;

		//현재 딜레이가 요청한 딜레이보다 크다. 입장하자.
		if (m_fDelayTimer >= m_NarrationDatas[m_iNarrationCount].lines[m_iCurrentLine].fDelayNextLine)
		{
			// 나레이션 내에 다음 라인이 존재하면 입장
			if (m_iCurrentLine < m_NarrationDatas[m_iNarrationCount].lines.size() - 1)
			{
				if (false == m_NarrationDatas[m_iNarrationCount].lines[m_iCurrentLine].isFinishedThisLine)
				{
					++m_iCurrentLine;
					m_fFadeTimer = 0.f;
					m_fDelayTimer = 0.f;
					m_fTextAlpha = 0.f;
					m_bAnimationStarted = false;
					GetAnimationObjectForLine(m_iCurrentLine, 0);
					m_isLeftRight = m_NarrationDatas[m_iNarrationCount].lines[m_iCurrentLine].isLeft;
					m_iCameraPos = m_NarrationDatas[m_iNarrationCount].lines[m_iCurrentLine].iCameraPos;
				}

				// 이 나레이션에서 장을 넘겨야하면 넘기자. isfinishedthisLine은 다음장으로 넘긴다.
				else if (true == m_NarrationDatas[m_iNarrationCount].lines[m_iCurrentLine].isFinishedThisLine)
				{
					if (false == m_isNextLineReady)
					{
						m_isNextLineReady = true;


						m_isWaitingPrint = true;
						//GetAnimationObjectForLine(m_iCurrentLine, 0);
					}

					m_fWaitingTime += _fTimeDelta;
					if (m_fWaitingTime >= m_NarrationDatas[m_iNarrationCount].lines[m_iCurrentLine].fwaitingTime)
					{
						// 대기시간 후 false
						m_isWaitingPrint = false;
						m_isNextLineReady = false;
					}

					if (false == m_isWaitingPrint)
					{

						++m_iCurrentLine;
						m_isLeftRight = true;
						m_fFadeTimer = 0.f;
						m_fDelayTimer = 0.f;
						m_fTextAlpha = 0.f;
						m_bAnimationStarted = false;
						m_isWaitingPrint = true;

						// TODO :: 테스트용도
						m_DisPlayTextLine = m_iCurrentLine;
						

						// 대기시간이 완료되었다.
						// 다음 장으로 이동하자.

						m_iCameraPos = m_NarrationDatas[m_iNarrationCount].lines[m_iCurrentLine].iCameraPos;

						//_float3 vPos = _float3(0.f, 0.f, 1.f);
						//if (true == m_NarrationDatas[m_iNarrationCount].lines[m_iCurrentLine].isDirTurn)
						//	Event_Book_Main_Section_Change_Start(1, &vPos);
						//else
						//	Event_Book_Main_Section_Change_Start(0, &vPos);

						//if (true == m_NarrationDatas[m_iNarrationCount].lines[m_iCurrentLine].isDirTurn)
						//{
						//	vPos = SetPlayerPos(CSection_Manager::GetInstance()->Get_Next_Section_Key());
						//	Event_Book_Main_Section_Change_Start(1, &vPos);
						//}
						//else
						//{
						//	if (CSection_Manager::GetInstance()->Get_Prev_Section_Key() == (TEXT("Chapter1_P0708")))
						//	{
						//		vPos = SetPlayerPos(TEXT("Chapter1_P0708"));
						//	}
						//	Event_Book_Main_Section_Change_Start(0, &vPos);
						//
						//	// 이전일경우
						//	//이전 섹션의 키를 받아온다.
						//	// 넘어갈 대상의 페이지를 가져온다.
						//	// 넘어가는 패턴 -> 
						//}

						_float3 vPos = _float3(0.f, 0.f, 1.f);
						if (true == m_NarrationDatas[m_iNarrationCount].lines[m_iCurrentLine].isDirTurn)
						{
							vPos = SetPlayerPos(CSection_Manager::GetInstance()->Get_Next_Section_Key());
							Event_Book_Main_Section_Change_Start(1, &vPos);
						}
						else
						{
							if (CSection_Manager::GetInstance()->Get_Prev_Section_Key() == (TEXT("Chapter1_P0708")))
							{
								vPos = SetPlayerPos(TEXT("Chapter1_P0708"));
							}
							Event_Book_Main_Section_Change_Start(0, &vPos);

							// 이전일경우
							//이전 섹션의 키를 받아온다.
							// 넘어갈 대상의 페이지를 가져온다.
							// 넘어가는 패턴 -> 
						}


						CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(m_NarrationDatas[m_iNarrationCount].lines[m_iCurrentLine].NarAnim[0].strSectionid, this);

						m_fWaitingTime += _fTimeDelta;
						m_isWaitingNextPage = true;



					}
				}
				else
				{
					GetAnimationObjectForLine(m_iCurrentLine, 0);
					m_isLeftRight = m_NarrationDatas[m_iNarrationCount].lines[m_iCurrentLine].isLeft;
					m_iCameraPos = m_NarrationDatas[m_iNarrationCount].lines[m_iCurrentLine].iCameraPos;
				}
			}
			else // 나레이션 내에 다음 라인이 없나요?
			{
				// 현재 나레이션이 종료 상태인지, 즉 bTerminal이 true이면 호출 및 종료 처리
				_float3 vPos = _float3(0.f, 0.f, 1.f);
				if (m_NarrationDatas[m_iNarrationCount].lines[m_iCurrentLine].isFinishedThisLine)
				{
					if (true == m_NarrationDatas[m_iNarrationCount].lines[m_iCurrentLine].isDirTurn)
					{
						vPos = SetPlayerPos(CSection_Manager::GetInstance()->Get_Next_Section_Key());

						Event_Book_Main_Section_Change_Start(1, &vPos);

					}
					else
					{
						if (CSection_Manager::GetInstance()->Get_Prev_Section_Key() == (TEXT("Chapter1_P0708")))
						{
							vPos = SetPlayerPos(TEXT("Chapter1_P0708"));
						}


						Event_Book_Main_Section_Change_Start(0, &vPos);

						// 이전일경우
						//이전 섹션의 키를 받아온다.
						// 넘어갈 대상의 페이지를 가져온다.
						// 넘어가는 패턴 -> 
					}



					/* 하드코딩 */
					// Chapter1_P1112_Narration_01
					if (m_NarrationDatas[m_iNarrationCount].strid == TEXT("Chapter1_P1112_Narration_01"))
					{
						Uimgr->Set_VioletMeet(true);
					}


					m_isStartNarration = false;
					m_isNarrationEnd = true;
					m_isPlayNarration = false;
					Uimgr->Set_TurnoffPlayNarration(false);

					// TODO :: 누수 잡는 중

					for (auto iter : m_pCurrentAnimObj)
					{
						Safe_Release(iter);
					}
					m_pCurrentAnimObj.clear();

					for (auto iter : m_vAnimObjectsByLine)
					{
						for (_int i = 0; i < iter.second.size(); ++i)
						{
							Safe_Release(iter.second[i]);
						}
					}
					m_vAnimObjectsByLine.clear();

					m_isLeftRight = true;
					m_iCameraPos = CAMERA_LEFT;


				}
				// 어라? 나레이션 내에 다음 라인이 있네요? 다시 업데이트 쳐줍시다.
				else if (m_iCurrentLine < m_NarrationDatas[m_iNarrationCount].lines.size() - 1)
				{
					++m_iNarrationCount;
					m_strSectionName = m_NarrationDatas[m_iNarrationCount].strSectionid;
					m_iCurrentLine = 0;
					m_fFadeTimer = 0.f;
					m_fDelayTimer = 0.f;
					m_fTextAlpha = 0.f;
					m_bAnimationStarted = false;
					m_isNarrationEnd = false;


					//m_vAnimObjectsByLine.erase(m_iCurrentLine);
					GetAnimationObjectForLine(m_iCurrentLine, 0);
					m_isLeftRight = m_NarrationDatas[m_iNarrationCount].lines[m_iCurrentLine].isLeft;
					m_iCameraPos = m_NarrationDatas[m_iNarrationCount].lines[m_iCurrentLine].iCameraPos;

					// 그런데 그 라인이 다음으로 넘기는 라인인가요?
					if (true == m_NarrationDatas[m_iNarrationCount].lines[m_iCurrentLine].isFinishedThisLine)
					{
						//if (true == m_NarrationDatas[m_iNarrationCount].lines[m_iCurrentLine].isDirTurn)
						//	Event_Book_Main_Section_Change_Start(1, &vPos);
						//else
						//	Event_Book_Main_Section_Change_Start(0, &vPos);

						if (true == m_NarrationDatas[m_iNarrationCount].lines[m_iCurrentLine].isDirTurn)
						{
							vPos = SetPlayerPos(CSection_Manager::GetInstance()->Get_Next_Section_Key());

							Event_Book_Main_Section_Change_Start(1, &vPos);

						}
						else
						{
							if (CSection_Manager::GetInstance()->Get_Prev_Section_Key() == (TEXT("Chapter1_P0708")))
							{
								vPos = SetPlayerPos(TEXT("Chapter1_P0708"));
							}


							Event_Book_Main_Section_Change_Start(0, &vPos);

							// 이전일경우
							//이전 섹션의 키를 받아온다.
							// 넘어갈 대상의 페이지를 가져온다.
							// 넘어가는 패턴 -> 
						}
						CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(m_NarrationDatas[m_iNarrationCount].lines[m_iCurrentLine].NarAnim[0].strSectionid, this);
					}
				}
			}
		}
	}
}




HRESULT CNarration::Ready_Components()
{
	return S_OK;
}



CNarration* CNarration::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CNarration* pInstance = new CNarration(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CNarration Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CNarration::Clone(void* _pArg)
{
	CNarration* pInstance = new CNarration(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CNarration Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CNarration::Free()
{
	// TODO :: 누수 잡는 중
	for (auto iter : m_pCurrentAnimObj)
	{
		Safe_Release(iter);
	}
	m_pCurrentAnimObj.clear();

	for (auto iter : m_vAnimObjectsByLine)
	{
		for (_int i = 0; i < iter.second.size(); ++i)
		{
			Safe_Release(iter.second[i]);
		}
	}
	m_vAnimObjectsByLine.clear();


	for (auto iter : m_vecAnimation)
	{
		Safe_Release(iter);
	}
	m_vecAnimation.clear();


	__super::Free();
}

HRESULT CNarration::Cleanup_DeadReferences()
{

	return S_OK;
}

void CNarration::PaseTokens(const _wstring& _Text, vector<TextTokens>& _OutToken)
{
	vector<TextTokens> vTokens;
	size_t i = 0;

	// 스케일쪽 변경 300 이면 3, 100이면 1
	_float fCurrentSize = 1.0f;

	while (i < _Text.length())
	{
		if (_Text[i] == L'{')
		{
			size_t iClosing = _Text.find(L'}', i);

			if (iClosing == _wstring::npos)
				break;

			_wstring strNum = _Text.substr(i + 1, iClosing - i - 1);
			_int fvalue = stoi(strNum);

			// 글자 크기 결정
			fCurrentSize = fvalue / 100.f;
			i = iClosing + 1;
		}
		else if (_Text[i] == L'#')
		{
			vTokens.push_back({ L"\n", fCurrentSize });
			++i;
		}
		else
		{
			size_t iNext = _Text.find_first_of(L"{#", i);
			_wstring strSub;

			if (iNext == _wstring::npos)
			{
				strSub = _Text.substr(i);
				i = _Text.length();
			}
			else
			{
				strSub = _Text.substr(i, iNext - i);
				i = iNext;
			}
			vTokens.push_back({ strSub, fCurrentSize });
		}
	}

	_OutToken.resize(vTokens.size());

	for (_int i = 0; i < vTokens.size(); ++i)
	{
		_OutToken[i].strText = vTokens[i].strText;
		_OutToken[i].fScale = vTokens[i].fScale;
	}
}

void CNarration::StopNarration()
{
	//m_isPlayNarration = false;
	//m_isStartNarration = false;
	//m_isNarrationEnd = true;
	//
	//// 진행 중인 애니메이션 객체들 정리
	//for (auto& animObj : m_pCurrentAnimObj)
	//{
	//	Safe_Release(animObj);
	//}
	//m_pCurrentAnimObj.clear();
	if (false == m_isStartNarration)
		return;

	m_isStartNarration = false;
	m_isNarrationEnd = true;
	m_isPlayNarration = false;
	Uimgr->Set_TurnoffPlayNarration(false);

	for (auto iter : m_pCurrentAnimObj)
	{
		Safe_Release(iter);
	}
	m_pCurrentAnimObj.clear();

	for (auto iter : m_vAnimObjectsByLine)
	{
		for (_int i = 0; i < iter.second.size(); ++i)
		{
			Safe_Release(iter.second[i]);
		}
	}
	m_vAnimObjectsByLine.clear();

	m_isLeftRight = true;
	m_iCameraPos = CAMERA_LEFT;
	_float3 vPos = _float3(0.f, 0.f, 0.f);


	if (true == m_NarrationDatas[m_iNarrationCount].lines[m_iCurrentLine].isDirTurn)
		Event_Book_Main_Section_Change_Start(1, &vPos);
	else
		Event_Book_Main_Section_Change_Start(0, &vPos);

}

_float3 CNarration::SetPlayerPos(wstring _strNarrationID)
{
	_float3 vPos = { 0.f, 0.f, 0.f };

	if (TEXT("Chapter1_P0506") == _strNarrationID)
		vPos = _float3(-693.f, -35.5f, 0.0f);
	else if (TEXT("Chapter1_P0708") == _strNarrationID)
		vPos = _float3(-932.f, -79.1f, 0.0f);
	else if (TEXT("Chapter2_P0708") == _strNarrationID)
		vPos = _float3(0.0f, -293.f, 0.0f);
	else if (TEXT("Chapter2_P0708") == _strNarrationID)
		vPos = _float3(0.0f, -333.f, 0.0f);
	else if (TEXT("Chapter6_P0506") == _strNarrationID)
		vPos = _float3(10.09f, -266.0f, 0.0f);
	else if (TEXT("Chapter6_P1112") == _strNarrationID)
		vPos = _float3(-16.6f, -606.0f, 0.0f);
	else if (TEXT("Chapter8_P0506") == _strNarrationID)
		vPos = _float3(408.1f, -87.1f, 0.0f);
	else if (TEXT("Chapter8_P1112") == _strNarrationID)
		vPos = _float3(-808.1f, 192.f, 0.0f);
	else
		vPos = _float3(0.f, 0.f, 0.f);

	return vPos;
}

_bool CNarration::isLeftRight()
{
	return m_isLeftRight;
}

