#pragma once
#include "Monster.h"
BEGIN(Client)
class CJumpBug final : public CMonster
{
public:
	enum Animation
	{
		DIE,
		IDLE,
		JUMP_DOWN,
		JUMP_UP,
		TURN_LEFT,
		TURN_RIGHT,
		HIT,
		LAST,
	};

	enum Animation2D
	{
		DIE_DOWN,
		DIE_RIGHT,
		DIE_UP,
		HIT_DOWN,
		HIT_RIGHT,
		HIT_UP,
		IDLE_DOWN,
		IDLE_RIGHT,
		IDLE_UP,
		JUMP_FALL_DOWN,
		JUMP_FALL_RIGHT,
		JUMP_FALL_UP,
		JUMP_LAND_DOWN,
		JUMP_LAND_RIGHT,
		JUMP_LAND_UP,
		JUMP_RISE_DOWN,
		JUMP_RISE_RIGHT,
		JUMP_RISE_UP,
		ANIM2D_LAST,
	};

private:
	CJumpBug(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CJumpBug(const CJumpBug& _Prototype);
	virtual ~CJumpBug() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

	virtual void	On_Hit(CGameObject* _pHitter, _int _iDamg, _fvector _vForce) override;


public:
	virtual void Change_Animation() override;
	void Animation_End(COORDINATE _eCoord, _uint iAnimIdx);

	virtual HRESULT				Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition = nullptr) override;

	virtual void Turn_Animation(_bool _isCW) override;

	virtual void Monster_Move(_fvector _vDirection) override;

private:
	_bool m_isJump = { false };

private:
	virtual	HRESULT					Ready_ActorDesc(void* _pArg);
	virtual HRESULT					Ready_Components();
	virtual HRESULT					Ready_PartObjects();

public:
	static CJumpBug* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END