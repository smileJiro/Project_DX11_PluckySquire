#include "stdafx.h"
#include "Narration.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "Section_Manager.h"
#include "Trigger_Manager.h"

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



	//if (KEY_DOWN(KEY::B))
	//{
	//	// 이건 각 스테이지 마다 RTSIZE가 변경될 수 있다. 가변적으로 사용하여야한다.
	//	_float2 vRTSize = _float2(RTSIZE_BOOK2D_X, RTSIZE_BOOK2D_Y);
	//	NextDialogue(vRTSize); // 다음 다이얼로그의 위치를 변경한다.
	//
	//}

}

void CNarration::Late_Update(_float _fTimeDelta)
{
	//Register_RenderGroup(RENDERGROUP::RG_3D, PRIORITY_3D::PR3D_UI);

	//if (true == Uimgr->Get_DisplayDialogue() && COORDINATE_3D == Uimgr->Get_Player()->Get_CurCoord())
	//{
	//	Register_RenderGroup(RENDERGROUP::RG_3D, PRIORITY_3D::PR3D_UI);
	//}

}

HRESULT CNarration::Render()
{
	//if (true == Uimgr->Get_DisplayDialogue())
	//{
	//	wsprintf(m_tDialogIndex, Uimgr->Get_DialogId());
	//
	//
	//	if (0 != Uimgr->Get_Dialogue(m_tDialogIndex)[0].lines[Uimgr->Get_DialogueLineIndex()].BG)
	//	{
	//		__super::Render(Uimgr->Get_Dialogue(m_tDialogIndex)[0].lines[Uimgr->Get_DialogueLineIndex()].BG, PASS_VTXPOSTEX::UI_POINTSAMPLE);
	//	}
	//	else
	//	{
	//		_float4 vColor = _float4(Uimgr->Get_Dialogue(m_tDialogIndex)[0].lines[Uimgr->Get_DialogueLineIndex()].Red / 255.f,
	//								Uimgr->Get_Dialogue(m_tDialogIndex)[0].lines[Uimgr->Get_DialogueLineIndex()].Green / 255.f,
	//								Uimgr->Get_Dialogue(m_tDialogIndex)[0].lines[Uimgr->Get_DialogueLineIndex()].Blue / 255.f,
	//								 1.f);
	//
	//		if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_RawValue("g_vColors", &vColor, sizeof(_float4))))
	//			return E_FAIL;
	//
	//		__super::Render(Uimgr->Get_Dialogue(m_tDialogIndex)[0].lines[Uimgr->Get_DialogueLineIndex()].BG, PASS_VTXPOSTEX::DIALOGUE_BG_COLOR);
	//	}
	//
	//
	//
	//	
	//
	//	if (Uimgr->Get_DialogueLineIndex() < Uimgr->Get_Dialogue(m_tDialogIndex)[0].lines.size())
	//	{
	//		// TODO :: 나중에 바꿔야함, 해당 값은 가변적이다.
	//		_float2 vRTSize = _float2(RTSIZE_BOOK2D_X, RTSIZE_BOOK2D_Y);
	//
	//		DisplayText(vRTSize);
	//	}
	//}
		

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
	//if (Uimgr->Get_DialogueLineIndex() >= Uimgr->Get_Dialogue(m_tDialogIndex)[0].lines.size())
	//	return E_FAIL;
	//
	//CGameInstance* pGameInstance = CGameInstance::GetInstance();
	//Safe_AddRef(pGameInstance);
	//
	//_float3 vTextPos3D = { 0.f, 0.f, 0.f };
	//_float3 vTextPos2D = { 0.f, 0.f, 0.f };
	//
	//if (COORDINATE_3D == Uimgr->Get_Player()->Get_CurCoord())
	//{
	//	vTextPos3D = Uimgr->Get_CalDialoguePos();
	//}
	//else if (COORDINATE_2D == Uimgr->Get_Player()->Get_CurCoord())
	//{
	//	vTextPos2D = Uimgr->Get_CalDialoguePos();
	//}
	//
	//const auto& currentLine = Uimgr->Get_DialogueLine(m_tDialogIndex, Uimgr->Get_DialogueLineIndex());
	//
	//static _wstring strDisplaytext;
	//static _float fWaitTime = 0.0f;
	//static _int iPreviousLineIndex = -1; 
	//
	//// 라인이 변경되었을 때 초기화
	//if (iPreviousLineIndex != Uimgr->Get_DialogueLineIndex())
	//{
	//	strDisplaytext.clear();
	//	fWaitTime = 0.0f;
	//	iPreviousLineIndex = Uimgr->Get_DialogueLineIndex();
	//	
	//}
	//
	//// 하나씩 출력되게 계산
	//_float fSpeed = currentLine.animation.speed / 1000.0f;
	//_int iFullWord = static_cast<int>(currentLine.text.length());
	//
	//// 노출 시킬 글자의 수
	//_int icurrentLength = static_cast<int>(strDisplaytext.length());
	//
	//if (icurrentLength < iFullWord)
	//{
	//	fWaitTime += fSpeed;
	//	if (fWaitTime >= 1.0f)
	//	{
	//		// 현재 대화의 총 글자의 배열에서 위에서 계산 노출 시킬 글자의 수만큼 이동후 그 텍스트를 더해주자.
	//		strDisplaytext += currentLine.text[icurrentLength];
	//
	//		// 글자 노출 대기 시간 초기화~
	//		fWaitTime = 0.0f;
	//	}
	//}
	//
	//// 2D 기준
	//_float2 vCalPos = { 0.f, 0.f };
	//// 중점
	//_float2 vMidPoint = { _vRTSize.x / 2.f, _vRTSize.y / 2.f };
	//
	//
	//vCalPos.x = vMidPoint.x + vTextPos2D.x;
	//vCalPos.y = vMidPoint.y - vTextPos2D.y;
	//
	//
	//// 대상 이름 출력
	//wsprintf(m_tFont, currentLine.Talker.c_str());
	//pGameInstance->Render_Font(TEXT("Font20"), m_tFont, vCalPos, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	//
	//
	//vCalPos.x += _vRTSize.x * 0.03f;
	//vCalPos.y -= +_vRTSize.y * 0.1f;
	//
	//// 대화 내용 출력
	//wsprintf(m_tFont, strDisplaytext.c_str());
	//pGameInstance->Render_Font(TEXT("Font24"), m_tFont, _float2(vCalPos.x - 120.f, vCalPos.y + 120.f), XMVectorSet(0.f, 0.f, 0.f, 1.f));
	//
	//Safe_Release(pGameInstance);
	//
	//return S_OK;
	//


	return S_OK;

}

