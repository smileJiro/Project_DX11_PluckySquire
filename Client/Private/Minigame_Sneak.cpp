#include "stdafx.h"
#include "Minigame_Sneak.h"
#include "GameInstance.h"
#include "Sneak_Tile.h"
#include "Sneak_DefaultObject.h"

IMPLEMENT_SINGLETON(CMinigame_Sneak)

CMinigame_Sneak::CMinigame_Sneak()
{
}

void CMinigame_Sneak::Set_Tiles(vector<vector<CSneak_Tile*>>& _Tiles)
{
	m_StageTiles = _Tiles;

	for (_int i = 0; i < m_StageTiles.size(); ++i)
	{
		for (auto& iter : m_StageTiles[i])
		{
			Safe_AddRef(iter);
		}
	}
}

void CMinigame_Sneak::Set_Objects(vector<vector<CSneak_DefaultObject*>>& _Objects)
{
	m_StageObjects = _Objects;

	for (_int i = 0; i < m_StageObjects.size(); ++i)
	{
		for (auto& iter : m_StageObjects[i])
		{
			Safe_AddRef(iter);
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
}
