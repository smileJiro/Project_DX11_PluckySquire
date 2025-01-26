#include "stdafx.h"
#include "Dialogue.h"
#include "GameInstance.h"

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

	return S_OK;
}


void CDialog::Update(_float _fTimeDelta)
{
	if (KEY_DOWN(KEY::B))
	{
		if (m_iCurrentLineIndex <= m_DialogDatas[0].lines.size())
		{
			++m_iCurrentLineIndex;

			if (m_iCurrentLineIndex == m_DialogDatas[0].lines.size())
			{
				m_isRender = false;
			}
		}
	}
}

void CDialog::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT CDialog::Render()
{
	if (true == m_isRender)
	{
		__super::Render(m_DialogDatas[0].lines[m_iCurrentLineIndex].BG, PASS_VTXPOSTEX::DEFAULT);

		if (m_iCurrentLineIndex < m_DialogDatas[0].lines.size())
		{
			DisplayTextWithAnimation();
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

					if (line.contains("Talker") && line["Talker"].is_string())
					{
						dialogLine.Talker = StringToWstring(line["Talker"].get<string>());
					}
						

					if (line.contains("text") && line["text"].is_string())
					{
						dialogLine.text = StringToWstring(line["text"].get<string>());
					}
						

					if (line.contains("BG") && line["BG"].is_number_integer())
					{
						dialogLine.BG = line["BG"].get<int>();
					}
						

					if (line.contains("location") && line["location"].is_number_integer())
					{
						dialogLine.location = static_cast<LOC>(line["location"].get<int>());
					}
						

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

HRESULT CDialog::DisplayTextWithAnimation()
{
	if (m_iCurrentLineIndex >= m_DialogDatas[0].lines.size())
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	_float2 vTextPos = _float2(0.f, 0.f);

	// 현재 대화의 절대 글자 수
	const auto& currentLine = m_DialogDatas[0].lines[m_iCurrentLineIndex];
	static _wstring strDisplaytext;
	static _float fWaitTime = 0.0f;
	static _int iPreviousLineIndex = -1; 

	// 라인이 변경되었을 때 초기화
	if (iPreviousLineIndex != m_iCurrentLineIndex)
	{
		strDisplaytext.clear();
		fWaitTime = 0.0f;
		iPreviousLineIndex = m_iCurrentLineIndex;
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
		vTextPos = _float2(g_iWinSizeX / 4.1f, g_iWinSizeY - g_iWinSizeY / 3.25f);
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
	pGameInstance->Render_Font(TEXT("Font40"), m_tFont, vTextPos, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	
	// 대화 내용 출력
	wsprintf(m_tFont, strDisplaytext.c_str());
	pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(vTextPos.x - 90.f, vTextPos.y + 70.f), XMVectorSet(0.f, 0.f, 0.f, 1.f));
	
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


	__super::Free();
	
}

HRESULT CDialog::Cleanup_DeadReferences()
{

	return S_OK;
}
