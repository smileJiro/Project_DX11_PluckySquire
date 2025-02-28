#pragma once
#include "Dialogue.h"
#include "NPC.h"
#include "Npc_OnlySocial.h"



BEGIN(Client)
class CNPC_Manager final : public CBase
{
	DECLARE_SINGLETON(CNPC_Manager)

private:
	CNPC_Manager();
	virtual ~CNPC_Manager() = default;

private:
	CNPC* m_pNPC = { nullptr };
	CNPC_OnlySocial* m_pOnlyNpc = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };


	
public:


	void								Set_NPC(CNPC* _npc) { m_pNPC = _npc; Safe_AddRef(_npc); }
	void								Set_OnlyNpc(CNPC_OnlySocial* _npc) { m_pOnlyNpc = _npc; Safe_AddRef(_npc); }

public:
	HRESULT								Level_Exit(_int iCurLevelID, _int _iChangeLevelID, _int _iNextChangeLevelID);
	HRESULT								Level_Logo_Exit(_int _iChangeLevelID, _int _iNextChangeLevelID);
	HRESULT								Level_Enter(_int _iChangeLevelID);


public:
	virtual void Free() override;

};

END