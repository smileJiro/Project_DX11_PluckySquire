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
class CSneak_Troop;

class CMinigame_Sneak : public CBase
{
	DECLARE_SINGLETON(CMinigame_Sneak)
public:
	enum SNEAK_GAME_STATE { NONE, START, PROGRESS, GAME_OVER, RESTART, STAGE_VICTORY, GAME_END };
private:
	CMinigame_Sneak();
	virtual ~CMinigame_Sneak() = default;

public:
	void	Start_Game();
	void	Update(_float _fTimeDelta);

public:
	void	Reach_Destination(_int _iPreIndex, _int _iDestIndex);
	void	Restart_Game();
	void	GameOver();		// 
	void	EndGame();
		
public:
	HRESULT Register_Tiles(vector<vector<CSneak_Tile*>>& _Tiles);
	HRESULT Register_Objects(vector<vector<CSneak_MapObject*>>& _Objects);
	HRESULT Register_Player(class CPip_Player* _pPlayer);
	HRESULT Register_Interacts(vector<vector<CSneak_InteractObject*>>& _Interacts);
	HRESULT Register_Troops(vector<vector<CSneak_Troop*>>& _Troops);

public:
#ifdef _DEBUG
	_float							Get_InputTime() const { return m_fAccTime; }
#endif
	_bool							Is_InputTime(_float _fTimeDelta) { if (PROGRESS == m_eGameState && 0 <= m_iCurStage && 0.32f <= m_fAccTime + _fTimeDelta) return true; return false; }
	_int							Get_NextTile(_int _iCurTile, F_DIRECTION _eDirection);
	_float2							Get_TilePosition(_int _iStage, _int _iIndex);
	_float2							Get_CurStageTilePosition(_int _iIndex);
	class CSneak_Tile*				Get_Tile(_int _iStage, _int _iIndex);
	class CSneak_InteractObject*	Get_InteractObject(_int _iStage, _int _iIndex);
	void							Update_DetectionField();
	void							Detect_Tiles(_Inout_ _int* _pTiles, _int _iDetectCount, _int _iCurIndex, F_DIRECTION _eDirection, _bool _isRed = false);
	void							DetectOff_Tiles(_Inout_ _int* _pTiles, _int _iDetectCount, _bool _isRed = false);
	_int							Get_PlayerTile();
	_bool							Is_MoveableTile(_int _iTileIndex);

private:	
	SNEAK_GAME_STATE m_eGameState = { NONE };
	_int	m_iCurStage = -1;
	_int	m_iFlipTime = 0;
	_float	m_fAccTime = 0.f;

private:
	class CGameInstance*			m_pGameInstance = { nullptr };
	class CPip_Player*				m_pPlayer = { nullptr };

	vector<vector<CSneak_Tile*>> m_StageTiles;
	vector<vector<CSneak_MapObject*>> m_StageObjects;
	vector<vector<CSneak_InteractObject*>> m_StageInteracts;
	vector<vector<CSneak_Troop*>> m_StageTroops;

private:
	void	Start_Stage();

	void	Before_Action();
	void	Action();
	void	Action_Player();
	void	Action_Troops();

	_bool	Is_TroopsBlocked(_int _iTileIndex);
	_bool	Is_Blocked(_int _iCurIndex, _int _iNextIndex, F_DIRECTION _eMoveDir, _Out_ class CSneak_InteractObject** _ppOutObject);
public:
	virtual void Free() override;
};

END