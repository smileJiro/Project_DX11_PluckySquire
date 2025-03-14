#pragma once
#include "ModelObject.h"

BEGIN(Client)

class CNpc_Humgrump : public CModelObject
{
public:
	enum HUMGRUMP_ANIM_STATE
	{
		CHAPTER6_IDLE = 52,
		CHAPTER6_TALK = 42, 
		CHAPTER6_GIVE_IT_AREST = 47, 
		CHAPTER6_GIVE_IT_AREST_IDLE = 48,
		CHAPTER6_SURPRISED = 53,
		CHAPTER6_SURPRISED_TURN = 58,
		CHAPTER6_SURPRISED_IDLE = 59,
		CHAPTER6_ZAP_INTO = 63,
		CHAPTER6_ZAP_IDLE = 64,
		CHAPTER6_ZAP_DEATH = 65,

		HUMGRUMP_ANIM_STATE_END
	};

protected:
	CNpc_Humgrump(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CNpc_Humgrump(const CNpc_Humgrump& _Prototype);
	virtual ~CNpc_Humgrump() = default;

public:
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	static CNpc_Humgrump*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg) override;
	virtual void			Free() override;
};

END
