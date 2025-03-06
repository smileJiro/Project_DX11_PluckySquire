#pragma once
#include "Base.h"
#include "Client_Defines.h"


BEGIN(Client)

class CSneak_Tile;
class CSneak_DefaultObject;

class CMinigame_Sneak : public CBase
{
	DECLARE_SINGLETON(CMinigame_Sneak)

private:
	CMinigame_Sneak();
	virtual ~CMinigame_Sneak() = default;

public:
	void Set_Tiles(vector<vector<CSneak_Tile*>>& _Tiles);
	void Set_Objects(vector<vector<CSneak_DefaultObject*>>& _Objects);
	
private:
	_int	m_iNowStage = 0;
	vector<vector<CSneak_Tile*>> m_StageTiles;
	vector<vector<CSneak_DefaultObject*>> m_StageObjects;

public:
	virtual void Free() override;
};

END