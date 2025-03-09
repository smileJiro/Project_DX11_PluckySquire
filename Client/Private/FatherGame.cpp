#include "stdafx.h"
#include "FatherGame.h"
#include "GameInstance.h"
#include "PortalLocker.h"
#include "Section_Manager.h"
#include "Section_2D_PlayMap.h"
#include "Portal_Default.h"
#include "ZetPack_Child.h"
/* Progress */
#include "FatherGame_Progress_Start.h"
#include "FatherGame_Progress_ZetPack.h"

IMPLEMENT_SINGLETON(CFatherGame)
CFatherGame::CFatherGame()
{
}

HRESULT CFatherGame::Start_Game(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	/* 1. 초기화 */
	m_pDevice = _pDevice;
	Safe_AddRef(m_pDevice);
	m_pContext = _pContext;
	Safe_AddRef(m_pContext);
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);

	m_Progress.reserve((size_t)FATHER_PROGRESS::FATHER_PROGRESS_ZETPACK + 1);

	/* 2. Progress 생성 후 Vector에 저장. */

	
	{/* Progress Start */
		CFatherGame_Progress_Start::FATHERGAME_PROGRESS_START_DESC ProgressStartDesc = {};
		ProgressStartDesc.iNumMonsters = 3;
		ProgressStartDesc.strMonsterPrototypeTag = TEXT("Prototype_GameObject_Goblin");
		CFatherGame_Progress* pProgressStart = CFatherGame_Progress_Start::Create(m_pDevice, m_pContext, &ProgressStartDesc);
		if (nullptr == pProgressStart)
			return E_FAIL;
		pProgressStart->Set_Active(true);
		m_Progress.push_back(pProgressStart); // 여기가 원본임. AddRef x
		m_ProgressClear.push_back(false);
	}/* Progress Start */

	
	{/* Progress ZetPack */
		CFatherGame_Progress_ZetPack::FATHERGAME_PROGRESS_ZETPACK_DESC ProgressJetPackDesc = {};
		CFatherGame_Progress* pProgressZetPack = CFatherGame_Progress_ZetPack::Create(m_pDevice, m_pContext, &ProgressJetPackDesc);
		if (nullptr == pProgressZetPack)
			return E_FAIL;
		pProgressZetPack->Set_Active(false);
		m_Progress.push_back(pProgressZetPack); // 여기가 원본임. AddRef x
		m_ProgressClear.push_back(false);
	}/* Progress ZetPack */



	/* 2. PortalDefender 3곳에 생성 */
	m_PortalLockers.resize((size_t)LOCKER_LAST);

	{/* PortalLocker ZetPack */
		CGameObject* pGameObject = nullptr;
		CPortalLocker::PORTALLOCKER_DESC PortalLockerDesc;
		CPortal* pTargetPortal = static_cast<CPortal_Default*>(static_cast<CSection_2D_PlayMap*>(CSection_Manager::GetInstance()->Find_Section(TEXT("Chapter6_SKSP_01")))->Get_Portal(0));
		if (nullptr == pTargetPortal)
			return E_FAIL;
		PortalLockerDesc.pTargetPortal = pTargetPortal;
		PortalLockerDesc.ePortalLockerType = CPortalLocker::TYPE_PURPLE;
		PortalLockerDesc.strSectionKey = TEXT("Chapter6_SKSP_01");

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_PortalLocker"), LEVEL_CHAPTER_6, TEXT("Layer_PortalLocker"), &pGameObject, &PortalLockerDesc)))
			return E_FAIL;

		m_PortalLockers[LOCKER_ZETPACK] = static_cast<CPortalLocker*>(pGameObject);
		Safe_AddRef(m_PortalLockers[LOCKER_ZETPACK]);
		if (FAILED(CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(PortalLockerDesc.strSectionKey, pGameObject, SECTION_2D_PLAYMAP_OBJECT)))
			return E_FAIL;		
	}/* PortalLocker ZetPack */


	//{/* PortalLocker PartHead */
	//	CGameObject* pGameObject = nullptr;
	//	CPortalLocker::PORTALLOCKER_DESC PortalLockerDesc;
	//	CPortal* pTargetPortal = static_cast<CPortal_Default*>(static_cast<CSection_2D_PlayMap*>(CSection_Manager::GetInstance()->Find_Section(TEXT("Chapter6_SKSP_04")))->Get_Portal(0));
	//	if (nullptr == pTargetPortal)
	//		return E_FAIL;
	//	PortalLockerDesc.pTargetPortal = pTargetPortal;
	//	PortalLockerDesc.ePortalLockerType = CPortalLocker::TYPE_YELLOW;
	//	PortalLockerDesc.strSectionKey = TEXT("Chapter6_SKSP_04");
	//
	//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_PortalLocker"), LEVEL_CHAPTER_6, TEXT("Layer_PortalLocker"), &pGameObject, &PortalLockerDesc)))
	//		return E_FAIL;
	//
	//	m_PortalLockers[LOCKER_PARTHEAD] = static_cast<CPortalLocker*>(pGameObject);
	//	Safe_AddRef(m_PortalLockers[LOCKER_PARTHEAD]);
	//
	//	if (FAILED(CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(PortalLockerDesc.strSectionKey, pGameObject, SECTION_2D_PLAYMAP_OBJECT)))
	//		return E_FAIL;
	//}/* PortalLocker PartHead */



	// 1. 모성 

	m_eGameState = GAME_PLAYING;

	return S_OK;
}

