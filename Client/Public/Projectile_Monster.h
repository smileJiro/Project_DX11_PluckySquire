#pragma once
#include "Monster.h"
#include "Client_Defines.h"

BEGIN(Client)
class CProjectile_Monster abstract : public CContainerObject
{
public:
	typedef struct tagProjectile_Monster_Desc : public CContainerObject::CONTAINEROBJ_DESC
	{
		_float fLifeTime;
	}PROJECTILE_MONSTER_DESC;

	enum PROJECTILE_MONSTERPART { PART_BODY, PART_EFFECT, PART_END };

protected:
	CProjectile_Monster(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CProjectile_Monster(const CProjectile_Monster& _Prototype);
	virtual ~CProjectile_Monster() = default;

public:
	virtual HRESULT Initialize_Prototype(); // 프로토 타입 전용 Initialize
	virtual HRESULT Initialize(void* _pArg); // 초기화 시 필요한 매개변수를 void* 타입으로 넘겨준다.
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	virtual HRESULT		Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition = nullptr) override;

public:
	virtual void			On_Hit(CGameObject* _pHitter, _float _fDamg, _fvector _vForce);

	virtual void OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)override;
	virtual void OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)override;
	virtual void OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)override;

	virtual void	On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void	On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void	On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;


	virtual void Active_OnEnable() override;
	virtual void Active_OnDisable() override;

protected:
	_float m_fLifeTime = { 0.f };
	_float m_fAccTime = { 0.f };

	_bool m_isStop = {};

private:
	//virtual HRESULT					Ready_ActorDesc(void* _pArg);
	//virtual HRESULT					Ready_Components();
	//virtual HRESULT					Ready_PartObjects();

public:
	virtual CGameObject* Clone(void* _pArg) =0;
	virtual void			Free() override;
};
END