#pragma once
#include "Base.h"

BEGIN(Client)
class CPlayer;
class IInteractable 
{
public:
	virtual void Interact(CPlayer* _pUser)abstract;
	virtual _bool Is_Interactable(CPlayer* _pUser)abstract;
	virtual _float Get_Distance(CPlayer* _pUser)abstract;
};

END