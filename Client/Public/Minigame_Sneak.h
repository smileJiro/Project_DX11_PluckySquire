#pragma once
#include "Base.h"
#include "Client_Defines.h"


BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CSneak_Tile;
class CSneak_DefaultObject;

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

	HRESULT Register_Tiles(vector<vector<CSneak_Tile*>>& _Tiles);
	HRESULT Register_Objects(vector<vector<CSneak_DefaultObject*>>& _Objects);
	HRESULT Register_Player(class CPip_Player* _pPlayer);


private:
	_int	m_iNowStage = -1;
	_bool	m_isStartGame = { false };
	_float	m_fAccTime = 0.f;

private:
	class CGameInstance*			m_pGameInstance = { nullptr };
	class CPip_Player*				m_pPlayer = { nullptr };

	vector<vector<CSneak_Tile*>> m_StageTiles;
	vector<vector<CSneak_DefaultObject*>> m_StageObjects;

private:
	void	Start_Stage();

	void	Before_Action();
	void	Action();

public:
	virtual void Free() override;
};

END