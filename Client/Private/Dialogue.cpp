#include "stdafx.h"
#include "Dialogue.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "Section_Manager.h"
#include "Section_2D.h"

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


	_float2 vCalScale = { 0.f, 0.f };
	vCalScale.x = m_vOriginSize.x * RATIO_BOOK2D_X;
	vCalScale.y = m_vOriginSize.y * RATIO_BOOK2D_Y;

	m_pControllerTransform->Set_Scale(vCalScale.x, vCalScale.y, 1.f);

	m_isRender = false;

	CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this, CSection_2D::SECTION_2D_UI);
	return S_OK;
}


void CDialog::Update(_float _fTimeDelta)
{



	if (KEY_DOWN(KEY::B))
	{
		// 이건 각 스테이지 마다 RTSIZE가 변경될 수 있다. 가변적으로 사용하여야한다.
		_float2 vRTSize = _float2(RTSIZE_BOOK2D_X, RTSIZE_BOOK2D_Y);
		NextDialogue(vRTSize); // 다음 다이얼로그의 위치를 변경한다.
	}


	// 다이얼로그 변경 시 이용 스위치이나 뭘듯 해야할듯
	//if (true == m_isRender)
	//{
	//	Uimgr->Set_DialogId(TEXT("dialog_01"));
	//	wsprintf(m_tDialogIndex, Uimgr->Get_DialogId());
	//}



}

void CDialog::Late_Update(_float _fTimeDelta)
{

}

