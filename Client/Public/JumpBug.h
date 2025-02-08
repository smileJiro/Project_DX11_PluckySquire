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

public:
	virtual void Change_Animation() override;
	void Animation_End(COORDINATE _eCoord, _uint iAnimIdx);

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