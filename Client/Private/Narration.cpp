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
	
}

void CNarration::Late_Update(_float _fTimeDelta)
{

}

HRESULT CNarration::Render()
{
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
			{
				NarData.strSectionid = StringToWstring(Nar["strSectionid"].get<string>());
			}

			if (Nar.contains("strid") && Nar["strid"].is_string())
			{
				NarData.strid = StringToWstring(Nar["strid"].get<string>());
			}

			if (Nar.contains("CurlevelId") && Nar["CurlevelId"].is_number_integer())
			{
				NarData.eCurlevelId = (LEVEL_ID)Nar["CurlevelId"].get<_int>();
			}

			if (Nar.contains("lines") && Nar["lines"].is_array())
			{
				for (auto& line : Nar["lines"])
				{
					NarrationDialogData DialogueData;

					if (line.contains("iduration") && line["iduration"].is_number_integer())
					{
						DialogueData.iduration = line["iduration"].get<_int>();
					}

					if (line.contains("ispeed") && line["ispeed"].is_number_integer())
					{
						DialogueData.ispeed = line["ispeed"].get<_int>();
					}

					if (line.contains("strtext") && line["strtext"].is_string())
					{
						DialogueData.strtext = StringToWstring(line["strtext"].get<_string>());
					}

					if (line.contains("fscale") && line["fscale"].is_number_float())
					{
						DialogueData.fscale = line["fscale"].get<_float>();
					}

					if (line.contains("fposX") && line["fposX"].is_number_float())
					{
						DialogueData.fpos.x = line["fposX"].get<_float>();
					}

					if (line.contains("fposY") && line["fposY"].is_number_float())
					{
						DialogueData.fpos.y = line["fposY"].get<_float>();
					}

					if (line.contains("fwaitingTime") && line["fwaitingTime"].is_number_float())
					{
						DialogueData.fwaitingTime = line["fwaitingTime"].get<_float>();
					}

					// 나레이션 관련 데이터 푸쉬백
					//NarData.lines.push_back(DialogueData);

					// 애니메이션이 있는가 없는가
					if (line.contains("NarAnim") && line["NarAnim"].is_array())
					{
						_int iLine = { 0 };

						for (auto& Anim : line["NarAnim"])
						{
							NarrationAnimation Animation;

							if (Anim.contains("strAnimationid") && Anim["strAnimationid"].is_string())
							{
								_tchar strCheck[MAX_PATH];
								wsprintf(strCheck, TEXT("NULL"));
								
								if (strCheck == StringToWstring(Anim["strAnimationid"].get<_string>()))
								{
									// 애니메이션이 없으면 NULL로 적는다. 만약 NULL로 되어있으면 끝내버린다.
									continue;
								}

								Animation.strAnimationid = StringToWstring(Anim["strAnimationid"].get<_string>());
							}

							if (Anim.contains("strSectionid") && Anim["strSectionid"].is_string())
							{
								Animation.strSectionid = StringToWstring(Anim["strSectionid"].get<_string>());
							}

							if (Anim.contains("fPosX") && Anim["fPosX"].is_number_float())
							{
								Animation.vPos.x = Anim["fPosX"].get<_float>();
							}

							if (Anim.contains("fPosY") && Anim["fPosY"].is_number_float())
							{
								Animation.vPos.y = Anim["fPosY"].get<_float>();
							}

							if (Anim.contains("fwaitingTime") && Anim["fwaitingTime"].is_number_float())
							{
								Animation.fWaitingTime = Anim["fwaitingTime"].get<_float>();
							}

							if (Anim.contains("vAnimationScaleX") && Anim["vAnimationScaleX"].is_number_float())
							{
								Animation.vAnimationScale.x = Anim["vAnimationScaleX"].get<_float>();
							}

							if (Anim.contains("vAnimationScaleY") && Anim["vAnimationScaleY"].is_number_float())
							{
								Animation.vAnimationScale.y = Anim["vAnimationScaleY"].get<_float>();
							}


							NarData.LineCount = iLine;
							++iLine;
							DialogueData.NarAnim.push_back(Animation);

							NarData.lines.push_back(DialogueData);


							if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(NarData.eCurlevelId, TEXT("Prototype_GameObject_Narration_Anim"), NarData.eCurlevelId, TEXT("Layet_UI"), &NarData)))
								return E_FAIL;

							m_NarrationDatas.push_back(NarData);

							// TODO :: 여기서 이미지를 생성 시킨다.


						}
					}
				}
			}
		}
	}

	return S_OK;
}

