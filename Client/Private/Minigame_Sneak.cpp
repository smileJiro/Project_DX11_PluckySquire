#include "stdafx.h"
#include "Minigame_Sneak.h"
#include "Client_Function.h"
#include "GameInstance.h"
#include "PlayerData_Manager.h"
#include "Camera_Manager.h"
#include "Event_Manager.h"
#include "Sneak_Tile.h"
#include "Sneak_MapObject.h"
#include "Sneak_InteractObject.h"
#include "Sneak_Troop.h"
#include "Pip_Player.h"
#include "Section_2D_MiniGame_Pip.h"
#include "Section_Manager.h"

IMPLEMENT_SINGLETON(CMinigame_Sneak)

CMinigame_Sneak::CMinigame_Sneak()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

void CMinigame_Sneak::Start_Game()
{
	m_eGameState = START;
	m_iCurStage = 0;

	//for (_int i = 0; i < m_StageTiles.size(); ++i)
	//{
	//	for (auto& iter : m_StageTiles[i])
	//	{
	//		//Event_SetActive(iter, false);
	//		iter->Set_Active(false);
	//	}
	//}

	//for (_int i = 0; i < m_StageObjects.size(); ++i)
	//{
	//	for (auto& iter : m_StageObjects[i])
	//	{
	//		//Event_SetActive(iter, false);
	//		iter->Set_Active(false);
	//	}
	//}

	//for (_int i = 0; i < m_StageInteracts.size(); ++i)
	//{
	//	for (auto& iter : m_StageInteracts[i])
	//	{
	//		//Event_SetActive(iter, false);
	//		iter->Set_Active(false);
	//	}
	//}

	//for (_int i = 0; i < m_StageTroops.size(); ++i)
	//{
	//	for (auto& iter : m_StageTroops[i])
	//	{
	//		//Event_SetActive(iter, false);
	//		iter->Set_Active(false);
	//	}

	//}
	//if (nullptr != m_pPlayer)
	//	m_pPlayer->Set_Active(false);
		//Event_SetActive(m_pPlayer, false);

	// Start Music
	m_pGameInstance->End_BGM();
}

void CMinigame_Sneak::Update(_float _fTimeDelta)
{
	if (NONE == m_eGameState)
		return;

#ifdef _DEBUG
	if (KEY_PRESSING(KEY::CTRL) && KEY_DOWN(KEY::R))
	{
		GameOver();
	}

	// 다음 스테이지
	if (KEY_PRESSING(KEY::CTRL) && KEY_DOWN(KEY::T))
	{
		++m_iCurStage;
		if (m_StageTiles.size() <= m_iCurStage)
			EndGame();
		Start_Stage();
		m_fAccTime = 0.f;
	}
#endif
	m_fAccTime += _fTimeDelta;

	switch (m_eGameState)
	{
	case START:
	{
		// 처음 시작
		if (1.5f <= m_fAccTime)
		{
			Start_Stage();
			m_fAccTime = 0.f;
		}
		break;
	}
	case PROGRESS:
	{
		if (0 <= m_iCurStage)
		{
			// 행동
			if ((0.32f <= m_fAccTime && nullptr != m_pPlayer && F_DIRECTION::F_DIR_LAST != m_pPlayer->Get_InputDirection()) || 0.62f <= m_fAccTime)
			{
				// 게임 진행 먼저 판단.
				Before_Action();
				Action();
				m_fAccTime = m_fAccTime - 0.5f;
			}
		}
		break;
	}
	case RESTART:
	{
		if (0.5f <= m_fAccTime)
		{
			m_eGameState = PROGRESS;
			m_pGameInstance->Start_BGM(TEXT("LCD_MUS_C09_P2122_STEALTHMINIGAME_LOOP3_FULL"), 30.f);
			m_fAccTime = 0.f;
		}

		break;
	}
	case GAME_OVER:
	{
		if (1.f <= m_fAccTime)
		{
			Restart_Game();
		}
		break;
	}
	case STAGE_VICTORY:
	{
		if (1.5f <= m_fAccTime)
		{
			++m_iCurStage;

			if (m_StageTiles.size() <= m_iCurStage)
				EndGame();
			else
				Start_Stage();
			m_fAccTime = 0.f;
		}
		break;
	}
	}

	
}