HRESULT CDialog::Render()
{
	if (true == Uimgr->Get_DisplayDialogue())
	{
		wsprintf(m_tDialogIndex, Uimgr->Get_DialogId());

		if (false == m_isFirstRefresh)
		{
			// TODO :: 우리의 2D 렌더타겟은 가변적이다. 현재는 가져올 방법이 없어 강제로 넣는다.
			_float2 vRTSize = _float2(RTSIZE_BOOK2D_X, RTSIZE_BOOK2D_Y);

			FirstCalPos(vRTSize);
		}

		if (0 != Uimgr->Get_Dialogue(m_tDialogIndex)[0].lines[Uimgr->Get_DialogueLineIndex()].BG)
		{
			__super::Render(Uimgr->Get_Dialogue(m_tDialogIndex)[0].lines[Uimgr->Get_DialogueLineIndex()].BG, PASS_VTXPOSTEX::UI_POINTSAMPLE);
		}
		else
		{
			_float4 vColor = _float4(Uimgr->Get_Dialogue(m_tDialogIndex)[0].lines[Uimgr->Get_DialogueLineIndex()].Red / 255.f,
									Uimgr->Get_Dialogue(m_tDialogIndex)[0].lines[Uimgr->Get_DialogueLineIndex()].Green / 255.f,
									Uimgr->Get_Dialogue(m_tDialogIndex)[0].lines[Uimgr->Get_DialogueLineIndex()].Blue / 255.f,
									 1.f);

			if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_RawValue("g_vColors", &vColor, sizeof(_float4))))
				return E_FAIL;

			__super::Render(Uimgr->Get_Dialogue(m_tDialogIndex)[0].lines[Uimgr->Get_DialogueLineIndex()].BG, PASS_VTXPOSTEX::DIALOGUE_BG_COLOR);
		}



		

		if (Uimgr->Get_DialogueLineIndex() < Uimgr->Get_Dialogue(m_tDialogIndex)[0].lines.size())
		{
			// TODO :: 나중에 바꿔야함, 해당 값은 가변적이다.
			_float2 vRTSize = _float2(RTSIZE_BOOK2D_X, RTSIZE_BOOK2D_Y);

			DisplayText(vRTSize);
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

// 폰트 렌더 해주는 역할
HRESULT CDialog::DisplayText(_float2 _vRTSize)
{
	if (Uimgr->Get_DialogueLineIndex() >= Uimgr->Get_Dialogue(m_tDialogIndex)[0].lines.size())
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	
	_float3 vTextPos3D = { 0.f, 0.f, 0.f };
	_float3 vTextPos2D = { 0.f, 0.f, 0.f };

	if (COORDINATE_3D == Uimgr->Get_Player()->Get_CurCoord())
	{
		vTextPos3D = Uimgr->Get_CalDialoguePos();
	}
	else if (COORDINATE_2D == Uimgr->Get_Player()->Get_CurCoord())
	{
		vTextPos2D = Uimgr->Get_CalDialoguePos();
	}

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
	
		 if (COORDINATE_3D ==  Uimgr->Get_Player()->Get_CurCoord())
		 {

			 vTextPos3D = _float3(g_iWinSizeX / 4.1f, g_iWinSizeY - g_iWinSizeY / 3.25f, 0.f);
			// 대상 이름 출력
			wsprintf(m_tFont, currentLine.Talker.c_str());
			pGameInstance->Render_Font(TEXT("Font28"), m_tFont, _float2(vTextPos3D.x, vTextPos3D.y), XMVectorSet(0.f, 0.f, 0.f, 1.f));

			// 대화 내용 출력
			wsprintf(m_tFont, strDisplaytext.c_str());
			pGameInstance->Render_Font(TEXT("Font35"), m_tFont, _float2(vTextPos2D.x - 120.f, vTextPos2D.y + 120.f), XMVectorSet(0.f, 0.f, 0.f, 1.f));

			Safe_Release(pGameInstance);
			return S_OK;
		 }
		 else if (COORDINATE_2D == Uimgr->Get_Player()->Get_CurCoord())
		 {
			 _float2 vPos = { 0.f , 0.f };

			 vPos.x = vTextPos2D.x - _vRTSize.x * 0.08f;
			 vPos.y = vTextPos2D.y + _vRTSize.y * 0.08f;


			 vTextPos2D = _float3(vPos.x, vPos.y, 0.f);
		 }
	}
	break;

	case LOC_MIDHIGH:   // 가운데 위에
	{
		_float2 vPos = { 0.f , 0.f };

		vPos.x = vTextPos2D.x - _vRTSize.x * 0.08f;
		vPos.y = vTextPos2D.y + _vRTSize.y * 0.08f;


		vTextPos2D = _float3(vPos.x, vPos.y, 0.f);
	}
	break;
	//
	case LOC_MIDLEFT:  // 가운데 좌측
	{
		_float2 vPos = { 0.f , 0.f };

		vPos.x = vTextPos2D.x - _vRTSize.x * 0.08f;
		vPos.y = vTextPos2D.y + _vRTSize.y * 0.08f;


		vTextPos2D = _float3(vPos.x, vPos.y, 0.f);
	}
	break;
	
	case LOC_MIDRIGHT: // 가운데 우측
	{
		_float2 vPos = { 0.f , 0.f };

		vPos.x = vTextPos2D.x - _vRTSize.x * 0.08f;
		vPos.y = vTextPos2D.y + _vRTSize.y * 0.08f;


		vTextPos2D = _float3(vPos.x, vPos.y, 0.f);
	}
	break;

	case LOC_LEFTDOWN: // 가운데 우측
	{
		_float2 vPos = { 0.f , 0.f };

		vPos.x = vTextPos2D.x - _vRTSize.x * 0.08f;
		vPos.y = vTextPos2D.y + _vRTSize.y * 0.08f;


		vTextPos2D = _float3(vPos.x, vPos.y, 0.f);
	}
	break;

	case LOC_LEFTHIGH: // 가운데 우측
	{
		_float2 vPos = { 0.f , 0.f };

		vPos.x = vTextPos2D.x - _vRTSize.x * 0.08f;
		vPos.y = vTextPos2D.y + _vRTSize.y * 0.08f;


		vTextPos2D = _float3(vPos.x, vPos.y, 0.f);
	}
	break;

	case LOC_RIGHTHIGH: // 가운데 우측
	{
		_float2 vPos = { 0.f , 0.f };

		vPos.x = vTextPos2D.x - _vRTSize.x * 0.08f;
		vPos.y = vTextPos2D.y + _vRTSize.y * 0.08f;


		vTextPos2D = _float3(vPos.x, vPos.y, 0.f);
	}
	break;

	case LOC_RIGHDOWN: // 가운데 우측
	{
		_float2 vPos = { 0.f , 0.f };

		vPos.x = vTextPos2D.x - _vRTSize.x * 0.08f;
		vPos.y = vTextPos2D.y + _vRTSize.y * 0.08f;


		vTextPos2D = _float3(vPos.x, vPos.y, 0.f);
	}
	break;
	}

	// 2D 기준
	_float2 vCalPos = { 0.f, 0.f };
	// 중점
	_float2 vMidPoint = { _vRTSize.x / 2.f, _vRTSize.y / 2.f };


	vCalPos.x = vMidPoint.x + vTextPos2D.x;
	vCalPos.y = vMidPoint.y - vTextPos2D.y;


	// 대상 이름 출력
	wsprintf(m_tFont, currentLine.Talker.c_str());
	pGameInstance->Render_Font(TEXT("Font20"), m_tFont, vCalPos, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	

	vCalPos.x += _vRTSize.x * 0.03f;
	vCalPos.y -= +_vRTSize.y * 0.1f;

	// 대화 내용 출력
	wsprintf(m_tFont, strDisplaytext.c_str());
	pGameInstance->Render_Font(TEXT("Font24"), m_tFont, _float2(vCalPos.x - 120.f, vCalPos.y + 120.f), XMVectorSet(0.f, 0.f, 0.f, 1.f));
	
	Safe_Release(pGameInstance);

	return S_OK;





}

