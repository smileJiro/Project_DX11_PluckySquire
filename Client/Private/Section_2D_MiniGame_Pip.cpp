#include "stdafx.h"
#include "Section_2D_MiniGame_Pip.h"
#include "GameInstance.h"
#include "2DMapObject.h"
#include "Map_2D.h"
#include "Section_Manager.h"
#include "Engine_Macro.h"
#include "MapObjectFactory.h"
#include "Trigger_Manager.h"
#include "Portal.h"
#include "Blocker.h"

#include "Minigame_Sneak.h"
#include "Sneak_Default_Tile.h"
#include "Sneak_MapObject.h"
#include "Sneak_InteractObject.h"
#include "Sneak_Troop.h"
#include "Pip_Player.h"
#include "PlayerData_Manager.h"

CSection_2D_MiniGame_Pip::CSection_2D_MiniGame_Pip(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CSection_2D_MiniGame(_pDevice, _pContext, MINIGAME, SECTION_2D_BOOK)
{
}

HRESULT CSection_2D_MiniGame_Pip::Initialize(void* _pDesc)
{
	if (FAILED(__super::Initialize(_pDesc)))
		return E_FAIL;


	if (FAILED(Ready_Objects(_pDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CSection_2D_MiniGame_Pip::Ready_Objects(void* _pDesc)
{
	// 섹션 내부에 생성해놓아야 할 객체가 있다면, 여기서 구현
	// 파일 로드든, 하드코딩이든 자유
	// :: 이 함수가 불리는 시점은? 
	//	Level Loader End
	//	Section_Level_Enter
	//						=> 여기!

#pragma region TILE
	{
		const json* pjsonTileInfo = m_pGameInstance->Find_Json_InLevel(TEXT("SneakTile"), LEVEL_CHAPTER_8);
		if (nullptr == pjsonTileInfo)
			return E_FAIL;

		CSneak_Default_Tile::SNEAK_TILEDESC TileDesc = {};
		CSneak_Tile* pTileOut = { nullptr };
		_int iStageSize = pjsonTileInfo->size();
		vector<vector<CSneak_Tile*>> StageTiles;

		for (_int i = 0; i < iStageSize; ++i)
		{
			vector<CSneak_Tile*> Tiles;
			Tiles.resize((*pjsonTileInfo)[i].size(), nullptr);

			_float2 vPosition = { 0.f, 0.f };
			if ((*pjsonTileInfo)[i][0].contains("Position"))
			{
				vPosition.x = (*pjsonTileInfo)[i][0]["Position"][0];
				vPosition.y = (*pjsonTileInfo)[i][0]["Position"][1];
			}

			TileDesc.iCurLevelID = LEVEL_CHAPTER_8;

			TileDesc.tTransform2DDesc.vInitialPosition.x = vPosition.x;
			TileDesc.tTransform2DDesc.vInitialPosition.y = vPosition.y;
			TileDesc.tTransform2DDesc.vInitialPosition.z = 0.f;

			for (_int j = 0; j < (*pjsonTileInfo)[i].size(); ++j)
			{
				if (nullptr != Tiles[j])
					continue;

				TileDesc.iTileIndex = (*pjsonTileInfo)[i][j]["Index"];

				for (_int k = 0; k < (*pjsonTileInfo)[i][j]["Adjacent"].size(); ++k)
				{
					TileDesc.iAdjacents[k] = (*pjsonTileInfo)[i][j]["Adjacent"][k];
				}

				for (_int k = 0; k < (_int)F_DIRECTION::F_DIR_LAST; ++k)
				{
					if (-1 == TileDesc.iAdjacents[k] || TileDesc.iAdjacents[k] >= Tiles.size())
						continue;

					if (nullptr == Tiles[TileDesc.iAdjacents[k]])
						continue;

					vPosition = Tiles[TileDesc.iAdjacents[k]]->Get_TilePosition();

					switch (k)
					{
					case (_int)F_DIRECTION::LEFT:
						TileDesc.tTransform2DDesc.vInitialPosition.x = vPosition.x + 65.f;
						TileDesc.tTransform2DDesc.vInitialPosition.y = vPosition.y;
						break;
					case (_int)F_DIRECTION::RIGHT:
						TileDesc.tTransform2DDesc.vInitialPosition.x = vPosition.x - 65.f;
						TileDesc.tTransform2DDesc.vInitialPosition.y = vPosition.y;
						break;
					case (_int)F_DIRECTION::UP:
						TileDesc.tTransform2DDesc.vInitialPosition.x = vPosition.x;
						TileDesc.tTransform2DDesc.vInitialPosition.y = vPosition.y - 65.f;
						break;
					case (_int)F_DIRECTION::DOWN:
						TileDesc.tTransform2DDesc.vInitialPosition.x = vPosition.x;
						TileDesc.tTransform2DDesc.vInitialPosition.y = vPosition.y + 65.f;
						break;
					default:
						break;
					}

					break;
				}

				if ((*pjsonTileInfo)[i][j].contains("Default"))
				{
					// Trap
					if (false == (*pjsonTileInfo)[i][j]["Default"])
					{
						if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHAPTER_8, TEXT("Prototype_GameObject_SneakTrapTile"), LEVEL_CHAPTER_8
							, TEXT("Layer_Sneak_Tile"), reinterpret_cast<CGameObject**>(&pTileOut), &TileDesc)))
							return E_FAIL;
					}
					// Default
					else
					{
						if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHAPTER_8, TEXT("Prototype_GameObject_SneakDefaultTile"), LEVEL_CHAPTER_8
							, TEXT("Layer_Sneak_Tile"), reinterpret_cast<CGameObject**>(&pTileOut), &TileDesc)))
							return E_FAIL;
					}
				}
				else
				{
					if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHAPTER_8, TEXT("Prototype_GameObject_SneakDefaultTile"), LEVEL_CHAPTER_8
						, TEXT("Layer_Sneak_Tile"), reinterpret_cast<CGameObject**>(&pTileOut), &TileDesc)))
						return E_FAIL;
				}

			

				if (nullptr != pTileOut)
				{
					Add_GameObject_ToSectionLayer(pTileOut, SECTION_PIP_TILE);
					Tiles[j] = pTileOut;
				}

			}
			StageTiles.push_back(Tiles);
		}

		CMinigame_Sneak::GetInstance()->Register_Tiles(StageTiles);
	}	
