#pragma once
#include "stdafx.h"
#include "Narration_new.h"
#include "GameInstance.h"
#include "Section_Manager.h"
#include "Trigger_Manager.h"
#include "CustomFont.h"
#include "Narration_Manager.h"
#include "UI_Manager.h"



#include "Effect2D_Manager.h"



CNarration_New::CNarration_New(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CUI(_pDevice, _pContext)
{
}

CNarration_New::CNarration_New(const CNarration_New& _Prototype)
	: CUI(_Prototype)
{
}

HRESULT CNarration_New::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNarration_New::Initialize(void* _pArg)
{
	/*
	------------------------------------------------------------------
	Narration_Manager에서 건넨준 데이터를 적용한다.
	
	적용 필요한 데이터
	strSectionId -> 어디 섹션에 노출시킬 것인가					// 변수화
	m_strSectionId = 

	strid -> 나레이션의 아이디													// 이건 해야하나?
	curlevelId -> 나레이션을 적용할 게임 레벨					// 변수화
	
	lines -> 실질적인 데이터									// 라인을 갖고있는 컨테이너를 만들자.
		idulation -> 몇초 유지? (사용 X)						
		ispeed -> 글씨 타이핑 몇초? (사용 X)
		strtext -> 노출 시킬 글자는? (token파싱해야함)
		strsfx -> 노출 시킬 사운드
		strsubsfx -> 노출시킬 추가 사운드
		fLineHeight -> 줄바꿈의 높이
		fFadeDauration -> 몇초간 페이드인 시킬것인가
		fDelayNextTime -> 페이드인 완료 후 다음 라인까지의 이동 시간
		fscale -> 크기는 얼마인가
		isLeft -> 좌측인가요 우측인가요
		fposx, fposy -> 노출 시킬 글자의 위치
		fwaitingtime -> 라인이 완료 후 페이지 넘기는 시간
			
			NarAnim -> 해당 라인의 나레이션 애니메이션
				strAnimationid -> 애니메이션의 아이디
				animationindex -> 해당 모델에 노출시킬 애니메이션 번호
				fPosx , fPosy -> 노출 시킬 애니메이션의 좌표
				isLoop -> 반복인가요?
				strSectionid -> 어디 섹션에 노출 시킬 건지
				vAnimationScaleX,Y -> 애니메이션의 스케일
				fWaitingTime -> 얼마나 기달릴것인가
	
	------------------------------------------------------------------
	*/


	NarrationData* pDesc = static_cast<NarrationData*>(_pArg);
	m_strSectionId = pDesc->strSectionid;
	m_eLevelId = pDesc->eCurlevelId;
	m_fFadeTime = pDesc->fFadeDuration;

	// 가져온 NarData를 m_NarrationData로 데이터 이동
	m_NarrationData.NarAnim.strtext				= pDesc->NarAnim.strtext;
	m_NarrationData.NarAnim.strSFX				= pDesc->NarAnim.strSFX;
	m_NarrationData.NarAnim.strSubSFX			= pDesc->NarAnim.strSubSFX;
	m_NarrationData.NarAnim.fLineHieght			= pDesc->NarAnim.fLineHieght;
	m_NarrationData.NarAnim.fDelayNextLine		= pDesc->NarAnim.fDelayNextLine;
	m_NarrationData.NarAnim.fscale				= pDesc->NarAnim.fscale;
	m_NarrationData.NarAnim.isLeft				= pDesc->NarAnim.isLeft;
	m_NarrationData.NarAnim.fFontPos.x			= pDesc->NarAnim.fFontPos.x;
	m_NarrationData.NarAnim.fFontPos.y			= pDesc->NarAnim.fFontPos.y;
	m_NarrationData.NarAnim.fwaitingTime		= pDesc->NarAnim.fwaitingTime;
	m_NarrationData.NarAnim.isDirTurn			= pDesc->NarAnim.isDirTurn;
	m_NarrationData.NarAnim.isFinishedThisLine	= pDesc->NarAnim.isFinishedThisLine;

	m_NarrationData.NarAnim.fLineHieght			= pDesc->NarAnim.fLineHieght;
	m_NarrationData.NarAnim.strAnimationid		= pDesc->NarAnim.strAnimationid;
	m_NarrationData.NarAnim.iAnimationIndex		= pDesc->NarAnim.iAnimationIndex;
	m_NarrationData.NarAnim.isLoop				= pDesc->NarAnim.isLoop;
	m_NarrationData.NarAnim.strSectionid		= pDesc->NarAnim.strSectionid;
	m_NarrationData.NarAnim.vPos.x				= pDesc->NarAnim.vPos.x;
	m_NarrationData.NarAnim.vPos.y				= pDesc->NarAnim.vPos.y;
	m_NarrationData.NarAnim.vAnimationScale.x	= pDesc->NarAnim.vAnimationScale.x;
	m_NarrationData.NarAnim.vAnimationScale.y	= pDesc->NarAnim.vAnimationScale.y;
	m_NarrationData.NarAnim.AnimationCount		= pDesc->NarAnim.AnimationCount;

	//for (int i = 0; i < pDesc->lines.size(); ++i)
	//{
	//	m_NarrationData.lines[i].NarAnim.resize(pDesc->lines[i].NarAnim.size());
	//
	//	for (int j = 0; j < pDesc->lines[i].NarAnim.size(); ++j)
	//	{
	//		m_NarrationData.lines[i].NarAnim[j].strAnimationid = pDesc->lines[i].NarAnim[j].strAnimationid;
	//		m_NarrationData.lines[i].NarAnim[j].iAnimationIndex = pDesc->lines[i].NarAnim[j].iAnimationIndex;
	//		m_NarrationData.lines[i].NarAnim[j].isLoop = pDesc->lines[i].NarAnim[j].isLoop;
	//		m_NarrationData.lines[i].NarAnim[j].strSectionid = pDesc->lines[i].NarAnim[j].strSectionid;
	//		m_NarrationData.lines[i].NarAnim[j].vPos.x = pDesc->lines[i].NarAnim[j].vPos.x;
	//		m_NarrationData.lines[i].NarAnim[j].vPos.y = pDesc->lines[i].NarAnim[j].vPos.y;
	//		m_NarrationData.lines[i].NarAnim[j].vAnimationScale.x = pDesc->lines[i].NarAnim[j].vAnimationScale.x;
	//		m_NarrationData.lines[i].NarAnim[j].vAnimationScale.y = pDesc->lines[i].NarAnim[j].vAnimationScale.y;
	//		
	//	}
	//}



	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	// 전체 라인의 전체 애니메이션을 만든다.
	if (FAILED(Ready_Components(&m_NarrationData)))
		return E_FAIL;


	_float2 vCalScale = { 0.f, 0.f };
	vCalScale.x = m_vOriginSize.x * RATIO_BOOK2D_X;
	vCalScale.y = m_vOriginSize.y * RATIO_BOOK2D_Y;
	m_isRender = false;
	m_isLeftRight = true;




	return S_OK;
}

void CNarration_New::Priority_Update(_float _fTimeDelta)
{

}


void CNarration_New::Update(_float _fTimeDelta)
{
	
	if (KEY_DOWN(KEY::G))
	{
		StopNarration();
		return;
	}

	// 처음 나레이션을 재생 시킨다.
	if (false == m_isNarrationEnd)
	{
		if (false == m_isPlayNarration && true == CNarration_Manager::GetInstance()->Get_Playing())
		{
			m_isPlayNarration = true;
			m_isStartNarration = true;


			CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(m_NarrationData.NarAnim.strSectionid, this, SECTION_2D_PLAYMAP_UI);
			//m_NarrationData.lines[0].NarAnim[0].pAnimModels->Play_Animation(_fTimeDelta, false);

			GetAnimationObjectForLine(m_iCurrentLine);

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
					GetAnimationObjectForLine(m_iCurrentLine);
					m_fWaitingTime = 0.f;
					m_fWaitingNextPageTime = 0.f;

				}
			}

			Update_Narration(_fTimeDelta);
		}
	}

}

