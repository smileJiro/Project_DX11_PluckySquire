#include "stdafx.h"
#include "Minigame_Sneak.h"
#include "GameInstance.h"
#include "PlayerData_Manager.h"
#include "Camera_Manager.h"
#include "Sneak_Tile.h"
#include "Sneak_MapObject.h"
#include "Sneak_InteractObject.h"
#include "Pip_Player.h"

IMPLEMENT_SINGLETON(CMinigame_Sneak)

CMinigame_Sneak::CMinigame_Sneak()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

void CMinigame_Sneak::Start_Game()
{
	m_isStartGame = true;

	for (_int i = 0; i < m_StageTiles.size(); ++i)
	{
		for (auto& iter : m_StageTiles[i])
		{
			iter->Set_Active(false);
		}
	}

	for (_int i = 0; i < m_StageObjects.size(); ++i)
	{
		for (auto& iter : m_StageObjects[i])
		{
			iter->Set_Active(false);
		}
	}

	m_pPlayer->Set_Active(false);

	// Start Music
	m_pGameInstance->End_BGM();
	//m_pGameInstance->Start_SFX(TEXT("Sneak_FailureSting"), 50.f);
}

void CMinigame_Sneak::Update(_float _fTimeDelta)
{
	if (false == m_isStartGame)
		return;

	m_fAccTime += _fTimeDelta;

	// 처음 시작
	if (-1 == m_iNowStage && 1.5f <= m_fAccTime)
	{
		m_iNowStage = 0;
		m_fAccTime = 0.f;
		Start_Stage();
	}
	else if (0 <= m_iNowStage)
	{
#ifdef _DEBUG
		if (0.49f <= m_fAccTime)
#else
		if (0.495f <= m_fAccTime)
#endif
		{
			// 행동
			Before_Action();
			Action();

			m_fAccTime = 0.5f - m_fAccTime;
		}
	}
}

void CMinigame_Sneak::Reach_Destination(_int _iPreIndex, _int _iDestIndex)
{
	if (0 > m_iNowStage || m_iNowStage >= m_StageTiles.size() || m_iNowStage >= m_StageInteracts.size())
		return;

	_bool isInteract = false;
	for (auto& iter : m_StageInteracts[m_iNowStage])
	{
		if (_iDestIndex == iter->Get_TileIndex() || _iPreIndex == iter->Get_TileIndex())
		{
			if (iter->Is_CollisionInteractable())
			{
				isInteract = true;
				iter->Interact();
			}
		}
	}

	// TODO : Is Interact
}

HRESULT CMinigame_Sneak::Register_Tiles(vector<vector<CSneak_Tile*>>& _Tiles)
{
	m_StageTiles = _Tiles;

	for (_int i = 0; i < m_StageTiles.size(); ++i)
	{
		for (auto& iter : m_StageTiles[i])
		{
			if (nullptr == iter)
				return E_FAIL;

			Safe_AddRef(iter);
			iter->Set_Active(false);
		}
	}
	return S_OK;
}

HRESULT CMinigame_Sneak::Register_Objects(vector<vector<CSneak_MapObject*>>& _Objects)
{
	m_StageObjects = _Objects;

	for (_int i = 0; i < m_StageObjects.size(); ++i)
	{
		for (auto& iter : m_StageObjects[i])
		{
			if (nullptr == iter)
				return E_FAIL;

			Safe_AddRef(iter);
			iter->Set_Active(false);
		}
	}
	return S_OK;
}

HRESULT CMinigame_Sneak::Register_Player(CPip_Player* _pPlayer)
{
	m_pPlayer = _pPlayer;

	if (nullptr == m_pPlayer)
		return E_FAIL;

	Safe_AddRef(m_pPlayer);

	m_pPlayer->Set_Active(false);

	return S_OK;
}

HRESULT CMinigame_Sneak::Register_Interacts(vector<vector<CSneak_InteractObject*>> _Interacts)
{
	m_StageInteracts = _Interacts;

	for (_int i = 0; i < m_StageInteracts.size(); ++i)
	{
		for (auto& iter : m_StageInteracts[i])
		{
			if (nullptr == iter)
				return E_FAIL;

			Safe_AddRef(iter);
			iter->Set_Active(false);
		}
	}

	return S_OK;
}