#pragma endregion TILE



#pragma region SNEAK OBJECT
	{
		const json* pjsonObjectsInfo = m_pGameInstance->Find_Json_InLevel(TEXT("SneakObjects"), LEVEL_CHAPTER_8);

		if (nullptr == pjsonObjectsInfo)
			return E_FAIL;

		CSneak_MapObject::FLIPOBJECT_DESC ObjDesc = {};
		CSneak_MapObject* pObjectOut = { nullptr };
		_int iStageSize = pjsonObjectsInfo->size();
		vector<vector<CSneak_MapObject*>> StageObjects;

		for (_int i = 0; i < iStageSize; ++i)
		{
			vector<CSneak_MapObject*> Objects;

			for (_int j = 0; j < (*pjsonObjectsInfo)[i].size(); ++j)
			{
				if ((*pjsonObjectsInfo)[i][j].contains("Model"))
					ObjDesc.strModelPrototypeTag_2D = STRINGTOWSTRING((*pjsonObjectsInfo)[i][j]["Model"]);

				ObjDesc.tTransform2DDesc.vInitialPosition.x = (*pjsonObjectsInfo)[i][j]["Position"][0];
				ObjDesc.tTransform2DDesc.vInitialPosition.y = (*pjsonObjectsInfo)[i][j]["Position"][1];

				ObjDesc.iCurLevelID = LEVEL_CHAPTER_8;

				if ((*pjsonObjectsInfo)[i][j].contains("Animation"))
				{
					ObjDesc._iInitAnim = (*pjsonObjectsInfo)[i][j]["Animation"][0];
					ObjDesc._iFlipAnim1 = (*pjsonObjectsInfo)[i][j]["Animation"][1];
					ObjDesc._iFlipAnim1End = (*pjsonObjectsInfo)[i][j]["Animation"][2];
					ObjDesc._iFlipAnim2 = (*pjsonObjectsInfo)[i][j]["Animation"][3];
					ObjDesc._iFlipAnim2End = (*pjsonObjectsInfo)[i][j]["Animation"][4];
				}
				else
				{
					ObjDesc._iInitAnim = -1;
					ObjDesc._iFlipAnim1 = -1;
					ObjDesc._iFlipAnim1End = -1;
					ObjDesc._iFlipAnim2 = -1;
					ObjDesc._iFlipAnim2End = -1;
				}

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHAPTER_8, TEXT("Prototype_GameObject_SneakMapObject"), LEVEL_CHAPTER_8
					, TEXT("Layer_Sneak_Objects"), reinterpret_cast<CGameObject**>(&pObjectOut), &ObjDesc)))
					return E_FAIL;


				if (nullptr != pObjectOut)
				{
					Add_GameObject_ToSectionLayer(pObjectOut, SECTION_PIP_MAPOBJECT);

					Objects.push_back(pObjectOut);
				}
			}

			StageObjects.push_back(Objects);
		}
		CMinigame_Sneak::GetInstance()->Register_Objects(StageObjects);
	}	