// 다음 다이얼로그에서 Json->Location에 따른 위치 변경
void CDialog::NextDialogue(_float2 _RTSize)
{
	_tchar _strDialogue[MAX_PATH] = {};
	wsprintf(_strDialogue, Uimgr->Get_DialogId());


	// 2D 기준
	if (Uimgr->Get_DialogueLineIndex() <= Uimgr->Get_Dialogue(_strDialogue)[0].lines.size())
	{
		Uimgr->Set_DialogueLineIndex(Uimgr->Get_DialogueLineIndex() + 1);

		_float2 vPos = {};
		if (false == m_isRender)
		{
			m_isRender = true;
			Uimgr->Set_PortraitRender(m_isRender);
		}
		
		
		switch (Uimgr->Get_DialogueLine(_strDialogue, Uimgr->Get_DialogueLineIndex()).location)
		{
		case LOC_MIDDOWN:
		{
			/* 예시 */
			//if (0 > m_pNpc->Get_State(CTransform::STATE_POSITION))
			//{
			//	vPos.x = _RTSize.x - _RTSize.x * 1.25f;
			//	vPos.y = _RTSize.y - _RTSize.y * 1.35f;
			//}
			//else
			//{

			// vPos는 플레이어의 포지션으로 가져와서 한다.
			// 일단은 가져오는 기반이 없으니 이렇게 먼저 진행하자.
			// 이렇게 하면 플레이어 기반으로 x값을 조정하고 Y값도 플레이어의 기반으로 y값을 조정한다.
			// 계산된 좌표를 uimgr에 넣고 초상화에 적용한다.


			/* TODO :: 이 부분을 파라미터로 들어온 NPC의 좌표 기준으로 맞춘다. */
			vPos.x = Uimgr->Get_DialoguePos().x;
			vPos.y = Uimgr->Get_DialoguePos().y;

			vPos.y -= _RTSize.y * 0.13f;

			//vPos.x -= vPos.x * 1.25f;
			//vPos.y -= vPos.y * 1.35f;
			int a = 0;

			//vPos.x = _RTSize.x - _RTSize.x * 1.25f;
			//vPos.y = _RTSize.y - _RTSize.y * 1.35f;
		//}

		}
		break;

		case LOC_MIDHIGH:
		{
			vPos.x = Uimgr->Get_DialoguePos().x;
			vPos.y = Uimgr->Get_DialoguePos().y;

			vPos.y += _RTSize.y * 0.21f;
		}
		break;

		case LOC_MIDLEFT:
		{
			vPos.x = Uimgr->Get_DialoguePos().x;
			vPos.y = Uimgr->Get_DialoguePos().y;

			vPos.x -= _RTSize.x * 0.14f;
			vPos.y += _RTSize.y * 0.05f;
		}
		break;


		case LOC_MIDRIGHT:
		{
			vPos.x = Uimgr->Get_DialoguePos().x;
			vPos.y = Uimgr->Get_DialoguePos().y;

			vPos.x += _RTSize.x * 0.165f;
			vPos.y += _RTSize.y * 0.05f;
		}
		break;

		case LOC_LEFTDOWN:
		{
			vPos.x = Uimgr->Get_DialoguePos().x;
			vPos.y = Uimgr->Get_DialoguePos().y;

			vPos.x -= _RTSize.x * 0.165f;
			vPos.y -= _RTSize.y * 0.14f;
		}
		break;

		case LOC_LEFTHIGH:
		{
			vPos.x = Uimgr->Get_DialoguePos().x;
			vPos.y = Uimgr->Get_DialoguePos().y;

			vPos.x -= _RTSize.x * 0.165f;
			vPos.y += _RTSize.y * 0.19f;
		}
		break;

		case LOC_RIGHTHIGH:
		{
			vPos.x = Uimgr->Get_DialoguePos().x;
			vPos.y = Uimgr->Get_DialoguePos().y;

			vPos.x += _RTSize.x * 0.165f;
			vPos.y += _RTSize.y * 0.19f;
		}
		break;

		case LOC_RIGHDOWN:
		{
			vPos.x = Uimgr->Get_DialoguePos().x;
			vPos.y = Uimgr->Get_DialoguePos().y;

			vPos.x += _RTSize.x * 0.165f;
			vPos.y -= _RTSize.y * 0.14f;
		}
		break;

		}

		Uimgr->Set_CalDialoguePos(_float3(vPos.x, vPos.y, 0.f));
		m_vCurPos = vPos;

		//vPos.x = vPos.x + RTSIZE_BOOK2D_X * 0.5f;
		//vPos.y = -vPos.y + RTSIZE_BOOK2D_Y * 0.5f;
		m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(vPos.x, vPos.y, 0.f, 1.f));

		if (Uimgr->Get_DialogueLineIndex() == Uimgr->Get_Dialogue(_strDialogue)[0].lines.size())
		{
			Uimgr->Set_DisplayDialogue(false);
			Uimgr->Set_PortraitRender(false);
			Uimgr->Set_DialogueLineIndex(0);
			m_isFirstRefresh = false;
			isOpenPanel(_strDialogue);
		}
	}
}

