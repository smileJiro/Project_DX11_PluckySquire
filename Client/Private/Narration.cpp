#pragma once
#include "stdafx.h"
#include "Narration.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "Section_Manager.h"
#include "Trigger_Manager.h"
#include "CustomFont.h"


CNarration::CNarration(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CUI (_pDevice, _pContext)
{
}

CNarration::CNarration(const CNarration& _Prototype)
	: CUI ( _Prototype )
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
	
	_float2 vCalScale = { 0.f, 0.f };
	vCalScale.x = m_vOriginSize.x * RATIO_BOOK2D_X;
	vCalScale.y = m_vOriginSize.y * RATIO_BOOK2D_Y;
	m_isRender = false;


	CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this, SECTION_2D_PLAYMAP_UI);

	return S_OK;
}


void CNarration::Update(_float _fTimeDelta)
{

	if (KEY_DOWN(KEY::J))
	{
		m_isStartNarration = true;
	}


	if (true == m_isStartNarration)
	{

		if (m_fTextAlpha < 1.f)
		{
			m_fFadeTimer += _fTimeDelta;

			// 텍스트 알파 = 타이머 / 현재 나레이션의 현재 라인
			//m_fTextAlpha = min(m_fFadeTimer / m_fFadeDuration, 1.f);
			 
			// TODO :: 박상욱
			m_fTextAlpha = min(m_fFadeTimer / m_NarrationDatas[m_iNarrationCount].lines[m_iCurrentLine].fFadeDuration, 1.f);


			if (!m_bAnimationStarted && m_fTextAlpha > 0.f)
			{
				m_bAnimationStarted = true;
				m_isFade = true;

				for (int i = 0; i < m_pCurrentAnimObj.size(); ++i)
				{
					if (nullptr == m_pCurrentAnimObj[i])
					{
						GetAnimationObjectForLine(m_iCurrentLine);
					}

					if (nullptr != m_pCurrentAnimObj[i])
					{
						m_pCurrentAnimObj[i]->StartAnimation();
					}
				}
			}
		}
		else
		{
			// fadein 완료 후, 다음 라인으로 전환하기 위한 대기 타이머 갱신

			if (m_iCurrentLine == m_NarrationDatas[m_iNarrationCount].lines.size() - 1 && m_fTextAlpha >= 1.f)
			{
				m_isFade = false;
				//m_fTextAlpha = 0.f;
			}

			m_fDelayTimer += _fTimeDelta;
			//if (m_fDelayTimer >= m_fDelayBetweenLines)
			//{

			// TODO :: 박상욱

			if (m_fDelayTimer >= m_NarrationDatas[m_iNarrationCount].lines[m_iCurrentLine].fFadeDuration)
			{
				// 다음 라인이 있다면 전환, 이전 라인은 그대로 1.0을 유지
				if (m_iCurrentLine + 1 < m_NarrationDatas[m_iNarrationCount].lines.size())
				{
					m_iCurrentLine++; // 다음 라인으로 전환
					// fadein 관련 타이머 초기화
					m_fFadeTimer = 0.f;
					m_fDelayTimer = 0.f;
					m_fTextAlpha = 0.f;
					m_bAnimationStarted = false;

					//m_pCurrentAnimObj = GetAnimationObjectForLine(m_iCurrentLine);
					GetAnimationObjectForLine(m_iCurrentLine);
				}

				if (m_iCurrentLine == m_NarrationDatas[m_iNarrationCount].lines.size() - 1)
				{
					if (true == m_isFade)
					{
						return;
					}

					_float3 vPos = _float3(0.f, 0.f, 1.f);

					if (m_iNarrationCount < m_NarrationDatas.size() - 1 && false == m_isFade)
					{
						++m_iNarrationCount;

						m_strSectionName = m_NarrationDatas[m_iNarrationCount].strSectionid;
						CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(m_NarrationDatas[m_iNarrationCount].strSectionid, this);

						m_iCurrentLine = 0; // 다음 라인으로 전환
						// fade?in 관련 타이머 초기화
						m_fFadeTimer = 0.f;
						m_fDelayTimer = 0.f;
						m_fTextAlpha = 0.f;
						m_bAnimationStarted = false;
						m_isNarrationEnd = true;

						//m_pCurrentAnimObj = GetAnimationObjectForLine(m_iCurrentLine);
						GetAnimationObjectForLine(m_iCurrentLine);

						Event_Book_Main_Section_Change_Start(1, &vPos);

						//CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this, SECTION_2D_PLAYMAP_UI);	

					}
					else if (m_iCurrentLine == m_NarrationDatas[m_iNarrationCount - 1].lines.size() - 1 && true == m_isNarrationEnd)
					{
						vPos = _float3(-692.f, -49.f, 1.f);
						Event_Book_Main_Section_Change_Start(1, &vPos);
						m_isNarrationEnd = false;
					}
				}
			}
		}
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


	DisplayText(_float2(RTSIZE_BOOK2D_X, RTSIZE_BOOK2D_Y));
	return S_OK;
}

HRESULT CNarration::LoadFromJson(const std::wstring& filePath)
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
		for (auto& Nar : doc["Narration"])
		{
			NarrationData NarData;

			if (Nar.contains("strSectionid") && Nar["strSectionid"].is_string())
				NarData.strSectionid = StringToWstring(Nar["strSectionid"].get<string>());

			if (Nar.contains("strid") && Nar["strid"].is_string())
				NarData.strid = StringToWstring(Nar["strid"].get<string>());

			if (Nar.contains("CurlevelId") && Nar["CurlevelId"].is_number_integer())
				NarData.eCurlevelId = (LEVEL_ID)Nar["CurlevelId"].get<_int>();

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

					if (line.contains("isLeft") && line["isLeft"].is_boolean())
						DialogueData.isLeft = line["isLeft"].get<bool>();

					if (line.contains("fscale") && line["fscale"].is_number_float())
						DialogueData.fscale = line["fscale"].get<float>();

					if (line.contains("isLeft") && line["isLeft"].is_boolean())
					{
						DialogueData.isLeft = line["isLeft"].get<_bool>();
					}

					if (line.contains("fposX") && line["fposX"].is_number_float())
						DialogueData.fpos.x = line["fposX"].get<float>();

					if (line.contains("fposY") && line["fposY"].is_number_float())
						DialogueData.fpos.y = line["fposY"].get<float>();

					if (line.contains("fwaitingTime") && line["fwaitingTime"].is_number_float())
						DialogueData.fwaitingTime = line["fwaitingTime"].get<float>();
					/////////////////////////////////////////////////////////////////////////////////
					if (line.contains("fLineHeight") && line["fLineHeight"].is_number_float())
						DialogueData.fLineHieght = line["fLineHeight"].get<float>();
					
					if (line.contains("fFadeDuration") && line["fFadeDuration"].is_number_float())
						DialogueData.fFadeDuration = line["fFadeDuration"].get<float>();
					
					if (line.contains("fDelayNextLine") && line["fDelayNextLine"].is_number_float())
						DialogueData.fDelayNextLine = line["fDelayNextLine"].get<float>();



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
								Animation.isLoop = Anim["isLoop"].get<_bool>();

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

							if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(tempData.eCurlevelId, TEXT("Prototype_GameObject_Narration_Anim"), tempData.eCurlevelId, TEXT("Layet_UI"), &pObject, &tempData)))
								return E_FAIL;

							// 생성한 애니메이션 객체 넣기
							CNarration_Anim* pAnim;
							pAnim = static_cast<CNarration_Anim*>(pObject);
							pAnimation.push_back(pAnim);

							// 원본 DialogueData에도 해당 애니메이션 데이터를 저장(필요에 따라 사용)
							DialogueData.NarAnim.push_back(Animation);
						}						
					}
					// 애니메이션 처리 후, 완성된 대화 데이터를 NarData에 추가한다.
					m_vAnimObjectsByLine.emplace(iLine, pAnimation);
					NarData.lines.push_back(DialogueData);
					++iLine;
				} // for each line
				
			}

			m_NarrationDatas.push_back(NarData);	
		}
	}

	return S_OK;
}

