#pragma once
#include "ModelObject.h"

BEGIN(Client)
class IBombSwitchReceiver
{
public:
	void Switch_Bomb(_bool _bOn) 
	{ 
		m_bBombSwitchOn = _bOn;
		On_BombSwitch(_bOn);
	}
	virtual void On_BombSwitch(_bool _bOn) = 0;

	_bool Is_BombSwitchOn() { return m_bBombSwitchOn; }
private:
	_bool m_bBombSwitchOn = false;
};
class CBombSwitch :
	public CModelObject
{
public:
	enum BOMB_SWITCH_STATE
	{
		OFF, ON, LAST
	};
	typedef struct tagBombSwitchDesc : public CModelObject::MODELOBJECT_DESC
	{
		IBombSwitchReceiver* pReceivers = nullptr;
		BOMB_SWITCH_STATE eStartState = OFF;
	}BOMB_SWITCH_DESC;
public:
	CBombSwitch(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CBombSwitch(const CBombSwitch& _Prototype);
	virtual ~CBombSwitch() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	virtual HRESULT Render()override;

public:
	virtual void On_Hit(CGameObject* _pHitter, _int _fDamg, _fvector _vForce) override;

	void Set_SwitchState(BOMB_SWITCH_STATE _eState);
	void Add_Receiver(IBombSwitchReceiver* _pReceiver) { m_pReceivers.push_back(_pReceiver); }
private:
	list<IBombSwitchReceiver*> m_pReceivers ;
	BOMB_SWITCH_STATE m_eBombSwitchOn = LAST;
public:
	static CBombSwitch* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;

};

END