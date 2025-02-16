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
	//플레이어가 가장 가까운 상호작용 가능한 오브젝트를 찾기 위해 사용됨.
	virtual _float Get_Distance(COORDINATE _eCoord, CPlayer* _pUser)abstract;
};

//예시  코드 : 상호작용하면 사망하는 NPC~
//class CNPC :public CContainerObject,  public IInteractable
//{
//	//상호작용 시 사망함.
//	virtual void Interact(CPlayer* _pUser) override
//	{
//		//사망 처리
//		Set_Dead();
//	}
//	//이미 죽어있다면 상호작용 불가능
//	virtual _bool Is_Interactable(CPlayer* _pUser) override
//	{
//		if (Is_Dead())
//			return false;
//		return true;
//	}
//	//플레이어와의 거리 반환.
//	virtual _float Get_Distance(CPlayer * _pUser) override
//	{
//		return XMVector3Length(Get_FinalPosition() - _pUser->Get_FinalPosition()).m128_f32[0];
//	}
//};
END