// 폰트 렌더 해주는 역할
HRESULT CNarration::DisplayText(_float2 _vRTSize)
{
	if (m_NarrationDatas.empty() || m_NarrationDatas[m_iNarrationCount].lines.empty())
		return S_OK;

	// 예를 들어, 첫 번째 NarrationData의 텍스트만 사용한다고 가정
	auto& lines = m_NarrationDatas[m_iNarrationCount].lines;

	// 0번부터 m_iCurrentLine까지의 모든 라인을 렌더링 (이미 fade?in이 완료된 라인은 alpha = 1.0)
	for (int i = 0; i <= m_iCurrentLine && i < lines.size(); i++)
	{
		float alpha = (i < m_iCurrentLine) ? 1.f : m_fTextAlpha; // 이전 라인은 완전 불투명, 현재 라인은 fade-in 중
		NarrationDialogData& dialogue = lines[i];

		// 해당 라인의 시작 위치 (각 라인이 서로 다른 위치로 지정되어 있다고 가정)
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

				if (true == dialogue.isLeft)
				{
					fx = dialogue.fpos.x; // 시작 x 좌표로 복원
					fy += m_NarrationDatas[m_iNarrationCount].lines[m_iCurrentLine].fLineHieght;  // y 좌표 증가
				}
				else if (false == dialogue.isLeft)
				{
					fx = dialogue.fpos.x + _vRTSize.x / 2.f; // 시작 x 좌표로 복원
					fy += m_NarrationDatas[m_iNarrationCount].lines[m_iCurrentLine].fLineHieght;  // y 좌표 증가
				}
				continue;
			}


			// 토큰의 크기를 측정하고 텍스트 렌더링 (알파값 적용)
			_vector vecSize = m_pGameInstance->Measuring(TEXT("Font40"), token.strText.c_str());
			_float2 vTextSize = { 0.f, 0.f };
			XMStoreFloat2(&vTextSize, vecSize);

			m_pGameInstance->Render_Scaling_Font(TEXT("Font40"), token.strText.c_str(), _float2(fx, fy), XMVectorSet(0.f, 0.f, 0.f, alpha) // alpha 적용
				,0.f, _float2(0.f, 0.f), token.fScale);

			fx += vTextSize.x * token.fScale * 0.98f;
		}
	}

	return S_OK;
}




// 다음 다이얼로그에서 Json->Location에 따른 위치 변경
void CNarration::NextDialogue(_float2 _RTSize)
{
	
}

vector<CNarration_Anim*> CNarration::GetAnimationObjectForLine(const _uint iLine, _int AnimCount)
{
	//if (iLine < 0 || iLine >= static_cast<int>(m_vAnimObjectsByLine.size()))
	//	return nullptr;
	vector<CNarration_Anim*> vecCopy;
	if (!m_pCurrentAnimObj.empty())
	{
		for (auto& iter : m_pCurrentAnimObj)
		{
			Safe_Release(iter);
		}

		m_pCurrentAnimObj.clear();
	}

	auto iter = m_vAnimObjectsByLine.find(iLine);

	if (iter != m_vAnimObjectsByLine.end())
	{

			m_pCurrentAnimObj = iter->second;
		
			for (_int i = 0 ; i < iter->second.size(); ++i)
			{
				Safe_AddRef(iter->second[i]);
			}


	}

	//for (const auto& it : vecCopy)
	//{
	//	m_pCurrentAnimObj.push_back(it);
	//}




	return m_pCurrentAnimObj;
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

