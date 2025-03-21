#include "stdafx.h"
#include "Dialogue.h"
#include "GameInstance.h"

#include "Dialog_Manager.h"
#include "UI_Manager.h"


#include "Section_Manager.h"
#include "Trigger_Manager.h"

CDialog::CDialog(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CUI(_pDevice, _pContext)
{
}

CDialog::CDialog(const CDialog& _Prototype)
	: CUI(_Prototype)
{
}

HRESULT CDialog::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDialog::Initialize(void* _pArg)
{
	UIOBJDESC* pDesc = static_cast<UIOBJDESC*>(_pArg);

	m_vDisplay3DSize = _float2(pDesc->fSizeX * 0.5f, pDesc->fSizeY * 0.5f);


	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;


	if (FAILED(Ready_Components()))
		return E_FAIL;







	_float2 vCalScale = { 0.f, 0.f };
	vCalScale.x = m_vOriginSize.x * RATIO_BOOK2D_X;
	vCalScale.y = m_vOriginSize.y * RATIO_BOOK2D_Y;

	m_vDisplay2DSize = vCalScale;

	m_pControllerTransform->Set_Scale(m_vDisplay2DSize.x, m_vDisplay2DSize.y, 1.f);
	//
	m_isRender = false;
	//
	////CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this, SECTION_2D_PLAYMAP_UI);
	//m_isAddSectionRender = true;


	if (!m_DialogDatas.empty())
	{
		m_strCurrentDialogId = m_DialogDatas[0].id;
		m_iCurrentLineIndex = 0;
		// 초상화 상태 업데이트
		UpdateDialogueLine();
	}


	return S_OK;
}


void CDialog::Update(_float _fTimeDelta)
{

	// 다이얼로그가 활성화된 경우에만 B 키 입력 처리
	// TODO :: 어찌해야할지?
	if (m_isDisplayDialogue && KEY_DOWN(KEY::B))
	{
		_float2 vRTSize = SECTION_MGR->Get_Section_RenderTarget_Size(Get_Dialogue(m_tDialogIndex)[0].Section);

		if (true == Get_Dialogue(m_tDialogId)[0].lines[m_iCurrentLineIndex].is2D)
		{
			if (m_iCurrentLineIndex != Get_Dialogue(m_tDialogId)[0].lines.size() - 1)
			{
				m_pGameInstance->Start_SFX_Delay(TEXT("DialogWriting_sfx"), 0.f, 30.f, false);
				m_pGameInstance->Start_SFX_Delay(TEXT("DialogWriting_sfx"), 0.1f, 30.f, false);
				m_pGameInstance->Start_SFX_Delay(TEXT("DialogWriting_sfx"), 0.2f, 30.f, false);
				m_pGameInstance->Start_SFX_Delay(TEXT("DialogWriting_sfx"), 0.3f, 30.f, false);
				m_pGameInstance->Start_SFX_Delay(TEXT("DialogWriting_sfx"), 0.4f, 30.f, false);
			}
		}
		else
		{
			if (false == Get_Dialogue(m_tDialogId)[0].lines[m_iCurrentLineIndex].MTMSound)
			{
				if (m_iCurrentLineIndex != Get_Dialogue(m_tDialogId)[0].lines.size() - 1)
				{
					m_pGameInstance->Start_SFX_Delay(TEXT("DialogWriting_sfx"), 0.f, 30.f, false);
					m_pGameInstance->Start_SFX_Delay(TEXT("DialogWriting_sfx"), 0.1f, 30.f, false);
					m_pGameInstance->Start_SFX_Delay(TEXT("DialogWriting_sfx"), 0.2f, 30.f, false);
					m_pGameInstance->Start_SFX_Delay(TEXT("DialogWriting_sfx"), 0.3f, 30.f, false);
					m_pGameInstance->Start_SFX_Delay(TEXT("DialogWriting_sfx"), 0.4f, 30.f, false);
					m_pGameInstance->Start_SFX_Delay(TEXT("DialogWriting_sfx"), 0.5f, 30.f, false);
					m_pGameInstance->Start_SFX_Delay(TEXT("DialogWriting_sfx"), 0.6f, 30.f, false);
					m_pGameInstance->Start_SFX_Delay(TEXT("DialogWriting_sfx"), 0.7f, 30.f, false);
				}
			}
			else
			{
				if (m_iCurrentLineIndex != Get_Dialogue(m_tDialogId)[0].lines.size() - 1)
				{
					m_pGameInstance->Start_SFX_Delay(TEXT("A_sfx_minibeard_plastic-001"), 0.f, 30.f, false);
					m_pGameInstance->Start_SFX_Delay(TEXT("A_sfx_minibeard_plastic-001"), 0.11f, 30.f, false);
					m_pGameInstance->Start_SFX_Delay(TEXT("A_sfx_minibeard_plastic-001"), 0.22f, 30.f, false);
					m_pGameInstance->Start_SFX_Delay(TEXT("A_sfx_minibeard_plastic-001"), 0.33f, 30.f, false);
					m_pGameInstance->Start_SFX_Delay(TEXT("A_sfx_minibeard_plastic-001"), 0.44f, 30.f, false);
					m_pGameInstance->Start_SFX_Delay(TEXT("A_sfx_minibeard_plastic-001"), 0.55f, 30.f, false);
					m_pGameInstance->Start_SFX_Delay(TEXT("A_sfx_minibeard_plastic-001"), 0.66f, 30.f, false);
					m_pGameInstance->Start_SFX_Delay(TEXT("A_sfx_minibeard_plastic-001"), 0.77f, 30.f, false);
					m_pGameInstance->Start_SFX_Delay(TEXT("A_sfx_minibeard_plastic-001"), 0.88f, 30.f, false);
				}
			}
		}
		



		NextDialogue(vRTSize); // 다음 다이얼로그의 위치를 변경한다.
	}

	if (m_isDisplayDialogue && true == m_isFirstRefresh)
		UpdateDialogueLine();

}

void CDialog::Late_Update(_float _fTimeDelta)
{
	//Register_RenderGroup(RENDERGROUP::RG_3D, PRIORITY_3D::PR3D_UI);


	if (true == m_isDisplayDialogue && false == Get_Dialogue(m_tDialogId)[0].lines[m_iCurrentLineIndex].is2D)
	{
		Register_RenderGroup(RENDERGROUP::RG_3D, PRIORITY_3D::PR3D_UI);
	}

	if (true == m_isDisplayDialogue && false == m_isFirstRefresh)
	{
		wstring CurrentDialog(Get_Dialogue(m_tDialogId)[0].Section);
		CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(CurrentDialog, this, SECTION_2D_PLAYMAP_UI);

		if (false == CDialog_Manager::GetInstance()->Get_Dialog()->CBase::Is_Active())
		{
			// TODO
			CDialog_Manager::GetInstance()->Get_Dialog()->CBase::Set_Active(true);

		}

		if (false == m_pPortrait->CBase::Is_Active())
		{
			m_pPortrait->CBase::Set_Active(true);
		}



	}


}

HRESULT CDialog::Render()
{
	if (true == m_isDisplayDialogue)
	{
		wsprintf(m_tDialogIndex, m_tDialogId);

		if (false == m_isFirstRefresh)
		{
			// TODO :: 우리의 2D 렌더타겟은 가변적이다. 현재는 가져올 방법이 없어 강제로 넣는다.
			//_float2 vRTSize = _float2(RTSIZE_BOOK2D_X, RTSIZE_BOOK2D_Y);


			_float2 vRTSize = SECTION_MGR->Get_Section_RenderTarget_Size(Get_Dialogue(m_tDialogIndex)[0].Section);

			FirstCalPos(vRTSize);
		}

		if (0 != Get_Dialogue(m_tDialogIndex)[0].lines[m_iCurrentLineIndex].BG)
		{
			__super::Render(Get_Dialogue(m_tDialogIndex)[0].lines[m_iCurrentLineIndex].BG, PASS_VTXPOSTEX::UI_POINTSAMPLE);
		}
		else
		{
			_float4 vColor = _float4(Get_Dialogue(m_tDialogIndex)[0].lines[m_iCurrentLineIndex].Red / 255.f,
				Get_Dialogue(m_tDialogIndex)[0].lines[m_iCurrentLineIndex].Green / 255.f,
				Get_Dialogue(m_tDialogIndex)[0].lines[m_iCurrentLineIndex].Blue / 255.f,
				1.f);

			if (FAILED(m_pShaderCom->Bind_RawValue("g_vColors", &vColor, sizeof(_float4))))
				return E_FAIL;

			__super::Render(Get_Dialogue(m_tDialogIndex)[0].lines[m_iCurrentLineIndex].BG, PASS_VTXPOSTEX::DIALOGUE_BG_COLOR);
		}





		if (m_iCurrentLineIndex < Get_Dialogue(m_tDialogIndex)[0].lines.size())
		{
			// TODO :: 나중에 바꿔야함, 해당 값은 가변적이다.
			//_float2 vRTSize = _float2(RTSIZE_BOOK2D_X, RTSIZE_BOOK2D_Y);
			_float2 vRTSize = _float2(CSection_Manager::GetInstance()->Get_Section_RenderTarget_Size(Get_Dialogue(m_tDialogIndex)[0].Section));
			m_vFontColor = Get_Dialogue(m_tDialogIndex)[0].lines[m_iCurrentLineIndex].vFontColor;

			//vRTSize = _float2(2680.f, 4440.f);

			DisplayText(vRTSize);
		}
	}

	// TODO :: 일단 여기에다가 두면 얼굴을 나중에 렌더한다. (박상욱)
	//if (m_isDisplayDialogue)
	//	UpdateDialogueLine();


	return S_OK;
}

void CDialog::UpdateDialogueLine()
{
	if (m_DialogDatas.empty())
		return;

	// 현재 다이얼로그 데이터 및 라인 가져오기

	// wstring CurrentDialog(Get_Dialogue(m_tDialogId)[0].Section);

	if (0 == wcslen(m_tDialogId))
		return;

	DialogData currentData = Get_Dialogue(m_tDialogId)[0];
	if (m_iCurrentLineIndex >= (int)currentData.lines.size())
		return;

	const DialogLine& currentLine = currentData.lines[m_iCurrentLineIndex];

	// 2D/3D 크기 계산: is2D 여부에 따라 다른 사이즈 사용 (예시 값 사용)
	//_float2 vDisplaySize = currentLine.is2D ? m_vDisplay2DSize : m_vDisplay3DSize;

	// 대화 기준 위치 계산: 현재 라인의 location에 따라 계산 (예제에서는 단순 값 사용)
	_float3 vDialoguePos = { m_vCalDialoguePos.x, m_vCalDialoguePos.y, m_vCalDialoguePos.z };

	// 초상화 상태 업데이트: 대화에서 계산한 값들을 Portrait로 직접 전달
	if (m_pPortrait)
	{
		m_pPortrait->SetPortraitState(
			currentLine.isPortrait,             /* 초상화 노출 여부 */
			currentLine.is2D,                   /* 2D 여부 */
			static_cast<CPortrait::PORT>(currentLine.portrait), /* 초상화 종류 변환 */
			//vDisplaySize,                       /* 표시 크기 */
			vDialoguePos,                       /* 대화 기준 위치 */
			currentData.Section                 /* 섹션 이름 */
		);
	}


	

}

HRESULT CDialog::NextLevelLoadJson(_int _iNextLevel)
{
	LEVEL_ID eId = (LEVEL_ID)_iNextLevel;

	switch (eId)
	{
	case Client::LEVEL_CHAPTER_2:
		if (FAILED(LoadFromJson(TEXT("../Bin/Resources/Dialogue/dialog_data_Chapter_02.json"))))
			return E_FAIL;
		break;
	case Client::LEVEL_CHAPTER_4:
		if (FAILED(LoadFromJson(TEXT("../Bin/Resources/Dialogue/dialog_data_Chapter_04.json"))))
			return E_FAIL;
		break;
	case Client::LEVEL_CHAPTER_6:
		if (FAILED(LoadFromJson(TEXT("../Bin/Resources/Dialogue/dialog_data_Chapter_06.json"))))
			return E_FAIL;
		break;
	case Client::LEVEL_CHAPTER_8:
		if (FAILED(LoadFromJson(TEXT("../Bin/Resources/Dialogue/dialog_data_Chapter_08.json"))))
			return E_FAIL;
		break;
	default:
		break;

	}

	return S_OK;
}

_bool CDialog::isCurDialogTargetChange()
{
	if (false == m_isDisplayDialogue)
		assert(TEXT("Not Play Dialogue"));

	return Get_Dialogue(m_tDialogId)[0].isChangeTalker;
}

_uint CDialog::Get_TargetIndex()
{
	if (false == m_isDisplayDialogue)
		assert(TEXT("Not Play Dialogue"));

	return Get_Dialogue(m_tDialogId)[0].lines[m_iCurrentLineIndex].TerlkerIndex;

}

HRESULT CDialog::LoadFromJson(const std::wstring& filePath)
{
	if (GetFileAttributes(filePath.c_str()) == INVALID_FILE_ATTRIBUTES)
	{
		MSG_BOX("Dialog File Not Found");
		return E_FAIL;
	}

	ifstream ifs(filePath, ios::binary);
	json doc;
	ifs >> doc;

	if (doc.contains("dialogs") && doc["dialogs"].is_array())
	{
		for (auto& dialog : doc["dialogs"])
		{
			DialogData dialogData;
			// 인덱스값
			if (dialog.contains("id") && dialog["id"].is_string())
			{
				dialogData.id = StringToWstring(dialog["id"].get<string>());
			}

			if (dialog.contains("Section") && dialog["Section"].is_string())
			{
				dialogData.Section = StringToWstring(dialog["Section"].get<string>());
			}

			// Dialog가 끝날 시 실행될 이벤트(트리거) 태그, 필요하지 않으면 내용이 없습니다.
			if (dialog.contains("TriggerTag"))
			{
				dialogData.wstrTriggerTag = StringToWstring(dialog["TriggerTag"]);
			}

			if (dialog.contains("TriggerID"))
			{
				dialogData.iTriggerID = dialog["TriggerID"];
			}

			if (dialog.contains("isChangeTalker") && dialog["isChangeTalker"].is_boolean())
			{
				dialogData.isChangeTalker = dialog["isChangeTalker"].get<_bool>();
			}

			// 대상 + 대화 + BG + 배경 + 타이핑 효과
			if (dialog.contains("lines") && dialog["lines"].is_array())
			{
				for (auto& line : dialog["lines"])
				{
					DialogLine dialogLine;

					// 초상화 대상
					if (line.contains("Portrait") && line["Portrait"].is_number_integer())
					{
						dialogLine.portrait = static_cast<PORTRAITNAME>(line["Portrait"].get<int>());
					}

					if (line.contains("isDefender") && line["isDefender"].is_boolean())
					{
						dialogLine.isDefender = line["isDefender"].get<_bool>();
					}

					// 말하는 대상
					if (line.contains("Talker") && line["Talker"].is_string())
					{
						dialogLine.Talker = StringToWstring(line["Talker"].get<string>());
					}

					// 대사
					if (line.contains("text") && line["text"].is_string())
					{
						dialogLine.text = StringToWstring(line["text"].get<string>());
					}

					// 다이얼로그 배경
					if (line.contains("BG") && line["BG"].is_number_integer())
					{
						dialogLine.BG = line["BG"].get<int>();
					}

					// 배경의 RGB 색상
					if (line.contains("Red") && line["Red"].is_number_integer())
					{
						dialogLine.Red = line["Red"].get<int>();
					}
					if (line.contains("Green") && line["Green"].is_number_integer())
					{
						dialogLine.Green = line["Green"].get<int>();
					}
					if (line.contains("Blue") && line["Blue"].is_number_integer())
					{
						dialogLine.Blue = line["Blue"].get<int>();
					}

					if (line.contains("FontColorR") && line["FontColorR"].is_number_float())
					{
						dialogLine.vFontColor.x = line["FontColorR"].get<_float>();
					}

					if (line.contains("FontColorG") && line["FontColorG"].is_number_float())
					{
						dialogLine.vFontColor.y = line["FontColorG"].get<_float>();
					}

					if (line.contains("FontColorB") && line["FontColorB"].is_number_float())
					{
						dialogLine.vFontColor.z = line["FontColorB"].get<_float>();
					}
					// 2D인가요? // 초상화가 있나요?
					if (line.contains("is2D") && line["is2D"].is_boolean())
					{
						dialogLine.is2D = line["is2D"].get<_bool>();
					}

					if (line.contains("isPortrait") && line["isPortrait"].is_boolean())
					{
						dialogLine.isPortrait = line["isPortrait"].get<_bool>();
					}

					if (line.contains("MTMSound") && line["MTMSound"].is_boolean())
					{
						dialogLine.MTMSound = line["MTMSound"].get<_bool>();
					}

					if (line.contains("TalkerIndex") && line["TalkerIndex"].is_number_integer())
					{
						dialogLine.TerlkerIndex = line["TalkerIndex"].get<int>();
					}


					if (line.contains("isLineEnter") && line["isLineEnter"].is_boolean())
					{
						dialogLine.isLineEnter = line["isLineEnter"].get<_bool>();
					}

					if (line.contains("iAnimationIndex") && line["iAnimationIndex"].is_number_integer())
					{
						dialogLine.iAnimationIndex = line["iAnimationIndex"].get<int>();
					}

					// 다이얼로그 위치 enum 참조
					if (line.contains("location") && line["location"].is_number_integer())
					{
						dialogLine.location = static_cast<LOC>(line["location"].get<int>());
					}

					if (line.contains("OffsetX") && line["OffsetX"].is_number_float())
					{
						dialogLine.vOffset.x = line["OffsetX"].get<_float>();
					}

					if (line.contains("OffsetY") && line["OffsetY"].is_number_float())
					{
						dialogLine.vOffset.y = line["OffsetY"].get<_float>();
					}




					dialogLine.Section = dialogData.Section;

					// 탁탁탁 나오는 애니메이션의 스피드 및 대기 시간
					if (line.contains("animation") && line["animation"].is_object())
					{
						auto anim = line["animation"];
						if (anim.contains("duration") && anim["duration"].is_number_integer())
						{
							dialogLine.animation.duration = anim["duration"].get<int>();
						}

						if (anim.contains("speed") && anim["speed"].is_number_integer())
						{
							dialogLine.animation.speed = anim["speed"].get<int>();
						}
					}
					dialogData.lines.push_back(dialogLine);
				}
			}
			m_DialogDatas.push_back(dialogData);
		}
	}
	return S_OK;
}

// 폰트 렌더 해주는 역할
HRESULT CDialog::DisplayText(_float2 _vRTSize)
{
	m_vRTSize = _vRTSize;

	_float fScaleX = _vRTSize.x / RTSIZE_BOOK2D_X;
	_float fScaleY = _vRTSize.y / RTSIZE_BOOK2D_Y;


	// 가로가 기나요? 세로가 기나요?
	_bool isColumn = { false };

	if (_vRTSize.x < _vRTSize.y)
	{
		// 세로가 더 길어요!!
		isColumn = true;
	}

	static _wstring strDisplaytext;
	static _float fWaitTime = 0.0f;
	static _int iPreviousLineIndex = -1;

	static _wstring sPrevDialogId;

	// 현재 다이얼로그 ID와 이전 다이얼로그 ID를 비교하여 다르면 초기화
	if (sPrevDialogId != m_tDialogId)
	{
		strDisplaytext.clear();  // 텍스트 초기화
		fWaitTime = 0.0f;        // 대기 시간 초기화
		iPreviousLineIndex = -1; // 라인 인덱스 초기화
		sPrevDialogId = m_tDialogId; // 현재 다이얼로그 ID 저장
	}

	if (m_iCurrentLineIndex >= Get_Dialogue(m_tDialogIndex)[0].lines.size())
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_float3 vTextPos3D = { 0.f, 0.f, 0.f };
	_float3 vTextPos2D = { 0.f, 0.f, 0.f };

	if (false == Get_Dialogue(m_tDialogId)[0].lines[m_iCurrentLineIndex].is2D)
	{
		vTextPos3D = m_vCalDialoguePos;
	}
	else if (true == Get_Dialogue(m_tDialogId)[0].lines[m_iCurrentLineIndex].is2D)
	{
		vTextPos2D = m_vCalDialoguePos;
	}

	// 노출 시킬 섹션 가져오기
	wstring CurrentDialog(Get_Dialogue(m_tDialogId)[0].Section);
	if (m_strDialogSection != CurrentDialog)
	{
		wsprintf(m_strDialogSection, CurrentDialog.c_str());
		CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(m_strDialogSection, this);
	}


	// 라인이 변경되었을 때 초기화
	if (iPreviousLineIndex != m_iCurrentLineIndex)
	{
		strDisplaytext.clear();
		fWaitTime = 0.0f;
		iPreviousLineIndex = m_iCurrentLineIndex;

	}

	// 하나씩 출력되게 계산
	const auto& currentLine = Get_DialogueLine(m_tDialogIndex, m_iCurrentLineIndex);
	_float fSpeed = currentLine.animation.speed / 1000.0f;
	_int iFullWord = static_cast<int>(currentLine.text.length());

	// 노출 시킬 글자의 수
	_int icurrentLength = static_cast<int>(strDisplaytext.length());

	if (icurrentLength < iFullWord)
	{
		fWaitTime += fSpeed;
		if (fWaitTime >= 1.0f)
		{
			// 현재 대화의 총 글자의 배열에서 위에서 계산 노출 시킬 글자의 수만큼 이동후 그 텍스트를 더해주자.
			strDisplaytext += currentLine.text[icurrentLength];

			// 글자 노출 대기 시간 초기화~
			fWaitTime = 0.0f;
		}
	}


	// 위치에 따른 텍스트 위치 조정

	// 3D 기준
	switch (currentLine.location)
	{
	case LOC_MIDDOWN:  // 가운데 아래
	{

		if (false == Get_Dialogue(m_tDialogId)[0].lines[m_iCurrentLineIndex].is2D)
		{
			if (9 != Get_Dialogue(m_tDialogIndex)[0].lines[m_iCurrentLineIndex].BG)
			{
				vTextPos3D = _float3(g_iWinSizeX / 3.25f, g_iWinSizeY - g_iWinSizeY / 4.5f, 0.f);
				// 대상 이름 출력
				wsprintf(m_tFont, currentLine.Talker.c_str());
				pGameInstance->Render_Font(TEXT("Font28"), m_tFont, _float2(vTextPos3D.x, vTextPos3D.y), XMVectorSet(m_vFontColor.x / 255.f, m_vFontColor.y / 255.f, m_vFontColor.z / 255.f, 1.f));

				// 대화 내용 출력
				if (false == Get_Dialogue(m_tDialogIndex)[0].lines[m_iCurrentLineIndex].isLineEnter)
				{
					wsprintf(m_tFont, strDisplaytext.c_str());
					pGameInstance->Render_Font(TEXT("Font35"), m_tFont, _float2(vTextPos3D.x - 65.f, vTextPos3D.y + 70.f), XMVectorSet(m_vFontColor.x / 255.f, m_vFontColor.y / 255.f, m_vFontColor.z / 255.f, 1.f));
				}
				else if (true == Get_Dialogue(m_tDialogIndex)[0].lines[m_iCurrentLineIndex].isLineEnter)
				{
					wsprintf(m_tFont, strDisplaytext.c_str());
					pGameInstance->Render_Font(TEXT("Font35"), m_tFont, _float2(vTextPos3D.x - 65.f, vTextPos3D.y + 50.f), XMVectorSet(m_vFontColor.x / 255.f, m_vFontColor.y / 255.f, m_vFontColor.z / 255.f, 1.f));
				}

				Safe_Release(pGameInstance);
				return S_OK;
			}
			else
			{

				if (true == Get_Dialogue(m_tDialogIndex)[0].lines[m_iCurrentLineIndex].isLineEnter)
				{
					vTextPos3D = _float3(g_iWinSizeX / 3.25f, g_iWinSizeY - g_iWinSizeY / 4.2f, 0.f);
					// 대상 이름 출력
					wsprintf(m_tFont, currentLine.Talker.c_str());
					pGameInstance->Render_Font(TEXT("Font28"), m_tFont, _float2(vTextPos3D.x, vTextPos3D.y), XMVectorSet(m_vFontColor.x / 255.f, m_vFontColor.y / 255.f, m_vFontColor.z / 255.f, 1.f));

					// 대화 내용 출력
					wsprintf(m_tFont, strDisplaytext.c_str());
					pGameInstance->Render_Font(TEXT("Font35"), m_tFont, _float2(vTextPos3D.x - 120.f, vTextPos3D.y + 55.f), XMVectorSet(m_vFontColor.x / 255.f, m_vFontColor.y / 255.f, m_vFontColor.z / 255.f, 1.f));

					Safe_Release(pGameInstance);
					return S_OK;
				}
				else
				{
					vTextPos3D = _float3(g_iWinSizeX / 3.25f, g_iWinSizeY - g_iWinSizeY / 4.5f, 0.f);
					// 대상 이름 출력
					wsprintf(m_tFont, currentLine.Talker.c_str());
					pGameInstance->Render_Font(TEXT("Font28"), m_tFont, _float2(vTextPos3D.x, vTextPos3D.y), XMVectorSet(m_vFontColor.x / 255.f, m_vFontColor.y / 255.f, m_vFontColor.z / 255.f, 1.f));

					// 대화 내용 출력
					wsprintf(m_tFont, strDisplaytext.c_str());
					pGameInstance->Render_Font(TEXT("Font35"), m_tFont, _float2(vTextPos3D.x - 120.f, vTextPos3D.y + 55.f), XMVectorSet(m_vFontColor.x / 255.f, m_vFontColor.y / 255.f, m_vFontColor.z / 255.f, 1.f));

					Safe_Release(pGameInstance);
					return S_OK;
				}


			}

		}
		else if (true == Get_Dialogue(m_tDialogId)[0].lines[m_iCurrentLineIndex].is2D)
		{

			if (false == isColumn)
			{
				_float2 vPos = { 0.f , 0.f };

				vPos.x = vTextPos2D.x - _vRTSize.x * 0.08f / fScaleX;
				vPos.y = vTextPos2D.y + _vRTSize.y * 0.08f / fScaleY;


				vTextPos2D = _float3(vPos.x, vPos.y, 0.f);
			}
			else if (true == isColumn)
			{
				_float2 vPos = { 0.f , 0.f };

				vPos.x = vTextPos2D.x - _vRTSize.x * 0.08f / fScaleX;
				vPos.y = vTextPos2D.y + _vRTSize.y * 0.08f / fScaleY;


				vTextPos2D = _float3(vPos.x, vPos.y, 0.f);
			}

		}
	}
	break;

	case LOC_MIDHIGH:   // 가운데 위에
	{
		_float2 vPos = { 0.f , 0.f };
		if (false == isColumn)
		{
			vPos.x = vTextPos2D.x - _vRTSize.x * 0.08f / fScaleX;
			vPos.y = vTextPos2D.y + _vRTSize.y * 0.08f / fScaleY;
			vTextPos2D = _float3(vPos.x, vPos.y, 0.f);
		}
		else if (true == isColumn)
		{

			vPos.x = vTextPos2D.x - _vRTSize.x * 0.08f / fScaleX;
			vPos.y = vTextPos2D.y + _vRTSize.y * 0.08f / fScaleY;
			vTextPos2D = _float3(vPos.x, vPos.y, 0.f);

			//vPos.x = vTextPos2D.x - _vRTSize.x * 0.135f ;
			//vPos.y = vTextPos2D.y + _vRTSize.y * 0.012f ;
			//vTextPos2D = _float3(vPos.x, vPos.y, 0.f);


		}
	}
	break;
	//
	case LOC_MIDLEFT:  // 가운데 좌측
	{
		_float2 vPos = { 0.f , 0.f };

		vPos.x = vTextPos2D.x - _vRTSize.x * 0.08f / fScaleX;
		vPos.y = vTextPos2D.y + _vRTSize.y * 0.08f / fScaleY;


		vTextPos2D = _float3(vPos.x, vPos.y, 0.f);
	}
	break;

	case LOC_MIDRIGHT: // 가운데 우측
	{


		_float2 vPos = { 0.f , 0.f };
		if (false == isColumn)
		{
			vPos.x = vTextPos2D.x - _vRTSize.x * 0.08f / fScaleX;
			vPos.y = vTextPos2D.y + _vRTSize.y * 0.08f / fScaleY;
			vTextPos2D = _float3(vPos.x, vPos.y, 0.f);
		}
		else if (true == isColumn)
		{

			vPos.x = vTextPos2D.x - _vRTSize.x * 0.08f / fScaleX;
			vPos.y = vTextPos2D.y + _vRTSize.y * 0.08f / fScaleY;
			vTextPos2D = _float3(vPos.x, vPos.y, 0.f);



			//wsprintf(m_tFont, currentLine.Talker.c_str());
			//pGameInstance->Render_Font(TEXT("Font20"), m_tFont, _float2(vTextPos2D.x, vTextPos2D.y), XMVectorSet(m_vFontColor.x / 255.f, m_vFontColor.y / 255.f, m_vFontColor.z / 255.f, 1.f));
			//
			//
			////vCalPos.x += _vRTSize.x * 0.03f;
			////vCalPos.y += +_vRTSize.y * 0.1f;
			//
			//// 대화 내용 출력
			//wsprintf(m_tFont, strDisplaytext.c_str());
			//pGameInstance->Render_Font(TEXT("Font24"), m_tFont, _float2(vTextPos2D.x - 50.f, vTextPos2D.y + 50.f), XMVectorSet(m_vFontColor.x / 255.f, m_vFontColor.y / 255.f, m_vFontColor.z / 255.f, 1.f));
			//Safe_Release(pGameInstance);
			//return S_OK;

		}
	}
	break;

	case LOC_LEFTDOWN: // 가운데 우측
	{
		_float2 vPos = { 0.f , 0.f };

		vPos.x = vTextPos2D.x - _vRTSize.x * 0.08f / fScaleX;
		vPos.y = vTextPos2D.y + _vRTSize.y * 0.08f / fScaleY;


		vTextPos2D = _float3(vPos.x, vPos.y, 0.f);



	}
	break;

	case LOC_LEFTHIGH: // 가운데 우측
	{
		_float2 vPos = { 0.f , 0.f };

		vPos.x = vTextPos2D.x - _vRTSize.x * 0.08f / fScaleX;
		vPos.y = vTextPos2D.y + _vRTSize.y * 0.08f / fScaleY;


		vTextPos2D = _float3(vPos.x, vPos.y, 0.f);
	}
	break;

	case LOC_RIGHTHIGH: // 가운데 우측
	{
		_float2 vPos = { 0.f , 0.f };

		vPos.x = vTextPos2D.x - _vRTSize.x * 0.08f / fScaleX;
		vPos.y = vTextPos2D.y + _vRTSize.y * 0.08f / fScaleY;


		vTextPos2D = _float3(vPos.x, vPos.y, 0.f);
	}
	break;

	case LOC_RIGHDOWN: // 가운데 우측
	{
		_float2 vPos = { 0.f , 0.f };

		vPos.x = vTextPos2D.x - _vRTSize.x * 0.08f / fScaleX;
		vPos.y = vTextPos2D.y + _vRTSize.y * 0.08f / fScaleY;


		vTextPos2D = _float3(vPos.x, vPos.y, 0.f);
	}
	break;
	case LOC_VERYMIDHIGH: // 가운데 우측
	{
		_float2 vPos = { 0.f , 0.f };

		if (true == Get_Dialogue(m_tDialogIndex)[0].lines[m_iCurrentLineIndex].isDefender)
		{
			vPos.x = vTextPos2D.x - _vRTSize.x * 0.08f / fScaleX;
			vPos.y = vTextPos2D.y + _vRTSize.y * 0.05f / fScaleY;


			vTextPos2D = _float3(vPos.x, vPos.y, 0.f);
		}
		else
		{
			vPos.x = vTextPos2D.x - _vRTSize.x * 0.08f / fScaleX;
			vPos.y = vTextPos2D.y + _vRTSize.y * 0.08f / fScaleY;


			vTextPos2D = _float3(vPos.x, vPos.y, 0.f);
		}




	}


	break;
	}

	// 2D 기준
	_float2 vCalPos = { 0.f, 0.f };
	// 중점




	if (isColumn == false)
	{
		_float2 vMidPoint = { _vRTSize.x / 2.f, _vRTSize.y / 2.f };

		vCalPos.x = vMidPoint.x + vTextPos2D.x;
		vCalPos.y = vMidPoint.y - vTextPos2D.y;

		// 대상 이름 출력
		//wsprintf(m_tFont, currentLine.Talker.c_str());
		//pGameInstance->Render_Font(TEXT("Font20"), m_tFont, _float2(vCalPos.x, vCalPos.y ), XMVectorSet(m_vFontColor.x / 255.f, m_vFontColor.y / 255.f, m_vFontColor.z / 255.f, 1.f));
		//
		//
		//
		//// 대화 내용 출력
		//wsprintf(m_tFont, strDisplaytext.c_str());
		//pGameInstance->Render_Font(TEXT("Font24"), m_tFont, _float2((vCalPos.x - 50.f), (vCalPos.y + 45.f)), XMVectorSet(m_vFontColor.x / 255.f, m_vFontColor.y / 255.f, m_vFontColor.z / 255.f, 1.f));

		if (true == Get_Dialogue(m_tDialogIndex)[0].lines[m_iCurrentLineIndex].isDefender)
		{
			//_float2 vMidPoint = { _vRTSize.x / 2.f, _vRTSize.y / 2.f };
			//
			//vCalPos.x = vMidPoint.x + vTextPos2D.x;
			//vCalPos.y = vMidPoint.y - vTextPos2D.y;

			wsprintf(m_tFont, currentLine.Talker.c_str());
			pGameInstance->Render_Font(TEXT("Font16"), m_tFont, _float2(vCalPos.x, vCalPos.y), XMVectorSet(m_vFontColor.x / 255.f, m_vFontColor.y / 255.f, m_vFontColor.z / 255.f, 1.f));


			// 대화 내용 출력
			wsprintf(m_tFont, strDisplaytext.c_str());
			pGameInstance->Render_Font(TEXT("Font18"), m_tFont, _float2((vCalPos.x), (vCalPos.y + 25.f)), XMVectorSet(m_vFontColor.x / 255.f, m_vFontColor.y / 255.f, m_vFontColor.z / 255.f, 1.f));

		}
		else
		{
			//_float2 vMidPoint = { _vRTSize.x / 2.f, _vRTSize.y / 2.f };
			//
			//vCalPos.x = vMidPoint.x + vTextPos2D.x;
			//vCalPos.y = vMidPoint.y - vTextPos2D.y;

			wsprintf(m_tFont, currentLine.Talker.c_str());
			pGameInstance->Render_Font(TEXT("Font20"), m_tFont, _float2(vCalPos.x, vCalPos.y), XMVectorSet(m_vFontColor.x / 255.f, m_vFontColor.y / 255.f, m_vFontColor.z / 255.f, 1.f));


			// 대화 내용 출력
			if (false == Get_Dialogue(m_tDialogId)[0].lines[m_iCurrentLineIndex].isLineEnter)
			{
				wsprintf(m_tFont, strDisplaytext.c_str());
				pGameInstance->Render_Font(TEXT("Font24"), m_tFont, _float2((vCalPos.x - 50.f), (vCalPos.y + 45.f)), XMVectorSet(m_vFontColor.x / 255.f, m_vFontColor.y / 255.f, m_vFontColor.z / 255.f, 1.f));
			}
			else if (true == Get_Dialogue(m_tDialogId)[0].lines[m_iCurrentLineIndex].isLineEnter)
			{
				wsprintf(m_tFont, strDisplaytext.c_str());
				pGameInstance->Render_Font(TEXT("Font24"), m_tFont, _float2((vCalPos.x - 50.f), (vCalPos.y + 40.f)), XMVectorSet(m_vFontColor.x / 255.f, m_vFontColor.y / 255.f, m_vFontColor.z / 255.f, 1.f));
			}
			
		}


	}
	else if (isColumn == true)
	{
		_float2 vMidPoint = { _vRTSize.x / 2.f, _vRTSize.y / 2.f };
		vCalPos.x = vMidPoint.x + vTextPos2D.x;
		vCalPos.y = vMidPoint.y - vTextPos2D.y;

		wsprintf(m_tFont, currentLine.Talker.c_str());
		pGameInstance->Render_Font(TEXT("Font20"), m_tFont, vCalPos, XMVectorSet(m_vFontColor.x / 255.f, m_vFontColor.y / 255.f, m_vFontColor.z / 255.f, 1.f));


		//vCalPos.x += _vRTSize.x * 0.03f;
		//vCalPos.y += +_vRTSize.y * 0.1f;

		// 대화 내용 출력

		if (false == Get_Dialogue(m_tDialogId)[0].lines[m_iCurrentLineIndex].isLineEnter)
		{
			wsprintf(m_tFont, strDisplaytext.c_str());
			pGameInstance->Render_Font(TEXT("Font24"), m_tFont, _float2(vCalPos.x - 50.f, vCalPos.y + 50.f), XMVectorSet(m_vFontColor.x / 255.f, m_vFontColor.y / 255.f, m_vFontColor.z / 255.f, 1.f));
		}
		else if (true == Get_Dialogue(m_tDialogId)[0].lines[m_iCurrentLineIndex].isLineEnter)
		{
			wsprintf(m_tFont, strDisplaytext.c_str());
			pGameInstance->Render_Font(TEXT("Font24"), m_tFont, _float2((vCalPos.x - 50.f), (vCalPos.y + 45.f/* * fScaleY*/)), XMVectorSet(m_vFontColor.x / 255.f, m_vFontColor.y / 255.f, m_vFontColor.z / 255.f, 1.f));
		}


		//wsprintf(m_tFont, strDisplaytext.c_str());
		//pGameInstance->Render_Font(TEXT("Font24"), m_tFont, _float2(vCalPos.x - 50.f, vCalPos.y + 50.f), XMVectorSet(m_vFontColor.x / 255.f, m_vFontColor.y / 255.f, m_vFontColor.z / 255.f, 1.f));
	}

	Safe_Release(pGameInstance);

	return S_OK;





}

// 다음 다이얼로그에서 Json->Location에 따른 위치 변경
void CDialog::NextDialogue(_float2 _RTSize)
{
	// 이 부분 확인 필요함...
	//if (!CDialog_Manager::GetInstance()->Get_DisPlayDialogue())
	//	return;

	if (!m_isDisplayDialogue)
		return;

	CPlayer* pPlayer = Uimgr->Get_Player();

	if (false == pPlayer->Is_PlayerInputBlocked())
	{
		pPlayer->Set_BlockPlayerInput(true);
	}

	m_vRTSize = _RTSize;

	// 현재 렌더사이즈 / 2560 의 사이즈의 비율
	_float fScaleX = _RTSize.x / RTSIZE_BOOK2D_X;
	_float fScaleY = _RTSize.y / RTSIZE_BOOK2D_Y;


	_tchar _strDialogue[MAX_PATH] = {};
	wsprintf(_strDialogue, m_tDialogId);

	if (wcslen(_strDialogue) == 0)
	{
		return;
	}

	// 2D 기준
	if (m_iCurrentLineIndex <= Get_Dialogue(_strDialogue)[0].lines.size())
	{
		m_iCurrentLineIndex += 1;

		if (m_iCurrentLineIndex <= Get_Dialogue(_strDialogue)[0].lines.size() - 1)
		{
			if (-1 != Get_Dialogue(_strDialogue)[0].lines[m_iCurrentLineIndex].iAnimationIndex)
			{
				CDialog_Manager::GetInstance()->Set_ActionChange(Get_Dialogue(_strDialogue)[0].lines[m_iCurrentLineIndex].iAnimationIndex);
			}
		}

		if (m_iCurrentLineIndex == Get_Dialogue(_strDialogue)[0].lines.size() - 1)
		{
			m_isLastDialogLine = true;
		}
		else
		{
			m_isLastDialogLine = false;
		}






		_float2 vPos = {};
		if (false == m_isRender)
		{
			m_isRender = true;
			m_isPortraitRender = true;
		}

		_float2 vOffset = { 0.f, 0.f };

		if (m_iCurrentLineIndex <= Get_Dialogue(_strDialogue)[0].lines.size() - 1)
		{
			vOffset = _float2(Get_Dialogue(m_tDialogId)[0].lines[m_iCurrentLineIndex].vOffset.x, Get_Dialogue(m_tDialogId)[0].lines[m_iCurrentLineIndex].vOffset.y);

			// 250314 :: 박상욱
			if (true == Get_Dialogue(m_tDialogId)[0].isChangeTalker)
			{
				CGameObject* pGameObject = CDialog_Manager::GetInstance()->Get_DialogTarget(Get_Dialogue(m_tDialogId)[0].lines[m_iCurrentLineIndex].TerlkerIndex);

				if (nullptr == pGameObject)
					assert(TEXT("Target Not Found"));

				_vector TargetPos = pGameObject->Get_ControllerTransform()->Get_State(COORDINATE_2D, CTransform::STATE_POSITION);

				m_vDialoguePos = _float4(TargetPos.m128_f32[0], TargetPos.m128_f32[1], 0.f, 1.f);
			}
			else
			{
				if (true == Get_Dialogue(m_tDialogId)[0].lines[m_iCurrentLineIndex].is2D)
				{
					if (1 <= CDialog_Manager::GetInstance()->Get_DialogTargetCount())
					{
						CGameObject* pGameObject = CDialog_Manager::GetInstance()->Get_DialogTarget(0);
						_vector TargetPos = pGameObject->Get_ControllerTransform()->Get_State(COORDINATE_2D, CTransform::STATE_POSITION);

						m_vDialoguePos = _float4(TargetPos.m128_f32[0], TargetPos.m128_f32[1], 0.f, 1.f);
					}
				}
			}
		}


		switch (Get_DialogueLine(_strDialogue, m_iCurrentLineIndex).location)
		{
		case LOC_MIDDOWN:
		{
			if (true == Get_Dialogue(m_tDialogId)[0].lines[m_iCurrentLineIndex].is2D)
			{
				if (!m_isAddSectionRender)
				{
					_tchar	strSectionID[MAX_PATH];
					wsprintf(strSectionID, m_strCurrentSection);

					if (TEXT("NOTWORD") == strSectionID)
					{
						CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this, SECTION_2D_PLAYMAP_UI);
					}
					else
					{
						wstring CurrentDialog(Get_Dialogue(m_tDialogId)[0].Section);

						if (true == Get_Dialogue(m_tDialogId)[0].lines[m_iCurrentLineIndex].is2D)
						{
							CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(CurrentDialog, this, SECTION_2D_PLAYMAP_UI);
						}
						else if (false == Get_Dialogue(m_tDialogId)[0].lines[m_iCurrentLineIndex].is2D)
						{
							Register_RenderGroup(RENDERGROUP::RG_3D, PRIORITY_3D::PR3D_UI);
						}
					}
					m_isAddSectionRender = true;
				}

				vPos.x = m_vDialoguePos.x;
				vPos.y = m_vDialoguePos.y;

				vPos.x += vOffset.x / fScaleX;
				vPos.y -= _RTSize.y * 0.13f / fScaleY + vOffset.y / fScaleY;
			}
			else if (false == Get_Dialogue(m_tDialogId)[0].lines[m_iCurrentLineIndex].is2D)
			{
				//if (true == m_isAddSectionRender)
				//{
				//	CSection_Manager::GetInstance()->Remove_GameObject_ToCurSectionLayer(this);
				//	m_isAddSectionRender = false;
				//}

				vPos = _float2(g_iWinSizeX / 2.f, g_iWinSizeY - g_iWinSizeY / 8.f);
				m_vCalDialoguePos = _float3(vPos.x, vPos.y, 0.f);

				m_vCurPos = vPos;

				m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(vPos.x - g_iWinSizeX * 0.5f, -vPos.y + g_iWinSizeY * 0.5f, 0.f, 1.f));

				return;

			}
		}
		break;

		case LOC_MIDHIGH:
		{
			if (!m_isAddSectionRender)
			{
				_tchar	strSectionID[MAX_PATH];
				wsprintf(strSectionID, m_strCurrentSection);

				if (TEXT("NOTWORD") == strSectionID)
				{
					CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this, SECTION_2D_PLAYMAP_UI);
				}
				else
				{
					wstring CurrentDialog(Get_Dialogue(m_tDialogId)[0].Section);
					CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(CurrentDialog, this, SECTION_2D_PLAYMAP_UI);
					//Section_Manager::GetInstance()->Add_GameObject_ToSectionLayer(strSectionID, this);
				}
				m_isAddSectionRender = true;
			}

			vPos.x = m_vDialoguePos.x;
			vPos.y = m_vDialoguePos.y;

			vPos.x += vOffset.x / fScaleX;
			vPos.y += _RTSize.y * 0.25f / fScaleY + vOffset.y / fScaleY;
		}
		break;

		case LOC_MIDLEFT:
		{
			if (!m_isAddSectionRender)
			{
				_tchar	strSectionID[MAX_PATH];
				wsprintf(strSectionID, m_strCurrentSection);

				if (TEXT("NOTWORD") == strSectionID)
				{
					CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this, SECTION_2D_PLAYMAP_UI);
				}
				else
				{
					wstring CurrentDialog(Get_Dialogue(m_tDialogId)[0].Section);
					CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(CurrentDialog, this, SECTION_2D_PLAYMAP_UI);
					//CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(strSectionID, this);
				}
				m_isAddSectionRender = true;
			}

			vPos.x = m_vDialoguePos.x;
			vPos.y = m_vDialoguePos.y;

			vPos.x -= _RTSize.x * 0.14f / fScaleX + vOffset.x / fScaleX;
			vPos.y += _RTSize.y * 0.05f / fScaleY + vOffset.y / fScaleY;
		}
		break;


		case LOC_MIDRIGHT:
		{
			if (!m_isAddSectionRender)
			{
				_tchar	strSectionID[MAX_PATH];
				wsprintf(strSectionID, m_strCurrentSection);

				if (TEXT("NOTWORD") == strSectionID)
				{
					CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this, SECTION_2D_PLAYMAP_UI);
				}
				else
				{
					wstring CurrentDialog(Get_Dialogue(m_tDialogId)[0].Section);
					CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(CurrentDialog, this, SECTION_2D_PLAYMAP_UI);
					//CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(strSectionID, this);
				}
				m_isAddSectionRender = true;
			}

			vPos.x = m_vDialoguePos.x;
			vPos.y = m_vDialoguePos.y;

			vPos.x += _RTSize.x * 0.165f / fScaleX + vOffset.x / fScaleX;
			vPos.y += _RTSize.y * 0.05f / fScaleY + vOffset.y / fScaleY;
		}
		break;

		case LOC_LEFTDOWN:
		{
			if (!m_isAddSectionRender)
			{
				_tchar	strSectionID[MAX_PATH];
				wsprintf(strSectionID, m_strCurrentSection);

				if (TEXT("NOTWORD") == strSectionID)
				{
					CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this, SECTION_2D_PLAYMAP_UI);
				}
				else
				{
					wstring CurrentDialog(Get_Dialogue(m_tDialogId)[0].Section);
					CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(CurrentDialog, this, SECTION_2D_PLAYMAP_UI);
					//CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(strSectionID, this);
				}
				m_isAddSectionRender = true;
			}

			vPos.x = m_vDialoguePos.x;
			vPos.y = m_vDialoguePos.y;

			vPos.x -= _RTSize.x * 0.165f / fScaleX + vOffset.x / fScaleX;
			vPos.y -= _RTSize.y * 0.14f / fScaleY + vOffset.y / fScaleY;
		}
		break;

		case LOC_LEFTHIGH:
		{
			if (!m_isAddSectionRender)
			{
				_tchar	strSectionID[MAX_PATH];
				wsprintf(strSectionID, m_strCurrentSection);

				if (TEXT("NOTWORD") == strSectionID)
				{
					CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this, SECTION_2D_PLAYMAP_UI);
				}
				else
				{
					wstring CurrentDialog(Get_Dialogue(m_tDialogId)[0].Section);
					CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(CurrentDialog, this, SECTION_2D_PLAYMAP_UI);
					//CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(strSectionID, this);
				}
				m_isAddSectionRender = true;
			}

			vPos.x = m_vDialoguePos.x;
			vPos.y = m_vDialoguePos.y;

			vPos.x -= _RTSize.x * 0.165f / fScaleX + vOffset.x / fScaleX;
			vPos.y += _RTSize.y * 0.19f / fScaleY + vOffset.y / fScaleY;
		}
		break;

		case LOC_RIGHTHIGH:
		{
			if (!m_isAddSectionRender)
			{
				_tchar	strSectionID[MAX_PATH];
				wsprintf(strSectionID, m_strCurrentSection);

				if (TEXT("NOTWORD") == strSectionID)
				{
					CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this, SECTION_2D_PLAYMAP_UI);
				}
				else
				{
					wstring CurrentDialog(Get_Dialogue(m_tDialogId)[0].Section);
					CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(CurrentDialog, this, SECTION_2D_PLAYMAP_UI);
					//CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(strSectionID, this);
				}
				m_isAddSectionRender = true;
			}

			vPos.x = m_vDialoguePos.x;
			vPos.y = m_vDialoguePos.y;

			vPos.x += _RTSize.x * 0.165f / fScaleX + vOffset.x / fScaleX;
			vPos.y += _RTSize.y * 0.19f / fScaleY + vOffset.y / fScaleY;
		}
		break;

		case LOC_RIGHDOWN:
		{
			if (!m_isAddSectionRender)
			{
				_tchar	strSectionID[MAX_PATH];
				wsprintf(strSectionID, m_strCurrentSection);

				if (TEXT("NOTWORD") == strSectionID)
				{
					CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this, SECTION_2D_PLAYMAP_UI);
				}
				else
				{
					wstring CurrentDialog(Get_Dialogue(m_tDialogId)[0].Section);
					CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(CurrentDialog, this, SECTION_2D_PLAYMAP_UI);
					//CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(strSectionID, this);
				}
				m_isAddSectionRender = true;
			}

			vPos.x = m_vDialoguePos.x;
			vPos.y = m_vDialoguePos.y;

			vPos.x += _RTSize.x * 0.165f / fScaleX + vOffset.x / fScaleX;
			vPos.y -= _RTSize.y * 0.14f / fScaleY + vOffset.y / fScaleY;
		}
		break;

		case LOC_VERYMIDHIGH:
		{
			if (!m_isAddSectionRender)
			{
				_tchar	strSectionID[MAX_PATH];
				wsprintf(strSectionID, m_strCurrentSection);

				if (TEXT("NOTWORD") == strSectionID)
				{
					CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this, SECTION_2D_PLAYMAP_UI);
				}
				else
				{
					wstring CurrentDialog(Get_Dialogue(m_tDialogId)[0].Section);
					CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(CurrentDialog, this, SECTION_2D_PLAYMAP_UI
					);
					//CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(strSectionID, this);
				}
				m_isAddSectionRender = true;
			}

			vPos.x = m_vDialoguePos.x;
			vPos.y = m_vDialoguePos.y;

			vPos.x += vOffset.x / fScaleX;
			vPos.y += _RTSize.y * 0.38f / fScaleY + vOffset.y / fScaleY;
		}


		}

		m_vCalDialoguePos = _float3(vPos.x, vPos.y, 0.f);
		m_vCurPos = vPos;

		//vPos.x = vPos.x + RTSIZE_BOOK2D_X * 0.5f;
		//vPos.y = -vPos.y + RTSIZE_BOOK2D_Y * 0.5f;
		m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(vPos.x, vPos.y, 0.f, 1.f));


		///////////////////////////////////////
		// 다이얼로그가 종료되었을경우 처리해주자. //
		///////////////////////////////////////

		if (m_iCurrentLineIndex == Get_Dialogue(_strDialogue)[0].lines.size())
		{
			// 다음 대사가 없으므로 트리거를 종료 시킨다.
			// AddSection 을 다음 다이얼로그를 위해 false를 시킨다.
			m_pPortrait->Set_AddSectionRender(false);

			// 불필요한 렌더 제거를 위해 렌더 그룹에서 제거한다.
			CSection_Manager::GetInstance()->Remove_GameObject_ToCurSectionLayer(this);
			// 트리거로 햇을수도 있으므로 트리거에게 종료했다고 보내준다.
			CTrigger_Manager::GetInstance()->On_End(m_tDialogId);
			//// Event가 있을 경우;
			if (-1 != Get_Dialogue(_strDialogue)[0].iTriggerID)
			{
				DialogData Data = Get_Dialogue(_strDialogue)[0];
				CTrigger_Manager::GetInstance()->Register_TriggerEvent(Data.wstrTriggerTag,
					Data.iTriggerID);
			}

			// TODO :: dialogmgr에 넣어야한다.
			CDialog_Manager::GetInstance()->Set_DialogEnd();

			m_iCurrentLineIndex = 0;
			m_isDisplayDialogue = false;
			m_isLastDialogLine = false;


			m_pPortrait->Set_PortraitRender(false);

			CPlayer* pPlayer = Uimgr->Get_Player();
			assert(pPlayer);
			pPlayer->Set_BlockPlayerInput(false);


			//wsprintf(m_strCurrentSection, L"");

			CDialog_Manager::GetInstance()->Clear_DialogTarget();

			m_isFirstRefresh = false;

			// 다이얼로그 끝나고 상점을 닫기 위해 해당 다이얼로그가 상점 관련인지 확인한다.
			isOpenPanel(_strDialogue);



		}
	}
}

