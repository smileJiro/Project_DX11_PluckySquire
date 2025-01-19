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
		_float fAlertRange;
		_float fChaseRange;
		_float fAttackRange;
		_float fDelayTime;
		_float fCoolTime;
	}MONSTER_DESC;

protected:
	CMonster(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CMonster(const CMonster& _Prototype);
	virtual ~CMonster() = default;

public:
	virtual void Set_State(MONSTER_STATE _eState)
	{
		m_eState = _eState;
	}
	virtual void Set_PreState(MONSTER_STATE _eState)
	{
		m_ePreState = _eState;
	}

	void Set_AnimChangeable(_bool _isChangeable)
	{
		m_isAnimChangeable = _isChangeable;
	}
	_bool Get_AnimChangeable()
	{
		return m_isAnimChangeable;
	}

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
	virtual void Change_Animation() {};

protected:
	void Delay_On() 
	{ 
		m_fAccTime = 0.f;
		m_isDelay = true; 
	}
	void Delay_Off()
	{
		m_isDelay = false;
		m_fAccTime = 0.f;
	}
	void CoolTime_On()
	{
		m_fAccTime = 0.f;
		m_isCool = true;
	}
	void CoolTime_Off()
	{
		m_isCool = false;
		m_fAccTime = 0.f;
	}

protected:
	MONSTER_STATE		m_eState = {};
	MONSTER_STATE		m_ePreState = {};
	CGameObject* m_pTarget = { nullptr };
	CFSM* m_pFSM = { nullptr };
	_float m_fAlertRange = { 0.f };
	_float m_fChaseRange = { 0.f };
	_float m_fAttackRange = { 0.f };
	_bool m_isAnimChangeable = { true };

	_float m_fAccTime = { 0.f };
	_bool m_isDelay = { false };
	_float m_fDelayTime = { 0.f };
	_bool m_isCool = { false };
	_float m_fCoolTime = { 0.f };
	_uint	 m_iAttackCount = { 0 };

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