void CFatherGame::Update()
{
	if (GAME_PLAYING != m_eGameState)
		return;

	for (_uint i = 0; i < m_Progress.size(); ++i)
	{
		/* Update */
		if(true == m_Progress[i]->Is_Active()) 
		{
			m_Progress[i]->Progress_Update();

			/* Clear Check */
			if (true == m_Progress[i]->Is_Clear())
			{
				/* True로 기록 , Active는 내부적으로 Progress가 관리. */
				m_ProgressClear[i] = true;
			}
		}
	}

	/* Game End Check */
	m_iClearCount = 0;
	for (_uint i = 0; i < m_Progress.size(); ++i)
	{
		if (true == m_ProgressClear[i] && false == m_Progress[i]->Is_Active())
		{
			++m_iClearCount;
		}

	}

	if (m_Progress.size() == m_iClearCount)
	{
		End_Game();
	}

}

HRESULT CFatherGame::End_Game()
{
	m_eGameState = GAME_END;

	Safe_Release(m_pZetPack_Child);
	for (auto& pProgress : m_Progress)
		Safe_Release(pProgress);
	m_Progress.clear();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

#ifdef _DEBUG
	cout << "FatherGame End" << endl;
#endif // _DEBUG

	return S_OK;
}

void CFatherGame::DeadCheck_ReferenceObject()
{
	for (_uint i = 0; i < PORTALLOCKER::LOCKER_LAST; ++i)
	{
		if (nullptr == m_PortalLockers[i])
			continue;

		if (true == m_PortalLockers[i]->Is_Dead())
		{
			Safe_Release(m_PortalLockers[i]);
			m_PortalLockers[i] = nullptr;
		}
	}
}

void CFatherGame::Start_Progress(FATHER_PROGRESS _eStartProgress)
{
	if (true == m_ProgressClear[_eStartProgress])
		assert(nullptr);
	if (nullptr == m_Progress[_eStartProgress])
		assert(nullptr);

	Event_SetActive(m_Progress[_eStartProgress], true);
}

void CFatherGame::OpenPortalLocker(PORTALLOCKER _ePortalLockerIndex)
{
	if (PORTALLOCKER::LOCKER_LAST <= _ePortalLockerIndex)
		return;

	if (nullptr == m_PortalLockers[_ePortalLockerIndex])
		return;

	m_PortalLockers[_ePortalLockerIndex]->Open_Locker();
	Safe_Release(m_PortalLockers[_ePortalLockerIndex]);
	m_PortalLockers[_ePortalLockerIndex] = nullptr;
}

void CFatherGame::Set_ZetPack_Child(CZetPack_Child* _pZetPack_Child)
{
	assert(_pZetPack_Child);
	assert(!m_pZetPack_Child);
	m_pZetPack_Child = _pZetPack_Child;
	Safe_AddRef(m_pZetPack_Child);
};

void CFatherGame::Free()
{
	for (auto& pPortalLocker : m_PortalLockers)
	{
		Safe_Release(pPortalLocker);
	}
	m_PortalLockers.clear();

	if (GAME_END != m_eGameState)
	{
		/* 정상적으로 게임 엔드가 호출되지 않은 경우에만 릴리즈 */
		Safe_Release(m_pZetPack_Child);

		for (auto& pProgress : m_Progress)
			Safe_Release(pProgress);
		m_Progress.clear();

		Safe_Release(m_pGameInstance);
		Safe_Release(m_pContext);
		Safe_Release(m_pDevice);
	}


	__super::Free();
}