#pragma endregion

#pragma region INTERACT_OBJECT
	{
		const json* pjsonInteractsInfo = m_pGameInstance->Find_Json_InLevel(TEXT("SneakInteracts"), LEVEL_CHAPTER_8);

		if (nullptr == pjsonInteractsInfo)
			return E_FAIL;

		CSneak_InteractObject::SNEAK_INTERACTOBJECT_DESC InteractDesc = {};
		CSneak_InteractObject* pInteractOut = { nullptr };
		_int iStageSize = pjsonInteractsInfo->size();
		vector<vector<CSneak_InteractObject*>> InteractObjects;

		for (_int i = 0; i < iStageSize; ++i)
		{
			vector<CSneak_InteractObject*> Objects;

			for (_int j = 0; j < (*pjsonInteractsInfo)[i].size(); ++j)
			{
				InteractDesc.iCurLevelID = LEVEL_CHAPTER_8;

				// Tile에 따른 위치 설정.
				if ((*pjsonInteractsInfo)[i][j].contains("TileIndex"))
				{
					InteractDesc._iTileIndex = (*pjsonInteractsInfo)[i][j]["TileIndex"];
				}

				_float2 vPosition = CMinigame_Sneak::GetInstance()->Get_TilePosition(i, InteractDesc._iTileIndex);

				InteractDesc.tTransform2DDesc.vInitialPosition.x = vPosition.x;
				InteractDesc.tTransform2DDesc.vInitialPosition.y = vPosition.y;

				if ((*pjsonInteractsInfo)[i][j].contains("Model"))
				{
					CSneak_InteractObject::INTERACTOBJECT_TYPE eType = (*pjsonInteractsInfo)[i][j]["Model"];

					// 감압판.
					if (CSneak_InteractObject::PRESSURE_PLATE == eType)
					{
						InteractDesc.strModelPrototypeTag_2D = TEXT("Sneak_PressurePlate");
						InteractDesc._isBlocked = false;
						InteractDesc._isBlockChangable = false;
						InteractDesc._isInteractable = false;
						InteractDesc._isCollisionInteractable = true;

						InteractDesc._iInitAnim = 2;
						InteractDesc._iFlipAnim1 = 0;
						InteractDesc._iFlipAnim1End = 1;
						InteractDesc._iFlipAnim2 = 3;
						InteractDesc._iFlipAnim2End = 2;

					}
					// 스위치
					else if (CSneak_InteractObject::SWITCH == eType)
					{
						InteractDesc.strModelPrototypeTag_2D = TEXT("Sneak_Switch");
						InteractDesc._isBlocked = true;
						InteractDesc._isBlockChangable = false;
						InteractDesc._eBlockDirection = F_DIRECTION::F_DIR_LAST;
						InteractDesc._isInteractable = true;
						InteractDesc._isCollisionInteractable = false;

						InteractDesc._iInitAnim = 2;
						InteractDesc._iFlipAnim1 = 1;
						InteractDesc._iFlipAnim1End = 3;
						InteractDesc._iFlipAnim2 = 0;
						InteractDesc._iFlipAnim2End = 2;
					}
					// V_DOOR
					else if (CSneak_InteractObject::V_DOOR == eType)
					{
						InteractDesc.strModelPrototypeTag_2D = TEXT("Sneak_Door");
						InteractDesc._isBlocked = (*pjsonInteractsInfo)[i][j]["Blocked"];
						InteractDesc._isBlockChangable = true;
						InteractDesc._eBlockDirection = F_DIRECTION::LEFT;
						InteractDesc._isInteractable = false;
						InteractDesc._isCollisionInteractable = false;

						if (InteractDesc._isBlocked)
							InteractDesc._iInitAnim = 1;
						else
							InteractDesc._iInitAnim = 3;

						InteractDesc._iFlipAnim1 = 0;
						InteractDesc._iFlipAnim1End = 1;
						InteractDesc._iFlipAnim2 = 2;
						InteractDesc._iFlipAnim2End = 3;

						InteractDesc.tTransform2DDesc.vInitialPosition.x -= 26.f;
						InteractDesc.tTransform2DDesc.vInitialPosition.y += 28.f;
					}
					// H_DOOR
					else if (CSneak_InteractObject::H_DOOR == eType)
					{
						InteractDesc.strModelPrototypeTag_2D = TEXT("Sneak_Door");
						InteractDesc._isBlocked = (*pjsonInteractsInfo)[i][j]["Blocked"];
						InteractDesc._isBlockChangable = true;
						InteractDesc._eBlockDirection = F_DIRECTION::UP;
						InteractDesc._isInteractable = false;
						InteractDesc._isCollisionInteractable = false;

						if (InteractDesc._isBlocked)
							InteractDesc._iInitAnim = 5;
						else
							InteractDesc._iInitAnim = 7;


						InteractDesc._iFlipAnim1 = 4;
						InteractDesc._iFlipAnim1End = 5;
						InteractDesc._iFlipAnim2 = 6;
						InteractDesc._iFlipAnim2End = 7;

						InteractDesc.tTransform2DDesc.vInitialPosition.x -= 0.f;
						InteractDesc.tTransform2DDesc.vInitialPosition.y += 55.f;
					}
				}

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHAPTER_8, TEXT("Prototype_GameObject_SneakInteractObject"), LEVEL_CHAPTER_8
					, TEXT("Layer_Sneak_Interacts"), reinterpret_cast<CGameObject**>(&pInteractOut), &InteractDesc)))
					return E_FAIL;

				if (nullptr != pInteractOut)
				{
					// 해당 오브젝트에 다른 Interacts가 포함되어 있거나, Tile을 알아야 할 경우.
					if ((*pjsonInteractsInfo)[i][j].contains("Tiles"))
					{
						for (_int k = 0; k < (*pjsonInteractsInfo)[i][j]["Tiles"].size(); ++k)
						{
							_int iTileIndex = (*pjsonInteractsInfo)[i][j]["Tiles"][k];

							CSneak_Tile* pTile = CMinigame_Sneak::GetInstance()->Get_Tile(i, iTileIndex);
							if (nullptr != pTile)
								pInteractOut->Register_Tiles(pTile);
						}
					}

					if ((*pjsonInteractsInfo)[i][j].contains("Objects"))
					{
						for (_int k = 0; k < (*pjsonInteractsInfo)[i][j]["Objects"].size(); ++k)
						{
							CSneak_InteractObject* pSubOut = { nullptr };

							// Tile에 따른 위치 설정.
							if ((*pjsonInteractsInfo)[i][j]["Objects"][k].contains("TileIndex"))
							{
								InteractDesc._iTileIndex = (*pjsonInteractsInfo)[i][j]["Objects"][k]["TileIndex"];
							}

							_float2 vPosition = CMinigame_Sneak::GetInstance()->Get_TilePosition(i, InteractDesc._iTileIndex);

							InteractDesc.tTransform2DDesc.vInitialPosition.x = vPosition.x;
							InteractDesc.tTransform2DDesc.vInitialPosition.y = vPosition.y;
							
							// TODO: Offset 줘야함.
							if ((*pjsonInteractsInfo)[i][j]["Objects"][k].contains("Model"))
							{
								CSneak_InteractObject::INTERACTOBJECT_TYPE eType = (*pjsonInteractsInfo)[i][j]["Objects"][k]["Model"];

								// 감압판.
								if (CSneak_InteractObject::PRESSURE_PLATE == eType)
								{
									InteractDesc.strModelPrototypeTag_2D = TEXT("Sneak_PressurePlate");
									InteractDesc._isBlocked = false;
									InteractDesc._isBlockChangable = false;
									InteractDesc._isInteractable = false;
									InteractDesc._isCollisionInteractable = true;

									InteractDesc._iInitAnim = 2;
									InteractDesc._iFlipAnim1 = 0;
									InteractDesc._iFlipAnim1End = 1;
									InteractDesc._iFlipAnim2 = 3;
									InteractDesc._iFlipAnim2End = 2;

								}
								// 스위치
								else if (CSneak_InteractObject::SWITCH == eType)
								{
									InteractDesc.strModelPrototypeTag_2D = TEXT("Sneak_Switch");
									InteractDesc._isBlocked = true;
									InteractDesc._isBlockChangable = false;
									InteractDesc._eBlockDirection = F_DIRECTION::F_DIR_LAST;
									InteractDesc._isInteractable = true;
									InteractDesc._isCollisionInteractable = false;

									InteractDesc._iInitAnim = 2;
									InteractDesc._iFlipAnim1 = 1;
									InteractDesc._iFlipAnim1End = 3;
									InteractDesc._iFlipAnim2 = 0;
									InteractDesc._iFlipAnim2End = 2;
								}
								// V_Door
								else if (CSneak_InteractObject::V_DOOR == eType)
								{
									InteractDesc.strModelPrototypeTag_2D = TEXT("Sneak_Door");
									InteractDesc._isBlocked = (*pjsonInteractsInfo)[i][j]["Objects"][k]["Blocked"];
									InteractDesc._isBlockChangable = true;
									InteractDesc._eBlockDirection = F_DIRECTION::LEFT;
									InteractDesc._isInteractable = false;
									InteractDesc._isCollisionInteractable = false;

									if (InteractDesc._isBlocked)
										InteractDesc._iInitAnim = 1;
									else
										InteractDesc._iInitAnim = 3;


									InteractDesc._iFlipAnim1 = 0;
									InteractDesc._iFlipAnim1End = 1;
									InteractDesc._iFlipAnim2 = 2;
									InteractDesc._iFlipAnim2End = 3;

									InteractDesc.tTransform2DDesc.vInitialPosition.x -= 26.f;
									InteractDesc.tTransform2DDesc.vInitialPosition.y += 28.f;
								}
								// H_Door
								else if (CSneak_InteractObject::H_DOOR == eType)
								{
									InteractDesc.strModelPrototypeTag_2D = TEXT("Sneak_Door");
									InteractDesc._isBlocked = (*pjsonInteractsInfo)[i][j]["Objects"][k]["Blocked"];
									InteractDesc._isBlockChangable = true;
									InteractDesc._eBlockDirection = F_DIRECTION::UP;
									InteractDesc._isInteractable = false;
									InteractDesc._isCollisionInteractable = false;

									if (InteractDesc._isBlocked)
										InteractDesc._iInitAnim = 5;
									else
										InteractDesc._iInitAnim = 7;


									InteractDesc._iFlipAnim1 = 4;
									InteractDesc._iFlipAnim1End = 5;
									InteractDesc._iFlipAnim2 = 6;
									InteractDesc._iFlipAnim2End = 7;

									InteractDesc.tTransform2DDesc.vInitialPosition.x -= 0.f;
									InteractDesc.tTransform2DDesc.vInitialPosition.y += 55.f;
								}
							}

							if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHAPTER_8, TEXT("Prototype_GameObject_SneakInteractObject"), LEVEL_CHAPTER_8
								, TEXT("Layer_Sneak_Interacts"), reinterpret_cast<CGameObject**>(&pSubOut), &InteractDesc)))
								return E_FAIL;

							if (nullptr != pSubOut)
							{
								if ((*pjsonInteractsInfo)[i][j]["Objects"][k].contains("NextGroup"))
									Add_GameObject_ToSectionLayer(pSubOut, SECTION_PIP_MOVEOBJECT);
								else
									Add_GameObject_ToSectionLayer(pSubOut, SECTION_PIP_MOVEMAPOBJECT);
								Objects.push_back(pSubOut);
								pInteractOut->Register_Objects(pSubOut);
							}
						}
					}

					if ((*pjsonInteractsInfo)[i][j].contains("NextGroup"))
						Add_GameObject_ToSectionLayer(pInteractOut, SECTION_PIP_MOVEOBJECT);
					else
						Add_GameObject_ToSectionLayer(pInteractOut, SECTION_PIP_MOVEMAPOBJECT);
					Objects.push_back(pInteractOut);
				}
			}

			InteractObjects.push_back(Objects);
		}
		CMinigame_Sneak::GetInstance()->Register_Interacts(InteractObjects);
	}