// 다음 다이얼로그에서 Json->Location에 따른 위치 변경
void CNarration::NextDialogue(_float2 _RTSize)
{
	//_tchar _strDialogue[MAX_PATH] = {};
	//wsprintf(_strDialogue, Uimgr->Get_DialogId());
	//
	//
	//// 2D 기준
	//if (Uimgr->Get_DialogueLineIndex() <= Uimgr->Get_Dialogue(_strDialogue)[0].lines.size())
	//{
	//	Uimgr->Set_DialogueLineIndex(Uimgr->Get_DialogueLineIndex() + 1);
	//
	//	_float2 vPos = {};
	//	if (false == m_isRender)
	//	{
	//		m_isRender = true;
	//	}
	//	
	//	Uimgr->Set_CalDialoguePos(_float3(vPos.x, vPos.y, 0.f));
	//	//m_vCurPos = vPos;
	//
	//	//vPos.x = vPos.x + RTSIZE_BOOK2D_X * 0.5f;
	//	//vPos.y = -vPos.y + RTSIZE_BOOK2D_Y * 0.5f;
	//	m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(vPos.x, vPos.y, 0.f, 1.f));
	//
	//	if (Uimgr->Get_DialogueLineIndex() == Uimgr->Get_Dialogue(_strDialogue)[0].lines.size())
	//	{
	//		// 다음 대사가 없으므로 트리거를 종료 시킨다.
	//	}
	//}
}

// 처음 다이얼로그 입장 시 위치 계산해주는 함



HRESULT CNarration::Ready_Components()
{
	//if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
	//	TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderComs[COORDINATE_2D]))))
	//	return E_FAIL;
	//
	///* Com_VIBuffer */
	//if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
	//	TEXT("Com_Model_2D"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
	//	return E_FAIL;
	//
	///* Com_Texture */
	//if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_Texture_DialogueBG"),
	//	TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	//	return E_FAIL;

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

	//}

	//CSection_Manager::GetInstance()->Remove_GameObject_ToCurSectionLayer(this);
	__super::Free();
	
}

HRESULT CNarration::Cleanup_DeadReferences()
{

	return S_OK;
}
