#pragma once
#include "FSM.h"
#include "Client_Defines.h"

BEGIN(Client)

class CState;
class CMonster;

class CFSM_Boss final : public CFSM
{
public:
	typedef struct tagFSMBossDesc
	{
		CMonster* pOwner;
		_int iCurLevel;
	}FSMBOSSDESC;

private:
	CFSM_Boss(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CFSM_Boss(const CFSM_Boss& _Prototype);
	virtual ~CFSM_Boss() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	//상태 전환과 상태 업데이트
	void Update(_float _fTimeDelta);

public:
	virtual HRESULT Add_State(_uint _iState) override;
	virtual HRESULT Change_State(_uint _iState) override;
	virtual HRESULT Set_State(_uint _iState) override;

private:
	_uint Determine_NextAttack();

private:
	//일단 순서대로 패턴 돌도록 설정
	_uint m_iAttackIdx = { 0 };

public:
	HRESULT CleanUp();

public:
	static CFSM_Boss* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CFSM_Boss* Clone(void* _pArg);

public:
	virtual void Free() override;
};

END