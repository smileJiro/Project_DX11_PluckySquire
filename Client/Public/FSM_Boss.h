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
	HRESULT Add_State(BOSS_STATE _eState);
	HRESULT Change_State(BOSS_STATE _eState);
	HRESULT Set_State(BOSS_STATE _eState);

private:
	map<BOSS_STATE, CState*> m_BossStates;
	BOSS_STATE		m_eCurBossState = { BOSS_STATE::LAST };

public:
	HRESULT CleanUp();

public:
	static CFSM_Boss* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CFSM_Boss* Clone(void* _pArg);

public:
	virtual void Free() override;
};

END