// 처음 다이얼로그 입장 시 위치 계산해주는 함수
void CDialog::FirstCalPos(_float2 _RTSize)
{
	if (true == Get_Dialogue(m_tDialogId)[0].lines[m_iCurrentLineIndex].is2D)
	{
		if (m_iCurrentLineIndex != Get_Dialogue(m_tDialogId)[0].lines.size())
		{
			m_pGameInstance->Start_SFX_Delay(TEXT("DialogWriting_sfx"), 0.f, 30.f, false);
			m_pGameInstance->Start_SFX_Delay(TEXT("DialogWriting_sfx"), 0.1f, 30.f, false);
			m_pGameInstance->Start_SFX_Delay(TEXT("DialogWriting_sfx"), 0.2f, 30.f, false);
			m_pGameInstance->Start_SFX_Delay(TEXT("DialogWriting_sfx"), 0.3f, 30.f, false);
			m_pGameInstance->Start_SFX_Delay(TEXT("DialogWriting_sfx"), 0.4f, 30.f, false);
		}
	}
	else
	{
		if (false == Get_Dialogue(m_tDialogId)[0].lines[m_iCurrentLineIndex].MTMSound)
		{
			if (m_iCurrentLineIndex != Get_Dialogue(m_tDialogId)[0].lines.size())
			{
				m_pGameInstance->Start_SFX_Delay(TEXT("DialogWriting_sfx"), 0.f, 30.f, false);
				m_pGameInstance->Start_SFX_Delay(TEXT("DialogWriting_sfx"), 0.1f, 30.f, false);
				m_pGameInstance->Start_SFX_Delay(TEXT("DialogWriting_sfx"), 0.2f, 30.f, false);
				m_pGameInstance->Start_SFX_Delay(TEXT("DialogWriting_sfx"), 0.3f, 30.f, false);
				m_pGameInstance->Start_SFX_Delay(TEXT("DialogWriting_sfx"), 0.4f, 30.f, false);
			}
		}
		else
		{
			if (m_iCurrentLineIndex != Get_Dialogue(m_tDialogId)[0].lines.size())
			{
				m_pGameInstance->Start_SFX_Delay(TEXT("A_sfx_minibeard_plastic-001"), 0.f, 30.f, false);
				m_pGameInstance->Start_SFX_Delay(TEXT("A_sfx_minibeard_plastic-001"), 0.11f, 30.f, false);
				m_pGameInstance->Start_SFX_Delay(TEXT("A_sfx_minibeard_plastic-001"), 0.22f, 30.f, false);
				m_pGameInstance->Start_SFX_Delay(TEXT("A_sfx_minibeard_plastic-001"), 0.33f, 30.f, false);
				m_pGameInstance->Start_SFX_Delay(TEXT("A_sfx_minibeard_plastic-001"), 0.44f, 30.f, false);
				m_pGameInstance->Start_SFX_Delay(TEXT("A_sfx_minibeard_plastic-001"), 0.55f, 30.f, false);
				m_pGameInstance->Start_SFX_Delay(TEXT("A_sfx_minibeard_plastic-001"), 0.66f, 30.f, false);
				m_pGameInstance->Start_SFX_Delay(TEXT("A_sfx_minibeard_plastic-001"), 0.77f, 30.f, false);
				m_pGameInstance->Start_SFX_Delay(TEXT("A_sfx_minibeard_plastic-001"), 0.88f, 30.f, false);
			}
		}
	}




	CPlayer* pPlayer = Uimgr->Get_Player();
	assert(pPlayer);

	pPlayer->Set_BlockPlayerInput(true);


	// 현재 섹션의 렌더 사이즈
	m_vRTSize = _RTSize;

	// 현재 렌더사이즈 / 2560 의 사이즈의 비율
	_float fScaleX = _RTSize.x / RTSIZE_BOOK2D_X;
	_float fScaleY = _RTSize.y / RTSIZE_BOOK2D_Y;

	_tchar _strDialogue[MAX_PATH] = {};
	wsprintf(_strDialogue, m_tDialogId);
	if (m_iCurrentLineIndex <= Get_Dialogue(_strDialogue)[0].lines.size())
	{
		// TODO :: 이거 제거 필요
		m_iCurrentLineIndex = m_iCurrentLineIndex;

		_float3 v3DPos = {};
		_float2 vPos = {};
		if (false == m_isRender)
		{
			m_isRender = true;
			m_isPortraitRender = Get_Dialogue(_strDialogue)[0].lines[m_iCurrentLineIndex].isPortrait;

		}

		// 다이얼로그 간 애니메이션 이동 //
		if (m_iCurrentLineIndex <= Get_Dialogue(_strDialogue)[0].lines.size() - 1)
		{
			// 애니메이션이 단일인 NPC 및 객체가 아닌 트리거, 이벤트 다이얼로그의 대한 안전 코드추가 //
			if (-1 != Get_Dialogue(_strDialogue)[0].lines[m_iCurrentLineIndex].iAnimationIndex)
			{
				CDialog_Manager::GetInstance()->Set_ActionChange(Get_Dialogue(_strDialogue)[0].lines[m_iCurrentLineIndex].iAnimationIndex);
			}
		}


		if (true == Get_Dialogue(m_tDialogId)[0].lines[m_iCurrentLineIndex].is2D)
		{
			m_pControllerTransform->Get_Transform(COORDINATE_2D)->Set_Scale(m_vDisplay2DSize.x, m_vDisplay2DSize.y, 1.f);
		}
		else if (false == Get_Dialogue(m_tDialogId)[0].lines[m_iCurrentLineIndex].is2D)
		{
			m_pControllerTransform->Get_Transform(COORDINATE_2D)->Set_Scale(m_vDisplay3DSize.x, m_vDisplay3DSize.y, 1.f);
		}


		_float2 vOffset = { 0.f, 0.f };

		if (m_iCurrentLineIndex <= Get_Dialogue(_strDialogue)[0].lines.size() - 1)
		{
			vOffset = _float2(Get_Dialogue(m_tDialogId)[0].lines[m_iCurrentLineIndex].vOffset.x, Get_Dialogue(m_tDialogId)[0].lines[m_iCurrentLineIndex].vOffset.y);


			// 250314 :: 박상욱
			if (true == Get_Dialogue(m_tDialogId)[0].isChangeTalker)
			{
				CGameObject* pGameObject = CDialog_Manager::GetInstance()->Get_DialogTarget(Get_Dialogue(m_tDialogId)[0].lines[m_iCurrentLineIndex].TerlkerIndex);
				_vector TargetPos = pGameObject->Get_ControllerTransform()->Get_State(COORDINATE_2D, CTransform::STATE_POSITION);

				m_vDialoguePos = _float4(TargetPos.m128_f32[0], TargetPos.m128_f32[1], 0.f, 1.f);
			}
			else
			{
				if (true == Get_Dialogue(m_tDialogId)[0].lines[m_iCurrentLineIndex].is2D)
				{
					if (1 <= CDialog_Manager::GetInstance()->Get_DialogTargetCount())
					{
						CGameObject* pGameObject = CDialog_Manager::GetInstance()->Get_DialogTarget(0);
						_vector TargetPos = pGameObject->Get_ControllerTransform()->Get_State(COORDINATE_2D, CTransform::STATE_POSITION);

						m_vDialoguePos = _float4(TargetPos.m128_f32[0], TargetPos.m128_f32[1], 0.f, 1.f);
					}
				}
				
			}


		}

		switch (Get_DialogueLine(_strDialogue, m_iCurrentLineIndex).location)
		{
		case LOC_MIDDOWN:
		{
			if (true == Get_Dialogue(m_tDialogId)[0].lines[m_iCurrentLineIndex].is2D)
			{
				if (!m_isAddSectionRender)
				{
					_tchar	strSectionID[MAX_PATH];
					wsprintf(strSectionID, m_strCurrentSection);

					if (TEXT("NOTWORD") == strSectionID)
					{
						CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this, SECTION_2D_PLAYMAP_UI);
					}
					else
					{
						wstring CurrentDialog(Get_Dialogue(m_tDialogId)[0].Section);
						CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(CurrentDialog, this, SECTION_2D_PLAYMAP_UI);
						//CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(strSectionID, this);
					}
					m_isAddSectionRender = true;
				}

				vPos.x = m_vDialoguePos.x;
				vPos.y = m_vDialoguePos.y;

				vPos.x += vOffset.x / fScaleX;
				vPos.y -= _RTSize.y * 0.13f / fScaleY + vOffset.y / fScaleY;
			}
			else if (false == Get_Dialogue(m_tDialogId)[0].lines[m_iCurrentLineIndex].is2D)
			{
				//if (true == m_isAddSectionRender)
				//{

					//if (TEXT("NOTWORD") == strSectionID)
					//{
					//	CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this, SECTION_2D_PLAYMAP_UI);
					//}
					//else
					//{
				m_strSectionName = L"";

				CSection_Manager::GetInstance()->Remove_GameObject_ToCurSectionLayer(this);
				Register_RenderGroup(RENDERGROUP::RG_3D, PRIORITY_3D::PR3D_UI);
				//}
			//	m_isAddSectionRender = false;
			//}

				vPos = _float2(g_iWinSizeX / 2.f, g_iWinSizeY - g_iWinSizeY / 8.f);
				m_vCalDialoguePos = _float3(vPos.x, vPos.y, 0.f);

				m_vCurPos = vPos;

				m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(vPos.x - g_iWinSizeX * 0.5f, -vPos.y + g_iWinSizeY * 0.5f, 0.f, 1.f));

				//m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(vPos.x, vPos.y, 0.f, 1.f));
				m_isFirstRefresh = true;


				return;
			}


		}
		break;

		case LOC_MIDHIGH:
		{
			if (!m_isAddSectionRender)
			{
				_tchar	strSectionID[MAX_PATH];
				wsprintf(strSectionID, m_strCurrentSection);

				if (TEXT("NOTWORD") == strSectionID)
				{


					CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this, SECTION_2D_PLAYMAP_UI);
				}
				else
				{
					wstring CurrentDialog(Get_Dialogue(m_tDialogId)[0].Section);
					CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(CurrentDialog, this, SECTION_2D_PLAYMAP_UI);
					//CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(strSectionID, this);
				}
				m_isAddSectionRender = true;
			}

			vPos.x = m_vDialoguePos.x;
			vPos.y = m_vDialoguePos.y;

			vPos.x += vOffset.x / fScaleX;
			vPos.y += _RTSize.y * 0.25f / fScaleY + vOffset.y / fScaleY;
		}
		break;

		case LOC_MIDLEFT:
		{
			if (!m_isAddSectionRender)
			{
				_tchar	strSectionID[MAX_PATH];
				wsprintf(strSectionID, m_strCurrentSection);

				if (TEXT("NOTWORD") == strSectionID)
				{
					CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this, SECTION_2D_PLAYMAP_UI);
				}
				else
				{
					wstring CurrentDialog(Get_Dialogue(m_tDialogId)[0].Section);
					CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(CurrentDialog, this, SECTION_2D_PLAYMAP_UI);
					//CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(strSectionID, this);
				}
				m_isAddSectionRender = true;
			}

			vPos.x = m_vDialoguePos.x;
			vPos.y = m_vDialoguePos.y;

			vPos.x -= _RTSize.x * 0.14f / fScaleX + vOffset.x / fScaleX;
			vPos.y += _RTSize.y * 0.05f / fScaleY + vOffset.y / fScaleY;
		}
		break;

		case LOC_MIDRIGHT:
		{
			if (!m_isAddSectionRender)
			{
				_tchar	strSectionID[MAX_PATH];
				wsprintf(strSectionID, m_strCurrentSection);

				if (TEXT("NOTWORD") == strSectionID)
				{
					CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this, SECTION_2D_PLAYMAP_UI);
				}
				else
				{
					wstring CurrentDialog(Get_Dialogue(m_tDialogId)[0].Section);
					CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(CurrentDialog, this, SECTION_2D_PLAYMAP_UI);
					//CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(strSectionID, this);
				}
				m_isAddSectionRender = true;
			}

			vPos.x = m_vDialoguePos.x;
			vPos.y = m_vDialoguePos.y;

			vPos.x += _RTSize.x * 0.165f / fScaleX + vOffset.x / fScaleX;
			vPos.y += _RTSize.y * 0.05f / fScaleY + vOffset.y / fScaleY;
		}
		break;

		case LOC_LEFTDOWN:
		{
			if (!m_isAddSectionRender)
			{
				_tchar	strSectionID[MAX_PATH];
				wsprintf(strSectionID, m_strCurrentSection);

				if (TEXT("NOTWORD") == strSectionID)
				{
					CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this, SECTION_2D_PLAYMAP_UI);
				}
				else
				{
					wstring CurrentDialog(Get_Dialogue(m_tDialogId)[0].Section);
					CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(CurrentDialog, this, SECTION_2D_PLAYMAP_UI);
					//CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(strSectionID, this);
				}
				m_isAddSectionRender = true;
			}

			vPos.x = m_vDialoguePos.x;
			vPos.y = m_vDialoguePos.y;

			vPos.x -= _RTSize.x * 0.165f / fScaleX + vOffset.x / fScaleX;
			vPos.y -= _RTSize.y * 0.14f / fScaleY + vOffset.y / fScaleY;
		}
		break;

		case LOC_LEFTHIGH:
		{
			if (!m_isAddSectionRender)
			{
				_tchar	strSectionID[MAX_PATH];
				wsprintf(strSectionID, m_strCurrentSection);

				if (TEXT("NOTWORD") == strSectionID)
				{
					CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this, SECTION_2D_PLAYMAP_UI);
				}
				else
				{
					wstring CurrentDialog(Get_Dialogue(m_tDialogId)[0].Section);
					CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(CurrentDialog, this, SECTION_2D_PLAYMAP_UI);
					//CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(strSectionID, this);
				}
				m_isAddSectionRender = true;
			}

			vPos.x = m_vDialoguePos.x;
			vPos.y = m_vDialoguePos.y;

			vPos.x -= _RTSize.x * 0.165f / fScaleX + vOffset.x / fScaleX;
			vPos.y += _RTSize.y * 0.19f / fScaleY + vOffset.y / fScaleY;
		}
		break;

		case LOC_RIGHTHIGH:
		{
			if (!m_isAddSectionRender)
			{
				_tchar	strSectionID[MAX_PATH];
				wsprintf(strSectionID, m_strCurrentSection);

				if (TEXT("NOTWORD") == strSectionID)
				{
					CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this, SECTION_2D_PLAYMAP_UI);
				}
				else
				{
					wstring CurrentDialog(Get_Dialogue(m_tDialogId)[0].Section);
					CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(CurrentDialog, this, SECTION_2D_PLAYMAP_UI);
					//CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(strSectionID, this);
				}
				m_isAddSectionRender = true;
			}

			vPos.x = m_vDialoguePos.x;
			vPos.y = m_vDialoguePos.y;

			vPos.x += _RTSize.x * 0.165f / fScaleX + vOffset.x / fScaleX;
			vPos.y += _RTSize.y * 0.19f / fScaleY + vOffset.y / fScaleY;
		}
		break;

		case LOC_RIGHDOWN:
		{
			if (!m_isAddSectionRender)
			{
				_tchar	strSectionID[MAX_PATH];
				wsprintf(strSectionID, m_strCurrentSection);

				if (TEXT("NOTWORD") == strSectionID)
				{
					CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this, SECTION_2D_PLAYMAP_UI);
				}
				else
				{
					wstring CurrentDialog(Get_Dialogue(m_tDialogId)[0].Section);
					CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(CurrentDialog, this, SECTION_2D_PLAYMAP_UI);
					//CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(strSectionID, this);
				}
				m_isAddSectionRender = true;
			}

			vPos.x = m_vDialoguePos.x;
			vPos.y = m_vDialoguePos.y;

			vPos.x += _RTSize.x * 0.165f / fScaleX + vOffset.x / fScaleX;
			vPos.y -= _RTSize.y * 0.14f / fScaleY + vOffset.y / fScaleY;
		}
		break;
		case LOC_VERYMIDHIGH:
		{
			if (!m_isAddSectionRender)
			{
				_tchar	strSectionID[MAX_PATH];
				wsprintf(strSectionID, m_strCurrentSection);


				if (TEXT("NOTWORD") == strSectionID)
				{
					CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this, SECTION_2D_PLAYMAP_UI);
				}
				else
				{
					wstring CurrentDialog(Get_Dialogue(m_tDialogId)[0].Section);
					CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(CurrentDialog, this, SECTION_2D_PLAYMAP_UI);
					//CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(strSectionID, this);
				}
				m_isAddSectionRender = true;
			}

			vPos.x = m_vDialoguePos.x;
			vPos.y = m_vDialoguePos.y;

			vPos.x += vOffset.x / fScaleX;
			vPos.y += _RTSize.y * 0.38f / fScaleY + vOffset.y / fScaleY;
		}
		break;
		}


		m_vCalDialoguePos = _float3(vPos.x, vPos.y, 0.f);
		m_vCurPos = vPos;

		m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(vPos.x, vPos.y, 0.f, 1.f));

	}
	m_isFirstRefresh = true;


}

