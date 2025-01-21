#pragma once
#include "Component.h"
#include "Client_Defines.h"

BEGIN(Client)

class CState;
class CMonster;

class CFSM : public CComponent
{
public:
	typedef struct tagFSMDesc
	{
		_bool isMelee;
		_float fAlertRange;
		_float fChaseRange;
		_float fAttackRange;
	}FSMDESC;

protected:
	CFSM(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CFSM(const CFSM& _Prototype);
	virtual ~CFSM() = default;

public:
	void Set_Owner(CMonster* _pOwner);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	//상태 전환과 상태 업데이트
	void Update(_float _fTimeDelta);

public:
	HRESULT Add_State(MONSTER_STATE _eState);
	HRESULT Change_State(MONSTER_STATE _eState);
	HRESULT Set_State(MONSTER_STATE _eState);


protected:
	CState* m_CurState = { nullptr };
	CMonster* m_pOwner = { nullptr };

private:
	map<MONSTER_STATE, CState*> m_States;
	MONSTER_STATE		m_eCurState = { MONSTER_STATE::LAST };

	_bool	m_isMelee = { true };
	_float	m_fAlertRange = {};
	_float	m_fChaseRange = {};
	_float	m_fAttackRange = {};

public:
	HRESULT CleanUp();

public:
	static CFSM* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CFSM* Clone(void* _pArg);

public:
	virtual void Free() override;
};

END