#pragma endregion INTERACT_OBJECT

#pragma region TROOPS
	{
		const json* pjsonTroopInfo = m_pGameInstance->Find_Json_InLevel(TEXT("SneakTroops"), LEVEL_CHAPTER_8);

		if (nullptr == pjsonTroopInfo)
			return E_FAIL;

		CSneak_Troop::SNEAK_TROOP_DESC TroopDesc = {};
		CSneak_Troop* pTroopOut = { nullptr };
		_int iStageSize = pjsonTroopInfo->size();
		vector<vector<CSneak_Troop*>> StageTroops;

		for (_int i = 0; i < iStageSize; ++i)
		{
			vector<CSneak_Troop*> Troops;

			for (_int j = 0; j < (*pjsonTroopInfo)[i].size(); ++j)
			{
				TroopDesc.iCurLevelID = LEVEL_CHAPTER_8;

				// Tile에 따른 위치 설정.
				if ((*pjsonTroopInfo)[i][j].contains("TileIndex"))
				{
					TroopDesc._iTileIndex = (*pjsonTroopInfo)[i][j]["TileIndex"];
				}

				_float2 vPosition = CMinigame_Sneak::GetInstance()->Get_TilePosition(i, TroopDesc._iTileIndex);

				TroopDesc.tTransform2DDesc.vInitialPosition.x = vPosition.x;
				TroopDesc.tTransform2DDesc.vInitialPosition.y = vPosition.y;


				if ((*pjsonTroopInfo)[i][j].contains("Moveable"))
					TroopDesc._isMoveable = (*pjsonTroopInfo)[i][j]["Moveable"];

				if ((*pjsonTroopInfo)[i][j].contains("Direction"))
					TroopDesc._eCurDirection = (*pjsonTroopInfo)[i][j]["Direction"];

				if ((*pjsonTroopInfo)[i][j].contains("TurnDirection"))
					TroopDesc._eTurnDirection = (*pjsonTroopInfo)[i][j]["TurnDirection"];
				else TroopDesc._eTurnDirection = F_DIRECTION::F_DIR_LAST;

				if ((*pjsonTroopInfo)[i][j].contains("RedDetect"))
					TroopDesc._isRedDetect = (*pjsonTroopInfo)[i][j]["RedDetect"];
				else
					TroopDesc._isRedDetect = false;

				if ((*pjsonTroopInfo)[i][j].contains("DetectCount"))
					TroopDesc._iDetectCount = (*pjsonTroopInfo)[i][j]["DetectCount"];
				else
					TroopDesc._iDetectCount = 3;


				if ((*pjsonTroopInfo)[i][j].contains("SecondDirection"))
					TroopDesc._eSecondTurnDirection = (*pjsonTroopInfo)[i][j]["SecondDirection"];
				else
					TroopDesc._eSecondTurnDirection = F_DIRECTION::F_DIR_LAST;


				if (TroopDesc._isRedDetect)
				{
					if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHAPTER_8, TEXT("Prototype_GameObject_SneakSentryTroop"), LEVEL_CHAPTER_8
						, TEXT("Layer_Sneak_Troops"), reinterpret_cast<CGameObject**>(&pTroopOut), &TroopDesc)))
						return E_FAIL;

				}
				else
				{
					if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHAPTER_8, TEXT("Prototype_GameObject_SneakTroop"), LEVEL_CHAPTER_8
						, TEXT("Layer_Sneak_Troops"), reinterpret_cast<CGameObject**>(&pTroopOut), &TroopDesc)))
						return E_FAIL;

				}

				if (nullptr != pTroopOut)
				{
					Add_GameObject_ToSectionLayer(pTroopOut, SECTION_PIP_MOVEOBJECT);
					Troops.push_back(pTroopOut);
				}
			}

			StageTroops.push_back(Troops);
		}
		CMinigame_Sneak::GetInstance()->Register_Troops(StageTroops);
	}

