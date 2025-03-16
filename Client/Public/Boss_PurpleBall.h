#pragma once
#include "Projectile_Monster.h"
#include "Client_Defines.h"

BEGIN(Client)
class CBoss_PurpleBall final : public CProjectile_Monster
{
//public:
//	typedef struct tagBoss_PurpleBall_Desc : public CProjectile_Monster::CONTAINEROBJ_DESC
//	{
//		_float fLifeTime;
//	}BOSS_PURPLEBALL_DESC;

protected:
	CBoss_PurpleBall(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CBoss_PurpleBall(const CBoss_PurpleBall& _Prototype);
	virtual ~CBoss_PurpleBall() = default;

public:
	virtual HRESULT Initialize_Prototype(); // 프로토 타입 전용 Initialize
	virtual HRESULT Initialize(void* _pArg); // 초기화 시 필요한 매개변수를 void* 타입으로 넘겨준다.
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

	virtual void OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)override;

public:
	HRESULT Cleanup_DeadReferences() override;
	virtual void Active_OnEnable() override;
	virtual void Active_OnDisable() override;

private:
	ID3D11Buffer* m_pFresnelBuffer = { nullptr };
	ID3D11Buffer* m_pColorBuffer = { nullptr };

private:
	virtual HRESULT					Ready_ActorDesc(void* _pArg);
	virtual HRESULT					Ready_Components();
	virtual HRESULT					Ready_PartObjects();

public:
	static CBoss_PurpleBall* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};
END