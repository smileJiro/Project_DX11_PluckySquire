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
	m_eGameState = START;

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

	for (_int i = 0; i < m_StageInteracts.size(); ++i)
	{
		for (auto& iter : m_StageInteracts[i])
		{
			iter->Set_Active(false);
		}
	}

	m_pPlayer->Set_Active(false);

	// Start Music
	m_pGameInstance->End_BGM();
}

void CMinigame_Sneak::Update(_float _fTimeDelta)
{
	if (NONE == m_eGameState)
		return;

	if (KEY_PRESSING(KEY::CTRL) && KEY_DOWN(KEY::R))
	{
		GameOver();
	}

	m_fAccTime += _fTimeDelta;

	switch (m_eGameState)
	{
	case START:
	{
		// 처음 시작
		if (1.5f <= m_fAccTime)
		{
			++m_iNowStage;
			m_fAccTime = 0.f;
			Start_Stage();
		}
		break;
	}
	case PROGRESS:
	{
		if (0 <= m_iNowStage && 0.495f <= m_fAccTime)
		{
			// 행동
			Before_Action();
			Action();

			m_fAccTime = 0.5f - m_fAccTime;
		}
		break;
	}
	case RESTART:
	{
		break;
	}
	case GAME_OVER:
	{

		break;
	}
	}

	//if (m_isRestartGame)
	//{
	//	if (1.f <= m_fAccTime)
	//	{
	//		m_isRestartGame = false;
	//		
	//		m_fAccTime = 0.f;
	//	}
	//}


	
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

	// TODO : Is Interact -> After Action
}

void CMinigame_Sneak::GameOver()
{
	// BGM 끄기, 게임오버 SFX, FADE OUT
	m_pGameInstance->End_BGM();
	m_pGameInstance->Start_SFX_Delay(TEXT("Sneak_FailureSting"), 0.25f, 50.f);

	if (m_iNowStage < m_StageTiles.size())
	{
		for (auto& iter : m_StageTiles[m_iNowStage])
		{
			iter->FadeOut();
		}
	}
	
	if (m_iNowStage < m_StageInteracts.size())
	{
		for (auto& iter : m_StageInteracts[m_iNowStage])
		{
			iter->FadeOut();
		}
	
		if (m_pPlayer)
		{
			//_float2 vStartPosition = Get_TilePosition(m_iNowStage, 0);
			//m_pPlayer->Action_Caught();
		}
	}

	m_fAccTime = 0.f;
	m_eGameState = GAME_OVER;
}

//void CMinigame_Sneak::Restart_Game()
//{
//	if (m_iNowStage < m_StageTiles.size())
//	{
//		for (auto& iter : m_StageTiles[m_iNowStage])
//		{
//			iter->Restart();
//		}
//	}
//
//	if (m_iNowStage < m_StageInteracts.size())
//	{
//		for (auto& iter : m_StageInteracts[m_iNowStage])
//		{
//			iter->Restart();
//		}
//
//		if (m_pPlayer)
//		{
//			_float2 vStartPosition = Get_TilePosition(m_iNowStage, 0);
//			m_pPlayer->Restart(vStartPosition);
//		}
//	}
//
//	m_isRestartGame = true;
//	m_fAccTime = 0.f;
//
//}

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
	if (0 > _iStage || _iStage >= m_StageTiles.size())
		return _float2(0.f, 0.f);
	if (_iIndex >= m_StageTiles[_iStage].size())
		return _float2(0.f, 0.f);
		
	return m_StageTiles[_iStage][_iIndex]->Get_TilePosition();
}

CSneak_Tile* CMinigame_Sneak::Get_Tile(_int _iStage, _int _iIndex)
{
	if (0 > _iStage || _iStage >= m_StageTiles.size())
		return nullptr;
	if (_iIndex >= m_StageTiles[_iStage].size())
		return nullptr;

	return m_StageTiles[_iStage][_iIndex];
}

CSneak_InteractObject* CMinigame_Sneak::Get_InteractObject(_int _iStage, _int _iIndex)
{
	if (0 > _iStage || _iStage >= m_StageInteracts.size())
		return nullptr;

	for (auto& iter : m_StageInteracts[_iStage])
	{
		if (_iIndex == iter->Get_TileIndex())
			return iter;
	}

	return nullptr;
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

	for (auto& iter : m_StageInteracts[m_iNowStage])
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

	m_eGameState = PROGRESS;
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
		Action_Player();
	}
}