#pragma endregion TROOPS


#pragma region PIP_PLAYER
	CPip_Player::PIP_DESC PipDesc = {};
	CPip_Player* pPlayer = { nullptr };

	PipDesc.iCurLevelID = LEVEL_CHAPTER_8;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHAPTER_8, TEXT("Prototype_GameObject_PipPlayer"), LEVEL_CHAPTER_8
		, TEXT("Layer_Sneak_Player"), reinterpret_cast<CGameObject**>(&pPlayer), &PipDesc)))
		return E_FAIL;

	if (nullptr != pPlayer)
		Add_GameObject_ToSectionLayer(pPlayer, SECTION_PIP_MOVEOBJECT);

	CMinigame_Sneak::GetInstance()->Register_Player(pPlayer);
#pragma endregion

	//	Section_Level_Enter End
	// Level Object Create
	return S_OK;
}

HRESULT CSection_2D_MiniGame_Pip::Add_GameObject_ToSectionLayer(CGameObject* _pGameObject, _uint _iLayerIndex)
{
	// 섹션에 넣기 전

	// 실구현부 : CSection::Add_GameObject_ToSectionLayer
	HRESULT hr = __super::Add_GameObject_ToSectionLayer(_pGameObject, _iLayerIndex);
	
	//섹션에 넣기 후 
	return hr;
}