void CMinigame_Sneak::Reach_Destination(_int _iPreIndex, _int _iDestIndex)
{
	if (0 > m_iCurStage || m_iCurStage >= m_StageTiles.size() || m_iCurStage >= m_StageInteracts.size())
		return;

	_bool isInteract = false;
	for (auto& iter : m_StageInteracts[m_iCurStage])
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

void CMinigame_Sneak::Restart_Game()
{
	// Object
	if (0 <= m_iCurStage && m_iCurStage < m_StageObjects.size())
	{
		for (auto& iter : m_StageObjects[m_iCurStage])
		{
			iter->Start_FadeAlphaIn(0.25f);
		}
	}

	// 타일
	if (0 <= m_iCurStage && m_iCurStage < m_StageTiles.size())
	{
		for (auto& iter : m_StageTiles[m_iCurStage])
		{
			iter->Restart();
			iter->Start_FadeAlphaIn(0.25f);
		}
	}
	
	// Interact objects
	if (0 <= m_iCurStage && m_iCurStage < m_StageInteracts.size())
	{
		for (auto& iter : m_StageInteracts[m_iCurStage])
		{
			iter->Restart();
			iter->Start_FadeAlphaIn(0.25f);
		}

	}

	// Troops
	if (0 <= m_iCurStage && m_iCurStage < m_StageTroops.size())
	{
		for (auto& iter : m_StageTroops[m_iCurStage])
		{
			iter->GameStart();
			iter->FadeIn(0.25f);
		}

	}

	// 플레이어
	if (m_pPlayer)
	{
		_float2 vStartPosition = Get_TilePosition(m_iCurStage, 0);

		m_pPlayer->Start_Stage(vStartPosition);
		m_pPlayer->FadeIn(0.25f);
	}

	
	m_fAccTime = 0.f;
	m_eGameState = RESTART;
}

void CMinigame_Sneak::GameOver()
{
	// BGM 끄기, 게임오버 SFX, FADE OUT
	m_pGameInstance->End_BGM();
	m_pGameInstance->Start_SFX_Delay(TEXT("Sneak_FailureSting"), 0.1f, 50.f);
	
	// Tile
	if (0 <= m_iCurStage && m_StageTiles.size() > m_iCurStage)
	{
		for (auto& iter : m_StageTiles[m_iCurStage])
		{
			iter->Start_FadeAlphaOut(0.5f);
		}
	}

	// Objects
	if (0 <= m_iCurStage && m_StageObjects.size() > m_iCurStage)
	{
		for (auto& iter : m_StageObjects[m_iCurStage])
		{
			iter->Start_FadeAlphaOut(0.5f);
		}
	}

	// Interact Objects
	if (0 <= m_iCurStage && m_StageInteracts.size() > m_iCurStage)
	{
		for (auto& iter : m_StageInteracts[m_iCurStage])
		{
			iter->Start_FadeAlphaOut(0.5f);
		}
	
	}

	// Troops
	if (0 <= m_iCurStage && m_StageTroops.size() > m_iCurStage)
	{
		for (auto& iter : m_StageTroops[m_iCurStage])
		{
			iter->FadeOut(0.5f);
		}
	}


	if (m_pPlayer)
	{
		//_float2 vStartPosition = Get_TilePosition(m_iCurStage, 0);
		m_pPlayer->Action_Caught();
	}



	m_fAccTime = 0.f;
	m_eGameState = GAME_OVER;
}

void CMinigame_Sneak::EndGame()
{
	// BGM 끄기? 
	m_eGameState = NONE;
	
	// 페이지 전환.
	_float3 fDefaultPos = { };
	Event_Book_Main_Section_Change_Start(1, &fDefaultPos);
	
	// 모든 오브젝트 끄기.
	for (_int i = 0; i < m_StageObjects.size(); ++i)
	{
		for (auto& Objects : m_StageObjects[i])
			Objects->Set_Active(false);
	}
	for (_int i = 0; i < m_StageTiles.size(); ++i)
	{
		for (auto& Objects : m_StageTiles[i])
			Objects->Set_Active(false);

	}
	for (_int i = 0; i < m_StageInteracts.size(); ++i)
	{
		for (auto& Objects : m_StageInteracts[i])
			Objects->Set_Active(false);
	}
	for (_int i = 0; i < m_StageTroops.size(); ++i)
	{
		for (auto& Objects : m_StageTroops[i])
			Objects->Set_Active(false);
	}


}


HRESULT CMinigame_Sneak::Register_Tiles(vector<vector<CSneak_Tile*>>& _Tiles)
{
	for (_int i = 0; i < _Tiles.size(); ++i)
	{
		vector<CSneak_Tile*> Tiles;
		for (auto& iter : _Tiles[i])
		{
			if (nullptr == iter)
				return E_FAIL;

			Safe_AddRef(iter);
			iter->Set_Active(false);

			Tiles.push_back(iter);
		}
		m_StageTiles.push_back(Tiles);
	}
	return S_OK;
}

HRESULT CMinigame_Sneak::Register_Objects(vector<vector<CSneak_MapObject*>>& _Objects)
{
	for (_int i = 0; i < _Objects.size(); ++i)
	{
		vector<CSneak_MapObject*> Objects;
		for (auto& iter : _Objects[i])
		{
			if (nullptr == iter)
				return E_FAIL;

			Safe_AddRef(iter);
			iter->Set_Active(false);
			
			Objects.push_back(iter);
		}
		m_StageObjects.push_back(Objects);
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

HRESULT CMinigame_Sneak::Register_Interacts(vector<vector<CSneak_InteractObject*>>& _Interacts)
{
	for (_int i = 0; i < _Interacts.size(); ++i)
	{
		vector<CSneak_InteractObject*> Interacts;
		for (auto& iter : _Interacts[i])
		{
			if (nullptr == iter)
				return E_FAIL;

			Safe_AddRef(iter);
			iter->Set_Active(false);

			Interacts.push_back(iter);
		}
		m_StageInteracts.push_back(Interacts);
	}

	return S_OK;
}

HRESULT CMinigame_Sneak::Register_Troops(vector<vector<CSneak_Troop*>>& _Troops)
{

	for (_int i = 0; i < _Troops.size(); ++i)
	{
		vector<CSneak_Troop*> Troops;
		for (auto& iter : _Troops[i])
		{
			if (nullptr == iter)
				return E_FAIL;

			Safe_AddRef(iter);
			iter->Set_Active(false);
			Troops.push_back(iter);
		}
		m_StageTroops.push_back(Troops);
	}

	return S_OK;
}


_int CMinigame_Sneak::Get_NextTile(_int _iCurTile, F_DIRECTION _eDirection)
{
	if (0 > m_iCurStage || m_StageTiles.size() <= m_iCurStage)
		return -1;
	if (0 > _iCurTile || m_StageTiles[m_iCurStage].size() <= _iCurTile)
		return -1;

	return m_StageTiles[m_iCurStage][_iCurTile]->Get_AdjacentTiles(_eDirection);
}

_float2 CMinigame_Sneak::Get_TilePosition(_int _iStage, _int _iIndex)
{
	if (0 > _iStage || _iStage >= m_StageTiles.size())
		return _float2(0.f, 0.f);
	if (_iIndex >= m_StageTiles[_iStage].size())
		return _float2(0.f, 0.f);
		
	return m_StageTiles[_iStage][_iIndex]->Get_TilePosition();
}

_float2 CMinigame_Sneak::Get_CurStageTilePosition(_int _iIndex)
{
	if (0 > m_iCurStage || m_iCurStage >= m_StageTiles.size())
		return _float2(0.f, 0.f);
	if (_iIndex >= m_StageTiles[m_iCurStage].size())
		return _float2(0.f, 0.f);

	return m_StageTiles[m_iCurStage][_iIndex]->Get_TilePosition();
}

CSneak_Tile* CMinigame_Sneak::Get_Tile(_int _iStage, _int _iIndex)
{
	if (0 > _iStage || _iStage >= m_StageTiles.size())
		return nullptr;
	if (0 > _iIndex || m_StageTiles[_iStage].size() <= _iIndex)
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

void CMinigame_Sneak::Update_DetectionField()
{
	if (0 <= m_iCurStage && m_StageTroops.size() > m_iCurStage)
	{
		for (auto& iter : m_StageTroops[m_iCurStage])
			iter->Update_Detection();
	}
}

void CMinigame_Sneak::Detect_Tiles(_Inout_ _int* _pTiles, _int _iDetectCount, _int _iCurIndex, F_DIRECTION _eDirection, _bool _isRed)
{
	if (nullptr != _pTiles || 0 < _iDetectCount || F_DIRECTION::F_DIR_LAST != _eDirection)
	{
		// Detection 영역을 리셋.
		for (_int i = 0; i < _iDetectCount; ++i)
		{
			CSneak_Tile* pTile = Get_Tile(m_iCurStage, _pTiles[i]);
			if (nullptr != pTile)
				pTile->Delete_Dector(_isRed);
			_pTiles[i] = -1;
		}

		// Detect 영역 판단.
		for (_int i = 0; i < _iDetectCount; ++i)
		{
			_int iNextTile = Get_NextTile(_iCurIndex, _eDirection);
			if (-1 == iNextTile) // 이제 Detect 불가.
				break;

			// Block 영역이 막고 있는지 확인.
			if (0 <= m_iCurStage && m_StageInteracts.size() > m_iCurStage)
			{
				if (Is_Blocked(_iCurIndex, iNextTile, _eDirection, nullptr))
				{
					break;
				}
			}


			m_StageTiles[m_iCurStage][iNextTile]->Add_Detector(_isRed);
			_iCurIndex = iNextTile;
			_pTiles[i] = _iCurIndex;
		}
	}
}

void CMinigame_Sneak::DetectOff_Tiles(_int* _pTiles, _int _iDetectCount, _bool _isRed)
{
	if (nullptr != _pTiles || 0 < _iDetectCount)
	{
		// Detection 영역을 리셋.
		for (_int i = 0; i < _iDetectCount; ++i)
		{
			CSneak_Tile* pTile = Get_Tile(m_iCurStage, _pTiles[i]);
			if (nullptr != pTile)
				pTile->Delete_Dector(_isRed);
			_pTiles[i] = -1;
		}
	}
}

_int CMinigame_Sneak::Get_PlayerTile()
{
	if (nullptr != m_pPlayer)
	{
		return m_pPlayer->Get_CurTile();
	}

	return -1;
}

_bool CMinigame_Sneak::Is_MoveableTile(_int _iTileIndex)
{
	if (0 > m_iCurStage || m_StageTiles.size() <= m_iCurStage)
		return false;
	if (0 > _iTileIndex || m_StageTiles[m_iCurStage].size() <= _iTileIndex)
		return false;

	return (m_StageTiles[m_iCurStage][_iTileIndex]->Get_TileState() == CSneak_Tile::OPEN) ? false : true;
}


void CMinigame_Sneak::Start_Stage()
{
	if (0 <= m_iCurStage && m_StageObjects.size() > m_iCurStage)
	{
		for (auto& iter : m_StageObjects[m_iCurStage])
		{
			Event_SetActive(iter, true);
			iter->Start_FadeAlphaIn(0.25f);
			CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter8_P1516"), iter, CSection_2D_MiniGame_Pip::SECTION_PIP_MAPOBJECT);

			//iter->Set_Active(true);
		}
	}

	if (0 <= m_iCurStage && m_StageTiles.size() > m_iCurStage)
	{
		for (auto& iter : m_StageTiles[m_iCurStage])
		{
			Event_SetActive(iter, true);
			iter->Start_FadeAlphaIn(0.25f);
			CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter8_P1516"), iter, CSection_2D_MiniGame_Pip::SECTION_PIP_TILE);

			//iter->Set_Active(true);
		}
	}

	if (0 <= m_iCurStage && m_StageInteracts.size() > m_iCurStage)
	{
		for (auto& iter : m_StageInteracts[m_iCurStage])
		{
			Event_SetActive(iter, true);
			iter->Start_FadeAlphaIn(0.25f);
			if (iter->Is_Nextgroup())
				CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter8_P1516"), iter, CSection_2D_MiniGame_Pip::SECTION_PIP_MOVEOBJECT);
			else
				CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter8_P1516"), iter, CSection_2D_MiniGame_Pip::SECTION_PIP_MOVEMAPOBJECT);

			//iter->Set_Active(true);
		}
	}

	if (0 <= m_iCurStage && m_StageTroops.size() > m_iCurStage)
	{
		for (auto& iter : m_StageTroops[m_iCurStage])
		{
			iter->GameStart();
			CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter8_P1516"), iter, CSection_2D_MiniGame_Pip::SECTION_PIP_MOVEOBJECT);

		}
	}

	if (nullptr != m_pPlayer)
	{
		if (false == m_pPlayer->Is_Active())
		{
			CPlayerData_Manager::GetInstance()->Set_CurrentPlayer(PLAYABLE_ID::PIP);
			CCamera_Manager::GetInstance()->Change_CameraTarget(m_pPlayer);
			CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter8_P1516"), m_pPlayer, CSection_2D_MiniGame_Pip::SECTION_PIP_MOVEOBJECT);

			m_pPlayer->FadeIn(0.25f);
		}
		if (0 <= m_iCurStage && m_StageTiles.size() > m_iCurStage)
			m_pPlayer->Start_Stage(m_StageTiles[m_iCurStage][0]->Get_TilePosition());

	}


	
	m_iFlipTime = 0;

	m_eGameState = PROGRESS;
	m_pGameInstance->Start_BGM(TEXT("LCD_MUS_C09_P2122_STEALTHMINIGAME_LOOP3_FULL"), 30.f);

}

void CMinigame_Sneak::Before_Action()
{
	/*
	* 0. 승리 판단.
	* 1. Troop Fall 판단.
	* 2. Player Caught 판단 -> 게임종료
	*/

	// 0. 승리 판단.
	if (nullptr != m_pPlayer)
	{
		_int iTileIndex = m_pPlayer->Get_CurTile();

		if (0 <= m_iCurStage && m_StageTiles.size() > m_iCurStage && m_StageTiles[m_iCurStage].size() - 1 == iTileIndex)
		{
			m_pPlayer->Action_Victory();
			m_eGameState = STAGE_VICTORY;
			m_fAccTime = 0.5f; // m_fAccTime = m_fAccTime - 0.5f에서 m_fAccTime 0.f로 만들기 위해서..

			m_pGameInstance->Start_SFX(TEXT("A_sfx_ReachFinalRoomExit"), 50.f);
			//m_pGameInstance->End_BGM();
			return;
		}
	}

	// 1. Troop Fall 판단.
	if (0 <= m_iCurStage && m_StageTroops.size() > m_iCurStage)
	{
		for (auto& iter : m_StageTroops[m_iCurStage])
		{
			_int iTileIndex = iter->Get_CurTile();

			CSneak_Tile* pTile = Get_Tile(m_iCurStage, iTileIndex);
			// Troop이 존재하는 타일이 열려있는 타일일 경우.
			if (nullptr != pTile && CSneak_Tile::OPEN == pTile->Get_TileState())
			{
				iter->Action_Fall();
			}
		}
	}

	// 2. Player Caught 판단
	if (0 <= m_iCurStage && m_StageTroops.size() > m_iCurStage)
	{
		for (auto& iter : m_StageTroops[m_iCurStage])
		{
			iter->Action_Catch();
		}
	}


}

void CMinigame_Sneak::Action()
{
	// Game Over 여부 판단.
	if (STAGE_VICTORY == m_eGameState || GAME_OVER == m_eGameState)
		return;

	// 맵 오브젝트들 움직임.
	++m_iFlipTime;
	if (2 <= m_iFlipTime && m_StageObjects.size() > m_iCurStage)
	{
		for (auto& iter : m_StageObjects[m_iCurStage])
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
	// Troop들 행동.
	Action_Troops();
}

void CMinigame_Sneak::Action_Player()
{
	F_DIRECTION eDir = m_pPlayer->Get_InputDirection();
	if (F_DIRECTION::F_DIR_LAST == eDir)
		return;

	_int iPlayerTile = m_pPlayer->Get_CurTile();

	_int iNextTile = Get_NextTile(iPlayerTile, eDir);

	// 이동 X ( 타일이 없다 & 이동 불가능 타일 & )
	if (0 > iNextTile || false == Is_MoveableTile(iNextTile) || Is_TroopsBlocked(iNextTile))
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
				else
				{
					m_pPlayer->Action_None();
				}
			}
		}
		// 이동 가능 !
		else
		{
			//_float2 vTargetPosition = m_StageTiles[m_iCurStage][iNextTile]->Get_TilePosition();
			_float2 vTargetPosition = Get_TilePosition(m_iCurStage, iNextTile);
			m_pPlayer->Action_Move(iNextTile, vTargetPosition);
		}
	}

	//// Player Interact
	//if (isPlayerInteract)
	//	m_pPlayer->Action_Flip();

}

void CMinigame_Sneak::Action_Troops()
{
	// 몬스터 Move, Turn
	if (0 <= m_iCurStage && m_StageTroops.size() > m_iCurStage)
	{
		for (auto& iter : m_StageTroops[m_iCurStage])
		{
			_bool isMove = { true };
			_int  iNextTile = { -1 };
			_int  iCurTile = { -1 };
			F_DIRECTION eCurDirection = F_DIRECTION::F_DIR_LAST;
			// 이동할 수 있는 Troop -> 이동 여부 확인하고 이동
			if (iter->Is_Moveable())
			{
				eCurDirection = iter->Get_CurrentDirection();
				iCurTile = iter->Get_CurTile();
				iNextTile = Get_NextTile(iCurTile, eCurDirection);

				// 이동 불가 판단. (없는 타일 & 현재 이동 불가능 타일)
				if (0 > iNextTile || false == Is_MoveableTile(iNextTile))
				{
					isMove = false;
				}
			}
			else 
			{
				isMove = false;
			}
			
			if (isMove)
			{
				// 근데 플레이어가 가는 타일이다 -> 이동 X Turn X 
				if (nullptr != m_pPlayer && (m_pPlayer->Get_CurTile() == iNextTile))
					continue;

				// 거기에 더해 Blocked한 타일인지 판단하자.
				CSneak_InteractObject* pOutObject = { nullptr };
				// Block 상태 확인
				_bool isBlocked = Is_Blocked(iCurTile, iNextTile, eCurDirection, &pOutObject);

				// 이동 불가 .
				if (isBlocked)
				{
					if (nullptr != pOutObject)
					{
						if (pOutObject->Is_Interactable())
						{
							pOutObject->Interact();
						}
						else
						{
							iter->Action_Turn();
						}
					}
				}
				else
				{

					_float2 vTargetPosition = Get_TilePosition(m_iCurStage, iNextTile);
					iter->Action_Move(iNextTile, vTargetPosition);
				}

			}
			// 혹은 Turn 혹은 Scout
			else
			{
				if (CSneak_Troop::SCOUT == iter->Get_TroopState())
					iter->Action_Scout();
				else
					iter->Action_Turn();
			}
			
		}
	}
}


// Troop의 예상경로에 존재하나 ? 
_bool CMinigame_Sneak::Is_TroopsBlocked(_int _iTileIndex)
{
	if (0 <= m_iCurStage && m_StageTroops.size() > m_iCurStage)
	{
		for (auto& iter : m_StageTroops[m_iCurStage])
		{
			if (_iTileIndex == iter->Predict_Route())
				return true;
		}

	}
	return false;
}

_bool CMinigame_Sneak::Is_Blocked(_int _iCurIndex, _int _iNextIndex, F_DIRECTION _eMoveDir, _Out_ CSneak_InteractObject** _ppOutObject)
{
	_bool isBlocked = { false };
	F_DIRECTION eBlockDir = F_DIRECTION::F_DIR_LAST;

	// Interact Object가 있나?
	CSneak_InteractObject* pInteract = Get_InteractObject(m_iCurStage, _iNextIndex);

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
	pInteract = Get_InteractObject(m_iCurStage, _iCurIndex);

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
		m_StageTiles[i].clear();
	}
	m_StageTiles.clear();

	for (_int i = 0; i < m_StageObjects.size(); ++i)
	{
		for (auto& iter : m_StageObjects[i])
		{
			Safe_Release(iter);
		}
		m_StageObjects[i].clear();
	}
	m_StageObjects.clear();

	for (_int i = 0; i < m_StageInteracts.size(); ++i)
	{
		for (auto& iter : m_StageInteracts[i])
		{
			Safe_Release(iter);
		}
		m_StageInteracts[i].clear();
	}
	m_StageInteracts.clear();

	for (_int i = 0; i < m_StageTroops.size(); ++i)
	{
		for (auto& iter : m_StageTroops[i])
		{
			Safe_Release(iter);
		}
	}
	m_StageTroops.clear();

	Safe_Release(m_pPlayer);
	Safe_Release(m_pGameInstance);

	__super::Free();
}
