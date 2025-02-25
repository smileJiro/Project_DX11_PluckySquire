#pragma once
#include "ModelObject.h"

BEGIN(Client)
class CMonster;
class CSoldier_Spear : public CModelObject
{
public:
	typedef struct tagSoldier_SpearDesc : public CModelObject::MODELOBJECT_DESC
	{
		class CMonster* pParent = nullptr;
	}SOLDIER_SPEAR_DESC;

private:
	CSoldier_Spear(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CSoldier_Spear(const CSoldier_Spear& _Prototype);
	virtual ~CSoldier_Spear() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void	Update(_float _fTimeDelta) override;
	virtual void	Late_Update(_float _fTimeDelta) override;
	virtual HRESULT	Render() override;


	virtual void OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other) override;
	virtual void OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)override;
	virtual void OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)override;
	virtual void Active_OnEnable() override;
	virtual void Active_OnDisable() override;

	virtual HRESULT	 Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition = nullptr) override;

	void Attack(CGameObject* _pVictim);

	_vector Get_LookDirection();


private:
	virtual	HRESULT					Ready_ActorDesc(void* _pArg);

private:
	_bool m_isAttackEnable = false;
	CMonster* m_pParent = { nullptr };

public:
	static CSoldier_Spear* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END