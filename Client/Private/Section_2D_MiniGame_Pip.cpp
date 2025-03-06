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
#include "Sneak_DefaultObject.h"
#include "Sneak_Drawer.h"

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

			for (_int k = 0; k < CSneak_Tile::LAST; ++k)
			{
				if (-1 == TileDesc.iAdjacents[k] || TileDesc.iAdjacents[k] >= Tiles.size())
					continue;

				if (nullptr == Tiles[TileDesc.iAdjacents[k]])
					continue;

				vPosition = Tiles[TileDesc.iAdjacents[k]]->Get_TilePosition();

				switch (k)
				{
				case CSneak_Tile::LEFT:
					TileDesc.tTransform2DDesc.vInitialPosition.x = vPosition.x + 65.f;
					TileDesc.tTransform2DDesc.vInitialPosition.y = vPosition.y;
					break;
				case CSneak_Tile::RIGHT:
					TileDesc.tTransform2DDesc.vInitialPosition.x = vPosition.x - 65.f;
					TileDesc.tTransform2DDesc.vInitialPosition.y = vPosition.y;
					break;
				case CSneak_Tile::UP:
					TileDesc.tTransform2DDesc.vInitialPosition.x = vPosition.x;
					TileDesc.tTransform2DDesc.vInitialPosition.y = vPosition.y - 65.f;
					break;
				case CSneak_Tile::DOWN:
					TileDesc.tTransform2DDesc.vInitialPosition.x = vPosition.x;
					TileDesc.tTransform2DDesc.vInitialPosition.y = vPosition.y + 65.f;
					break;
				default:
					break;
				}

				break;
			}

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

			if (nullptr != pTileOut)
			{
				Add_GameObject_ToSectionLayer(pTileOut, SECTION_PIP_TILE);
				Tiles[j] = pTileOut;
			}

		}
		StageTiles.push_back(Tiles);
	}	

	CMinigame_Sneak::GetInstance()->Set_Tiles(StageTiles);
#pragma endregion TILE

#pragma region SNEAK OBJECT
	const json* pjsonObjectsInfo = m_pGameInstance->Find_Json_InLevel(TEXT("SneakObjects"), LEVEL_CHAPTER_8);

	if (nullptr == pjsonObjectsInfo)
		return E_FAIL;

	CSneak_DefaultObject::MODELOBJECT_DESC ObjDesc = {};
	CSneak_DefaultObject* pObjectOut = { nullptr };
	iStageSize = pjsonObjectsInfo->size();
	vector<vector<CSneak_DefaultObject*>> StageObjects;

	for (_int i = 0; i < iStageSize; ++i)
	{
		vector<CSneak_DefaultObject*> Objects;

		for (_int j = 0; j < (*pjsonObjectsInfo)[i].size(); ++j)
		{
			if ((*pjsonObjectsInfo)[i][j].contains("Model"))
				ObjDesc.strModelPrototypeTag_2D = STRINGTOWSTRING((*pjsonObjectsInfo)[i][j]["Model"]);

			ObjDesc.tTransform2DDesc.vInitialPosition.x = (*pjsonObjectsInfo)[i][j]["Position"][0];
			ObjDesc.tTransform2DDesc.vInitialPosition.y = (*pjsonObjectsInfo)[i][j]["Position"][1];

			ObjDesc.iCurLevelID = LEVEL_CHAPTER_8;

			if ((*pjsonObjectsInfo)[i][j].contains("Drawer"))
			{
				CSneak_Drawer::DRAWER_DESC DrawerDesc = {};
				DrawerDesc.iCurLevelID = LEVEL_CHAPTER_8;
				DrawerDesc.tTransform2DDesc = ObjDesc.tTransform2DDesc;
				DrawerDesc.isBright = (*pjsonObjectsInfo)[i][j]["Drawer"];

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHAPTER_8, TEXT("Prototype_GameObject_SneakDrawer"), LEVEL_CHAPTER_8
					, TEXT("Layer_Sneak_Objects"), reinterpret_cast<CGameObject**>(&pObjectOut), &DrawerDesc)))
					return E_FAIL;
			}

			else
			{
				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHAPTER_8, TEXT("Prototype_GameObject_SneakMapObject"), LEVEL_CHAPTER_8
					, TEXT("Layer_Sneak_Objects"), reinterpret_cast<CGameObject**>(&pObjectOut), &ObjDesc)))
					return E_FAIL;
			}


			if (nullptr != pObjectOut)
			{
				if ((*pjsonObjectsInfo)[i][j].contains("NextGroup"))
				{
					Add_GameObject_ToSectionLayer(pObjectOut, SECTION_PIP_MAPOBJECT_2);
				}
				else
					Add_GameObject_ToSectionLayer(pObjectOut, SECTION_PIP_MAPOBJECT);

				Objects.push_back(pObjectOut);
			}
		}

		StageObjects.push_back(Objects);
	}
	CMinigame_Sneak::GetInstance()->Set_Objects(StageObjects);
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
		return XMVectorGetY(pLeftGameObject->Get_FinalPosition()) > XMVectorGetY(pRightGameObject->Get_FinalPosition());
		}, SECTION_2D_PLAYMAP_OBJECT);

	// 레이어 렌더러 삽입
	if (FAILED(Add_RenderGroup_GameObjects()))
		return E_FAIL;

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
