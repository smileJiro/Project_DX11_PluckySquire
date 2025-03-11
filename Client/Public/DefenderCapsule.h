#pragma once
#include "DefenderMonster.h"
BEGIN(Engine)
class CModelObject;
END
BEGIN(Client)
class CDefenderCapsule :
    public CDefenderMonster
{
public:
	typedef struct tagDefenderCapsuleDesc : public CDefenderMonster::DEFENDER_MONSTER_DESC
	{
		_uint iPersonCount = 1;
	}DEFENDER_CAPSULE_DESC;
public:
	enum ANIM_STATE
	{
		VORTEX_INTO,
		VORTEX_OUT,
		VORTEX_SPHERE_RELEASE,
		SPHERE_APPEAR_FAST,
		SPHERE_BREAK,
		SPHERE_LOOP,
	};
	enum DEFENDEDRCAPSULE_PART_ID
	{
		DEFENDER_CAPSULE_PART_BODY,
		DEFENDER_CAPSULE_PART_UI,
		DEFENDER_CAPSULE_PART_LAST
	};
private:
	CDefenderCapsule(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CDefenderCapsule(const CDefenderCapsule& _Prototype);
	virtual ~CDefenderCapsule() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Update(_float _fTimeDelta) override;
	virtual HRESULT Render();
public:
	virtual void On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void On_Hit(CGameObject* _pHitter, _int _iDamg, _fvector _vForce) override;
	virtual void On_Explode()override;
	virtual void On_Spawned()override;
	virtual void On_Teleport()override;
	virtual void On_LifeTimeOut()override;
	void On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx);

public:

private:
	HRESULT	Ready_PartObjects();
private:
	CModelObject* m_pBody = nullptr;
	CModelObject* m_pUI= nullptr;

	_uint m_iPersonCount = 1;
public:
	static CDefenderCapsule* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END