void CNarration_New::Late_Update(_float _fTimeDelta)
{

}

HRESULT CNarration_New::Render()
{
	//if (false == CBase::Is_Active())
	//{
	//	CBase::Set_Active(true);
	//}

	// TODO :: 해당 부분은 가변적이다 추후 변경해야한다.
	DisplayText(_float2(RTSIZE_BOOK2D_X, RTSIZE_BOOK2D_Y));
	return S_OK;
}


// 폰트 렌더 해주는 역할
HRESULT CNarration_New::DisplayText(_float2 _vRTSize)
{
	if (true == m_isWaitingNextPage)
	{
		return S_OK;
	}


	// 예를 들어, 첫 번째 NarrationData의 텍스트만 사용한다고 가정
	//auto& lines = m_NarrationData.NarAnim;

	// 이쪽이 문제다...
	// 0번부터 m_iCurrentLine까지의 모든 라인을 렌더링 (이미 fade-in이 완료된 라인은 alpha = 1.0)

		// 이전 라인은 완전 불투명, 현재 라인은 fade-in 중
		float alpha = (m_isNarrationEnd) ? 1.f : m_fTextAlpha;
		NarrationDialogData& dialogue = m_NarrationData.NarAnim;

		// 해당 라인의 시작 위치
		float fx = { 0.f };
		float fy = { 0.f };

		if (true == dialogue.isLeft)
		{
			fx = dialogue.fFontPos.x;
			fy = dialogue.fFontPos.y;
		}
		else if (false == dialogue.isLeft)
		{
			fx = dialogue.fFontPos.x + _vRTSize.x / 2.f;
			fy = dialogue.fFontPos.y;
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
					fx = dialogue.fFontPos.x; // 시작 x 좌표 복원
					fy += dialogue.fLineHieght; // 각 라인의 고유 fLineHeight 사용
				}
				else // 오른쪽 텍스트의 경우
				{
					fx = dialogue.fFontPos.x + _vRTSize.x / 2.f; // 시작 x 좌표 복원
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

	return S_OK;
}


void CNarration_New::Update_Narration(_float _fTimeDelta)
{
	// 페이드인 진행중일 경우
	if (m_fTextAlpha < 1.f)
	{
		m_fFadeTimer += _fTimeDelta;
		m_fTextAlpha = min(m_fFadeTimer / m_NarrationData.fFadeDuration, 1.f);

		if (!m_bAnimationStarted && m_fTextAlpha > 0.f)
		{
			m_bAnimationStarted = true;
			m_isFade = true;

			// 처음 진행 되는 경우

			if (nullptr == m_NarrationData.NarAnim.pAnimModels)
			{
				assert(nullptr);

			}
			// 애니메이션 시작해
			if (nullptr != m_NarrationData.NarAnim.pAnimModels)
			{
				if (false == this->CBase::Is_Active())
				{
					CBase::Set_Active(true);
				}

				m_NarrationData.NarAnim.pAnimModels->Play_Animation(_fTimeDelta, false);
				m_isPlayAnimation = true;

			}


		}
	}
	// 페이드인이 1 이상되어서 더이상 페이드 하면 안되는 경우
	else if (1.f <= m_fTextAlpha)
	{
		if (m_fTextAlpha >= 1.f)
		{
			// 페이드 상태가 아니다.
			m_isFade = false;
		}
		m_fDelayTimer += _fTimeDelta;

		if (m_fDelayTimer >= m_NarrationData.NarAnim.fDelayNextLine)
		{


			if (m_NarrationData.NarAnim.isFinishedThisLine)
			{
				if (false == m_isNarrationEnd)
					Set_EndNarrationPlayerPos();

				/* 하드코딩 */
				// Chapter1_P1112_Narration_01
				if (m_NarrationData.strid == TEXT("Chapter1_P1112_Narration_01"))
				{
					Uimgr->Set_VioletMeet(true);
				}


				m_isStartNarration = false;
				m_isNarrationEnd = true;
				m_isPlayNarration = false;

				m_isLeftRight = true;


			}


		}
	}
}





HRESULT CNarration_New::Ready_Components(NarrationData* _pDesc)
{
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	CComponent* pComponent = static_cast<CComponent*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, m_eLevelId, _pDesc->NarAnim.strAnimationid, nullptr));

	if (nullptr == pComponent)
		return E_FAIL;

	m_pModelCom = static_cast<C2DModel*>(pComponent);

	return S_OK;
}



