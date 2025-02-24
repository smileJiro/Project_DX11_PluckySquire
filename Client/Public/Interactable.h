#pragma once
#include "Base.h"
#include "Key_Manager.h"

BEGIN(Client)
enum class INTERACT_RESULT
{
	SUCCESS,//Interact가 호출됨.
	FAIL,//Charging도 아닌데 Interact가 호출되지 않음.
	CHARGING,//Charging 중
	PLAYER_INTERACT_RESULT_LAST
};
class CPlayer;
class IInteractable 
{
public:
	enum INTERACT_TYPE
	{
		NORMAL,//키가 눌리면 Interact 호출
		CHARGE, // 키가 일정시간 이상 눌리면 Interact 호출
		HOLDING, // 키가 눌리는 동안 매 번 Interact 호출
		INTERACT_TYPE_LAST
	};
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

	_bool Is_ChargeCompleted() { return m_fInteractTimeAcc >= m_fInteractChargeTime; }
	_bool Is_Interacting() {return m_fInteractTimeAcc > 0.f;}
	_float Get_ChargeProgress() { return m_fInteractTimeAcc / m_fInteractChargeTime; }
	KEY Get_InteractKey() { return m_eInteractKey; }	
	INTERACT_TYPE Get_InteractType() { return m_eInteractType; }

	//사용법 : 플레이어의 Interact 인풋을 확인하고 Try_Interact를 호출.
	INTERACT_RESULT Try_Interact(CPlayer* _pPlayer, _float _fTimeDelta)
	{
		if (nullptr == _pPlayer)
			return INTERACT_RESULT::FAIL;

		IInteractable::INTERACT_TYPE eInteractType = Get_InteractType();
		if (IInteractable::INTERACT_TYPE::NORMAL == eInteractType)
		{
			Interact(_pPlayer);
			return INTERACT_RESULT::SUCCESS;
		}
		else if (IInteractable::INTERACT_TYPE::CHARGE == eInteractType)
		{
			if (Is_ChargeCompleted())
			{
				End_Interact(_pPlayer);
				Interact(_pPlayer);
				return INTERACT_RESULT::SUCCESS;
			}
			else
			{
				Charge(_pPlayer, _fTimeDelta);
				return INTERACT_RESULT::CHARGING;
			}

		}
		else if (IInteractable::INTERACT_TYPE::HOLDING == eInteractType)
		{
			Holding(_pPlayer, _fTimeDelta);
			Interact(_pPlayer);
			return INTERACT_RESULT::SUCCESS;
		}

	}
	void End_Interact(CPlayer* _pPlayer)
	{
		On_ChargeCanceled(_pPlayer);
		m_fInteractTimeAcc = 0.f;
	}
protected:
	void Holding(CPlayer* _pPlayer, _float _fTimeDelta)
	{
		if (m_fInteractTimeAcc == 0.f)
			On_Touched(_pPlayer);
		m_fInteractTimeAcc += _fTimeDelta;
	}
	_bool End_Holding(CPlayer* _pPlayer)
	{
		On_EndHolding(_pPlayer);
		return m_fInteractTimeAcc = 0.f;
	}
	void Charge(CPlayer* _pPlayer, _float _fTimeDelta)
	{
		if (m_fInteractTimeAcc == 0.f)
			On_Touched(_pPlayer);
		m_fInteractTimeAcc += _fTimeDelta;
	}


	//플레이어가 건드렸을 때(상호작용 버튼을 눌렀을 때) 호출됨. Charge가 시작된 경우를 의미.
	virtual void On_Touched(CPlayer* _pPlayer) {}
	//플레이어가 상호작용을 취소했을 때 호출됨.
	virtual void On_ChargeCanceled(CPlayer* _pPlayer) {}
	virtual void On_EndHolding(CPlayer* _pPlayer) {}
protected:
	_float m_fInteractTimeAcc = 0.0f;
	//차징하는 데 걸리는 시간. 차징 타입이 아니면 0으로 두기.
	_float m_fInteractChargeTime = 0.0f;

	INTERACT_TYPE m_eInteractType = INTERACT_TYPE::NORMAL;
	KEY m_eInteractKey = KEY::E;
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