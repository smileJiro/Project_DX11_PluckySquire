#pragma once
#include "Door_2D.h"
#include "BombSwitch.h"

BEGIN(Client)

class CDoor_Yellow : public CDoor_2D, public IBombSwitchReceiver
{
public:
	typedef struct tagDoorYellowDesc : public CDoor_2D::DOOR_2D_DESC
	{
		_bool	isPressurePlate = true;
		_float3 vPressurePlatePos = {};
	} DOOR_YELLOW_DESC;

private:
	CDoor_Yellow(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CDoor_Yellow(const CDoor_Yellow& _Prototype);
	virtual ~CDoor_Yellow() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void	Update(_float _fTimeDelta) override;
	virtual void	Late_Update(_float _fTimeDelta) override;

public:
	void			On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx);


private:
	class CPressure_Plate* m_pPressurePlate = { nullptr };

private:
	HRESULT			Ready_Part(const DOOR_YELLOW_DESC* _pDesc);
	void			Set_AnimLoop();
	virtual void	Switch_Animation_By_State() override;

	_bool			m_isPressurePlate = true;

public:
	static CDoor_Yellow* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;

	// IBombSwitchReceiver을(를) 통해 상속됨
	void On_BombSwitch(_bool _bOn) override;
};

END