#pragma once

#include "Client_Defines.h"
#include "ContainerObject.h"

BEGIN(Client)
class CFSM;
class CMonster abstract : public CContainerObject
{
public:
	enum MONSTERPART { PART_BODY, PART_WEAPON, PART_EFFECT, PART_END };

	typedef struct tagMonsterDesc : public CContainerObject::CONTAINEROBJ_DESC
	{
		_float fChaseRange;
		_float fAttackRange;
	}MONSTER_DESC;

protected:
	CMonster(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CMonster(const CMonster& _Prototype);
	virtual ~CMonster() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Update(_float _fTimeDelta) override;
	virtual void Update(_float _fTimeDelta) override;
	virtual void Late_Update(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_float Calculate_Distance();
	virtual void Attack(_float _fTimeDelta);

public:
	virtual void Set_State(MONSTER_STATE _eState) 
	{
		m_eState = _eState;
	}

protected:
	MONSTER_STATE		m_eState = {};
	_uint				m_iPreState = {};
	CGameObject* m_pTarget = { nullptr };
	CFSM* m_pFSM = { nullptr };
	_float m_fChaseRange = { 0.f };
	_float m_fAttackRange = { 0.f };

public:
	HRESULT Cleanup_DeadReferences() override;
	virtual void Active_OnEnable() override;
	virtual void Active_OnDisable() override;


protected:
	virtual HRESULT Ready_Components() = 0;
	virtual HRESULT Ready_PartObjects() = 0;


public:
	virtual CGameObject* Clone(void* _pArg) = 0;
	virtual void Free() override;
};

END