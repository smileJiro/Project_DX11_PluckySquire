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
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& Prototype);
	virtual ~CMonster() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_float Calculate_Distance();
	virtual void Attack(_float fTimeDelta);

public:
	virtual void Set_State(MONSTER_STATE eState) {};

protected:
	_uint				m_iState = {};
	_uint				m_iPreState = {};
	CController_Transform* m_pPlayerTransform = { nullptr };
	CFSM* m_pFSM = { nullptr };
	_float m_fChaseRange = { 0.f };
	_float m_fAttackRange = { 0.f };


protected:
	virtual HRESULT Ready_Components() = 0;
	virtual HRESULT Ready_PartObjects() = 0;


public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END