// 처음 다이얼로그 입장 시 위치 계산해주는 함수
void CDialog::FirstCalPos(_float2 _RTSize)
{
	_tchar _strDialogue[MAX_PATH] = {};
	wsprintf(_strDialogue, Uimgr->Get_DialogId());
	if (Uimgr->Get_DialogueLineIndex() <= Uimgr->Get_Dialogue(_strDialogue)[0].lines.size())
	{
		Uimgr->Set_DialogueLineIndex(Uimgr->Get_DialogueLineIndex());

		_float2 vPos = {};
		if (false == m_isRender)
		{
			m_isRender = true;
			Uimgr->Set_PortraitRender(m_isRender);
		}


		switch (Uimgr->Get_DialogueLine(_strDialogue, Uimgr->Get_DialogueLineIndex()).location)
		{
		case LOC_MIDDOWN:
		{
			vPos.x = Uimgr->Get_DialoguePos().x;
			vPos.y = Uimgr->Get_DialoguePos().y;

			vPos.y -= _RTSize.y * 0.13f;
		}
		break;

		case LOC_MIDHIGH:
		{
			vPos.x = Uimgr->Get_DialoguePos().x;
			vPos.y = Uimgr->Get_DialoguePos().y;

			vPos.y += _RTSize.y * 0.21f;
		}
		break;

		case LOC_MIDLEFT:
		{
			vPos.x = Uimgr->Get_DialoguePos().x;
			vPos.y = Uimgr->Get_DialoguePos().y;

			vPos.x -= _RTSize.x * 0.14f;
			vPos.y += _RTSize.y * 0.05f;
		}
		break;

		case LOC_MIDRIGHT:
		{
			vPos.x = Uimgr->Get_DialoguePos().x;
			vPos.y = Uimgr->Get_DialoguePos().y;

			vPos.x += _RTSize.x * 0.165f;
			vPos.y += _RTSize.y * 0.05f;
		}
		break;

		case LOC_LEFTDOWN:
		{
			vPos.x = Uimgr->Get_DialoguePos().x;
			vPos.y = Uimgr->Get_DialoguePos().y;

			vPos.x -= _RTSize.x * 0.165f;
			vPos.y -= _RTSize.y * 0.14f;
		}
		break;

		case LOC_LEFTHIGH:
		{
			vPos.x = Uimgr->Get_DialoguePos().x;
			vPos.y = Uimgr->Get_DialoguePos().y;

			vPos.x -= _RTSize.x * 0.165f;
			vPos.y += _RTSize.y * 0.19f;
		}
		break;

		case LOC_RIGHTHIGH:
		{
			vPos.x = Uimgr->Get_DialoguePos().x;
			vPos.y = Uimgr->Get_DialoguePos().y;

			vPos.x += _RTSize.x * 0.165f;
			vPos.y += _RTSize.y * 0.19f;
		}
		break;

		case LOC_RIGHDOWN:
		{
			vPos.x = Uimgr->Get_DialoguePos().x;
			vPos.y = Uimgr->Get_DialoguePos().y;

			vPos.x += _RTSize.x * 0.165f;
			vPos.y -= _RTSize.y * 0.14f;
		}
		break;
		}

		Uimgr->Set_CalDialoguePos(_float3(vPos.x, vPos.y, 0.f));
		m_vCurPos = vPos;

		m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(vPos.x, vPos.y, 0.f, 1.f));

	}
	m_isFirstRefresh = true;


}

void CDialog::isOpenPanel(_tchar* _DialogId)
{
	_tchar NpcName[MAX_PATH] = {};
	_tchar strSrcName[MAX_PATH] = {};
	wsprintf(NpcName, Uimgr->Get_Dialogue(_DialogId)[0].lines[0].Talker.c_str());
	
	// 상점용
	wsprintf(strSrcName, TEXT("마르티나"));
	if (0 == wcscmp(NpcName, strSrcName))
	{
		//상점 오픈하게
		//	bool 변수로
		Uimgr->Set_DialogueFinishShopPanel(true);
	}


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
