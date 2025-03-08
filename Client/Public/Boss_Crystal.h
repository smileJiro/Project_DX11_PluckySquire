#pragma once
#include "Projectile_Monster.h"
#include "Client_Defines.h"

BEGIN(Client)

class CButterGrump;

class CBoss_Crystal final : public CProjectile_Monster
{
public:
	typedef struct tagBoss_Crystal_Desc : public CProjectile_Monster::PROJECTILE_MONSTER_DESC
	{
		CButterGrump* pSpawner = nullptr;
	}BOSS_CRYSTAL_DESC;

protected:
	CBoss_Crystal(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CBoss_Crystal(const CBoss_Crystal& _Prototype);
	virtual ~CBoss_Crystal() = default;

public:
	virtual HRESULT Initialize_Prototype(); // 프로토 타입 전용 Initialize
	virtual HRESULT Initialize(void* _pArg); // 초기화 시 필요한 매개변수를 void* 타입으로 넘겨준다.
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	virtual void OnContact_Enter(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas) override;
	virtual void OnContact_Stay(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas) override;
	virtual void OnContact_Exit(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas) override;

public:
	HRESULT Cleanup_DeadReferences() override;
	virtual void Active_OnEnable() override;
	virtual void Active_OnDisable() override;

private:
	virtual HRESULT					Ready_ActorDesc(void* _pArg);
	virtual HRESULT					Ready_Components();
	virtual HRESULT					Ready_PartObjects();

private:
	_uint m_iHp = { 0 };
	CButterGrump* m_pSpawner = { nullptr };

public:
	static CBoss_Crystal* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};
END