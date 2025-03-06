#pragma once
#include "ContainerObject.h"
/* 1. 캔들 컨테이너는 바디, UI 를 파트로 소유한다. 이펙트는 이펙트 구현현황에따라 파트로 할지 별도로 재생을 시킬지 결정한다. */
/* 2. 캔들 컨테이너는 자기 자신의 총 4가지 상태를 가진다. (Idle, TurnOn, FlameLoop, TurnOff)*/
BEGIN(Engine)
class CLight_Target;
END

BEGIN(Client)

class CCandle final: public CContainerObject
{
public:
	typedef struct tagCandleDesc : CContainerObject::CONTAINEROBJ_DESC
	{
		_float4 vCandleUIDesc = {}; // x : PosX, y : PosY, z : SizeX, w : SizeY

	}CANDLE_DESC;
public:
	enum CANDLE_PART { CANDLE_BODY, CANDLE_UI, CANDLE_LAST };
	enum STATE { STATE_FLAMELOOP, STATE_TURNOFF, STATE_TURNON, STATE_IDLE, STATE_LAST };
	// IDLE : 발화 하지 않은 기본 상태.
	// TURNON : 처음 발화 에 진입 하는 상태
	// FLAMELOOP : 발화 중 
	// TURNOFF : 촛불이 꺼지는 상태 (인게임에서는 이러한 상태는 존재 하지 않는 것을 보임 애니메이션은 있긴함. >>> 근데 꺼질때 알파 옅어지면서 꺼짐)
private:
	CCandle(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCandle(const CCandle& _Prototype);
	virtual ~CCandle() = default;

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize(void* _pArg) override;
	virtual void					Priority_Update(_float _fTimeDelta) override;
	virtual void					Update(_float _fTimeDelta) override;
	virtual void					Late_Update(_float _fTimeDelta) override;
	virtual HRESULT					Render() override;

public:
	virtual void					OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other);
	virtual void					OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other);
	virtual void					OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other);

public:
	// Get
	STATE							Get_CurState() const { return m_eCurState; }
	// Set 
private:
	STATE							m_ePreState = STATE::STATE_LAST;
	STATE							m_eCurState = STATE::STATE_LAST;

private:
	CLight_Target*					m_pTargetLight = nullptr;
private:
	void							State_Change();

	void							State_Change_Idle();
	void							State_Change_TurnOn();
	void							State_Change_FlameLoop();	// 사실 FlameLoop는 필요가없어.그냥 애니메이션 재생용이지.
	void							State_Change_TurnOff();

private:
	void							Action_State(_float _fTimeDelta);

	void							Action_State_Idle(_float _fTimeDelta);
	void							Action_State_TurnOn(_float _fTimeDelta);
	void							Action_State_FlameLoop(_float _fTimeDelta);
	void							Action_State_TurnOff(_float _fTimeDelta);

private:
	HRESULT							Ready_Components();
	HRESULT							Ready_PartObjects(CANDLE_DESC* _pDesc);

private: 
	HRESULT							Ready_TargetLight();

public:
	static CCandle*					Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCandle*						Clone(void* _pArg) override;
	void							Free() override;
};
END
