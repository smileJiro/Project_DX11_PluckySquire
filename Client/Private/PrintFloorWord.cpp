#include "stdafx.h"
#include "PrintFloorWord.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "Section_2D.h"
#include "UI_Manager.h"
#include "Section_Manager.h"


CPrintFloorWord::CPrintFloorWord(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CFloorWord(_pDevice, _pContext)
{
}

CPrintFloorWord::CPrintFloorWord(const CPrintFloorWord& _Prototype)
	: CFloorWord(_Prototype)
{
}

HRESULT CPrintFloorWord::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CPrintFloorWord::Initialize(void* _pArg)
{
 	CFloorWord::FLOORTEXT* pDesc = static_cast<FLOORTEXT*>(_pArg);

	m_vRenderPos.x = pDesc->fPosX;
	m_vRenderPos.y = pDesc->fPosY;
	m_strSFX = pDesc->strSFX;
	m_strSection = pDesc->strSection;
	m_isLengthCheck = pDesc->isLengthCheck;

	wsprintf(m_tFloorWord, pDesc->strText.c_str());

	if (FAILED(__super::Initialize_Child(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(m_strSection, this, SECTION_2D_PLAYMAP_BACKGROUND);

	return S_OK;
}

void CPrintFloorWord::Priority_Update(_float _fTimeDelta)
{
}

void CPrintFloorWord::Update(_float _fTimeDelta)
{
	_float fThisPosX = m_vRenderPos.x;
	_float fThisPosY = m_vRenderPos.y;
	CPlayer* pPlayer = Uimgr->Get_Player();
	if (nullptr == pPlayer)
	{
		MSG_BOX("Null이면 안되는데 null이 들어온상황. ");
		// 레벨전환시 순서 잘 살펴야함 상욱형 >> 플레이어생성전인데 유아이매니저에서 플레이어를 참조했다던지, 레벨전환시 여기서 터짐
		return;
	} 

	CController_Transform* pControllerTransform = pPlayer->Get_ControllerTransform();

	if (nullptr == pControllerTransform)
	{
		MSG_BOX("Null이면 안되는데 null이 들어온상황. ");
		// 레벨전환시 순서 잘 살펴야함 상욱형 >> 플레이어생성전인데 유아이매니저에서 플레이어를 참조했다던지, 레벨전환시 여기서 터짐
		return;
	}
	_float fPlayerPosX = pControllerTransform->Get_Transform(COORDINATE_2D)->Get_State(CTransform::STATE_POSITION).m128_f32[0];
	_float fPlayerPosY = pControllerTransform->Get_Transform(COORDINATE_2D)->Get_State(CTransform::STATE_POSITION).m128_f32[1];
	
	
	
	_bool isSameSection = { false };
	
	if (nullptr != CSection_Manager::GetInstance()->Get_SectionKey(Uimgr->Get_Player()))
	{
		auto pPlayerCurSection = CSection_Manager::GetInstance()->Get_SectionKey(Uimgr->Get_Player());
		wstring strPlayerCurSection = *(pPlayerCurSection);

		if (m_strSection == strPlayerCurSection)
		{
			isSameSection = true;
		}
	}

	
	if (m_isLengthCheck)
	{
		if (190.f > fabs(fThisPosX - fPlayerPosX) &&
			190.f > fabs(fThisPosY - fPlayerPosY) &&
			COORDINATE_2D == pPlayer->Get_CurCoord() && 
			false == m_isFadeIn && true == isSameSection )
		{
			Start_FloorWord();
		}
	}

	Add_Amount(_fTimeDelta);

}

void CPrintFloorWord::Late_Update(_float _fTimeDelta)
{
}

HRESULT CPrintFloorWord::Render()
{
	/* TODO :: 현재 RT 사이즈는 가변적이다. 추후 변경해야한다. */


	// 2D 기준
	_float2 vCalPos = { 0.f, 0.f };
	// 중점
	_float2 vMidPoint = { RTSIZE_BOOK2D_X / 2.f, RTSIZE_BOOK2D_Y / 2.f };


	vMidPoint = _float2(CSection_Manager::GetInstance()->Get_Section_RenderTarget_Size(CSection_Manager::GetInstance()->Get_Cur_Section_Key()).x / 2,
		CSection_Manager::GetInstance()->Get_Section_RenderTarget_Size(CSection_Manager::GetInstance()->Get_Cur_Section_Key()).y/ 2);
	vCalPos.x = vMidPoint.x + m_vRenderPos.x;
	vCalPos.y = vMidPoint.y - m_vRenderPos.y;


	m_pGameInstance->Render_Font(TEXT("Font28"), m_tFloorWord, vCalPos, XMVectorSet(0.f, 0.f, 0.f, m_fAmount));

	return S_OK;
}



HRESULT CPrintFloorWord::Ready_Components()
{
	return S_OK;
}


CPrintFloorWord* CPrintFloorWord::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CPrintFloorWord* pInstance = new CPrintFloorWord(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CPrintFloorWord Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CPrintFloorWord::Clone(void* _pArg)
{
	CPrintFloorWord* pInstance = new CPrintFloorWord(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CPrintFloorWord Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CPrintFloorWord::Free()
{
	__super::Free();
}

HRESULT CPrintFloorWord::Cleanup_DeadReferences()
{
	return S_OK;
}

void CPrintFloorWord::Start_FloorWord()
{
	m_isFadeIn = true;

	// NOTWORD 소리가 나면 안되는 floorword
	if (TEXT("NOTWORD") != m_strSFX)
		m_pGameInstance->Start_SFX_Delay(m_strSFX, 0.f, 50.f, false);
}

void CPrintFloorWord::Add_Amount(_float _fTimeDelta)
{
	if (true == m_isFadeIn && false == m_isFadeInComplete)
	{
		m_fAmount += _fTimeDelta * 1.8f;

		if (m_fAmount > 1.f)
		{
			m_fAmount = 1.f;
			m_isFadeInComplete = true;
		}
	}
}

//HRESULT CPrintFloorWord::Cleanup_DeadReferences()
//{
//	return S_OK;
//}

