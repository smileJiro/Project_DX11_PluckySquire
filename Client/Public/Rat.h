#pragma once
#include "Monster.h"
class CRat final : public CMonster
{
public:
	enum Animation
	{
		IDLE,
		RUN,
		SLEEP,
		TURN_LEFT,
		TURN_RIGHT,
		DIE,
		HIT,
		LAST,
	};

private:
	CRat(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CRat(const CRat& _Prototype);
	virtual ~CRat() = default;

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
	virtual HRESULT					Ready_Components();
	virtual HRESULT					Ready_PartObjects();

public:
	static CRat* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

