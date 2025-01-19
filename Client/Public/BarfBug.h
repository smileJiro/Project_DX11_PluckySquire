#pragma once
#include "Monster.h"
class CBarfBug final : public CMonster
{
public:
	enum Animation
	{
		ALERT,
		BARF,
		DIE,
		IDLE,
		STRAFE_BACK,
		STRAFE_LEFT,
		STRAFE_RIGHT,
		TURN_LEFT,
		TURN_RIGHT,
		WALK,
		DAMAGE,
		LAST,
	};

private:
	CBarfBug(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CBarfBug(const CBarfBug& _Prototype);
	virtual ~CBarfBug() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	virtual void Change_Animation() override;
	virtual void Attack(_float _fTimeDelta) override;
	void Alert_End(COORDINATE _eCoord, _uint iAnimIdx);
	void	Attack_End(COORDINATE _eCoord, _uint iAnimIdx);

private:
	virtual HRESULT					Ready_Components();
	virtual HRESULT					Ready_PartObjects();

public:
	static CBarfBug* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

