#pragma once
#include "ModelObject.h"

BEGIN(Client)

class CNpc_Humgrump : public CModelObject
{
public:
	enum HUMGRUMP_ANIM_STATE
	{
		CHAPTER6_IDLE = 105,
		CHAPTER6_TALK = 107,
		CHAPTER6_TALK_STAND = 108,
		CHAPTER6_FINGER_RAISE = 86,
		CHAPTER6_FINGER_TALK = 87,
		CHAPTER6_FINGER_DOWN = 84,
		CHAPTER6_WINDUP = 115,
		CHAPTER6_BEAM = 81,
		CHAPTER6_BEAM_END = 83,
		CHAPTER6_TRANSFORM_IDLE = 110,
		CHAPTER6_TRANSFORM_TURN = 111,
		CHAPTER6_TRANSFORM_TURN_TALK = 113,
		CHAPTER6_TRANSFORM_LAUGH_INTO = 103,
		CHAPTER6_TRANSFORM_LAUGH_IDLE = 104,

		CHAPTER8_IDLE_UP = 100,
		CHAPTER8_TURN = 114,
		CHAPTER8_TALK_HAPPY = 75,
		CHAPTER8_IDLE = 98,
		CHAPTER8_TRANSFORM = 109,
		CHAPTER8_TRANSFORM_TALK = 106,
		CHAPTER8_BUBBLE_BRUST = 14,
		CHAPTER8_BUBBLE_BRUST_TALK = 15,
		CHAPTER8_BUBBLE_BRUST_TALK_ANGRY = 29,
		CHAPTER8_FLY_AWAY = 43,

		CHAPTER8_PUPA_INTRO_0 = 7,
		CHAPTER8_PUPA_IDLE_NOFACE = 6,
		CHAPTER8_PUPA_INTRO_1 = 8,
		CHAPTER8_PUPA_IDLE = 5,
		CHAPTER8_PUPA_GRUNT_INTO = 3,
		CHAPTER8_PUPA_GRUNT_0 = 1,
		CHAPTER8_PUPA_GRUNT_OUT = 4,
		CHAPTER8_PUPA_TALK_0 = 9,
		CHAPTER8_PUPA_GRUNT_1 = 2,
		CHAPTER8_PUPA_TALK_1 = 10,
		CHAPTER8_PUPA_GROW = 0,

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
