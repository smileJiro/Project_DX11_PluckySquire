#pragma once
#include "Monster.h"
#include "Client_Defines.h"

BEGIN(Client)
class CProjectile_BarfBug final : public CContainerObject
{
public:
	typedef struct tagProjectile_BarfBug_Desc : public CContainerObject::CONTAINEROBJ_DESC
	{
		_float fLifeTime;
	}PROJECTILE_BARFBUG_DESC;

	enum Animation2D
	{
		PROJECTILE,
		PROJECTILESPLAT,
		ANIM2D_LAST,
	};

protected:
	CProjectile_BarfBug(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CProjectile_BarfBug(const CProjectile_BarfBug& _Prototype);
	virtual ~CProjectile_BarfBug() = default;

public:
	virtual HRESULT Initialize_Prototype(); // 프로토 타입 전용 Initialize
	virtual HRESULT Initialize(void* _pArg); // 초기화 시 필요한 매개변수를 void* 타입으로 넘겨준다.
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	virtual HRESULT		Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition = nullptr) override;
	void Change_Animation();

public:
	virtual void Active_OnEnable() override;
	virtual void Active_OnDisable() override;

private:
	_float m_fLifeTime = { 0.f };
	_float m_fAccTime = { 0.f };

private:
	virtual HRESULT					Ready_ActorDesc(void* _pArg);
	virtual HRESULT					Ready_Components();
	virtual HRESULT					Ready_PartObjects();

public:
	static CProjectile_BarfBug* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};
END