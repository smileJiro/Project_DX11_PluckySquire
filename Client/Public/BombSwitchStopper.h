#pragma once
#include "ModelObject.h"
#include "BombSwitch.h"

BEGIN(Client)
class CBombSwitchStopper :
	public CModelObject, public IBombSwitchReceiver
{
public:

public:
	typedef struct tagBombSwitchStopperDesc : public CModelObject::MODELOBJECT_DESC
	{

	}BOMB_SWITCH_STOPPER_DESC;
public:
	CBombSwitchStopper(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CBombSwitchStopper(const CBombSwitchStopper& _Prototype);
	virtual ~CBombSwitchStopper() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;

public:
	virtual void On_BombSwitch(_bool _bOn) override;
private:

public:
	static CBombSwitchStopper* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;

};

END