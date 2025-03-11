#pragma once
#include "Projectile_Monster.h"
#include "Client_Defines.h"

BEGIN(Client)
class CProjectile_SketchSpace_UFO final : public CProjectile_Monster
{
public:

	enum Animation2D
	{
		PROJECTILE_HIT,
		PROJECTILE_LOOP,
		ANIM2D_LAST,
	};

private:
	CProjectile_SketchSpace_UFO(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CProjectile_SketchSpace_UFO(const CProjectile_SketchSpace_UFO& _Prototype);
	virtual ~CProjectile_SketchSpace_UFO() = default;

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
	void Animation_End(COORDINATE _eCoord, _uint iAnimIdx);

public:
	virtual void			On_Hit(CGameObject* _pHitter, _float _fDamg, _fvector _vForce);

	virtual void	On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void	On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void	On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;


	virtual void Active_OnEnable() override;
	virtual void Active_OnDisable() override;

private:
	virtual HRESULT					Ready_Components();
	virtual HRESULT					Ready_PartObjects();

public:
	static CProjectile_SketchSpace_UFO* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};
END