HRESULT CSection_2D_MiniGame_Pip::Section_AddRenderGroup_Process()
{
	// 매 프레임 렌더링 직전에 호출되는 함수
	
	
	// 오브젝트 그룹 소트
	Sort_Layer([](const CGameObject* pLeftGameObject, const CGameObject* pRightGameObject)->_bool {
		return XMVectorGetY(pLeftGameObject->Get_FinalPosition()) < XMVectorGetY(pRightGameObject->Get_FinalPosition());
		}, SECTION_PIP_MAPOBJECT);

	Sort_Layer([](const CGameObject* pLeftGameObject, const CGameObject* pRightGameObject)->_bool {
		return XMVectorGetY(pLeftGameObject->Get_FinalPosition()) >XMVectorGetY(pRightGameObject->Get_FinalPosition());
		}, SECTION_PIP_MOVEMAPOBJECT);


	Sort_Layer([](const CGameObject* pLeftGameObject, const CGameObject* pRightGameObject)->_bool {
		return XMVectorGetY(pLeftGameObject->Get_FinalPosition()) > XMVectorGetY(pRightGameObject->Get_FinalPosition());
		}, SECTION_PIP_MOVEOBJECT);

	// 레이어 렌더러 삽입
	if (FAILED(Add_RenderGroup_GameObjects()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSection_2D_MiniGame_Pip::Section_Enter(const _wstring& _strPreSectionTag)
{
	CMinigame_Sneak::GetInstance()->Start_Game();

	return S_OK;
}

HRESULT CSection_2D_MiniGame_Pip::Section_Exit(const _wstring& _strNextSectionTag)
{
	CPlayerData_Manager::GetInstance()->Set_CurrentPlayer(PLAYABLE_ID::NORMAL);
	return S_OK;
}


CSection_2D_MiniGame_Pip* CSection_2D_MiniGame_Pip::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pDesc)
{
	CSection_2D_MiniGame_Pip* pInstance = new CSection_2D_MiniGame_Pip(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_pDesc)))
	{
		MSG_BOX("Failed Create CSection_2D_MiniGame_Pip");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSection_2D_MiniGame_Pip::Free()
{
	__super::Free();
}
