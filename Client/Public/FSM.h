#pragma once
#include "Component.h"

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
		_float fAlert2DRange;
		_float fChase2DRange;
		_float fAttack2DRange;
		CMonster* pOwner;
	}FSMDESC;

protected:
	CFSM(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CFSM(const CFSM& _Prototype);
	virtual ~CFSM() = default;

public:
	//void Set_Owner(CMonster* _pOwner);
	void Set_PatrolBound();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	//상태 전환과 상태 업데이트
	void Update(_float _fTimeDelta);

public:
	virtual HRESULT Add_State(_uint _iState);
	virtual HRESULT Change_State(_uint _iState);
	virtual HRESULT Set_State(_uint _iState);


protected:
	CState* m_pCurState = { nullptr };
	CMonster* m_pOwner = { nullptr };

protected:
	map<_uint, CState*> m_States;
	_uint		m_iCurState = { };

private:
	_bool	m_isMelee = { true };
	_float	m_fAlertRange = {};
	_float	m_fChaseRange = {};
	_float	m_fAttackRange = {};
	_float	m_fAlert2DRange = {};
	_float	m_fChase2DRange = {};
	_float	m_fAttack2DRange = {};

public:
	HRESULT CleanUp();

public:
	static CFSM* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CFSM* Clone(void* _pArg);

public:
	virtual void Free() override;
};

END