void CDialog::isOpenPanel(_tchar* _DialogId)
{
	_tchar NpcName[MAX_PATH] = {};
	_tchar strSrcName[MAX_PATH] = {};
	wsprintf(NpcName, Get_Dialogue(_DialogId)[0].lines[0].Talker.c_str());

	// 상점용
	wsprintf(strSrcName, TEXT("마르티나"));
	if (0 == wcscmp(NpcName, strSrcName))
	{
		//상점 오픈하게
		//	bool 변수로


		// TODO :: 이거 끝나는거 어떻게 할지?
		Uimgr->Set_DialogueFinishShopPanel(true);
	}


}

// 프로토 타입 이후 수정 예정, 하드코딩
void CDialog::isCloseDialogueForTalket(_tchar* _DialogId)
{
	_tchar NpcName[MAX_PATH] = {};
	_tchar strSrcName[MAX_PATH] = {};
	wsprintf(NpcName, Get_Dialogue(_DialogId)[0].lines[0].Talker.c_str());

	wsprintf(strSrcName, TEXT("바이올렛"));

	if (0 == wcscmp(NpcName, strSrcName))
	{

	}
}

vector<CDialog::DialogData> CDialog::Get_Dialogue(const _wstring& _id)
{
	auto iter = find_if(m_DialogDatas.begin(), m_DialogDatas.end(),
		[&_id](const CDialog::DialogData& dialog)
		{
			return dialog.id == _id;
		});

	if (iter != m_DialogDatas.end())
	{
		return { *iter };
	}
	else
	{
		return {};
	}
}

CDialog::DialogLine CDialog::Get_DialogueLine(const _wstring& _id, _int _LineIndex)
{
	auto iter = find_if(m_DialogDatas.begin(), m_DialogDatas.end(), [&](const CDialog::DialogData& Data)
		{
			return Data.id == _id;
		});

	if (iter != m_DialogDatas.end())
	{
		if (_LineIndex < iter->lines.size())
		{
			return iter->lines[_LineIndex];
		}
	}

	return CDialog::DialogLine{};
}

_int CDialog::Get_CurrentLineIndex()
{
	if (false == m_isDisplayDialogue)
		return -1;

	return m_iCurrentLineIndex;
}







HRESULT CDialog::Ready_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_Model_2D"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_Texture_DialogueBG"),
		TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}


CDialog* CDialog::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CDialog* pInstance = new CDialog(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CDialog Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CDialog::Clone(void* _pArg)
{
	CDialog* pInstance = new CDialog(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CDialog Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CDialog::Free()
{

	//}
	Safe_Release(m_pPortrait);
	//CSection_Manager::GetInstance()->Remove_GameObject_ToCurSectionLayer(this);
	__super::Free();

}

HRESULT CDialog::Cleanup_DeadReferences()
{

	return S_OK;
}
