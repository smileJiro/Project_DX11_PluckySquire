#pragma once
#include "ModelObject.h"

BEGIN(Client)

class CNpc_MoonBeard : public CModelObject
{
public:
	enum MOONBEARD_ANIM_STATE
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

		MOONBEARD_ANIM_STATE_END
	};

protected:
	CNpc_MoonBeard(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CNpc_MoonBeard(const CNpc_MoonBeard& _Prototype);
	virtual ~CNpc_MoonBeard() = default;

public:
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	static CNpc_MoonBeard*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg) override;
	virtual void			Free() override;
};

END