void CMinigame_Sneak::Action_Player()
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
	if (0 > iNextTile || false == Is_Moveable(iNextTile))
	{
		m_pPlayer->Action_None();
	}
	// 일단 이동 가능한 타일이 존재, Block이 있는지 확인하고 이동하기.
	else
	{
		CSneak_InteractObject* pOutObject = { nullptr };
		// Block 상태 확인
		_bool isBlocked = Is_Blocked(iPlayerTile, iNextTile, eDir, &pOutObject);

		// 이동 불가 .
		if (isBlocked)
		{
			if (nullptr != pOutObject)
			{
				if (pOutObject->Is_Interactable())
				{
					pOutObject->Interact();
					m_pPlayer->Action_Flip();
				}
			}
		}
		// 이동 가능 !
		else
		{
			_float2 vTargetPosition = m_StageTiles[m_iNowStage][iNextTile]->Get_TilePosition();
			m_pPlayer->Action_Move(iNextTile, vTargetPosition);
		}
	}

	//// Player Interact
	//if (isPlayerInteract)
	//	m_pPlayer->Action_Flip();

}

_bool CMinigame_Sneak::Is_Moveable(_int _iTileIndex)
{
	if (0 > m_iNowStage || m_StageTiles.size() <= m_iNowStage)
		return false;
	if (0 > _iTileIndex || m_StageTiles[m_iNowStage].size() <= _iTileIndex)
		return false;

	return (m_StageTiles[m_iNowStage][_iTileIndex]->Get_TileState() == CSneak_Tile::OPEN) ? false : true;
}

_bool CMinigame_Sneak::Is_Blocked(_int _iCurIndex, _int _iNextIndex, F_DIRECTION _eMoveDir, _Out_ CSneak_InteractObject** _ppOutObject)
{
	_bool isBlocked = { false };
	F_DIRECTION eBlockDir = F_DIRECTION::F_DIR_LAST;

	// Interact Object가 있나?
	CSneak_InteractObject* pInteract = Get_InteractObject(m_iNowStage, _iNextIndex);

	if (nullptr != pInteract)
	{
		// Block 확인!
		if (pInteract->Is_Blocked())
		{
			isBlocked = true;
			eBlockDir = pInteract->Get_BlockDirection();

			// 최종 Block 확인
			if (F_DIRECTION::F_DIR_LAST == eBlockDir)
			{
				isBlocked = true;
			}
			else if (F_DIRECTION::LEFT == eBlockDir)
			{
				if (F_DIRECTION::RIGHT == _eMoveDir)
					isBlocked = true;
				else
					isBlocked = false;
			}
			else if (F_DIRECTION::RIGHT == eBlockDir)
			{
				if (F_DIRECTION::LEFT == _eMoveDir)
					isBlocked = true;
				else
					isBlocked = false;
			}
			else if (F_DIRECTION::UP == eBlockDir)
			{
				if (F_DIRECTION::DOWN == _eMoveDir)
					isBlocked = true;
				else
					isBlocked = false;
			}
			else if (F_DIRECTION::DOWN == eBlockDir)
			{
				if (F_DIRECTION::UP == _eMoveDir)
					isBlocked = true;
				else
					isBlocked = false;
			}

			if (isBlocked)
			{
				if (nullptr != _ppOutObject)
					*_ppOutObject = pInteract;
				return isBlocked;
			}
		}
	}

	// 현재 Tile에 Block 있나? 
	pInteract = nullptr;
	pInteract = Get_InteractObject(m_iNowStage, _iCurIndex);

	if (nullptr != pInteract)
	{
		// Block 확인!
		if (pInteract->Is_Blocked())
		{
			isBlocked = true;
			eBlockDir = pInteract->Get_BlockDirection();

			// 최종 Block 확인
			if (F_DIRECTION::F_DIR_LAST == eBlockDir)
			{
				isBlocked = true;
			}
			else if (eBlockDir == _eMoveDir)
			{
				isBlocked = true;
			}
			else
			{
				isBlocked = false;
			}

			if (isBlocked)
			{
				if (nullptr != _ppOutObject)
					*_ppOutObject = pInteract;
				return isBlocked;
			}
		}
	}

	return false;
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