// 폰트 렌더 해주는 역할
HRESULT CNarration::DisplayText(_float2 _vRTSize)
{
	// 처음 위치
	float fx = m_NarrationDatas[0].lines[0].fpos.x;
	float fy = m_NarrationDatas[0].lines[0].fpos.y;


	// 들어오는 텍스트
	//_wstring strDisplayText = TEXT("{150}옛{100}날 옛적에#{125}저트{100}라는#{110}용감한 견습 기사{100}가#있었습니다.");

	_wstring strDisplayText = m_NarrationDatas[0].lines[0].strtext;

	// 오직 글자만 노출 (특수기호 제거)
	_wstring FullDisPlayText = strDisplayText;

	FullDisPlayText.erase(remove_if(FullDisPlayText.begin(), FullDisPlayText.end(), [](_tchar ch)
		{
			return ch == L'{' || ch == L'}' || ch == L'#' || ch == L'@' || ch == L'0' || ch == L'1' || ch == L'2' || ch == L'3' || ch == L'4' || ch == L'5' || ch == L'6' || ch == L'7' || ch == L'8' || ch == L'9';
		}), FullDisPlayText.end());


	vector<TextTokens> tokens;

	// 각각의 글자의 스케일 및 글자를 가져온다.
	PaseTokens(strDisplayText, tokens);
	_int	iRemainLine = { 0 };

	for (int i = 0; i < tokens.size(); ++i)
	{
		if (tokens[i].strText == L"\n")
		{
			++iRemainLine;
		}
	}

	for (const auto& token : tokens)
	{
		if (token.strText == L"\n")
		{
			fx = m_NarrationDatas[0].lines[0].fpos.x;
			fy += m_fLineHeight;

			if (iRemainLine > 0)
			{
				iRemainLine--;
			}
		}



		int tokenLength = static_cast<int>(token.strText.length());

		_float CalY = { 0.f };
		
		if (iRemainLine > tokenLength)
		{

			_float2 vTextSize = { 0.f, 0.f };
			_vector vecSize = m_pGameInstance->Measuring(TEXT("Font40"), token.strText.c_str());
			XMStoreFloat2(&vTextSize, vecSize);

			if (1.f < token.fScale)
			{
				CalY -= vTextSize.y / 2.f;
			}
			
			m_pGameInstance->Render_Scaling_Font(TEXT("Font40"), token.strText.c_str(), _float2(fx, fy + CalY), XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f, _float2(0.f, 0.f), token.fScale);

			//_float2 vTextSize = { 0.f, 0.f };
			//
			//_vector vecSize = m_pGameInstance->Measuring(TEXT("Font40"), token.strText.c_str());
			//XMStoreFloat2(&vTextSize, vecSize);
			fx += vTextSize.x * token.fScale * 0.98f;
			
		}
		else
		{
			_wstring strPartial = token.strText.substr(0, iRemainLine);
			CGameInstance* pGameInstance = CGameInstance::GetInstance();


			_vector vecSize = m_pGameInstance->Measuring(TEXT("Font40"), token.strText.c_str());
			_float2 vTextSize = { 0.f, 0.f };

			XMStoreFloat2(&vTextSize, vecSize);
			
			if (1.f < token.fScale)
			{
				CalY -= vTextSize.y / 10.f;
			}




			pGameInstance->Render_Scaling_Font(TEXT("Font40"), token.strText.c_str(), _float2(fx, fy + CalY), XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f, _float2(0, 0), token.fScale);

			fx += vTextSize.x * token.fScale * 0.98f;
			
		}
		//break;

	}

	return S_OK;

}




// 다음 다이얼로그에서 Json->Location에 따른 위치 변경
void CNarration::NextDialogue(_float2 _RTSize)
{
	
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

void CNarration::DrawText(SpriteBatch* spriteBatch, SpriteFont* spriteFont, const _wstring& text, float fscale, float& fX, float fY)
{
	//CGameInstance* pGameInstance = CGameInstance::GetInstance();
	//// 현재 위치를 기준으로 텍스트를 그린다.
	//_float2 vPos = { 0.f, 0.f };
	//pGameInstance->Render_Scaling_Font(TEXT("Font35"), text.c_str(), vPos, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f,
	//	_float2(0, 0), fscale);
	//
	//
	//// 그려진 텍스트의 가로 길이를 측정하여 x 좌표를 이동
	//_vector sizeVec = spriteFont->MeasureString(text.c_str());
	//_float2 textSize;
	//XMStoreFloat2(&textSize, sizeVec);
	//fX += textSize.x * fscale;
}
