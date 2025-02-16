#pragma once
#include "Base.h"
BEGIN(Client)
class CPlayer;
class IInteractable 
{
public:

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

	void Charge(_float _fTimeDelta) { m_fInteractChargeTimeAcc += _fTimeDelta; }
	_bool Is_ChargeComplete() { return m_fInteractChargeTimeAcc >= m_fInteractChargeTime; }
	_bool End_Charge() { return m_fInteractChargeTimeAcc  = 0; }
protected:
	_float m_fInteractChargeTimeAcc = 0.0f;
	//차징하는 데 걸리는 시간. 차징 타입이 아니면 0으로 두기.
	_float m_fInteractChargeTime = 0.0f;
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
//	virtual _float Get_Distance(COORDINATE _eCoord, CPlayer * _pUser) override
//	{
//		return XMVector3Length(m_pControllerTransform->Get_Transform(_eCoord)->Get_State(CTransform::STATE_POSITION)
//			-_pUser->Get_ControllerTransform()->Get_Transform(_eCoord)->Get_State(CTransform::STATE_POSITION)).m128_f32[0];
//	}
//};
END