#include "stdafx.h"
#include "Playable.h"

CPlayable::CPlayable(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, PLAYALBE_ID _ePlayableID)
	:CCharacter(_pDevice, _pContext)
	, m_ePlayableID(_ePlayableID)
{
}

CPlayable::CPlayable(const CPlayable& _Prototype)
	:CCharacter(_Prototype)
	, m_ePlayableID(_Prototype.m_ePlayableID)
{
}
