#include "stdafx.h"
#include "Dialog_Manager.h"
#include "UI_Manager.h"

IMPLEMENT_SINGLETON(CDialog_Manager)

CDialog_Manager::CDialog_Manager()
	:m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}



_bool CDialog_Manager::Get_isLastDialog()
{
	if (nullptr == m_pDialogue)
		assert(nullptr);

	return m_pDialogue->Get_isLastDialogLine();
}

void CDialog_Manager::Set_DialogId(const _tchar* _id, const _tchar* _strCurSection, _bool _DisplayDialogue, _bool _DisPlayPortrait)
{
	CPlayer* pPlayer = Uimgr->Get_Player();

	if (nullptr == m_pDialogue)
		return;

	if (false == m_pDialogue->CBase::Is_Active())
	{
		m_pDialogue->CBase::Set_Active(true);
	}

	m_pDialogue->Set_DialogueId(_id);


	if (nullptr == _strCurSection)
	{

		m_pDialogue->Set_strCurSection(TEXT("NOTWORD"));
		
		if (COORDINATE_2D == pPlayer->Get_CurCoord())
		{
			_float2 vPlayerPos = _float2(pPlayer->Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_State(CTransform::STATE_POSITION).m128_f32[0], 
										pPlayer->Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_State(CTransform::STATE_POSITION).m128_f32[1]);
			
			Set_DialoguePos(_float3(vPlayerPos.x, vPlayerPos.y, 0.f));
		}
		else if (COORDINATE_3D == pPlayer->Get_CurCoord())
		{
			// 여기에 들어오는게 어떻게 들어오나..?
		}
	}
	else
	{
		m_pDialogue->Set_strCurSection(_strCurSection);
	}

	m_pDialogue->Set_DisPlayDialogue(_DisplayDialogue);
	m_pDialogue->Set_DisplayPortraitRender(_DisPlayPortrait);
}

void CDialog_Manager::Clear_DialogTarget()
{
	for (int i = 0; i < m_DialogTargets.size(); ++i)
	{
		Safe_Release(m_DialogTargets[i]);
	}
	m_DialogTargets.clear();
}

void CDialog_Manager::Set_ActionChange(_int _AnimIndex)
{
	// 현재 다이얼로그를 알아야한다.
	// 현재 다이얼로그의 talkerchange가 있는지 알아야한다.
	// 현재 다이얼로그 라인에 target을 알아야한다.

	// 해당 타겟의 애니메이션을 변경시킨다.

	// 타겟 변경이 없을경우 (일반적인 경우)
	if (false == m_pDialogue->isCurDialogTargetChange())
	{
		CNPC* pNPC = dynamic_cast<CNPC*>(m_DialogTargets[0]);

		if (nullptr == pNPC)
			assert(TEXT("DialogMGR pNPC"));

		pNPC->Set_SocialNpcAnimationIndex(_AnimIndex);




	}
	else
	{
		_int targetIndex = m_pDialogue->Get_TargetIndex();

		CNPC* pNPC = dynamic_cast<CNPC*>(m_DialogTargets[targetIndex]);

		if (nullptr == pNPC)
			assert(TEXT("TARGET NPC NOT FOUND"));

		pNPC->Set_SocialNpcAnimationIndex(_AnimIndex);
	}


}

void CDialog_Manager::Set_DialogEnd()
{
	m_pDialogue->Set_strCurSection(L"");
	m_isDisPlayDialogue = false;
	m_pDialogue->Set_DisPlayDialogue(false);
	m_pDialogue->Set_DisplayPortraitRender(false);

	if (nullptr != m_pNPC)
	{
		Safe_Release(m_pNPC);
		m_pNPC = nullptr;
	}
}

_int CDialog_Manager::Get_CurrentLineIndex()
{
	if (nullptr != m_pDialogue)
		return m_pDialogue->Get_CurrentLineIndex();
	else 
	{
		assert(nullptr);
		return -1;
	}
}

HRESULT CDialog_Manager::Level_Exit(_int iCurLevelID, _int _iChangeLevelID, _int _iNextChangeLevelID)
{
	if (nullptr != m_pDialogue)
	{
		Safe_Release(m_pDialogue);
		m_pDialogue = nullptr;
	}

	if (nullptr != m_pNPC)
	{
		Safe_Release(m_pNPC);
		m_pNPC = nullptr;
	}

	for (int i = 0; i < m_DialogTargets.size(); ++i)
	{
		Safe_Release(m_DialogTargets[i]);
	}
	m_DialogTargets.clear();


	return S_OK;
}

HRESULT CDialog_Manager::Level_Logo_Exit(_int _iChangeLevelID, _int _iNextChangeLevelID)
{

	return S_OK;
}

HRESULT CDialog_Manager::Level_Enter(_int _iChangeLevelID)
{
	if (nullptr == m_pDialogue)
		return S_OK;

	m_pDialogue->NextLevelLoadJson(_iChangeLevelID);

	return S_OK;
}


void CDialog_Manager::Free()
{
	for (int i = 0; i < m_DialogTargets.size(); ++i)
	{
		Safe_Release(m_DialogTargets[i]);
	}
	m_DialogTargets.clear();


	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDialogue);
	Safe_Release(m_pNPC);
	__super::Free();
}

