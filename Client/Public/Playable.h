#pragma once
#include "Character.h"
BEGIN(Client)
enum class PLAYALBE_ID
{
    NORMAL,
	DEFENDER,
    LAST
};
class CPlayable :
    public CCharacter
{
protected:
	explicit CPlayable(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, PLAYALBE_ID _ePlayableID);
	explicit CPlayable(const CPlayable& _Prototype);
	virtual ~CPlayable() = default;

public:
	//GET
	_bool Is_PlayerInputBlocked() { return m_bBlockInput; }
	PLAYALBE_ID Get_PlayableID() { return m_ePlayableID; }
	//SET
	void Set_BlockPlayerInput(_bool _bBlock) { m_bBlockInput = _bBlock; }
private:
	PLAYALBE_ID m_ePlayableID = PLAYALBE_ID::LAST;
	_bool m_bBlockInput = false;
};

END