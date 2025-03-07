#include "stdafx.h"
#include "FatherGame.h"
#include "GameInstance.h"

/* Progress */
#include "FatherGame_Progress_Start.h"

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

	m_Progress.reserve((size_t)FATHER_PROGRESS_START + 1);

	/* 2. Progress 생성 후 Vector에 저장. */
	CFatherGame_Progress_Start::FATHERGAME_PROGRESS_START_DESC ProgressStartDesc = {};
	ProgressStartDesc.iNumMonsters = 3;
	ProgressStartDesc.strMonsterPrototypeTag = TEXT("Prototype_GameObject_Goblin");
	CFatherGame_Progress* pProgressStart = CFatherGame_Progress_Start::Create(m_pDevice, m_pContext, &ProgressStartDesc);
	if (nullptr == pProgressStart)
		return E_FAIL;

	pProgressStart->Set_Active(true);
	m_Progress.push_back(pProgressStart);
	Safe_AddRef(pProgressStart);

	m_ProgressClear.push_back(false);


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
	for (_bool isClear : m_ProgressClear)
	{
		if (true == isClear)
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
	for (auto& pProgress : m_Progress)
		Safe_Release(pProgress);
	m_Progress.clear();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	return S_OK;
}

void CFatherGame::Free()
{
	if (GAME_END != m_eGameState)
	{
		/* 정상적으로 게임 엔드가 호출되지 않은 경우에만 릴리즈 */
		for (auto& pProgress : m_Progress)
			Safe_Release(pProgress);
		m_Progress.clear();

		Safe_Release(m_pGameInstance);
		Safe_Release(m_pContext);
		Safe_Release(m_pDevice);
	}


	__super::Free();
}
