#include "stdafx.h"
#include "Dialogue.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "Section_Manager.h"

CDialog::CDialog(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CUI (_pDevice, _pContext)
{
}

CDialog::CDialog(const CDialog& _Prototype)
	: CUI ( _Prototype )
{
}

HRESULT CDialog::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDialog::Initialize(void* _pArg)
{
	UIOBJDESC* pDesc = static_cast<UIOBJDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;



	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(LoadFromJson(TEXT("../Bin/Resources/Dialogue/dialog_data.json"))))
		return E_FAIL;

	m_isRender = true;

	CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this);

	return S_OK;
}


void CDialog::Update(_float _fTimeDelta)
{
	if (KEY_DOWN(KEY::B))
	{
		if (Uimgr->Get_DialogueLineIndex() <= Uimgr->Get_Dialogue(TEXT("dialog_01"))[0].lines.size())
		{
			Uimgr->Set_DialogueLineIndex(Uimgr->Get_DialogueLineIndex() + 1);

			_float2 vPos = {};

			//vPos.x = m_pControllerTransform->Get_State(CTransform::STATE_POSITION).m128_f32[0];
			//vPos.y = m_pControllerTransform->Get_State(CTransform::STATE_POSITION).m128_f32[1];
			
			//if (COORDINATE::COORDINATE_2D == Uimgr->Get_Player()->Get_CurCoord())
			//{
				switch (Uimgr->Get_DialogueLine(TEXT("dialog_01"), Uimgr->Get_DialogueLineIndex()).location)
				{
				case LOC_DEFAULT:
				{
					vPos.x = RTSIZE_BOOK2D_X / 10.f;
					vPos.y = RTSIZE_BOOK2D_Y / 10.f;
				}
				break;

				case LOC_MIDDLE:
				{

				}
				break;

				case LOC_MIDLEFT:
				{

				}
				break;
				}
			//}
			//else if (COORDINATE::COORDINATE_3D == Uimgr->Get_Player()->Get_CurCoord())
			//{
			//
			//}
			

			//vPos.x = vPos.x + g_iWinSizeX * 0.5f;
			//vPos.y = -vPos.y + g_iWinSizeY * 0.5f;
			m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(vPos.x, vPos.y, 0.f, 1.f));

			if (Uimgr->Get_DialogueLineIndex() == Uimgr->Get_Dialogue(TEXT("dialog_01"))[0].lines.size())
			{
				m_isRender = false;
				Uimgr->Set_PortraitRender(false);
			}
		}
	}


	// 다이얼로그 변경 시 이용 스위치이나 뭘듯 해야할듯
	Uimgr->Set_DialogId(TEXT("dialog_01"));
	wsprintf(m_tDialogIndex, Uimgr->Get_DialogId());


}

void CDialog::Late_Update(_float _fTimeDelta)
{

}

HRESULT CDialog::Render()
{
	if (true == m_isRender)
	{
		__super::Render(Uimgr->Get_Dialogue(m_tDialogIndex)[0].lines[Uimgr->Get_DialogueLineIndex()].BG, PASS_VTXPOSTEX::UI_POINTSAMPLE);

		if (Uimgr->Get_DialogueLineIndex() < Uimgr->Get_Dialogue(m_tDialogIndex)[0].lines.size())
		{
			DisplayText();
		}
	}
		

	return S_OK;
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
						
					// 다이얼로그 위치 enum 참조
					if (line.contains("location") && line["location"].is_number_integer())
					{
						dialogLine.location = static_cast<LOC>(line["location"].get<int>());
					}
						
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
			Uimgr->Pushback_Dialogue(dialogData);
		}
	}
	return S_OK;
}

HRESULT CDialog::DisplayText()
{
	if (Uimgr->Get_DialogueLineIndex() >= Uimgr->Get_Dialogue(m_tDialogIndex)[0].lines.size())
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	_float2 vTextPos = _float2(0.f, 0.f);



	// 현재 대화의 절대 글자 수
	//const auto& currentLine = Uimgr->Get_Dialogue(TEXT("dialog_01"))[0].lines[m_iCurrentLineIndex];

	const auto& currentLine = Uimgr->Get_DialogueLine(m_tDialogIndex, Uimgr->Get_DialogueLineIndex());

	static _wstring strDisplaytext;
	static _float fWaitTime = 0.0f;
	static _int iPreviousLineIndex = -1; 

	// 라인이 변경되었을 때 초기화
	if (iPreviousLineIndex != Uimgr->Get_DialogueLineIndex())
	{
		strDisplaytext.clear();
		fWaitTime = 0.0f;
		iPreviousLineIndex = Uimgr->Get_DialogueLineIndex();
		
	}

	// 하나씩 출력되게 계산
	_float fSpeed = currentLine.animation.speed / 1000.0f;
	_int iFullWord = static_cast<int>(currentLine.text.length());

	// 노출 시킬 글자의 수
	_int icurrentLength = static_cast<int>(strDisplaytext.length());

	


	// 여기서부터 어떻게노출을 시킬것인가?
	// 
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
	case LOC_DEFAULT:  // 가운데 아래
	{	
		// 3D rlwns
		//vTextPos = _float2(g_iWinSizeX / 4.1f, g_iWinSizeY - g_iWinSizeY / 3.25f);


		//vTextPos = _float2(g_iWinSizeX / 1.25f, g_iWinSizeY  * 1.77f);
		vTextPos = _float2(g_iWinSizeX / 1.25f, g_iWinSizeY * 1.77f);
	}
	break;

	case LOC_MIDDLE:   // 정 가운데
	{
		vTextPos = _float2(g_iWinSizeX / 2.f, g_iWinSizeY / 2.f);
	}
	break;

	case LOC_MIDLEFT:  // 가운데 좌측
	{
		vTextPos = _float2(g_iWinSizeX / 2.f, g_iWinSizeY / 2.f);
	}
	break;

	case LOC_MIDRIGHT: // 가운데 우측
	{
		vTextPos = _float2(g_iWinSizeX / 2.f, g_iWinSizeY / 2.f);
	}
	break;
	}

	// 2D 기준



	// 대상 이름 출력
	wsprintf(m_tFont, currentLine.Talker.c_str());
	pGameInstance->Render_Font(TEXT("Font54"), m_tFont, vTextPos, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	
	// 대화 내용 출력
	wsprintf(m_tFont, strDisplaytext.c_str());
	pGameInstance->Render_Font(TEXT("Font40"), m_tFont, _float2(vTextPos.x - 120.f, vTextPos.y + 120.f), XMVectorSet(0.f, 0.f, 0.f, 1.f));
	
	Safe_Release(pGameInstance);

	return S_OK;





}





HRESULT CDialog::Ready_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderComs[COORDINATE_2D]))))
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

	//CSection_Manager::GetInstance()->Remove_GameObject_ToCurSectionLayer(this);
	__super::Free();
	
}

HRESULT CDialog::Cleanup_DeadReferences()
{

	return S_OK;
}
