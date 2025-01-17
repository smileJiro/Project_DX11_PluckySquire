#pragma once
#include "Component.h"
#include "PlayerState.h"

BEGIN(Client)

class CTransition;
class CPlayerState;
class  CStateMachine :
    public CComponent
{
public:
protected:
	CStateMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStateMachine(const CStateMachine& Prototype);
	virtual ~CStateMachine() = default;

public:
	HRESULT Initialize_Prototype();
	HRESULT Initialize_Prototype(const _char* strJsonFilePath);
	virtual HRESULT Initialize(void* pArg) override;
	void Check_Transition(_float fTimeDelta);

	void Add_ConditionVariable(_uint iConditionVarID, float* pfValue);
	void Add_ConditionVariable(_uint iConditionVarID, int* piValue);
	void Add_ConditionVariable(_uint iConditionVarID, bool* pbValue);
	void Add_TriggerConditionVariable(_uint iConditionVarID);
	CPlayerState* Add_State(_uint iStateID);
	CTransition* Add_Transition(_uint iStateID, _uint iNextState);
	CTransition* Add_SubTransition(_uint iStateID, _uint iNextState);

	void Bind_Condition(CTransition* pTransition, _uint iConditionVarID, CONDITION_TYPE eCondType, int iValue);
	void Bind_Condition(CTransition* pTransition, _uint iConditionVarID, CONDITION_TYPE eCondTyp, float fValue);
	void Bind_Condition(CTransition* pTransition, _uint iConditionVarID, CONDITION_TYPE eCondTyp, bool bValue);
	void Bind_TriggerCondition(CTransition* pTransition, _uint iConditionVarID);

	void Trigger_ConditionVariable(_uint iConditionVarID);
	void Remove_Condition(_uint iConditionID);

	void Register_OnStateChangeCallBack(const function<void(_uint)>& fCallback) { m_listStateChangeCallback.push_back(fCallback); }
	void Register_OnSubStateChangeCallBack(const function<void(_uint)>& fCallback) { m_listSubStateChangeCallback.push_back(fCallback); }

	_uint Get_CurrentState() { return m_iCurrentState; }
	_uint Get_CurrentSubState() { return m_iCurrentSubState; }
	void Set_CurrentState(_uint iState);
	void Set_CurrentState(_uint iMainState, _uint iSubState);
private:
	_uint m_iCurrentState = { 0 };
	_uint m_iCurrentSubState = { 0 };
	map<_uint, CPlayerState*> m_mapStates;

	map<_uint, ConditionVariable*> m_mapConditionVariable;
	map<_uint, TriggerConditionVariable*> m_mapTriggerConditionVariable;

	list<function<void(_uint)>> m_listStateChangeCallback;
	list<function<void(_uint)>> m_listSubStateChangeCallback;
public:
	static CStateMachine* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	static CStateMachine* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* strJsonFilePath);

	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END