_float2 CMinigame_Sneak::Get_TilePosition(_int _iStage, _int _iIndex)
{
	if (_iStage >= m_StageTiles.size())
		return _float2(0.f, 0.f);
	if (_iIndex >= m_StageTiles[_iStage].size())
		return _float2(0.f, 0.f);
		
	return m_StageTiles[_iStage][_iIndex]->Get_TilePosition();
}

CSneak_Tile* CMinigame_Sneak::Get_Tile(_int _iStage, _int _iIndex)
{
	if (_iStage >= m_StageTiles.size())
		return nullptr;
	if (_iIndex >= m_StageTiles[_iStage].size())
		return nullptr;

	return m_StageTiles[_iStage][_iIndex];
}

void CMinigame_Sneak::Start_Stage()
{
	if (0 > m_iNowStage || m_StageTiles.size() < m_iNowStage)
		return;

	for (auto& iter : m_StageTiles[m_iNowStage])
	{
		iter->Set_Active(true);
	}

	for (auto& iter : m_StageObjects[m_iNowStage])
	{
		iter->Set_Active(true);
	}

	if (nullptr != m_pPlayer && false == m_pPlayer->Is_Active())
	{
		CPlayerData_Manager::GetInstance()->Set_CurrentPlayer(PLAYABLE_ID::PIP);
		// TEMP;
		CCamera_Manager::GetInstance()->Change_CameraTarget(m_pPlayer);
	}

	if (m_StageTiles.size() > m_iNowStage)
		m_pPlayer->Start_Stage(m_StageTiles[m_iNowStage][0]->Get_TilePosition());
	
	m_iFlipTime = 0;

	m_pGameInstance->Start_BGM(TEXT("LCD_MUS_C09_P2122_STEALTHMINIGAME_LOOP3_FULL"), 30.f);

}

void CMinigame_Sneak::Before_Action()
{
}

void CMinigame_Sneak::Action()
{
	// 맵 오브젝트들 움직임.
	++m_iFlipTime;

	if (2 <= m_iFlipTime && m_StageObjects.size() > m_iNowStage)
	{
		for (auto& iter : m_StageObjects[m_iNowStage])
		{
			iter->Flip();
		}
		m_iFlipTime = 0;
	}

	// 플레이어 움직임.
	if (nullptr != m_pPlayer)
	{
		F_DIRECTION eDir = m_pPlayer->Get_InputDirection();
		if (F_DIRECTION::F_DIR_LAST == eDir)
			return;

		_int iPlayerTile = m_pPlayer->Get_CurTile();

		if (m_StageTiles.size() <= m_iNowStage)
			return;
		if (iPlayerTile >= m_StageTiles[m_iNowStage].size())
			return;

		_int iNextTile = m_StageTiles[m_iNowStage][iPlayerTile]->Get_AdjacentTiles(eDir);



		// 이동 X
		if (0 > iNextTile)
		{
			m_pPlayer->Action_None();
		}

		// 이동 O
		else
		{
			_float2 vTargetPosition = m_StageTiles[m_iNowStage][iNextTile]->Get_TilePosition();

			m_pPlayer->Action_Move(iNextTile, vTargetPosition);
		}

	}
}

void CMinigame_Sneak::Free()
{
	for (_int i = 0; i < m_StageTiles.size(); ++i)
	{
		for (auto& iter : m_StageTiles[i])
		{
			Safe_Release(iter);
		}
	}
	m_StageTiles.clear();

	for (_int i = 0; i < m_StageObjects.size(); ++i)
	{
		for (auto& iter : m_StageObjects[i])
		{
			Safe_Release(iter);
		}
	}
	m_StageObjects.clear();

	for (_int i = 0; i < m_StageInteracts.size(); ++i)
	{
		for (auto& iter : m_StageInteracts[i])
		{
			Safe_Release(iter);
		}
	}
	m_StageInteracts.clear();

	Safe_Release(m_pPlayer);
	Safe_Release(m_pGameInstance);

	__super::Free();
}
