#pragma once
#include "ModelObject.h"

BEGIN(Client)
class CMonster;
class CButterGrump_Shield : public CModelObject
{
public:
	typedef struct tagButterGrump_ShieldDesc : public CModelObject::MODELOBJECT_DESC
	{
		class CMonster* pParent = nullptr;
	}BUTTERGRUMP_SHIELD_DESC;

private:
	CButterGrump_Shield(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CButterGrump_Shield(const CButterGrump_Shield& _Prototype);
	virtual ~CButterGrump_Shield() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void	Update(_float _fTimeDelta) override;
	virtual void	Late_Update(_float _fTimeDelta) override;
	virtual HRESULT	Render() override;

	virtual void OnContact_Enter(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas) override;
	virtual void OnContact_Stay(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas) override;
	virtual void OnContact_Exit(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas) override;

	//virtual void OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other) override;
	//virtual void OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)override;
	//virtual void OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)override;

	virtual void Active_OnEnable() override;
	virtual void Active_OnDisable() override;

public:
	void Shield_Break(const COLL_INFO& _Other);

private:
	virtual	HRESULT					Ready_ActorDesc(void* _pArg);

private:
	_bool m_isHitEnable = { true };
	CMonster* m_pParent = { nullptr };

public:
	static CButterGrump_Shield* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END