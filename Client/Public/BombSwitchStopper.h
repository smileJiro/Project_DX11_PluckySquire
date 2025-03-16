#pragma once
#include "ModelObject.h"
#include "BombSwitch.h"

BEGIN(Client)
class CBombSwitchStopper :
	public CModelObject, public IBombSwitchReceiver
{
public:
	enum STOPPER_TYPE
	{
		RECT,
		SQUARE,
	};
	enum ANIM_STATE
	{
		RECT_DOWN,
		RECT_MOVE_DOWN,
		RECT_MOVE_UP,
		RECT_UP,
		SQUARE_DOWN,
		SQUARE_MOVE_DOWN,
		SQUARE_MOVE_UP,
		SQUARE_UP,
	};
	enum STOPPER_STATE
	{
		UP,
		DOWN,
		STOPPER_STATE_LAST
	};
public:
	typedef struct tagBombSwitchStopperDesc : public CModelObject::MODELOBJECT_DESC
	{
		STOPPER_TYPE eType = RECT;
		STOPPER_STATE eInitialState = UP;
	}BOMB_SWITCH_STOPPER_DESC;
public:
	CBombSwitchStopper(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CBombSwitchStopper(const CBombSwitchStopper& _Prototype);
	virtual ~CBombSwitchStopper() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	virtual HRESULT Render() override;
public:
	virtual void On_BombSwitch(_bool _bOn) override;
	void On_AnimEnd(COORDINATE _eCoord, _uint _iAnimIdx);
	void Set_State(STOPPER_STATE _eState);
private:
	STOPPER_TYPE m_eType = RECT;
	STOPPER_STATE m_eState = STOPPER_STATE_LAST;
public:
	static CBombSwitchStopper* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;

};

END