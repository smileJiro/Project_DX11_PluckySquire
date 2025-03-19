#pragma once
#include "Dialogue.h"
#include "NPC.h"
#include "Npc_OnlySocial.h"
#include "NPC_Social.h"
#include "WorldMapNPC.h"


BEGIN(Client)
class CNPC_Manager final : public CBase
{
	DECLARE_SINGLETON(CNPC_Manager)

private:
	CNPC_Manager();
	virtual ~CNPC_Manager() = default;

private:
	CWorldMapNPC* m_pWorldNpc = { nullptr };
	CNPC* m_pNPC = { nullptr };
	CNPC_OnlySocial* m_pOnlyNpc = { nullptr };
	vector<CNPC_Social*> m_pNpcSocials;
	CGameInstance* m_pGameInstance = { nullptr };


	
public:


	void								Set_NPC(CNPC* _npc) { m_pNPC = _npc; Safe_AddRef(_npc); }
	void								Set_OnlyNpc(CNPC_OnlySocial* _npc) { m_pOnlyNpc = _npc; Safe_AddRef(_npc); }
	void								SocialNpc_PushBack(CNPC_Social* _SocialNpc) 
	{ 
		m_pNpcSocials.push_back(_SocialNpc); 
		Safe_AddRef(_SocialNpc); 
	}
	CNPC_Social*						Find_SocialNPC(wstring _NPCName);
	void								Set_WorldmapNpc(CWorldMapNPC* _WorldNpc) { m_pWorldNpc = _WorldNpc; Safe_AddRef(_WorldNpc); }
	CWorldMapNPC*						Get_WorldmapNpc()
	{
		//if (nullptr == m_pWorldNpc)
		//	assert(m_pWorldNpc);

		return m_pWorldNpc;
	}

	HRESULT								ChangeDialogue(_wstring _NPCName, _wstring _DialogueID);
	HRESULT								Remove_SocialNPC(_wstring _NPCName);


public:
	HRESULT								Level_Exit(_int iCurLevelID, _int _iChangeLevelID, _int _iNextChangeLevelID);
	HRESULT								Level_Logo_Exit(_int _iChangeLevelID, _int _iNextChangeLevelID);
	HRESULT								Level_Enter(_int _iChangeLevelID);


public:
	virtual void Free() override;

};

END