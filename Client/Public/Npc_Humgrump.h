#pragma once
#include "ModelObject.h"

BEGIN(Client)

class CNpc_Humgrump : public CModelObject
{
public:
	enum HUMGRUMP_ANIM_STATE
	{
		CHAPTER6_IDLE = 94,
		CHAPTER6_TALK = 96,
		CHAPTER6_TALK_STAND = 97,
		CHAPTER6_FINGER_RAISE = 75,
		CHAPTER6_FINGER_TALK = 76,
		CHAPTER6_FINGER_DOWN = 73,
		CHAPTER6_WINDUP = 104,
		CHAPTER6_BEAM = 70,
		CHAPTER6_BEAM_END = 72,
		CHAPTER6_TRANSFORM_IDLE = 99,
		CHAPTER6_TRANSFORM_TURN = 100,
		CHAPTER6_TRANSFORM_TURN_TALK = 102,
		CHAPTER6_TRANSFORM_LAUGH_INTO = 92,
		CHAPTER6_TRANSFORM_LAUGH_IDLE = 93,

		CHAPTER8_IDLE_UP = 89,
		CHAPTER8_TURN = 103,
		CHAPTER8_TALK_HAPPY = 67,
		CHAPTER8_IDLE = 87,
		CHAPTER8_TRANSFORM = 98,
		CHAPTER8_TRANSFORM_TALK = 95,
		CHAPTER8_BUBBLE_BRUST = 3,
		CHAPTER8_BUBBLE_BRUST_TALK = 4,
		CHAPTER8_BUBBLE_BRUST_TALK_ANGRY = 18,
		CHAPTER8_FLY_AWAY = 32,

		HUMGRUMP_ANIM_STATE_END
	};

	typedef struct tagHumgrumpDesc : public CModelObject::MODELOBJECT_DESC
	{
		_bool		isLoop = { true };
		_uint		iStartAnimIndex = {};
		_wstring	strSectionTag = {};

		_bool		isOppositeSide = { false };
	}HUMGRUMP_DESC;

protected:
	CNpc_Humgrump(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CNpc_Humgrump(const CNpc_Humgrump& _Prototype);
	virtual ~CNpc_Humgrump() = default;

public:
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	void					Set_Opposite_Side();

public:
	static CNpc_Humgrump*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg) override;
	virtual void			Free() override;
};

END