CNarration_New* CNarration_New::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CNarration_New* pInstance = new CNarration_New(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CNarration_New Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CNarration_New::Clone(void* _pArg)
{
	CNarration_New* pInstance = new CNarration_New(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CNarration_New Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CNarration_New::Free()
{
	// TODO :: 누수 잡는 중

	// 모델 삭제.
	//Safe_Release(m_NarrationData.NarAnim.pAnimModels);




	__super::Free();
}

HRESULT CNarration_New::Cleanup_DeadReferences()
{

	return S_OK;
}

void CNarration_New::GetAnimationObjectForLine(const _uint iLine)
{
	//for (size_t i = 0; i < m_NarrationData.lines[iLine].NarAnim.size(); ++i)
	//{
	//	CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(
	//		m_NarrationData.lines[iLine].NarAnim[i].strSectionid,
	//		m_NarrationData.lines[iLine].NarAnim[i].pAnimModels
	//	);
	//}

	//if (m_iCurrentLine <= m_NarrationData.lines.size())
	//{
	//	for (int i = 0; i < m_iCurrentLine; ++i)
	//	{
	//		for (int j = 0; j < m_NarrationData.lines[i].NarAnim.size(); ++j)
	//		{
	//			if (m_iAnimationIndex <= m_NarrationData.lines[i].NarAnim.size())
	//			{
	//				m_NarrationData.lines[i].NarAnim[j].pAnimModels->Play_Animation();
	//			}
	//			
	//		}
	//	}
	//}
	

	if (TEXT("NOT") != m_NarrationData.NarAnim.strSFX)
		m_pGameInstance->Start_SFX_Delay(m_NarrationData.NarAnim.strSFX, 0.f, 50.f, false);

	if (TEXT("NOT") != m_NarrationData.NarAnim.strSubSFX)
		m_pGameInstance->Start_SFX_Delay(m_NarrationData.NarAnim.strSubSFX, 0.f, 30.f, false);
}

HRESULT CNarration_New::Set_EndNarrationPlayerPos()
{
	_float3 vPos = _float3(0.f, 0.f, 1.f);

	if (true == m_NarrationData.NarAnim.isDirTurn)
	{
		// 다음 장으로 이동하기

		if (CSection_Manager::GetInstance()->Get_Next_Section_Key() == TEXT("Chapter1_P0506"))
		{
			vPos = _float3(-693.f, -35.5f, 0.0f);
		}
		else if (CSection_Manager::GetInstance()->Get_Next_Section_Key() == TEXT("Chapter2_P0708"))
		{
			vPos = _float3(30.0f, 2322.f, 0.0f);
		}
		else if (CSection_Manager::GetInstance()->Get_Next_Section_Key() == TEXT("Chapter2_P0506"))
		{
			vPos = _float3(0.0f, -333.f, 0.0f);
		}

		Event_Book_Main_Section_Change_Start(1, &vPos);

	}
	else // 이전 장으로 이동하기
	{
		if (CSection_Manager::GetInstance()->Get_Prev_Section_Key() == (TEXT("Chapter1_P0708")))
		{
			vPos = _float3(-932.f, -79.1f, 0.0f);
		}

		Event_Book_Main_Section_Change_Start(0, &vPos);
	}

	return S_OK;
}

void CNarration_New::PaseTokens(const _wstring& _Text, vector<TextTokens>& _OutToken)
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

void CNarration_New::StopNarration()
{
	if (false == m_isStartNarration)
		return;

	m_isStartNarration = false;
	m_isNarrationEnd = true;
	m_isPlayNarration = false;
	//Uimgr->Set_TurnoffPlayNarration(false);

	m_isLeftRight = true;
	_float3 vPos = _float3(0.f, 0.f, 0.f);


	if (true == m_NarrationData.NarAnim.isDirTurn)
		Event_Book_Main_Section_Change_Start(1, &vPos);
	else
		Event_Book_Main_Section_Change_Start(0, &vPos);

}



_bool CNarration_New::isLeftRight()
{
	return m_isLeftRight;
}

