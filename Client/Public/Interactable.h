#pragma once
#include "Base.h"
#include "Key_Manager.h"

BEGIN(Client)
enum class INTERACT_RESULT
{
	INTERACT_START,
	INTERACT_END,
	SUCCESS,//Interact가 호출됨.
	FAIL,//Charging도 아닌데 Interact가 호출되지 않음.
	CHARGING,//Charging 중
	CHARGE_CANCEL,//Charging 중
	NO_INPUT,
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
	_bool Is_Interacting() { return m_bInteracting;}
	_float Get_ChargeProgress() { return m_fInteractTimeAcc / m_fInteractChargeTime; }
	KEY Get_InteractKey() { return m_eInteractKey; }	
	INTERACT_TYPE Get_InteractType() { return m_eInteractType; }
	void Set_Interacting(_bool _bInteracting) { m_bInteracting = _bInteracting; }
	
	void Start_Interact(CPlayer* _pPlayer)
	{
		On_InteractionStart(_pPlayer);
		m_fInteractTimeAcc = 0.f;
		m_bInteracting = true;
	}
	void Pressing(CPlayer* _pPlayer, _float _fTimeDelta)
	{
		On_Pressing(_pPlayer, _fTimeDelta);
		m_fInteractTimeAcc += _fTimeDelta;
		m_bInteracting = true;
	}
	void End_Interact(CPlayer* _pPlayer)
	{
		On_InteractionEnd(_pPlayer);
		m_fInteractTimeAcc = 0.f;
		m_bInteracting = false;
	}

	wstring Get_InteractName() { return m_strInteractName; } // 인터렉트가 된 대상의 이름을 가져온다.
	_bool Is_UIPlayerHeadUp() { return m_bUIPlayerHeadUp; }	// 유저의 머리위에 노출시킬것인가 아닌가
protected:

	//플레이어가 건드렸을 때(상호작용 버튼을 눌렀을 때) 호출됨. Charge가 시작된 경우를 의미.
	virtual void On_InteractionStart(CPlayer* _pPlayer) {}
	virtual void On_Pressing(CPlayer* _pPlayer, _float _fTimeDelta){}
	//플레이어가 상호작용을 취소했을 때 호출됨.
	virtual void On_InteractionEnd(CPlayer* _pPlayer) {}
protected:
	_float m_fInteractTimeAcc = 0.0f;
	//차징하는 데 걸리는 시간. 차징 타입이 아니면 0으로 두기.
	_float m_fInteractChargeTime = 0.0f;

	INTERACT_TYPE m_eInteractType = INTERACT_TYPE::NORMAL;
	KEY m_eInteractKey = KEY::E;
	_bool m_bInteracting = false;

	wstring m_strInteractName = TEXT("");
	_bool m_bUIPlayerHeadUp = false;
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