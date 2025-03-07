#pragma once
#include "Base.h"
#include "Client_Defines.h"


BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CSneak_Tile;
class CSneak_MapObject;
class CSneak_InteractObject;

class CMinigame_Sneak : public CBase
{
	DECLARE_SINGLETON(CMinigame_Sneak)
public:
	
private:
	CMinigame_Sneak();
	virtual ~CMinigame_Sneak() = default;

public:
	void	Start_Game();
	void	Update(_float _fTimeDelta);

public:
	void	Reach_Destination(_int _iPreIndex, _int _iDestIndex);

public:
	HRESULT Register_Tiles(vector<vector<CSneak_Tile*>>& _Tiles);
	HRESULT Register_Objects(vector<vector<CSneak_MapObject*>>& _Objects);
	HRESULT Register_Player(class CPip_Player* _pPlayer);
	HRESULT Register_Interacts(vector<vector<CSneak_InteractObject*>> _Interacts);

	_bool	Is_InputTime() { return (0 <= m_iNowStage && 0.35f <= m_fAccTime); }
	_float2 Get_TilePosition(_int _iStage, _int _iIndex);
	class CSneak_Tile* Get_Tile(_int _iStage, _int _iIndex);

private:
	_int	m_iNowStage = -1;
	_int	m_iFlipTime = 0;
	_bool	m_isStartGame = { false };
	_float	m_fAccTime = 0.f;

private:
	class CGameInstance*			m_pGameInstance = { nullptr };
	class CPip_Player*				m_pPlayer = { nullptr };

	vector<vector<CSneak_Tile*>> m_StageTiles;
	vector<vector<CSneak_MapObject*>> m_StageObjects;
	vector<vector<CSneak_InteractObject*>> m_StageInteracts;

private:
	void	Start_Stage();

	void	Before_Action();
	void	Action();

public:
	virtual void Free() override;
};

END