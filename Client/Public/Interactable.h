#pragma once
#include "Base.h"

BEGIN(Client)
class CPlayer;
class IInteractable 
{
public:
	//상호작용 성공했을 때 호출됨.
	//상호작용 시 동작할 내용을 구현
	virtual void Interact(CPlayer* _pUser)abstract;
	//상호작용 가능한 상태인지 반환하는 함수.
	//자신이 상호작용 불가능한 상태면 false 반환
	virtual _bool Is_Interactable(CPlayer* _pUser)abstract;
	//사용자와의 거리를 반환하는 함수. 
	virtual _float Get_Distance(CPlayer* _pUser)abstract;
};

END