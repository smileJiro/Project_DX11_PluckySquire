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


	_float2 vCalScale = { 0.f, 0.f };
	vCalScale.x = m_vOriginSize.x * RATIO_BOOK2D_X;
	vCalScale.y = m_vOriginSize.y * RATIO_BOOK2D_Y;

	m_pControllerTransform->Set_Scale(vCalScale.x, vCalScale.y, 1.f);

	m_isRender = false;

	CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this);
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
	if (true == m_isRender)
	{
		Uimgr->Set_DialogId(TEXT("dialog_01"));
		wsprintf(m_tDialogIndex, Uimgr->Get_DialogId());
	}



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

HRESULT CDialog::DisplayText(_float2 _vRTSize)
{
	if (Uimgr->Get_DialogueLineIndex() >= Uimgr->Get_Dialogue(m_tDialogIndex)[0].lines.size())
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	_float2 vTextPos3D = _float2(0.f, 0.f);
	_float2 vTextPos2D = Uimgr->Get_DialoguePos();



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
		/*
		 ////    3D 기준    ////
		 if (3D)
		 {

		 vTextPos3D = _float2(g_iWinSizeX / 4.1f, g_iWinSizeY - g_iWinSizeY / 3.25f);
			// 대상 이름 출력
			wsprintf(m_tFont, currentLine.Talker.c_str());
			pGameInstance->Render_Font(TEXT("Font28"), m_tFont, vTextPos2D, XMVectorSet(0.f, 0.f, 0.f, 1.f));

			// 대화 내용 출력
			wsprintf(m_tFont, strDisplaytext.c_str());
			pGameInstance->Render_Font(TEXT("Font35"), m_tFont, _float2(vTextPos2D.x - 120.f, vTextPos2D.y + 120.f), XMVectorSet(0.f, 0.f, 0.f, 1.f));

			Safe_Release(pGameInstance);
			return;
		 }
		 else (2D)
		 {
		 }
		*/

		_float2 vPos = { 0.f , 0.f };

		vPos.x = vTextPos2D.x - _vRTSize.x * 0.1f;
		vPos.y = vTextPos2D.y + _vRTSize.y * 0.1f;


		vTextPos2D = vPos;


	}
	break;

	//case LOC_MIDDLE:   // 정 가운데
	//{
	//	vTextPos2D = _float2(g_iWinSizeX / 2.f, g_iWinSizeY / 2.f);
	//}
	//break;
	//
	//case LOC_MIDLEFT:  // 가운데 좌측
	//{
	//	vTextPos2D = _float2(g_iWinSizeX / 2.f, g_iWinSizeY / 2.f);
	//}
	//break;
	//
	//case LOC_MIDRIGHT: // 가운데 우측
	//{
	//	vTextPos2D = _float2(g_iWinSizeX / 2.f, g_iWinSizeY / 2.f);
	//}
	//break;
	}

	// 2D 기준
	_float2 vCalPos = { 0.f, 0.f };
	// 중점
	_float2 vMidPoint = { _vRTSize.x / 2.f, _vRTSize.y / 2.f };


	vCalPos.x = vMidPoint.x + vTextPos2D.x;
	vCalPos.y = vMidPoint.y - vTextPos2D.y;


	// 대상 이름 출력
	wsprintf(m_tFont, currentLine.Talker.c_str());
	pGameInstance->Render_Font(TEXT("Font28"), m_tFont, vCalPos, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	

	vCalPos.x += _vRTSize.x * 0.03f;
	vCalPos.y -= +_vRTSize.y * 0.1f;

	// 대화 내용 출력
	wsprintf(m_tFont, strDisplaytext.c_str());
	pGameInstance->Render_Font(TEXT("Font35"), m_tFont, _float2(vCalPos.x - 120.f, vCalPos.y + 120.f), XMVectorSet(0.f, 0.f, 0.f, 1.f));
	
	Safe_Release(pGameInstance);

	return S_OK;





}

// Json->Location에 따른 위치 변경
void CDialog::NextDialogue(_float2 _RTSize)
{
	// 2D 기준
	if (Uimgr->Get_DialogueLineIndex() <= Uimgr->Get_Dialogue(TEXT("dialog_01"))[0].lines.size())
	{
		Uimgr->Set_DialogueLineIndex(Uimgr->Get_DialogueLineIndex() + 1);

		_float2 vPos = {};
		if (false == m_isRender)
		{
			m_isRender = true;
			Uimgr->Set_PortraitRender(m_isRender);
		}
		
		
		switch (Uimgr->Get_DialogueLine(TEXT("dialog_01"), Uimgr->Get_DialogueLineIndex()).location) 
		{
		case LOC_DEFAULT:
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

			vPos.x = Uimgr->Get_Player()->Get_Transform()->Get_State(CTransform_2D::STATE_POSITION).m128_f32[0];
			vPos.y = Uimgr->Get_Player()->Get_Transform()->Get_State(CTransform_2D::STATE_POSITION).m128_f32[1];

			vPos.y -= _RTSize.y * 0.13f;

			//vPos.x -= vPos.x * 1.25f;
			//vPos.y -= vPos.y * 1.35f;
			int a = 0;

				//vPos.x = _RTSize.x - _RTSize.x * 1.25f;
				//vPos.y = _RTSize.y - _RTSize.y * 1.35f;
			//}
			
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

		Uimgr->Set_DialoguePos(vPos);
		m_vCurPos = vPos;

		//vPos.x = vPos.x + RTSIZE_BOOK2D_X * 0.5f;
		//vPos.y = -vPos.y + RTSIZE_BOOK2D_Y * 0.5f;
		m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(vPos.x, vPos.y, 0.f, 1.f));

		if (Uimgr->Get_DialogueLineIndex() == Uimgr->Get_Dialogue(TEXT("dialog_01"))[0].lines.size())
		{
			m_isRender = false;
			Uimgr->Set_PortraitRender(m_isRender);
		}
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
