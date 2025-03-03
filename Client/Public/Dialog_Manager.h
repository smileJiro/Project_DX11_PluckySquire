#pragma once
#include "Dialogue.h"
#include "NPC.h"



BEGIN(Client)
class CDialog_Manager final : public CBase
{
	DECLARE_SINGLETON(CDialog_Manager)

private:
	CDialog_Manager();
	virtual ~CDialog_Manager() = default;

private:
	CGameInstance*	m_pGameInstance;
	CDialog*		m_pDialogue = { nullptr };
	
	_bool			m_isDisPlayDialogue = { false };
	CNPC* m_pNPC = { nullptr };

	
public:
	_bool								Get_DisPlayDialogue() { return m_pDialogue->Get_DisPlayDialogue(); }
	void								Set_DisPlayDialogue(_bool _isDisplay) { m_pDialogue->Set_DisPlayDialogue(_isDisplay); }
	void								Set_DialogId(const _tchar* _id, const _tchar* _strCurSection = nullptr, _bool _DisplayDialogue = true, _bool _DisPlayPortrait = true);
	CDialog*							Get_Dialog() { return m_pDialogue; }

	void								Set_Dialog(CDialog* _pDialog) { m_pDialogue = _pDialog; Safe_AddRef(_pDialog);}
	void								Set_NPC(CNPC* _npc) { m_pNPC = _npc; Safe_AddRef(_npc); }
	void								Set_ActionChange(_int _AnimIndex) { m_pNPC->Set_SocialNpcAnimationIndex(_AnimIndex); }

	void								Set_DialogEnd();
	void								Set_DialoguePos(_float3 _vPos) { m_pDialogue->Set_DialoguePos(_vPos); }

public:
	HRESULT								Level_Exit(_int iCurLevelID, _int _iChangeLevelID, _int _iNextChangeLevelID);
	HRESULT								Level_Logo_Exit(_int _iChangeLevelID, _int _iNextChangeLevelID);
	HRESULT								Level_Enter(_int _iChangeLevelID);


public:
	virtual void Free() override;

};

END